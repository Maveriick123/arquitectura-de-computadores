// * SUMMA: Scalable Universal Matrix Multiplication Algorithm

/*
* m, n, k, alfa pasados por teclado y asumimos que son múltiplos de  raíz de nprocs.
* Resultados correctos para mallas cuadradas.
* La operación a realizar es de la forma: alfa * A(m*k) * B(k*n) = C(m*n)
* Sólo reservamos espacio para las matrices enteras en el p0. Liberamos memoria luego...
* Inicialización de A y B en el p0.
* Distribución de A y B con tipos de datos derivados. En este caso como son datos del mismo tipo espaciados a intervalos regulares:
Usamos MPI_Type_vector...
* Liberación de tipos de datos derivados al terminar la distribución y de sendcounts y displs.
* Broadcast internos al bucle se utilizan usando los nuevos comunicadores por filas y columnas.
*/

/*
    * mpicc -Wall summa.c -o summa -lm
    * mpirun -np 16 -oversubscribe ./summa 8 4 8 1
*/

#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


void productoMatrices(float alfa, float *A, float *B, float *res, int m, int n, int k){
    int i, j, z;
    // El orden de los bucles es importante para el correcto funcionamiento 
    for(i = 0; i < m; i++){
        for(j = 0; j < n; j++){
            for(z = 0; z < k; z++){
                res[i*n+j] += alfa * A[i*k+z] * B[z*n+j];
            }
        }
    }
    return;
}


void printMatriz(float *M, int filas, int columnas){
    int i,j;
    for(i = 0; i < filas; i++){
        for(j = 0; j < columnas; j++){
            printf("%c%12.2f%c", (j == 0)?' ': ' ', M[i*columnas + j], (j+1==columnas)?'\n': ' ');
        }
    }
    return;
}

int main(int argc, char *argv[]){

    // Flag para el uso de split o topologías virtuales

    int split = 0; // Si 0, entonces hacemos uso de topologías virtuales


    int nprocs, myid;
    int m, n, k;
    float alfa;
    int i, j;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);


    /*-------------- Distribución de alfa y dimensiones de las matrices ----------------*/

    // Proceso 0 lee los parametros de entrada

    if(!myid){
        // Asumimos que los parámetros de entrada son correctos para mallas cuadradas
        // También asumimos que m,n y k son múltiplos de  la raíz de nprocs
        if(argc > 4){ // 4 parámetros más el nombre del ejecutable
            m = atoi(argv[1]);
            n = atoi(argv[2]);
            k = atoi(argv[3]);
            alfa = atof(argv[4]);
        } else{
            printf("Número de parámetros incorrecto");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 0;
        }
    }

    int size1, size2;
    char *buffer;
    int position;

    /*
    MPI_Pack_size: Returns  the  upper bound on the amount of space needed to pack a message. 
    int MPI_Pack_size(int incount, MPI_Datatype datatype, MPI_Comm comm, int *size)
    */


    // Tamaño necesario para empaquetar m, n y k
    MPI_Pack_size(3, MPI_INT, MPI_COMM_WORLD, &size1);

    // Tamaño necesario para empaquetar alfa
    MPI_Pack_size(1, MPI_FLOAT, MPI_COMM_WORLD, &size2);

    buffer = malloc(size1 + size2); // En C los char ocupan un byte...

    if(!myid){

        // Packs data of a given datatype into contiguous memory...
        /* int MPI_Pack(const void *inbuf, int incount, MPI_Datatype datatype,
                        void *outbuf, int outsize, int *position, MPI_Comm comm)
                        MPI_Pack(&m, 1, MPI_INT,)*/
        position = 0;
        MPI_Pack(&m,1,MPI_INT,buffer,size1+size2,&position,MPI_COMM_WORLD);
        MPI_Pack(&n,1,MPI_INT,buffer,size1+size2,&position,MPI_COMM_WORLD);
        MPI_Pack(&k,1,MPI_INT,buffer,size1+size2,&position,MPI_COMM_WORLD);

        MPI_Pack(&alfa, 1, MPI_FLOAT, buffer, size1 + size2, &position, MPI_COMM_WORLD);
    }

    MPI_Bcast(buffer, size1 + size2, MPI_PACKED, 0, MPI_COMM_WORLD);

    if(myid){
        /*
        MPI_Unpack - Unpacks a datatype into contiguous memory...
        int MPI_Unpack(const void *inbuf, int insize, int *position,
            void *outbuf, int outcount, MPI_Datatype datatype,
            MPI_Comm comm)*/
        
        position = 0;
        
        MPI_Unpack(buffer, size1 + size2, &position, &m, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(buffer, size1 + size2, &position, &n, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(buffer, size1 + size2, &position, &k, 1, MPI_INT, MPI_COMM_WORLD);

        MPI_Unpack(buffer, size1 + size2, &position, &alfa, 1, MPI_FLOAT, MPI_COMM_WORLD);
    }

    free(buffer); // Una vez desempaquetados los metadatos liberamos el buffer.

    /*-------------------- INICIALIZACIÓN DE A Y B -------------------------*/

    float *A;
    float *B;
    float *C;

    // Sólo se reserva espacio en el proceso 0...
    if(!myid){
        A = malloc(m*k*sizeof(float));
        B = malloc(n*k*sizeof(float));
        C = malloc(m*n*sizeof(float));


        // C es row-major-order
        // Hacemos acceso lineal a memoria contigua
        for(i = 0;i < m;i++){
            for(j = 0;j < k;j++){
                A[i*k+j] = i + j;
            }
        }

        for(i = 0; i < k; i++){
            for(j = 0; j < n; j++){
                // 1.0f evita conversiones implicitas, más eficiente --> float nativo
                B[i*n+j] = 1.0f + 2.0f*i + j;
            }
        }
    }

    // Malla de procesos...

    int procs_per_dimension;
    procs_per_dimension = (int)sqrt(nprocs);

    /* -------------------------- Distribución de A y B por bloques -------------------------------*/

    int localM, localK, localN;

    localM = m/procs_per_dimension;
    localK = k/procs_per_dimension;
    localN = n/procs_per_dimension;
    
    float *localA, *localB, *localC;

    localA = malloc(localM * localK * sizeof(float));
    localB = malloc(localK * localN * sizeof(float));
    localC = calloc(localM * localN,sizeof(float)); // Importante inicializarlo a cero!!

    MPI_Datatype sub_matrix_A, extentA;
    MPI_Datatype sub_matrix_B, extentB;
    
    MPI_Type_vector(localM, localK, k, MPI_FLOAT, &sub_matrix_A);
    //MPI_Type_commit(&sub_matrix_A);

    MPI_Type_vector(localK, localN, n, MPI_FLOAT, &sub_matrix_B);
    //MPI_Type_commit(&sub_matrix_B);
        
    /*
    MPI_Type_create_resized - Returns a new data type with new extent and upper and lower bounds.

    int MPI_Type_create_resized(MPI_Datatype oldtype, MPI_Aint lb,
        MPI_Aint extent, MPI_Datatype *newtype)*/

    MPI_Type_create_resized(sub_matrix_A, 0, sizeof(float), &extentA);
    MPI_Type_commit(&extentA);

    MPI_Type_create_resized(sub_matrix_B, 0, sizeof(float), &extentB);
    MPI_Type_commit(&extentB);

    /*int MPI_Scatterv(const void *sendbuf, const int sendcounts[], const int displs[],
            MPI_Datatype sendtype, void *recvbuf, int recvcount,
            MPI_Datatype recvtype, int root, MPI_Comm comm)*/

    int *sendcounts; 
    int *displs;
    int proc;

    if(!myid){
        sendcounts = malloc(nprocs * sizeof(int));
        displs = malloc(nprocs * sizeof(int)); 
        for(i = 0; i < procs_per_dimension; i++){
            for(j = 0; j < procs_per_dimension; j++){
                proc = i * procs_per_dimension + j;
                sendcounts[proc] = 1;
                displs[proc] = i * localM * k + j * localK;
            }
        }
    }

    MPI_Scatterv(A, sendcounts, displs, extentA, localA, localM * localK, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if(!myid){
        for (i = 0; i < procs_per_dimension; i++){
            for(j = 0; j < procs_per_dimension; j++){
                proc = i * procs_per_dimension + j;
                sendcounts[proc] = 1;
                displs[proc] = (i * localK * n) + j * localN; 
            }
        }
    }

    MPI_Scatterv(B, sendcounts, displs, extentB, localB, localK * localN, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if(!myid){
        free(sendcounts);
        free(displs);
    }

    MPI_Type_free(&sub_matrix_A);
    MPI_Type_free(&sub_matrix_B);
    MPI_Type_free(&extentA);
    MPI_Type_free(&extentB);

    /*------------ Comunicación por filas y por columnas --------------------*/

    int my_row, my_col;
    MPI_Comm comm_row = MPI_COMM_NULL; 
    MPI_Comm comm_col = MPI_COMM_NULL;

    if(split){
        my_row = myid / procs_per_dimension;
        my_col = myid % procs_per_dimension;

        /*
        MPI_Comm_split  - Creates new communicators based on colors and keys.
        int MPI_Comm_split(MPI_Comm comm, int color, int key,
            MPI_Comm *newcomm)*/

        MPI_Comm_split(MPI_COMM_WORLD, my_row, my_col, &comm_row);
        MPI_Comm_split(MPI_COMM_WORLD, my_col, my_row, &comm_col);

    }else{
        const int ndims = 2;
        int dims[ndims];
        dims[0] = procs_per_dimension;
        dims[1]  =procs_per_dimension;
        int periods[ndims];
        periods[0]  = 0;
        periods[1] = 0;
        int coords[ndims];

        MPI_Comm comm_grid;
        MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, periods, 0, &comm_grid);

        MPI_Cart_coords(comm_grid, myid, 2, coords);
        my_row = coords[0];
        my_col = coords[1];

        int remain_dims[ndims];
        remain_dims[0] = 0; // Fijas la fila
        remain_dims[1] = 1;
        MPI_Cart_sub(comm_grid, remain_dims, &comm_row);
        remain_dims[0] = 1; 
        remain_dims[1] = 0; // Fijas la columna
        MPI_Cart_sub(comm_grid, remain_dims, &comm_col);
        MPI_Comm_free(&comm_grid);
    }

    /*----------------- Implemetación de SUMMA -------------------*/

    float *bufferA = malloc(localM * localK * sizeof(float));
    float *bufferB = malloc(localK * localN * sizeof(float));

    for(int it = 0; it < procs_per_dimension; it++){
        // Broadcast de A_*i en horizontal(por filas)
        if(my_col == it) memcpy(bufferA, localA, localM*localK*sizeof(float));
        MPI_Bcast(bufferA, localM * localK, MPI_FLOAT, it, comm_row);
        // Broadcast de B_i* en vertical(por columnas)
        if(my_row == it) memcpy(bufferB, localB, localK*localN*sizeof(float));
        MPI_Bcast(bufferB, localK * localN, MPI_FLOAT, it, comm_col);
        productoMatrices(alfa, bufferA, bufferB, localC, localM, localN, localK);
    }

    free(localA);
    free(localB);
    free(bufferA);
    free(bufferB);


    MPI_Comm_free(&comm_row);
    MPI_Comm_free(&comm_col);

    /* -------------- Recolección (Distribución inversa) ------------------- */

    MPI_Datatype sub_matrix_C, extentC;
    MPI_Type_vector(localM, localN, n, MPI_FLOAT, &sub_matrix_C);
    MPI_Type_create_resized(sub_matrix_C, 0, sizeof(float), &extentC);

    // MPI_Type_commit(&sub_matrix_C);
    MPI_Type_commit(&extentC);

    int *recvcounts;
    int *displsR;

    if (!myid) {
        recvcounts = malloc(nprocs * sizeof(int));
        displsR = malloc(nprocs * sizeof(int));
    
        for (i = 0; i < procs_per_dimension; i++) {
            for (j = 0; j < procs_per_dimension; j++) {

                proc = i * procs_per_dimension + j;

                recvcounts[proc] = 1; 
                displsR[proc] = (i * localM * n) + (j * localN);
            }
        }
    }

    MPI_Gatherv(localC, localM * localN, MPI_FLOAT, C, recvcounts, displsR, extentC, 0, MPI_COMM_WORLD);


    MPI_Type_free(&extentC);
    MPI_Type_free(&sub_matrix_C);
    free(localC);

    if(!myid){
        free(recvcounts);
        free(displsR);
        printf("\nResultado en Paralelo\n");
        printf("\tMatriz A\n");
        printMatriz(A, m, k);
        printf("\tMatriz B\n");
        printMatriz(B, k, n);
        printf("\tMatriz C\n");
        printMatriz(C, m, n);
        printf("\nComprobación en Secuencial\n");
        float *m_sec = calloc(m*n, sizeof(float));
        productoMatrices(alfa, A, B, m_sec, m, n, k);
        int cnt = 0;
        for (i = 0; i < m; i++){
            for(j = 0; j < n; j++){
                if(C[i*n+j]!=m_sec[i*n+j]){
                    printf("\t (*) Error en la fila %d columna %d, ya que %f != %f", i, j, C[i*n+j],m_sec[i*n+j]);
                    cnt++;
                }
            }
        }
        (cnt == 0)? printf("\t (*) Matrices idénticas\n\n"): printf("\t (*) Matrices distintas\n\n");
        free(m_sec);
        free(A);
        free(B);
        free(C);
    }
    MPI_Finalize();
    return 0;
}
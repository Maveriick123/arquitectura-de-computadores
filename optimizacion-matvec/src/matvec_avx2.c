#include <immintrin.h>
#include <stdio.h>
#include "polybench.h"
#define N 10000
#define T 100

int main() {
	float * a, * x, *y; 
	posix_memalign( (void*)&a, 32, sizeof(float)*N*N);
	posix_memalign( (void*)&x, 32, sizeof(float)*N);
	posix_memalign( (void*)&y, 32, sizeof(float)*N);

	for( int i = 0; i < N; ++i ) {
		for( int j = 0; j < N; ++j ) {
			a[i*N+j] = j;
		}
		x[i] = i;
		y[i] = 0;
	}


	#ifdef POLYBENCH_GFLOPS
        polybench_program_total_flops = 2L * T * N * N;
    #endif

    polybench_start_instruments;
	for( int t = 0; t < 100; ++t ){
		for( int i = 0; i < N; i+=4 ) {
            __m256 vec_y0 = _mm256_setzero_ps();
            __m256 vec_y1 = _mm256_setzero_ps();
            __m256 vec_y2 = _mm256_setzero_ps();
            __m256 vec_y3 = _mm256_setzero_ps();
            __m256 vec_y4 = _mm256_setzero_ps();
            __m256 vec_y5 = _mm256_setzero_ps();
            __m256 vec_y6 = _mm256_setzero_ps();
            __m256 vec_y7 = _mm256_setzero_ps();
			for( int j = 0; j < N; j+=16 ) {
                __m256 vec_x0 = _mm256_load_ps(&x[j]);
                __m256 vec_a0 = _mm256_load_ps(&a[i * N + j]);
                vec_y0 = _mm256_fmadd_ps(vec_a0, vec_x0, vec_y0);

                __m256 vec_a1 = _mm256_load_ps(&a[(i+1) * N + j]);
                vec_y1 = _mm256_fmadd_ps(vec_a1, vec_x0, vec_y1);

                __m256 vec_a2 = _mm256_load_ps(&a[(i+2) * N + j]);
                vec_y2 = _mm256_fmadd_ps(vec_a2, vec_x0, vec_y2);

                __m256 vec_a3 = _mm256_load_ps(&a[(i+3) * N + j]);
                vec_y3 = _mm256_fmadd_ps(vec_a3, vec_x0, vec_y3);

                __m256 vec_x4 = _mm256_load_ps(&x[j+8]);
                __m256 vec_a4 = _mm256_load_ps(&a[i * N + j +8]);
                vec_y4 = _mm256_fmadd_ps(vec_a4, vec_x4, vec_y4);

                __m256 vec_a5 = _mm256_load_ps(&a[(i+1) * N + j + 8]);
                vec_y5 = _mm256_fmadd_ps(vec_a5, vec_x4, vec_y5);

                __m256 vec_a6 = _mm256_load_ps(&a[(i+2) * N + j + 8]);
                vec_y6 = _mm256_fmadd_ps(vec_a6, vec_x4, vec_y6);

                __m256 vec_a7 = _mm256_load_ps(&a[(i+3) * N + j + 8]);
                vec_y7 = _mm256_fmadd_ps(vec_a7, vec_x4, vec_y7);

			}
            __m256 sum04 = _mm256_add_ps(vec_y0, vec_y4);
            __m256 sum15 = _mm256_add_ps(vec_y1, vec_y5);
            __m256 sum26 = _mm256_add_ps(vec_y2, vec_y6);
            __m256 sum37 = _mm256_add_ps(vec_y3, vec_y7);
            __attribute__((aligned(32))) float tmp1[8], tmp2[8], tmp3[8], tmp4[8];
            _mm256_store_ps(tmp1, sum04);
            y[i] = tmp1[0] + tmp1[1] + tmp1[2] + tmp1[3] +
            tmp1[4] + tmp1[5] + tmp1[6] + tmp1[7];
            _mm256_store_ps(tmp2, sum15);
            y[i+1] = tmp2[0] + tmp2[1] + tmp2[2] + tmp2[3] +
            tmp2[4] + tmp2[5] + tmp2[6] + tmp2[7];
            _mm256_store_ps(tmp3, sum26);
            y[i+2] = tmp3[0] + tmp3[1] + tmp3[2] + tmp3[3] +
            tmp3[4] + tmp3[5] + tmp3[6] + tmp3[7];
            _mm256_store_ps(tmp4, sum37);
            y[i+3] = tmp4[0] + tmp4[1] + tmp4[2] + tmp4[3] +
            tmp4[4] + tmp4[5] + tmp4[6] + tmp4[7];
		}
	}
	polybench_stop_instruments;
    polybench_print_instruments;

	printf( "y[40] = %f\n", y[40] );
}

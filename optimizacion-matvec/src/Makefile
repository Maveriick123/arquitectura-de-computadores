CC = gcc
CFLAGS = -Ofast -march=native -I. -g -DPOLYBENCH_GFLOPS
SRCS = matvec_avx2.c polybench.c
TARGET = experimento

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)
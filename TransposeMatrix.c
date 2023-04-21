#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

int** MemoryAllocate(int n) {
    int** matrix = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; ++i) {
        matrix[i] = (int*)malloc(n * sizeof(int));
    }
    return matrix;
}

void PrintMatrix(int** matrix, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char** argv) {

    int num_procs;
    int rank;
    int n = strtol(argv[1], NULL, 10);
    int** matrix = MemoryAllocate(n);


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    unsigned int seed;
    if (rank == 0) {

        seed = time(NULL);

        for (int i = 1; i < num_procs; ++i){
            MPI_Send(&seed, 1, MPI_UNSIGNED, i, 1, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(&seed, 1, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    srand(seed);

    ////////
    for (int i = 0; i < n; ++i){
        for (int j = 0; j < n; ++j){
            matrix[i][j] = random() / (RAND_MAX/100);
        }
    }

    if (rank != 0) {
        int k = rank-1;
        while (k < n) {
            for (int j = 0; j < n; ++j){
                matrix[k][j] = matrix[j][k];
            }

            MPI_Send(matrix[k], n, MPI_INT, 0, k, MPI_COMM_WORLD);
            //printf("%d: Send with %d tag\n", rank, k);
            k += (num_procs - 1);
        }
    }

    if (rank == 0) {
        double t1, t2;
        t1 = MPI_Wtime();
        if (n < 6) {
            PrintMatrix(matrix, n);
        }
        for (int i = 0; i < n; ++i) {
            MPI_Recv(matrix[i], n, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //printf("%d: Received with %d tag\n", rank, i);
        }
        if (n < 6) {
            PrintMatrix(matrix, n);
        }
        t2 = MPI_Wtime();

        printf("%f time spend\n", t2 - t1);
    }

    for (int i = 0; i < n; ++i) {
        free(matrix[i]);
    }
    free(matrix);

    MPI_Finalize();
    return 0;
}
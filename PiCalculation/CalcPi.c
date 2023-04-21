#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char** argv) {

    int num_procs;
    int rank;
    int iters = strtol(argv[1], NULL, 10);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);



    long double local_sum = 0;
    MPI_Status status;
    int j = rank;
    while (j < iters){
        local_sum += 1.0/(j+1)/(j+1);
        j += (num_procs);
    }

    long double global_sum = 0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_DOUBLE, MPI_SUM, 0,
               MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Total sum = %.10Lf\n", sqrtl(global_sum*6));
    }

    MPI_Finalize();
}

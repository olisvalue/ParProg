#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void round_robin(int rank, int procs, int iter);

int main(int argc, char** argv) {

    int num_procs;
    int rank;
    int iter = strtol(argv[1], NULL, 10);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    round_robin(rank, num_procs, iter);

    MPI_Finalize();
    return 0;
}

void round_robin(int rank, int procs, int iter){
    long int rand_mine, rand_prev;
    int rank_next = (rank + 1) % procs;
    int rank_prev = rank == 0 ? procs - 1 : rank - 1;
    MPI_Status status;
    srandom(time(NULL) + rank);
    rand_mine = random() / (RAND_MAX/100);

    double t1, t2;
    t1 = MPI_Wtime();
    //printf("%d: My random is %ld\n", rank, rand_mine);

    for (int i = 0; i < iter; ++i) {
        MPI_Sendrecv((void *) &rand_mine, 1, MPI_LONG, rank_next, 1,
                     (void *) &rand_prev, 1, MPI_LONG, rank_prev, 1,
                     MPI_COMM_WORLD, &status);
    }
    t2 = MPI_Wtime();
    printf("%lf ", t2 - t1);
}
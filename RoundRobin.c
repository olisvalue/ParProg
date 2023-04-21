#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void round_robin(int rank, int procs);

int main(int argc, char** argv) {

    int num_procs;
    int rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("%d: It's %d proc, total: %d \n", rank, rank, num_procs);
    round_robin(rank, num_procs);
    printf("%d: Bye \n", rank);
    MPI_Finalize();
}

void round_robin(int rank, int procs){
    long int rand_mine, rand_prev;
    int rank_next = (rank + 1) % procs;
    int rank_prev = rank == 0 ? procs - 1 : rank - 1;
    MPI_Status status;
    srandom(time(NULL) + rank);
    rand_mine = random() / (RAND_MAX/100);

    double t1, t2;
    t1 = MPI_Wtime();
    printf("%d: My random is %ld\n", rank, rand_mine);

    MPI_Sendrecv((void *)&rand_mine, 1, MPI_LONG, rank_next, 1,
                 (void *)&rand_prev, 1, MPI_LONG, rank_prev, 1,
                 MPI_COMM_WORLD, &status);
    t2 = MPI_Wtime();
    printf("%d: I had %ld number, %d had %ld, we spent %f time\n", rank, rand_mine, rank_prev, rand_prev, t2 - t1);

}
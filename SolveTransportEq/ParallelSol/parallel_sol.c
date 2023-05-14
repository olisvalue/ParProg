#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define pi 3.14159265359


void write_to_file(int rank, int num_procs, double* data, int num_rows, int col_num, char* filename) {
    MPI_File file;
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE| MPI_MODE_WRONLY, MPI_INFO_NULL, &file); // открываем файл для записи
    MPI_Offset offset = rank * num_rows * sizeof(double) + col_num * sizeof(double); // вычисляем позицию для записи
    MPI_File_write_at(file, offset, data, num_rows, MPI_DOUBLE, MPI_STATUS_IGNORE); // записываем данные в файл
    MPI_File_close(&file); // закрываем файл
}

double phi(double x0){return cos(pi * x0);}
double psi(double t0){return exp(-t0);}
double alpha(double t, double x){return 2;}
double f(double t, double x){return t+x;}
void apply_bord(double** u, double* ts, double* xs, int k, int n){
    for (int i = 0; i < k; ++i){
        u[i][0] = psi(ts[i]);
    }
    for (int j = 0; j < n; ++j){
        u[0][j] = phi(xs[j]);
    }
}
double** mem_alloc(int k, int n) {
    double** matrix = (double**)malloc(k * sizeof(double*));
    for (int i = 0; i < k; ++i) {
        matrix[i] = (double*)malloc(n * sizeof(double));
    }
    return matrix;
}
void mem_free(double** matrix, int k) {
    for (int i = 0; i < k; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}

double* linspace(double start, double stop, int num) {
    double* array = (double*)malloc(num * sizeof(double));
    double step = (stop - start) / (num - 1);
    for (int i = 0; i < num; i++) {
        array[i] = start + i * step;
    }
    return array;
}

void print_matrix(double** matrix, int k, int n) {
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < n-1; ++j) {
            printf("%lf, ", matrix[i][j]);
        }
        printf("%lf\n", matrix[i][n-1]);
    }
}

void get_size_vals(int* k, int* n, int argc, char** argv) {
    if (argc >= 3) {
// convert the second and third argument to integers
        *k = strtol(argv[1], NULL, 10);
        *n = strtol(argv[2], NULL, 10);
    } else {
        *k = 100;
        *n = 100;
    }
}

void calc_params(int n, int rank, int num_procs, int* num_xs, int* n0) {
    *num_xs = n / num_procs;
    *n0 = *num_xs * rank;
    if (rank == num_procs - 1) {
        *num_xs += n % num_procs;
    }
    if (rank != 0) {
        *n0 = *n0 - 1;
        *num_xs = *num_xs + 1;
    }
}

double** calculate_part(int k, int n, double T, double X, int rank, int num_procs){
    int num_xs, n0;
    calc_params(n, rank, num_procs, &num_xs, &n0);
    double** u = mem_alloc(k, num_xs);
    double* ts = linspace(0, T, k);
    double* xs = (double*)malloc(num_xs * sizeof(double));
    double tau = ts[1] - ts[0];
    double h = X/(n-1);
    for (int j = 0; j < num_xs; ++j){
        xs[j] = n0*h + h*j;
    }
    double beta = tau/h;

    if (rank == 0) {
        apply_bord(u, ts, xs, k, num_xs);
    }
    else {
        apply_bord(u, ts, xs, 1, num_xs);
    }
    for (int i = 1; i < k; ++i){
        if (rank != 0) {
            MPI_Recv(&u[i][0], 1, MPI_DOUBLE, rank-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        for (int j = 1; j < num_xs; ++j){
            u[i][j] = 1/(1 + beta*alpha(ts[i],xs[j]))* (2*f(ts[i],xs[j])*tau -(u[i][j-1]-u[i-1][j-1]-u[i-1][j]) - beta*alpha(ts[i],xs[j])*(u[i-1][j]-u[i][j-1]-u[i-1][j-1]));
        }
        if (rank != num_procs - 1) {
            MPI_Send(&u[i][num_xs - 1], 1, MPI_DOUBLE, rank+1, 1, MPI_COMM_WORLD);
        }
    }
    free(ts);
    free(xs);
    return u;
}



int main(int argc, char** argv) {

    int k, n;
    get_size_vals(&k, &n, argc, argv);

    MPI_Init(&argc, &argv);
    int num_procs;
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double t1, t2;
    t1 = MPI_Wtime();
    double T = 1, X = 1;
    double** u;
    u = calculate_part(k, n, T, X, rank, num_procs);
    t2 = MPI_Wtime();
    printf("time: %lf\n", t2 - t1);

    int num_xs, n0;
    calc_params(n, rank, num_procs, &num_xs, &n0);
    if (rank != 0) {
        n0=n0+1;
    }


    if (rank == 0){
        remove("data.csv");
    }

    MPI_File file;
    int size = sizeof(double) + 6;
    char *filename = "data.csv";
    MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL,
                  &file); // открываем файл для записи
    for (int i = 0; i < k; ++i) {
        int numsizebuf = num_xs;
        if (rank != 0){
            numsizebuf -= 1;
        }
        char buf[size * (numsizebuf)];
        for (int j = 0; j < size * num_xs; ++j) {
            buf[j] = ' ';
        }
        int sh;
        for (int j = 0; j < num_xs-1; ++j) {
            if (rank != 0){
                sh = sprintf(buf + j * size, "%.8lf,", u[i][j+1]);
            }
            else{
                sh = sprintf(buf + j * size, "%.8lf,", u[i][j]);
            }
            buf[j * size + sh] = ' ';
        }

        if (rank == (num_procs - 1)) {
            sh = sprintf(buf + (num_xs - 2) * size, "%.8lf", u[i][num_xs - 1]);
            buf[(num_xs - 2) * size + sh] = ' ';
            buf[(num_xs - 1) * size + size-1] = '\n';
        } else {
            sh = sprintf(buf + (num_xs - 1) * size, "%.8lf,", u[i][num_xs - 1]);
            buf[(num_xs - 1) * size + sh] = ' ';
        }

        MPI_Offset offset = i * n * size + n0 * size;
        MPI_File_write_at(file, offset, buf, size * (numsizebuf), MPI_CHAR, MPI_STATUS_IGNORE);
    }

    MPI_File_close(&file); // закрываем файл

    //print_matrix(u, k, num_xs);
    mem_free(u, k);
    MPI_Finalize();
    return 0;
}
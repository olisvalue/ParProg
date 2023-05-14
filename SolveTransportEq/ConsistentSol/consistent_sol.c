#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

double phi(double x0){return cos(M_PI * x0);}
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

void left_corner_solver(double** u, int k, int n){
    double T = 1;
    double X = 1;
    double* ts = linspace(0, T, k);
    double* xs = linspace(0, X, n);
    double tau = ts[1] - ts[0];
    double h = xs[1] - xs[0];
    apply_bord(u, ts, xs, k, n);
    for (int i = 1; i < k; ++i){
        for (int j = 1; j < n; ++j){
            u[i][j] = (f(ts[i],xs[j]) - (u[i][j-1] - u[i-1][j-1])/tau) * h / alpha(ts[i],xs[j]) + u[i][j-1];
        }
    }
    free(ts);
    free(xs);
}

void rectangle_solver(double** u, int k, int n){
    double T = 1;
    double X = 1;
    double* ts = linspace(0, T, k);
    double* xs = linspace(0, X, n);
    double tau = ts[1] - ts[0];
    double h = xs[1] - xs[0];
    double beta = tau/h;
    apply_bord(u, ts, xs, k, n);
    for (int i = 1; i < k; ++i){
        for (int j = 1; j < n; ++j){
            u[i][j] = 1/(1 + beta*alpha(ts[i],xs[j]))* (2*f(ts[i],xs[j])*tau -(u[i][j-1]-u[i-1][j-1]-u[i-1][j]) - beta*alpha(ts[i],xs[j])*(u[i-1][j]-u[i][j-1]-u[i-1][j-1]));
        }
    }
    free(ts);
    free(xs);
}

void print_matrix(double** matrix, int k, int n) {
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%lf ", matrix[i][j]);
        }
        printf("\n");
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

int main(int argc, char** argv){
    remove("data.csv");
    int k, n;
    get_size_vals(&k, &n, argc, argv);

    MPI_Init(&argc, &argv);

    double t1, t2;
    t1 = MPI_Wtime();
    double** u = mem_alloc(k, n);
    rectangle_solver(u, k = k, n = n);
    t2 = MPI_Wtime();
    printf("time: %lf\n", t2 - t1);


    FILE *fp = fopen("data.csv", "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    for(int i=0; i<k; i++){
        for(int j=0; j<n-1; j++){
            fprintf(fp, "%f,", u[i][j]);
        }
        fprintf(fp, "%f", u[i][n-1]);
        fprintf(fp, "\n");
    }

    fclose(fp);
    mem_free(u, k);

    MPI_Finalize();
    return 0;
}
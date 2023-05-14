#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>


#define a 0.01L
#define b 5.0L
typedef long double DOUBLE;
static inline DOUBLE f(DOUBLE x){return sinl(1/x);}

DOUBLE global_sum = 0;
DOUBLE curr_x = a;
DOUBLE max_err;
int n_threads;
pthread_mutex_t mutex, mutex_sum;


DOUBLE calc_sec_deriv(DOUBLE* x, DOUBLE h){
    return (f(*x) - 2*f(*x + h) + f(*x+ h*2))/h/h;
}

void integrate(DOUBLE* local_sum, DOUBLE* left_bord, int* N, DOUBLE* h){
    DOUBLE local = 0;
    for (int i = 0; i <= *N; ++i){
        local += f(*left_bord + i*(*h));
    }
    local -= (f(*left_bord) + f(*left_bord + (*N)*(*h)))/2;
    *local_sum += local*(*h);
}

void* thread_function(void* info) {
    DOUBLE local_sum = 0;
    while (curr_x < b){
        //lock
        pthread_mutex_lock(&mutex);
        if (curr_x < b) {
            //critical section//
            DOUBLE sec_deriv = calc_sec_deriv(&curr_x, 1e-7);
            if (fabsl(sec_deriv) < 1e-5){
                sec_deriv = fabsl(sec_deriv) + 0.01L;
            }
            DOUBLE h = fminl(sqrtl(12.0 * max_err / (b-a)/sqrtl(fabsl(sec_deriv))), 0.1L);
            int N = 1/sqrtl(h);
            DOUBLE left_bord = curr_x;
            if (curr_x + N*h > b){
                h = (b - curr_x)/N;
                curr_x = b;
            }else {
                curr_x += N * h;
            }
            //unlock
            pthread_mutex_unlock(&mutex);
            integrate(&local_sum, &left_bord, &N, &h);
        }
        else {
            //just unlock
            pthread_mutex_unlock(&mutex);
        }
    }

    pthread_mutex_lock(&mutex_sum);
    global_sum += local_sum;
    pthread_mutex_unlock(&mutex_sum);
    return NULL;
}

int main(int argc, char** argv) {

    //args
    n_threads = strtol(argv[1], NULL, 10);
    max_err = strtold(argv[2], NULL);
    //thread allocation
    pthread_t* threads = (pthread_t*)malloc(n_threads * sizeof(pthread_t));

    //start measuring time
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);
    //integration
    for (int i=0; i<n_threads; ++i) {
        pthread_create(&threads[i], NULL, thread_function, NULL);
    }
    for (int i = 0; i < n_threads; ++i) {
        if (pthread_join(threads[i], NULL) < 0) {
            perror("pthread_join(): ");
            exit(-1);
        };
    }
    //end of measuring time
    gettimeofday(&end_time, NULL);
    double elapsed_time = (double)(end_time.tv_sec - start_time.tv_sec) + (double)(end_time.tv_usec - start_time.tv_usec) / 1000000;

    printf("%.10lf\n", elapsed_time);
    printf("Calculated integral: %.20Lf\n", global_sum);

    DOUBLE accurate_ans = 2.035467080916575L;
    printf("Accurate answer: %.20Lf\n", accurate_ans);
    printf("Err: %.20Lf\n", fabsl(global_sum - accurate_ans));
}
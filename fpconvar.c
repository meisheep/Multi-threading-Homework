#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#define NUM_THREADS 3

int TCOUNT, COUNT_LIMIT;
int p = 1;
int q;
pthread_mutex_t p_mutex;
pthread_cond_t p_threshold_cv;

bool isPrime(int num)
{
    int sqrt_num = sqrt(num);
    for(int d = 2; d <= sqrt_num; d++)
        if(num % d == 0)
            return false;
    return true;

}

void *prime_count(void *t)
{
    int my_id = (int) t;
    while(p < TCOUNT) {
        pthread_mutex_lock(&p_mutex);
        p++;
        if (p == COUNT_LIMIT) {
            pthread_cond_signal(&p_threshold_cv);
            printf("prime_count(): thread %d, p = %d prime reached.\n", my_id, p);
            printf("Just sent signal.\n");
        } else {
            printf("prime_count(): thread %d, p = %d\n", my_id, p);
            if(isPrime(p)) {
                q = p;
                printf("prime_count(): thread %d, find prime = %d\n", my_id, p);
            }
        }
        pthread_mutex_unlock(&p_mutex);

        /* Do some work so threads can alternate on mutex lock */
        usleep(500);
    }
    pthread_exit(NULL);
}

void *watch_count(void *t)
{
    int my_id = (int) t;
    printf("Starting watch_count(): thread %d\n", my_id);
    pthread_mutex_lock(&p_mutex);
    if (p < COUNT_LIMIT) {
        printf("watch_count(): thread %d going into wait...\n", my_id);
        pthread_cond_wait(&p_threshold_cv, &p_mutex);
        printf("watch_count(): thread %d Condition signal received. p = %d\n", my_id, p);
        p += q;
        printf("watch_count(): thread %d count now = %d.\n", my_id, p);
    }
    pthread_mutex_unlock(&p_mutex);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int i, rc, t1=1, t2=2, t3=3;

    TCOUNT = atoi(argv[1]);
    COUNT_LIMIT = atoi(argv[2]);

    pthread_t threads[3];
    /* 1. Initialize mutex and condition variable objects */
    pthread_mutex_init(&p_mutex, NULL);
    pthread_cond_init (&p_threshold_cv, NULL);
    pthread_create(&threads[0], NULL, watch_count, (void *)t1);
    pthread_create(&threads[1], NULL, prime_count, (void *)t2);
    pthread_create(&threads[2], NULL, prime_count, (void *)t3);
    /* 2. Wait for all threads to complete */
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("Main(): Waited on %d threads. Final value of count = %d. Done.\n", NUM_THREADS, p);
    /* 3. Clean up and exit */
    pthread_mutex_destroy(&p_mutex);
    pthread_cond_destroy(&p_threshold_cv);
    pthread_exit(NULL);
}
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 3

int count = 0;
pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;
int TCOUNT, COUNT_LIMIT;

void *inc_count(void *t)
{
 int j,i;
 double result=0.0;
 int my_id = (int)t;
 for (i=0; i < TCOUNT; i++) {
 pthread_mutex_lock(&count_mutex);
 count++;
if (count == COUNT_LIMIT) {
 pthread_cond_signal(&count_threshold_cv);
 printf("inc_count(): thread %d, count = %d Threshold reached. ", my_id, count);
 printf("Just sent signal.\n");
 }
 printf("inc_count(): thread %d, count = %d, unlocking mutex\n", my_id, count);
 pthread_mutex_unlock(&count_mutex);
 /* Do some work so threads can alternate on mutex lock */
 sleep(1);
 }
 pthread_exit(NULL);
}

void *watch_count(void *t)
{
 int my_id = (int)t;
 printf("Starting watch_count(): thread %d\n", my_id);
pthread_mutex_lock(&count_mutex);
 if (count < COUNT_LIMIT) {
 printf("watch_count(): thread %d going into wait...\n", my_id);
 pthread_cond_wait(&count_threshold_cv, &count_mutex);
 count += 125;
 printf("watch_count(): thread %d Condition signal received.\n", my_id);
 printf("watch_count(): thread %d count now = %d.\n", my_id, count);
 }
 pthread_mutex_unlock(&count_mutex);
 pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
 int i, rc, t1=1, t2=2, t3=3;
 TCOUNT = atoi(argv[1]);
 COUNT_LIMIT = atoi(argv[2]);

 pthread_t threads[3];
 /*1. Initialize mutex and condition variable objects */
 pthread_mutex_init(&count_mutex, NULL);
 pthread_cond_init (&count_threshold_cv, NULL);
 pthread_create(&threads[0], NULL, watch_count, (void *)t1);
 pthread_create(&threads[1], NULL, inc_count, (void *)t2);
 pthread_create(&threads[2], NULL, inc_count, (void *)t3);
 /* 2. Wait for all threads to complete */
 for (i = 0; i < NUM_THREADS; i++) {
 pthread_join(threads[i], NULL);
 }
 printf ("Main(): Waited on %d threads. Final value of count = %d. Done.\n",
 NUM_THREADS, count);
 /* 3. Clean up and exit */
 pthread_mutex_destroy(&count_mutex);
 pthread_cond_destroy(&count_threshold_cv);
 pthread_exit(NULL);
}
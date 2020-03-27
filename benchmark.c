#include <stdio.h>
#include <time.h>
#include <sys/time.h>


/* benchmark tools */
struct timeval start_time;
struct timeval end_time;

void start_timer(FILE *file){
    gettimeofday(&start_time, NULL);
    fprintf(file, "%d\n", start_time.tv_usec);
}

void finish_timer(FILE *file){
    gettimeofday(&end_time, NULL);
    fprintf(file, "%d\n", end_time.tv_usec);
}

/* benchmark tools */
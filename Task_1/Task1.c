#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "my_rand.h"
#include "timer.h"

int random_count;
int thread_count;
int* thread_results;
double radius = 0.5;
void* routine(void* rank){
    int hit_count=0;
    double x;
    double y;
    unsigned my_rank = (unsigned)rank+1;
    int address =(int)rank;
    for(int i=0;i<random_count/thread_count;i++){
        x = my_drand(&my_rank);
        y = my_drand(&my_rank);
        if(((x-radius)*(x-radius)+(y-radius)*(y-radius))<=radius*radius){
            hit_count+=1;
        }
    }
    thread_results[address]=hit_count;
    return NULL;
}

double Multiple(){
    double start,end;
    int total_hits = 0;
    thread_results = malloc(thread_count*sizeof(int));
    pthread_t* thread_handles = malloc(thread_count*sizeof(pthread_t));
    GET_TIME(start);
    for(int i=0;i<thread_count;i++){
        int err = pthread_create(&thread_handles[i],NULL,routine, (void*)i);
        if(err!=0){
            perror("Error");
        }
    }
    for(int i=0;i<thread_count;i++){
        pthread_join(thread_handles[i],NULL);
    }
    for(int i=0;i<thread_count;i++){
        total_hits+=thread_results[i];
    }
    GET_TIME(end);
    printf("%lf\n", 4*((double)total_hits)/(random_count));
    free(thread_handles);
    free(thread_results);
    return end-start;
}

double Single(){
    double start,end;

    GET_TIME(start);
    int total_hits = 0;
    double x;
    double y;

    unsigned my_rank = (unsigned)1;
    for(int j=0;j<random_count;j++){
        x = my_drand(&my_rank);
        y = my_drand(&my_rank);
        if(((x-radius)*(x-radius)+(y-radius)*(y-radius))<=radius*radius){
            total_hits+=1;
        }
    }
    GET_TIME(end);
    printf("%lf\n", 4*((double)total_hits)/(random_count));
    return end-start;
}

int main(int argc, char** argv){
    thread_count = strtol(argv[1], NULL, 10);
    random_count = strtol(argv[2], NULL, 10);

    double single_time=0;
    double multiple_time=0;

    Multiple();
    //Single();
    /*
    for(int i=0;i<1000;i++){
        multiple_time+=Multiple();
    }
    multiple_time/=1000;*/
    //printf("Median multiple time = %e seconds\n", multiple_time);
    /*
    for(int i=0;i<1000;i++){
        single_time+=Single();
    }
    single_time/=1000;*/
    //printf("Median single time = %e seconds\n", single_time);
    //printf("S=%f E=%f\n", single_time/multiple_time, (single_time/multiple_time)/thread_count);
    return 0;
}
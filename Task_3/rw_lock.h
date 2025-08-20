#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;

    pthread_cond_t rd_cond;

    pthread_cond_t wr_cond;

    int rd_count;

    int rd_wait;

    int wr_wait;

    int wr;
} rwlock_t;

void rwlock_init(rwlock_t* lock){
    pthread_mutex_init(&lock->mutex, NULL);
    pthread_cond_init(&lock->rd_cond,NULL);
    pthread_cond_init(&lock->wr_cond,NULL);
    lock->rd_count = 0;
    lock->wr_wait = 0;
    lock->rd_wait = 0;
    lock->wr=0;
}

void rwlock_destroy(rwlock_t* lock){
    pthread_mutex_destroy(&lock->mutex);
    pthread_cond_destroy(&lock->rd_cond);
    pthread_cond_destroy(&lock->wr_cond);
}

void rwlock_rdlock(rwlock_t* lock){
    pthread_mutex_lock(&lock->mutex);
    lock->rd_wait++;
    while(lock->wr){
        pthread_cond_wait(&lock->rd_cond,&lock->mutex);
    }
    lock->rd_wait--;
    lock->rd_count++;
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_wrlock(rwlock_t* lock){
    pthread_mutex_lock(&lock->mutex);
    lock->wr_wait++;
    while(lock->wr || lock->rd_count>0){
        pthread_cond_wait(&lock->wr_cond,&lock->mutex);
    }
    lock->wr_wait--;
    lock->wr = 1;
    pthread_mutex_unlock(&lock->mutex);
}

void rwlock_unlock(rwlock_t* lock){
    pthread_mutex_lock(&lock->mutex);
    if(lock->wr){
        lock->wr=0;
        if(lock->wr_wait>0){
            pthread_cond_signal(&lock->wr_cond);
        }
        else{
            pthread_cond_broadcast(&lock->rd_cond);
        }
    }
    else{
        lock->rd_count--;
        if(lock->wr_wait>0){
            pthread_cond_signal(&lock->wr_cond);
        }
        else{
            pthread_cond_broadcast(&lock->rd_cond);
        }
    }
    pthread_mutex_unlock(&lock->mutex);
}
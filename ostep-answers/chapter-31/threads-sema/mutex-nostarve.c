#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//

// I don't think there is any solid proof of the lack of deadlocking in the test I wrote
// The explanation is as follows:

/*
The two most important mutexes to understand are the first and second turnstiles.
The mutex is just used to procted the first_count. First, a thread enters aquire 
and updates the first rom count. If it manages to reaquire the mutex before another 
thread aquires it and increments the count, than the first count will become 0. 

Anytime the first count becomes 0, we unlock the second turnstile. We continue 
spinning the second turnstile until the second room is empty, and then we lock 
it and reunlock the first turnstile. If the first room is not empty when a thread 
moves a value to the second room, we don't unlock the second turnstile and we keep
letting threads into the first room.

Because there are a finite number of threads, we must eventually reach a state 
where no threads are trying to get into the first room, as they will all be stuck 
in the first room until nothing interrupts a threads entrance into the first room.
*/

#define NUM_THREADS 8

size_t global_count = 0;

typedef struct __ns_mutex_t {
    sem_t mutex;
    size_t first_count;
    size_t second_count;
    sem_t first_turnstile;
    sem_t second_turnstile;
} ns_mutex_t;

ns_mutex_t* lock;

void ns_mutex_init(ns_mutex_t *m) {
    sem_init(&m->first_turnstile, 0, 1);
    sem_init(&m->second_turnstile, 0, 0);
    sem_init(&m->mutex, 0, 1);
    m->first_count = 0;
    m->second_count = 0;
}

void ns_mutex_acquire(ns_mutex_t *m) {
    sem_wait(&m->mutex);
    m->first_count++;
    sem_post(&m->mutex);

    sem_wait(&m->first_turnstile);
    sem_wait(&m->mutex);
    m->second_count++;
    m->first_count--;

    if (m->first_count == 0){
        // no other threads made a request
        sem_post(&m->mutex);
        sem_post(&m->second_turnstile);
    }

    else {
        // keep processing threads into the first room
        sem_post(&m->mutex);
        sem_post(&m->first_turnstile);

    }

    sem_wait(&m->second_turnstile);
    m->second_count--;
}

void ns_mutex_release(ns_mutex_t *m) {
    if (m->second_count != 0){
        sem_post(&m->second_turnstile);
    }
    else {
        sem_post(&m->first_turnstile);
    }
}

void *worker(void *arg) {
    for(size_t i = 0; i < 4; i++){
        ns_mutex_acquire(lock);
        global_count++;
        printf("Global Count: %zu\n", global_count);
        ns_mutex_release(lock);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    
    lock = malloc(sizeof(ns_mutex_t));
    assert(lock != NULL);

    ns_mutex_init(lock);

    printf("begin\n");

    for (size_t i = 0; i < NUM_THREADS; i++){
	    Pthread_create(&threads[i], NULL, worker, NULL);
    }

    for (size_t i = 0; i < NUM_THREADS; i++){ 
	    Pthread_join(threads[i], NULL);
    }

    printf("end\n");

    return 0;
}



#include "pool.h"
#include <pthread.h>
#include <stdlib.h>

#define MAX_TASKS 200

typedef struct {
    task_fn function;
    void *argument;
    void *result;
    int completed;
} Task;

Task tasks[MAX_TASKS];

int task_count = 0;
int next_task = 0;
int first_incomplete_task = 0;
int stopping = 0;

int thread_count = 0;
pthread_t *threads;

pthread_mutex_t lock;
pthread_cond_t task_available;
pthread_cond_t task_completed;

static void *thread_run_tasks(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock);

        // sleep until a task becomes available or pool stops
        while (next_task >= task_count && !stopping) {
            pthread_cond_wait(&task_available, &lock);
        }

        // exit when stopping and all tasks claimed
        if (next_task >= task_count && stopping) {
            pthread_mutex_unlock(&lock);
            return NULL;
        }

        int id = next_task;
        next_task += 1;

        task_fn function = tasks[id].function;
        void *argument = tasks[id].argument;

        //unlock to perform function
        pthread_mutex_unlock(&lock);
        void *result = function(argument);
        pthread_mutex_lock(&lock);

        tasks[id].result = result;
        tasks[id].completed = 1;

        //iterate consecutive completion counter
        while (first_incomplete_task < task_count && tasks[first_incomplete_task].completed) {
            first_incomplete_task += 1;
        }

        pthread_cond_broadcast(&task_completed);

        pthread_mutex_unlock(&lock);
    }
}

void pool_setup(int threads_count) {
    pthread_mutex_lock(&lock);

    task_count = 0;
    next_task = 0;
    first_incomplete_task = 0;
    stopping = 0;
    thread_count = threads_count;

    threads = malloc(sizeof(pthread_t) * threads_count);

    pthread_mutex_unlock(&lock);

    for (int i = 0; i < threads_count; i += 1) {
        pthread_create(&threads[i], NULL, thread_run_tasks, NULL);
    }
}

int pool_submit_task(task_fn task, void *argument) {
    pthread_mutex_lock(&lock);

    //initialize task and signal availability
    int id = task_count;
    tasks[id] = (Task){task, argument, NULL, 0};
    task_count++;
    pthread_cond_signal(&task_available);

    pthread_mutex_unlock(&lock);

    return id;
}

void *pool_get_task_result(int task_id) {
    pthread_mutex_lock(&lock);

    void *result = NULL;

    //check task id bounds
    if (task_id >= 0 && task_id < task_count && tasks[task_id].completed) {
        result = tasks[task_id].result;
    }

    pthread_mutex_unlock(&lock);

    return result;
}

void pool_wait(void) {
    pthread_mutex_lock(&lock);
    int target = task_count;

    while (first_incomplete_task < target) {
        pthread_cond_wait(&task_completed, &lock);
    }

    pthread_mutex_unlock(&lock);
}

void pool_stop(void) {
    pthread_mutex_lock(&lock);

    // tell all threads to exit
    stopping = 1;
    pthread_cond_broadcast(&task_available);

    pthread_mutex_unlock(&lock);

    for (int i = 0; i < thread_count; i += 1) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    thread_count = 0;
}

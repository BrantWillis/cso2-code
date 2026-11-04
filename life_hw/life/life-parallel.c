#include "life.h"
#include <pthread.h>

pthread_barrier_t barrier;

typedef struct {
    LifeBoard *state;
    LifeBoard *next_state;
    int v_start;
    int v_stop;
    int steps;
    int first;
} ThreadArg;

void *simulate_partition(void *args) {
    ThreadArg *thread_args = (ThreadArg *)args;

    LifeBoard *state = thread_args->state;
    LifeBoard *next_state = thread_args->next_state;

    for (int step = 0; step < thread_args->steps; step += 1) {

        /* We use the range [1, width - 1) here instead of
         * [0, width) because we fix the edges to be all 0s.
         */
        for (int y = thread_args->v_start; y < thread_args->v_stop; y += 1) {
            for (int x = 1; x < state->width - 1; x += 1) {
                
                /* For each cell, examine a 3x3 "window" of cells around it,
                 * and count the number of live (true) cells in the window. */
                int live_in_window = 0;
                for (int y_offset = -1; y_offset <= 1; y_offset += 1)
                    for (int x_offset = -1; x_offset <= 1; x_offset += 1)
                        if (LB_get(state, x + x_offset, y + y_offset))
                            live_in_window += 1;
                
                /* Cells with 3 live neighbors remain or become live.
                   Live cells with 2 live neighbors remain live. */
                LB_set(next_state, x, y,
                    live_in_window == 3 /* dead cell with 3 neighbors or live cell with 2 */ ||
                    (live_in_window == 4 && LB_get(state, x, y)) /* live cell with 3 neighbors */
                );
            }
        }
        
        //first thread designated to do board swap
        pthread_barrier_wait(&barrier);
        if (thread_args->first == 1) {
            LB_swap(state, next_state);
        }
        pthread_barrier_wait(&barrier);
    }

    void *ans = NULL;
    return ans;
}

void simulate_life_parallel(int threads, LifeBoard *state, int steps) {
    LifeBoard *next_state = LB_new(state->width, state->height);
    pthread_barrier_init(&barrier, NULL, threads);

    pthread_t id[threads];
    ThreadArg tasks[threads];

    //each thread computes an amount of board rows equal to (rows - 2)/threads
    for(int i=0; i<threads; i+=1) {
        ThreadArg arg = {
            .next_state = next_state,
            .state = state,
            .v_start = 1 + i * (state->height - 2) / threads,
            .v_stop = 1 + (i + 1) * (state->height - 2) / threads,
            .steps = steps,
            .first = !i //i=0 is the first thread (will perform swaps)
        };
        tasks[i] = arg;
        pthread_create(&id[i], NULL, simulate_partition, &tasks[i]);
    }

    for(int i=0; i<threads; i+=1) {
        void *ans;
        pthread_join(id[i], &ans);
    }
    LB_del(next_state);
    pthread_barrier_destroy(&barrier);
}
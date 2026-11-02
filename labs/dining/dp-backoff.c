#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

pthread_barrier_t barrier; // optional: to hopefully make deadlock more consistent

pthread_t philosopher[5];
pthread_mutex_t chopstick[5];

int give_rand(int seed) {
    //for i = 0, return from  50-100
    //for i = 1, return from 100-200
    //for i = 2, return from 200-400
    //for i = 3, return from 400-800
    //formula: low is (2 ^ i) * 50
    //        high is (2 ^ i) * 100
    int low = pow(2, seed) * 50;
    int high = low * 2;
    return (rand() % (high - low + 1)) + low;
}

void wait_rand(int seed) {
    usleep(give_rand(seed));
}

void *pickUpAndEat(void *arg) {
    int n = (int) (long)arg;

    // optional: sync up threads to make deadlock hopefully happen more consistently
    pthread_barrier_wait(&barrier);
    for(int i = 0;;i+=1) {
        // take two chopsticks
        pthread_mutex_lock(&chopstick[n]);
        printf("Philosopher %d got chopstick %d\n", n, n);
        if (pthread_mutex_trylock(&chopstick[(n+1)%5]) != 0) {
            printf("Philosopher %d failed to get chopstick %d\n", n, (n+1)%5);
            printf("Philosopher %d set down chopstick %d\n", n, n);
            pthread_mutex_unlock(&chopstick[n]);
            wait_rand(i);
            continue;
        }
        else {
            printf("Philosopher %d got chopstick %d\n", n, (n+1)%5);
            break;
        }
    }
    
    printf ("Philosopher %d is eating\n",n);
    sleep(1);
    
    // set them back down
    printf("Philosopher %d set down chopstick %d\n", n, (n+1)%5);
    pthread_mutex_unlock(&chopstick[(n+1)%5]);
    printf("Philosopher %d set down chopstick %d\n", n, n);
    pthread_mutex_unlock(&chopstick[n]);
    return NULL;
}

int main(int argc, const char *argv[]) {
    srand(time(NULL));

    pthread_barrier_init(&barrier, NULL, 5);

    for(int i = 0; i < 5; i += 1)
        pthread_mutex_init(&chopstick[i], NULL);

    for(int i =0; i < 5; i += 1)
        pthread_create(&philosopher[i], NULL, pickUpAndEat, (void *)(size_t)i);

    for(int i=0; i < 5; i += 1)
        pthread_join(philosopher[i], NULL);

    for(int i=0; i < 5; i += 1)
        pthread_mutex_destroy(&chopstick[i]);

    pthread_barrier_destroy(&barrier);

    return 0;
}
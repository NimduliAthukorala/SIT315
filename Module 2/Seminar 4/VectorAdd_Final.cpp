#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <pthread.h>

using namespace std::chrono;
using namespace std;

#define NUM_THREADS 10

struct ADD
{
    int *v1, *v2, *v3;
    int start;
    int end;
};

struct Random
{
    int *v;
    int start;
    int end;
};

void *addVector(void *args)
{
    ADD *create_add = ((struct ADD *)args);
    for (int i=create_add->start; i<create_add->end; i++) {
        create_add->v3[i] = create_add->v1[i] + create_add->v2[i];
    }
    return NULL;
}

void *randomVector(void *args)
{
    Random *create_random = ((struct Random *)args);

    for (int i=create_random->start; i<create_random->end; i++) {
        create_random->v[i] = rand() % 100;
    }
    return NULL;
}

int main(){

    unsigned long size = 100000000;

    srand(time(0));

    int *v1, *v2, *v3;

    pthread_t random_t[NUM_THREADS];
    pthread_t add_t[NUM_THREADS];

    //ToDo: Allocate a block of dynamic uninitialized memory for each vector
    v1 = (int *) malloc(size * sizeof(int *));
    v2 = (int *) malloc(size * sizeof(int *));
    v3 = (int *) malloc(size * sizeof(int *));

    int partition = size/(NUM_THREADS/2);

    // Randomly generate first vector
    for (size_t i=0; i<NUM_THREADS/2; i++)
    {
        struct Random *create_random = (struct Random *)malloc(sizeof(struct Random));

        create_random -> v = v1;

        create_random -> start = i*partition;

        create_random -> end = (i+1) == (NUM_THREADS/2)?size:((i+1)*partition);

        pthread_create(&random_t[i], NULL, randomVector, (void *)create_random);
    }

    // Randomly generate second vector
    for (size_t i=0; i<NUM_THREADS/2; i++)
    {
        struct Random *create_random = (struct Random *)malloc(sizeof(struct Random));

        create_random -> v = v2;

        create_random -> start = i*partition;

        create_random -> end = (i+1) == (NUM_THREADS/2)?size:((i+1)*partition);

        pthread_create(&random_t[i+NUM_THREADS/2], NULL, randomVector, (void *)create_random);
    }

    // Join random threads
    for (size_t i=0; i<NUM_THREADS; i++)
    {
        pthread_join(random_t[i], NULL);
    }

    //ToDo: Allocate the current time to start indicating the start time for duration
    auto start = high_resolution_clock::now();

    partition = size/NUM_THREADS;

    for (size_t i=0; i<NUM_THREADS; i++)
    {
        struct ADD *create_add = (struct ADD *)malloc(sizeof(struct ADD));

        create_add -> v1 = v1;

        create_add -> v2 = v2;

        create_add -> v3 = v3;

        create_add -> start = i*partition;  

        create_add -> end = (i+1) == NUM_THREADS?size:((i+1)*partition); 

        pthread_create(&add_t[i], NULL, addVector, (void *)create_add);
    }

    for (size_t i=0; i<NUM_THREADS; i++)
    {
        pthread_join(add_t[i], NULL);
    }

    //stop
    auto stop = high_resolution_clock::now();

    //ToDo: Allocate the difference in time between stop and start to duration
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}
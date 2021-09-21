#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <omp.h>


using namespace std::chrono;
using namespace std;


int main(){

    unsigned long size = 100000;

    int NUM_THREADS = 4;

    int atomic_total = 0;
    int reduction_total = 0;
    int private_total = 0;
    int critical_total = 0;

    srand(time(0));

    int *v1, *v2, *v3;

    omp_set_num_threads(NUM_THREADS);
    

    //ToDo: Add Comment
    v1 = (int *) malloc(size * sizeof(int *));
    v2 = (int *) malloc(size * sizeof(int *));
    v3 = (int *) malloc(size * sizeof(int *));


    //ToDo: Allocate the current time to start indicating the start time for duration
    auto start = high_resolution_clock::now();

    // Activity 1 
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < size; i++)
        {
            v1[i] = rand() % 100;
        }
        #pragma omp for
        for (int i = 0; i < size; i++)
        {
            v2[i] = rand() % 100;
        }

        #pragma omp for
        for (int i = 0; i < size; i++)
        {
            v3[i] = v1[i] + v2[i];
        }
    }

    auto stop = high_resolution_clock::now();


    
    // Question 1 - Add default(none) attribute
    #pragma omp parallel default(none) shared(size, v1, v2, v3)
    {
        #pragma omp for
        for (size_t i = 0; i < size; i++)
        {
            v3[i] = v1[i] + v2[i];
        }
    }

      
    // Question 2 - Compute total sum with atomic update directive
    #pragma omp parallel default(none) shared(v3, atomic_total, size)
    {
        #pragma omp for
        for (int i = 0; i < size; i++)
        {
            #pragma omp atomic
            atomic_total += v3[i];
        }
    }


    // Question 3 - Compute total sum with the reduction clause
    #pragma omp parallel default(none) shared(v3, size) reduction(+:reduction_total)
    {
        #pragma omp for
        for (int i = 0; i < size; i++)
        {
            reduction_total += v3[i];
        }
    }


    // Question 4 - Each thread computes its own part to a private variable and then use a critical section to calculate total sum
    #pragma omp parallel default(none) shared(critical_total, v3, size) firstprivate(private_total)
    {
        #pragma omp for
        for (int i = 0; i < size; i++)
        {
            private_total += v3[i];
        }

        #pragma omp critical
        {
            critical_total += private_total;
        }
    }


    // Question 5 - Add default(none) attribute
    #pragma omp parallel default(none) shared(size, v1, v2, v3)
    {
        #pragma omp for
        for (size_t i = 0; i < size; i++)
        {
            v3[i] = v1[i] + v2[i];
        }
    }


    
    // Question 6 - Scheduling techniques

    // #pragma omp for schedule(static, 4)
    // #pragma omp for schedule(static, 8)
    // #pragma omp for schedule(dynamic, 4)
    // #pragma omp for schedule(dynamic, 8)
    // #pragma omp for schedule(guided, 4)
    // #pragma omp for schedule(guided, 8)

    #pragma omp parallel
    {
        #pragma omp for schedule(dynamic, 8)
        for (int i = 0; i < size; i++)
        {
            v1[i] = rand() % 100;
        }
        #pragma omp for schedule(dynamic, 8)
        for (int i = 0; i < size; i++)
        {
            v2[i] = rand() % 100;
        }

        #pragma omp for schedule(dynamic, 8)
        for (int i = 0; i < size; i++)
        {
            v3[i] = v1[i] + v2[i];
        }
    }

    // Display the vector
    for (size_t i = 0; i < size; i++)
    {
        cout << v3[i] << "  ";
    }

    cout << endl;

    // Display the totals
    cout << "Atomic Total: " << atomic_total << endl;
    cout << "Reduction Total: " << reduction_total << endl;
    cout << "Critical Total: " << critical_total << endl;

    //ToDo: Allocate the difference in time between stop and start to duration
    auto duration = duration_cast<microseconds>(stop - start);


    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace std::chrono;


// Function that swaps two items in a vector
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}


// Function returns the partition index
int partition(int v[], int low, int high)
{
    int pivot = v[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (v[j] <= pivot)
        {
            i++;
            swap(&v[i], &v[j]);
        }
    }
    swap(&v[i + 1], &v[high]);
    return (i + 1);
}


// Quick Sort Function
int* quickSort(int v[], int low, int high)
{
    if (low < high)
    {
        int partition_index = partition(v, low, high);
        
        #pragma omp task firstprivate(v, low, high, partition_index)
        {
            quickSort(v, low, partition_index - 1);
        }

        {
            quickSort(v, partition_index + 1, high);
        }
    }

    return 0;
}


// Function to display the current status of the array
void printArray(int print_vector[], unsigned long size)
{
    for (int i = 0; i < size; i++)
        printf("%d ", print_vector[i]);
    printf("\n");
}


int main()
{
    unsigned long size = 1000;
    int num_threads = 4;

    srand(time(0));

    int *vector;
    vector = (int*)malloc(size * sizeof(int*));

    omp_set_num_threads(num_threads);

    // Generate random number between 0 to 99 and assign it to the vector location
    for (int i = 0; i < size; i++)
    {
            vector[i] = rand() % 100;
    }

    // Print random vector
    //printArray(vector,size);

    // Allocate the current time to start indicating the start time for duration
    auto start = high_resolution_clock::now();

    #pragma omp parallel
    {
        #pragma omp single nowait
        {
            quickSort(vector, 0, size - 1);
        }
    }

    auto stop = high_resolution_clock::now();

    // Print sorted vector
    //printArray(vector,size);

    // Allocate the difference in time between stop and start to duration
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;
}

#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>


using namespace std::chrono;
using namespace std;


// Function that swaps two items in a vector
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}


// Function returns the partition index
int partition (int v[], int low, int high)
{
    int pivot = v[high];  // initialize the pivot
    int i = (low - 1);
 
    for (int j = low; j <= high- 1; j++)
    {
        // checks if the element at the given position is less than or equal to the pivot
        if (v[j] <= pivot)
        {
            i++; // if so the index of the smaller element is increased
            swap(&v[i], &v[j]); // elements are then swaped using swap function
        }
    }
    swap(&v[i + 1], &v[high]);
    return (i + 1);
}


// Quick Sort Function
int* quickSort(int v[], int low, int high)
{
    if(low < high)
    {
        // Call partition function to obtain partition index
        int partition_index = partition(v, low, high);

        // recursively sorts the vector
        quickSort(v, low, partition_index - 1);
        quickSort(v, partition_index + 1, high);
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


// Main Function
int main()
{
    unsigned long size = 10000000;

    srand(time(0));

    int *vector;
    vector = (int*)malloc(size * sizeof(int*));

    // Generate random number between 0 to 99 and assign it to the vector location
    for (int i = 0; i < size; i++)
    {
            vector[i] = rand() % 100;
    }

    // Print random vector
    //printArray(vector,size);

    // Allocate the current time to start indicating the start time for duration
    auto start = high_resolution_clock::now();

    quickSort(vector, 0, size-1);

    auto stop = high_resolution_clock::now();

    // Print sorted vector
    //printArray(vector,size);

    // Allocate the difference in time between stop and start to duration
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;
}
#include <mpi.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>


using namespace std::chrono;
using namespace std;

void randomVector(int vector[], int size)
{
    for (int i = 0; i < size; i++)
    {
        //ToDo: Generate random number between 0 to 99 and assign it to the vector location
        vector[i] = rand() % 100;
    }
}


int main(int argc, char** argv){

    unsigned long size = 100000000;

    srand(time(0));

    int *v1, *v2, *v3, *v3_sub, *v1_sub, *v2_sub, totalSum;

    int numtasks, rank, res, name_len, tag=1;

    char name[MPI_MAX_PROCESSOR_NAME];

    MPI_Status status;
    // Initialize the MPI environment
    MPI_Init(&argc,&argv);
    // Get the number of tasks/process
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    // Get the rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Find the processor name
    MPI_Get_processor_name(name, &name_len);

    int length = size/numtasks;

    int localSum = 0;

    v1_sub = (int *) malloc(length * sizeof(int *));
    v2_sub = (int *) malloc(length * sizeof(int *));
    v3_sub = (int *) malloc(length * sizeof(int *));
    
    if(rank == 0){

        v1 = (int *) malloc(size * sizeof(int *));
        v2 = (int *) malloc(size * sizeof(int *));
        v3 = (int *) malloc(size * sizeof(int *));

        randomVector(v1, size);
        randomVector(v2, size);
    }

    //ToDo: Allocate the current time to start indicating the start time for duration
    auto start = high_resolution_clock::now();

    MPI_Scatter(v1, length, MPI_INT, v1_sub,length, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Scatter(v2, length, MPI_INT, v2_sub,length, MPI_INT, 0, MPI_COMM_WORLD);

    for (size_t i = 0; i < length; i++)
    {
        v3_sub[i] += v1_sub[i] + v2_sub[i];
        localSum += v3_sub[i];
    }

    MPI_Gather(v3_sub, length, MPI_INT, v3, length, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Reduce(&localSum, &totalSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        
    if(rank == 0){
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by function: " << duration.count() << " microseconds"  <<" and the total sum is " << totalSum  
         <<endl;
    }

    // Finalize the MPI environment
    MPI_Finalize();
}
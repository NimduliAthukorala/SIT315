// import Libraries
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <pthread.h>
#include <fstream>

using namespace std::chrono;
using namespace std;

// set the number of rows and columns for the matrix
#define N 10
// set the maximum random value for matrix A and B
#define max_value 10
// set the maximum number of threads
#define max_threads 4


struct MultiTask
{
    int ** A;
    int ** B;
    int ** C;
    int start;
    int end;
};

struct RandomTask
{
    int ** m;
    int seed_factor;
    int start;
    int end;
};


void* random_matrix(void *args)
{
    RandomTask *task =((struct RandomTask *)args);

    srand(time(NULL) * task->seed_factor);

    for(int i = task->start; i < task->end; i++)
        for (int j = 0; j < N; ++j)
        {
            task->m[i][j] = rand() % max_value;
        }
    return 0;
}


// function to print a given matrix by accepting the name of the matrix
void print_matrix(int **matrix) {

    cout << "Displaying Matrix: " << endl;

    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
     }       
     cout << endl;
}


// function to carry out the calculation of matrix A multiplied by matrix B
void* multiply(void* args)
{
    MultiTask *task =((struct MultiTask *)args);

    for(int i = task->start; i < task->end; i++)
    { 
        for (int j = 0; j < N; j++)
		{
			task->C[i][j] = 0;
            for (int k = 0; k < N; k++)
			{
				task->C[i][j] += task->A[i][k] * task->B[k][j];
			}
		}
	}
    return 0;
}



// main code
int main()
{
    int seed_factor = 16;

    int ** A,** B,** C;  

    A = (int**) malloc(N*sizeof(int*));  
    for (int i = 0; i < N; i++) {
        A[i] = (int*) malloc(N*sizeof(int)); 
    } 

    B = (int**) malloc(N*sizeof(int*));  
    for (int i = 0; i < N; i++) {
        B[i] = (int*) malloc(N*sizeof(int)); 
    }
    
    C = (int**) malloc(N*sizeof(int*));  
    for (int i = 0; i < N; i++) {
        C[i] = (int*) malloc(N*sizeof(int));
    } 

    pthread_t thread_multiply[max_threads];
    pthread_t thread_random[max_threads];

    int partition_size = N / (max_threads / 2);

    for(size_t i = 0; i < max_threads / 2; i++)
    {
        struct RandomTask *task = (struct RandomTask *)malloc(sizeof(struct RandomTask));
        task->seed_factor = seed_factor++;
        task->m=A;
        task->start = i * partition_size;
        task->end = (i+1) == (max_threads / 2) ? N :((i+1) * partition_size);
        pthread_create(&thread_random[i], NULL, random_matrix, (void*)task);
    }

    for(size_t i = 0; i < max_threads / 2; i++)
    {
        struct RandomTask *task = (struct RandomTask *)malloc(sizeof(struct RandomTask));
        task->seed_factor = seed_factor++;
        task->m=B;
        task->start = i * partition_size;
        task->end = (i+1) == (max_threads / 2) ? N :((i+1) * partition_size);
        pthread_create(&thread_random[i], NULL, random_matrix, (void*)task);
    }

    for(size_t i = 0; i < max_threads; i++) {
        pthread_join(thread_random[i], NULL);
    }

    print_matrix(A);
    print_matrix(B);

    //Clock to track runtime
    auto start = high_resolution_clock::now();

    partition_size = N / max_threads;

    for (int i = 0; i < max_threads; i++)
    {
        struct MultiTask *task = (struct MultiTask *)malloc(sizeof(struct MultiTask));
        task->A=A;
        task->B=B;
        task->C=C;

        task->start = i * partition_size; 
        task->end = (i+1) == max_threads ? N :((i+1) * partition_size);
        pthread_create(&thread_multiply[i], NULL, multiply, (void *)task);
    }

    for (int i = 0; i < max_threads; i++)
    {
        pthread_join(thread_multiply[i], NULL);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    
    // call print_matrix function for matrix C
    print_matrix(C);

    // create and open txt file and write Matrix C to it
    ofstream MyFile("PMM_Output.txt");
    MyFile << "This is Matrix C : \n";
    for (int i=0; i<N; i++) {

        for (int j=0; j<N; j++) {
            MyFile << C[i][j] << " ";
        }
        MyFile << "\n";
     }
    MyFile.close();

    cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
 
    return 0;
}
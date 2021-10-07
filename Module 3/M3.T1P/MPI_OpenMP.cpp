#include <iostream>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <mpi.h>
#include <omp.h>

using namespace std::chrono;
using namespace std;

#define THREADS 4

int N = 1000;
int **A, **B, **C;


void init(int** &matrix, int rows, int cols, bool initialise);
void print( int** matrix, int rows, int cols);
void multiply(int **A, int **B, int **C, int rows);
void head(int num_processes);
void node(int process_rank, int num_processes);


int main(int argc, char** argv) 
{
    omp_set_num_threads(THREADS);

    MPI_Init(NULL, NULL);

    int num_processes;
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    int process_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

    if(process_rank == 0) 
        head(num_processes);
    else
        node(process_rank, num_processes);
    
    MPI_Finalize();
}


void head(int num_processes)
{
    init(A, N, N, true), init(B, N, N, true), init(C, N, N, false);

    //print(A, N, N);
    //print(B, N, N);

    int num_rows_per_process_from_A = N / num_processes;
    int num_elements_to_bcast = (N * N);
    int num_elements_to_scatter_or_gather = (N * N) / num_processes;

    auto start = high_resolution_clock::now();

    MPI_Scatter(&A[0][0], num_elements_to_scatter_or_gather ,  MPI_INT , &A , 0, MPI_INT, 00 , MPI_COMM_WORLD);
    MPI_Bcast(&B[0][0], num_elements_to_bcast , MPI_INT , 0 , MPI_COMM_WORLD);

    #pragma omp parallel default(none) shared(A, B, C, num_rows_per_process_from_A)
    {
        multiply(A, B, C, num_rows_per_process_from_A);
    }

    MPI_Gather(MPI_IN_PLACE, num_elements_to_scatter_or_gather , MPI_INT, &C[0][0] , num_elements_to_scatter_or_gather , MPI_INT, 0 , MPI_COMM_WORLD);

    //print(C, N, N);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Time taken by function: " << duration.count() << " milliseconds" << endl;
}

void node(int process_rank, int num_processes)
{
    int num_rows_per_process_from_A = N / num_processes;
    int num_elements_to_bcast = (N * N);
    int num_elements_to_scatter_or_gather = (N * N) / num_processes;

    init(A, num_rows_per_process_from_A, N, true), init(B, N, N, false), init(C, num_rows_per_process_from_A, N, false);

    MPI_Scatter(NULL, num_elements_to_scatter_or_gather , MPI_INT , &A[0][0], num_elements_to_scatter_or_gather, MPI_INT, 0 , MPI_COMM_WORLD);
    MPI_Bcast(&B[0][0], num_elements_to_bcast , MPI_INT , 0 , MPI_COMM_WORLD);
    
    #pragma omp parallel default(none) shared(A, B, C, num_rows_per_process_from_A)
    {
        multiply(A, B, C, num_rows_per_process_from_A);
    }

    MPI_Gather(&C[0][0], num_elements_to_scatter_or_gather , MPI_INT, NULL, num_elements_to_scatter_or_gather , MPI_INT, 0 , MPI_COMM_WORLD);
}

void init(int** &matrix, int rows, int cols, bool initialise) {
    matrix = (int **) malloc(sizeof(int*) * rows * cols);  
    int* tmp = (int *) malloc(sizeof(int) * cols * rows); 

    for(int i = 0 ; i < N ; i++) {
        matrix[i] = &tmp[i * cols];
    }
  
    if(!initialise) return;

    for(long i = 0 ; i < rows; i++) {
        for(long j = 0 ; j < cols; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}

void multiply(int** m1, int** m2, int** m3, int rows)
{
    #pragma omp for
    for(int i = 0; i < rows ; i++) 
    {
        for(int j = 0; j < N; j++) 
        {
            for(int k = 0; k < N; k++)
            {
                m3[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
}

void print(int** matrix, int rows, int cols) 
{
    for(long i = 0; i < rows; i++)
    {
        for (long j = 0; j < cols; j++)
        {                       
            printf("%d ",  matrix[i][j]); 
        }
        printf("\n");
    }
    printf("----------------------------\n");
}
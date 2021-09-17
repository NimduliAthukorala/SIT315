// import Libraries
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <omp.h>
#include <fstream>

using namespace std::chrono;
using namespace std;


// set the maximum random value for matrix A and B
#define max_value 10
// set the maximum number of threads
#define num_threads 2
// set the number of rows and columns for the matrix
const int N = 100;


// function to print a given matrix by accepting the name of the matrix
void print_matrix(int **matrix) 
{

    cout << "Displaying Matrix: " << endl;

    for (int i=0; i<N; i++) 
    {
        for (int j=0; j<N; j++) 
        {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }       
    cout << endl;
}


// function to carry out the calculation of matrix A multiplied by matrix B
void* multiply(int **A, int **B, int **C)
{
    #pragma omp for
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            C[i][j] = 0;
            for (int k = 0; k < N; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}



// main code
int main()
{
    int ** A,** B,** C;

    //Set the omp number of threads to whatever the global is
    omp_set_num_threads(num_threads);

    srand(time(0));

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
     
    // generate random numbers
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            A[i][j] = rand() % max_value;
            B[i][j] = rand() % max_value;
        }
    }
    
    // call print_matrix function for matrix A and B
    print_matrix(A);
    print_matrix(B);

    auto start = high_resolution_clock::now();

    #pragma omp parallel default(none) shared(A, B, C)
    multiply(A, B, C);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    // call print_matrix function for matrix C
    print_matrix(C);

    // create and open txt file and write Matrix C to it
    ofstream MyFile("OpenMP_Output.txt");
    MyFile << "This is Matrix C : \n";
    for (int i=0; i<N; i++) {

        for (int j=0; j<N; j++) {
            MyFile << C[i][j] << " ";
        }
        MyFile << "\n";
     }
    MyFile.close();

    cout << "Time taken by function: " << duration.count() << " milliseconds" << endl;
 
    return 0;
}
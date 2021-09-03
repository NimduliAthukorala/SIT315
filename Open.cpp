// import Libraries
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <omp.h>
#include <fstream>

using namespace std::chrono;
using namespace std;


// set the number of rows and columns for the matrix
#define N 10
// set the maximum random value for matrix A and B
#define max_value 10
// set the maximum number of threads
#define num_threads 4

// declare 3 matices of size N*N
int A[N][N];
int B[N][N];
int C[N][N];


// function to initilize matrix A and B with random values and C with 0 to help calculate the total later 
void random_number() {
    for (int i=0; i<N; i++) {

        for (int j=0; j<N; j++) {
            A[i][j] = rand() % max_value;
            B[i][j] = rand() % max_value;
            C[i][j] = 0;
        }
     }
}



// function to print a given matrix by accepting the name of the matrix
void print_matrix(string matrix) {

    cout << "Displaying Matrix: " << matrix << endl;

    for (int i=0; i<N; i++) {

        for (int j=0; j<N; j++) {
            if (matrix == "A") {
                cout << A[i][j] << " ";
            } else if (matrix == "B") {
                cout << B[i][j] << " ";
            } else {
                cout << C[i][j] << " ";
            }
        }
        cout << endl;
     }       
     cout << endl;
}


// function to carry out the calculation of matrix A multiplied by matrix B
void* multiply()
{
    #pragma omp for
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
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
    srand(time(0));
    // call random_number function
    random_number();
    
    // call print_matrix function for matrix A and B
    print_matrix("A");
    print_matrix("B");

    //Set the omp number of threads to whatever the global is
    omp_set_num_threads(num_threads);

    auto start = high_resolution_clock::now();

    #pragma omp parallel
    {
        multiply();
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    
    // call print_matrix function for matrix C
    print_matrix("C");

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

    cout << "Time taken by function: " << duration.count() << " microseconds" << endl;
 
    return 0;
}
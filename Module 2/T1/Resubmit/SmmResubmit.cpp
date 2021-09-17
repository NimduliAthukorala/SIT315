// import Libraries
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <bits/stdc++.h>
#include <chrono>


using namespace std::chrono;
using namespace std;

// set the number of rows and columns for the matrix
#define N 2000
// set the maximum random value for matrix A and B
#define max_value 10

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
void multiply() {
    for (int i = 0; i < N; i++) {

        for (int j = 0; j < N; j++) {

            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}


// main code
int main(void)
{
    srand(time(0));
    // call random_number function
    random_number();

    // call print_matrix function for matrix A and B
    print_matrix("A");
    print_matrix("B");

    // record start time
    auto start = high_resolution_clock::now();
    // call multiply function
    multiply();
    // record end time
    auto stop = high_resolution_clock::now();

    // call print_matrix function for matrix C
    print_matrix("C");

    // create and open txt file and write Matrix C to it
    ofstream MyFile("SMM_Output.txt");
    MyFile << "This is Matrix C : \n";
    for (int i=0; i<N; i++) {

        for (int j=0; j<N; j++) {
            MyFile << C[i][j] << " ";
        }
        MyFile << "\n";
     }
    MyFile.close();

    // calculate the time taken to do the multiplication
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken by function: " << duration.count() << " milliseconds" << endl;
 
    return 0;
}

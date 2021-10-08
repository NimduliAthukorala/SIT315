#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <unistd.h>

int main(int argc, char** argv) {
    int numtasks, dest,rank, name_len, tag =1;
    char name[MPI_MAX_PROCESSOR_NAME];
    char hello[13];

    MPI_Status status;

    // Initialize the MPI environment
    MPI_Init(&argc,&argv);

    // Get the number of tasks/process
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    // Get the rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Find the processor name
    MPI_Get_processor_name(name, &name_len);

    if (rank==0)
    {
        strcpy(hello,"Hello World!");
        dest = 1;

        MPI_Send((void*)hello, 13, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    }
    else if (rank!=0)
    {
        dest = 0;
       
        MPI_Recv((void*)hello, 13, MPI_CHAR, dest, tag, MPI_COMM_WORLD, &status);


        printf("Rank %d: Message: %s\n",rank,hello);
    }

    usleep(500000);

    //part 2
    
    if (rank==0){
        strcpy(hello, "Hello World!");}
    
    MPI_Bcast((void*)hello, 13, MPI_CHAR, 0, MPI_COMM_WORLD);
   
    printf("Rank %d: Message: %s\n", rank, hello);

    // Finalize the MPI environment
    MPI_Finalize();
}
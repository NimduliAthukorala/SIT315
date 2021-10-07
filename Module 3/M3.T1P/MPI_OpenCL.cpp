#include <iostream>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <mpi.h>
#include <CL/cl.h>
#include <chrono>

#define PRINT 1

using namespace std::chrono;
using namespace std;

int N = 1000;
int *A, *B, *C;
cl_mem bufA, bufB, bufC;

cl_device_id device_id;
cl_context context;
cl_program program;
cl_kernel kernel;
cl_command_queue queue;
cl_event event = NULL;

int err;

cl_device_id create_device();
void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname);
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename);
void setup_kernel_memory();
void copy_kernel_args();
void free_memory();

void openCL(int rows);
void init(int* &matrix, int rows, int cols, bool initialise);
void print( int* matrix, int size);
void head(int num_processes);
void node(int process_rank, int num_processes);

int main(int argc, char** argv) 
{
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

    //print(A, N);
    //print(B, N);

    int num_rows_per_process_from_A = N / num_processes;
    int num_elements_to_bcast = (N * N);
    int num_elements_to_scatter_or_gather = (N * N) / num_processes;

    auto start = high_resolution_clock::now();

    MPI_Scatter(&A[0], num_elements_to_scatter_or_gather ,  MPI_INT , &A , 0, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&B[0], num_elements_to_bcast , MPI_INT , 0 , MPI_COMM_WORLD);

    openCL(num_rows_per_process_from_A);

    MPI_Gather(MPI_IN_PLACE, num_elements_to_scatter_or_gather , MPI_INT, &C[0] , num_elements_to_scatter_or_gather , MPI_INT, 0 , MPI_COMM_WORLD);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Time taken by function: " << duration.count() << " milliseconds" << endl;

    free_memory();

    //print(C, N);
}


void node(int process_rank, int num_processes)
{
    int num_rows_per_process_from_A = N / num_processes;
    int num_elements_to_bcast = (N * N);
    int num_elements_to_scatter_or_gather = (N * N) / num_processes;

    init(A, num_rows_per_process_from_A, N, false), init(B, N, N, false), init(C, num_rows_per_process_from_A, N, false);

    MPI_Scatter(NULL, num_elements_to_scatter_or_gather , MPI_INT , &A[0], num_elements_to_scatter_or_gather, MPI_INT, 0 , MPI_COMM_WORLD);
    MPI_Bcast(&B[0], num_elements_to_bcast , MPI_INT , 0 , MPI_COMM_WORLD);
    
    openCL(num_rows_per_process_from_A);

    MPI_Gather(&C[0], num_elements_to_scatter_or_gather , MPI_INT, NULL, num_elements_to_scatter_or_gather , MPI_INT, 0 , MPI_COMM_WORLD);

    free_memory();
}

void init(int* &matrix, int rows, int cols, bool initialise) 
{
    matrix = (int *)malloc(sizeof(int*) * rows * cols); 

    if(!initialise) return;

    for(long i = 0 ; i < rows*cols; i++) 
    {
        matrix[i] = rand() % 10; 
    }
}

void print(int* matrix, int size) 
{
    if (PRINT == 0)
    {
        return;
    }

    for(long i = 0; i < size; i++)
    {
        for (long j = 0; j < size; j++)
        {                       
            printf("%d ",  matrix[i*size + j]); 
        }
        printf("\n");
    }
    printf("----------------------------\n");
}

void openCL(int rows)
{
    size_t global[] = {(size_t)rows, (size_t)N, (size_t)N};

    setup_openCL_device_context_queue_kernel((char *)"./multiply.cl", (char *)"multiply_matrix");

    setup_kernel_memory();
    copy_kernel_args();

    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, NULL, 0, NULL, &event);
    clWaitForEvents(1, &event);
    clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0, N* rows *sizeof(int), &C[0], 0, NULL, NULL);
}

void free_memory()
{
    //free the buffers
    clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufC);

    //free opencl objects
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    free(A);
    free(B);
    free(C);
}

void copy_kernel_args()
{
    clSetKernelArg(kernel, 0, sizeof(int), (void *)&N);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufA);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&bufB);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&bufC);

    if (err < 0)
    {
        perror("Couldn't create a kernel argument");
        printf("error = %d", err);
        exit(1);
    }
}

void setup_kernel_memory()
{
    bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, N * N * sizeof(int), NULL, NULL);
    bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, N * N * sizeof(int), NULL, NULL);
    bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, N * N * sizeof(int), NULL, NULL);

    // Copy matrices to the GPU
    clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0, N * N * sizeof(int), &A[0], 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0, N * N * sizeof(int), &B[0], 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufC, CL_TRUE, 0, N * N * sizeof(int), &C[0], 0, NULL, NULL);
}

void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname)
{
    device_id = create_device();
    cl_int err;

    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (err < 0)
    {
        perror("Couldn't create a context");
        exit(1);
    }

    program = build_program(context, device_id, filename);
    
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    if (err < 0)
    {
        perror("Couldn't create a command queue");
        exit(1);
    };


    kernel = clCreateKernel(program, kernelname, &err);
    if (err < 0)
    {
        perror("Couldn't create a kernel");
        printf("error =%d", err);
        exit(1);
    };
}

cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename)
{

    cl_program program;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;

    /* Read program file and place content into buffer */
    program_handle = fopen(filename, "r");
    if (program_handle == NULL)
    {
        perror("Couldn't find the program file");
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char *)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);


    program = clCreateProgramWithSource(ctx, 1, (const char **)&program_buffer, &program_size, &err);
    if (err < 0)
    {
        perror("Couldn't create the program");
        exit(1);
    }
    free(program_buffer);

    /* Build program 
   The fourth parameter accepts options that configure the compilation. 
   These are similar to the flags used by gcc. For example, you can 
   define a macro with the option -DMACRO=VALUE and turn off optimization 
   with -cl-opt-disable.
   */
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err < 0)
    {

        /* Find size of log and print to std output */
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                              0, NULL, &log_size);
        program_log = (char *)malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                              log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    return program;
}

cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   } 

   // Access a device
   // GPU
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      // CPU
      printf("GPU not found\n");
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return dev;
}

/* 
__kernel void multiply_matrix(const int N, __global const int* a, __global const int* b, __global int* c)
{ 
    int row = get_global_id(0); 
    int col = get_global_id(1); 
    
    int Cres = 0; 
    for(int i = 0;i< N; i++) 
    {
        Cres += a[row*N + i ] * b[i*N + col];
    } 
    
    c[row*N + col]= Cres; } 
*/
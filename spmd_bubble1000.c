/*
Ideias para analise: trocar o percentual de troca e analisar
o numero de iterações necessarias e tempo.
*/

#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

/*
full size 10016
exchange tests: 500, 1000, 2000, 5000, 10000, 15000.
*/

#define EXCHANGE_N 1000
#define FULL_ARR_SIZE 1001600


int my_rank; //Process id.
int proc_n; //Total number of processes
MPI_Status status;
int *arr;
int arr_size;
int arr_ext_size;
int *proc_status;
int iter = 0;

void fill_reverse_arr(int size) {
    int i;
    int first = (proc_n - my_rank) * size;
    for(i=0; i<size; i++) {
        arr[i] = first-i;
    }
}

//Optimized version where every pass reduces the number
//of comparisons by 1.
void bubblesort(int *arr, int size) {
    int i;
    int temp;
    int comp_length = size;
    int ordered = 0;
    while(!ordered) {
        ordered = 1;
        for(i=0; i<comp_length-1; i++) {
            if(arr[i]>arr[i+1]) {
                temp = arr[i];
                arr[i] = arr[i+1];
                arr[i+1] = temp;
                ordered = 0;
            }
        }
        comp_length--; 
    }
}

int is_ordered(int *arr, int size) {
    int i;
    for (i=0; i<size-1; i++) {
        if(arr[i]>arr[i+1])
            return 0;
    }
    return 1;
}

void print_arr(int *arr, int size) {
    int i;
    for(i=0; i<size; i++) {
        printf("%d, ", arr[i]);
    }
    printf("\n");
}

void main(int argc, char **argv) {
    
    MPI_Init(&argc , &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
    
    int i;
    //Array that stores state for each process.
    //int *proc_status;
    
    arr_size = FULL_ARR_SIZE/proc_n;
    arr_ext_size = arr_size+EXCHANGE_N;
    
    if((proc_status = malloc(proc_n*sizeof(int))) == NULL) {
        printf("[%d]malloc failed.\n", my_rank);
        exit(1);
    }
    
    if((arr = malloc(arr_ext_size*sizeof(int)))==NULL) {
        printf("[%d]malloc failed.\n", my_rank);
        exit(1);
    }
    
    fill_reverse_arr(arr_size);
    
    //printf("[%d]arr: ", my_rank);
    //print_arr(arr, arr_size);
    
    //Last process never checks when receiving from the right because this receive
    //never happens. It will always consider itself done and never do a partial
    //sort, only full ones.
    proc_status[proc_n-1] = 1;
    
    if(my_rank == 0) {
        printf("Processes: %d\n", proc_n);
        printf("Full array size: %d\n", FULL_ARR_SIZE);
        printf("Process array size: %d\n", arr_size);
        printf("Exchange chunk: %d\n", EXCHANGE_N);
        double ratio = EXCHANGE_N/((double)arr_size);
        printf("Exchange/process %%: %.2f\n", ratio*100.0);
    }
    
    //printf("[%d]start.\n", my_rank);
    
    //Location of the array from which elements will be received
    //from the neighbor process to the right.
    int *exch_ptr = &arr[arr_size];
    //Location of the array from which elements will be
    //partially sorted before the second exchange
    int *partial_sort_ptr = &arr[arr_size - EXCHANGE_N];
    
    int done = 0;
    
    //Stores the smallest element sent by the process to the right.
    int comp_n;
    
    double t1 = MPI_Wtime();
    
    //===================Sorting loop========================
    while(1) {
        iter++;
        bubblesort(arr, arr_size);
        
        if(my_rank!=0) {
            //Send the smallest element of the array to the
            //process to the left.
            MPI_Send(arr, 1, MPI_INT,
                     my_rank-1, 1, MPI_COMM_WORLD);
        }
        
        if(my_rank!=proc_n-1) {
            //Receive the smallest elements of the process to
            //the right
            MPI_Recv(&comp_n, EXCHANGE_N, MPI_INT,
                     my_rank+1, 1, MPI_COMM_WORLD, &status);
            
            if(arr[arr_size-1] > comp_n) {
                //The last element of the array is greater than the
                //smallest element of the neighbor process. Not done yet.
                proc_status[my_rank] = 0;
            } else {
                //Nothing to change.
                proc_status[my_rank] = 1;
            }
        }
        
        //Broadcast status.
        done = 1;
        for(i=0; i<proc_n; i++) {
            MPI_Bcast(&proc_status[i], 1, MPI_INT, i, MPI_COMM_WORLD);
            if(proc_status[i]==0) {
                //Some process found unsorted elements.
                done = 0;
                break;
            }
        }
        
        //Check if everything is sorted.
        if(done == 1) {
            break;
        }
        //If not:
        if(my_rank!=0) {
            //Send smallest elements to the process to the the left.
            MPI_Send(arr, EXCHANGE_N, MPI_INT, my_rank-1, 1, MPI_COMM_WORLD);
        }  
        if(my_rank!=proc_n-1) {
            //Receive a chunk with the smallest elements from the
            //process to the right.
            MPI_Recv(exch_ptr, EXCHANGE_N, MPI_INT,
                     my_rank+1, 1, MPI_COMM_WORLD, &status);
            //Partially sort received elements.
            bubblesort(partial_sort_ptr, EXCHANGE_N*2);
            //Return same amount of the biggest elements to neighbor to the right.
            MPI_Send(exch_ptr, EXCHANGE_N, MPI_INT,
                     my_rank+1, 1, MPI_COMM_WORLD);
        }
        
        if(my_rank!=0) {
            //Receive those elements.
            MPI_Recv(arr, EXCHANGE_N, MPI_INT, my_rank-1, 1, MPI_COMM_WORLD, &status);
        }
    
    }
    
    double t2 = MPI_Wtime();
    if(my_rank==0) {
        printf("\nTime: %.2f\nIterations: %d\n",
               t2-t1, iter);
    }
    
    //printf("[%d]arr: ", my_rank);
    //print_arr(arr, arr_size);
    
    free(arr);
    free(proc_status);
    
    MPI_Finalize();
}

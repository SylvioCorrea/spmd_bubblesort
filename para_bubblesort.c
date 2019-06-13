/*
Ideias para analise: trocar o percentual de troca e analisar
o numero de iterações necessarias e tempo.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include "mmulti.h"

#define EXCHANGE_N 2
#define ARR_SIZE 10
#define ARR_EXT_SIZE (ARR_SIZE + EXCHANGE_N)


int my_rank; //Process id.
int proc_n; //Total number of processes


int simple_pow(int b, int p) {
    int i;
    int res = 1;
    for(i=0; i<p; i++, res*=b) {}
    return res;
}

void fill_reverse_arr(int *arr, int last, int size) {
    int i;
    for(i=0; i<size; i++) {
        arr[i] = last-i;
    }
}

void bubblesort(int *arr, int lo, int hi) {
    int i;
    int ordered = 0;
    while(!ordered) {
        ordered = 1;
        for(i=0; i<size-1; i++) {
            if(arr[i]>arr[i+1]) {
                temp = arr[i];
                arr[i] = arr[i+1];
                arr[i+1] = temp;
                ordered = 0;
            }
        } 
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

void main() {
    MPI_Status status;
    MPI_Init(&argc , &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
    
    int i;
    
    //Array o be sorted.
    int *arr;
    //Array that stores state for each process.
    //int *proc_status;
    
    int proc_status = 0;
    
    if(arr = malloc(ARR_EXT_SIZE*sizeof(int))==NULL) {
        printf("malloc failed.\n");
        exit(1);
    }
    
    fill_reverse_arr(arr, my_rank, ARR_SIZE);
    /*
    if(proc_status = malloc(proc_n*sizeof(int))==NULL) {
        printf("malloc failed.\n");
        exit(1);
    }*/
    
    for(i=0; i<proc_n; i++) {
        proc_status[i] = 0;
    }
    
    if(my_rank == 0) {
        printf("Executing with %d processes.\n", proc_n);
        printf("Array size: %d.\n", ARR_SIZE);
    }
    
    //Location of the array from which elements will be sent
    //to the neighbor process.
    int *exch_ptr = &arr[ARR_SIZE];
    //Location of the array from which elements will be
    //sorted before the second exchange
    int *partial_sort_ptr = &arr[ARR_SIZE - EXCHANGE_N];
    int done = 0;
    //===================Main loop========================
    while(!done) {
        done = 1;
        
        bubblesort(arr, ARR_SIZE);
        
        if(my_rank!=0) {
            MPI_Send(arr, EXCHANGE_N, MPI_INT,
                     my_rank-1, 1, MPI_COMM_WORLD);
                     
            MPI_Recv(arr, EXCHANGE_N, MPI_INT,
                     my_rank-1, 1, MPI_COMM_WORLD, &status);
                     
            //TODO
        }
        if(my_rank!=proc_n-1) {
            MPI_Recv(exch_ptr, EXCHANGE_N, MPI_INT,
                     my_rank+1, 1, MPI_COMM_WORLD, &status);
        
            if(arr[ARR_SIZE-1]>arr[ARR_SIZE]) {
                //The last element of the array is greater than the
                //smallest element of the neighbor process. Partially sort.
                bubblesort(partial_sort_ptr, EXCHANGE_N*2);
                
                //Send 2 biggest elements to neighbor.
                MPI_Send(exch_ptr; EXCHANGE_N, MPI_INT,
                         my_rank+1, 1, MPI_COMM_WORLD)
                
                //Store status as not done yet.
                proc_status[my_rank] = 0;
            }
        }
        
        //Broadcast status.
        for(i=0; i<proc_n; i++) {
            MPI_Brd
        }
    }
    
    
    
    free(arr);
    free(proc_status);
}












#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ARR_SIZE 20

int simple_pow(int b, int p) {
    int i;
    int res = 1;
    for(i=0; i<p; i++, res*=b) {}
    return res;
}

void fill_arr(int *arr, int size) {
    int i;
    printf("%d\n", arr);
    printf("teste\n");
    for(i=0; i<size; i++) {
        arr[i] = i;
        printf("%d\n", i);
    }
}

void quicksort(int *arr, int lo, int hi) {
    if(lo<hi) {
        int p = partition(arr, lo, hi);
        quicksort(arr, lo, p);
        quicksort(arr, p+1, hi);
    }
}

int partition(int *arr, int lo, int hi) {
    int pivot = arr[lo + (hi-lo)/2];
    int i = lo-1;
    int j = hi+1;
    int temp;
    while(1) {
        do {
            i++;
        }while(arr[i]<pivot);
        do {
            j--;
        }while(arr[j]>pivot);
        
        if(i>=j) {
            return j;
        }
        
        temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
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
    srand(1); //same seed everytime for consistency on measurements.
    int *arr = malloc(ARR_SIZE * sizeof(int));
    if(arr=NULL) {
        printf("Malloc failed!\n");
        exit(1);
    }
    printf("%d\n", NULL);
    fill_arr(arr, ARR_SIZE);
    printf("t1\n");
    quicksort(arr, 0, ARR_SIZE);
    printf("t1\n");
    print_arr(arr, ARR_SIZE);
}












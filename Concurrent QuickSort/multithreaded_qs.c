#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define swap(x,y) do {\
    __typeof__(x) tmp = x;\
    x = y;\
    y = tmp;\
} while(0)

typedef struct arg_struct {
    int *array;
    int start;
    int end;
} args;

void *quickSortThread(void *args);
void insertionSort(int *array, int start, int end);
int partition(int *array, int start, int end, int pivot);
void quickSort(int *array, int start, int end);

void insertionSort(int *array, int start, int end) {
    int *last = array + (end - start + 1);
	for(int *i = array + 1; i < last; i++)
		if(*i < *(i-1)) {
			int ival;
	        for(ival = *i; i > array && *(i-1) > ival; i--) {
		        *i = *(i-1);
	        }
	        *i = ival;
        }
}

int partition(int *array, int start, int end, int pivot) {

    int index = start, pvalue = array[pivot];
    swap(array[pivot], array[end]);
    

    for(int i = start; i < end; i++) {
        if(array[i] <= pvalue) {
            swap(array[i], array[index++]);
        }
    }

    swap(array[index], array[end]);
    return index;
}

void quickSort(int *array, int start, int end) {
    if(end <= start) {
        return;
    }

    int pivotIndex = start + (end - start)/2;
    pivotIndex = partition(array, start, end, pivotIndex);

    struct arg_struct *args;
    args->array = array;
    args->start = start;
    args->end = end;

    pthread_t thread;
    pthread_create(&thread, NULL, quickSortThread, (void *) &args);
    quickSort(array, pivotIndex + 1, end);
}

void *quickSortThread(void *args) {
    struct arg_struct *arg = (struct arg_struct *) args;
    quickSort(arg->array, arg->start, arg->end);
    return NULL;
}

int main(void) {
    int n = 0, *array;
    array = (int *) malloc(1024 * sizeof(int));

    scanf("%d", &n);

    for(int i = 0; i < n; i++) {
        scanf("%d", array + i);
    }

    if(n <= 5) {
        insertionSort(array, 0, n-1);

        for(int i = 0; i < n; i++) {
            printf("%d ", array[i]);
        }

        printf("\n");
        return 0;
    }

    quickSort(array, 0, n-1);

    for(int i = 0; i < n; i++) {
            printf("%d ", array[i]);
        }

        printf("\n");
        return 0;
}
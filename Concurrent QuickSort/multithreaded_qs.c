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
int partition(int *array, int low, int high);
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

int partition(int *array, int low, int high) {
 int pivot = array[high];
    int i = low-1;
    for(int j=low;j<=high-1;j++){
        if(array[j]<pivot){
            i++;
            swap(array[i],array[j]);
        }
    }
    swap(array[i+1],array[high]);
    return i+1;
}

void quickSort(int *array, int start, int end) {
    if(end <= start) {
        return;
    }

    int pivotIndex = partition(array, start, end);

    struct arg_struct args1;
    args1.array = array;
    args1.start = start;
    args1.end = pivotIndex-1;


    struct arg_struct args2;
    args2.array = array;
    args2.start = pivotIndex+1;
    args2.end = end;

    pthread_t thread1,thread2;
    pthread_create(&thread1, NULL, quickSortThread, (void *) &args1);
    pthread_create(&thread2, NULL, quickSortThread, (void *) &args2);
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
}

void *quickSortThread(void *args) {
    struct arg_struct *arg = (struct arg_struct *) args;
    quickSort(arg->array, arg->start, arg->end);
    return NULL;
}

int main(void) {
    int n = 0, *array;
    array = (int *) malloc(100000 * sizeof(int));

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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

/*
#define swap(x,y) do {\
    __typeof__(x) tmp = x;\
    x = y;\
    y = tmp;\
} while(0)
*/

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
    int pivot = start, lChild = -1, rChild = -1, ptemp, status;

    if(start >= end) {
        return;
    }

    ptemp = partition(array, start, end, pivot);

    lChild = fork();

    if(lChild < 0) {
        perror("Concurrent Quicksort: LChild Fork");
        exit(1);
    } else if(lChild == 0) {
        quickSort(array, start, ptemp-1);
        exit(0);
    } else {
        rChild = fork();

        if(rChild < 0) {
            perror("Concurrent Quicksort: RChild Fork");
            exit(1);
        } else if( rChild == 0) {
            quickSort(array, ptemp + 1, end);
            exit(0);
        }
    }

    waitpid(lChild, &status, 0);
    waitpid(rChild, &status, 0);
}

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

int main(void) {
    int n = 0, *array, *sarray, id;
    array = (int *) malloc(1024 * sizeof(int));
    sarray = (int *) malloc(1024 * sizeof(int));

    key_t key = IPC_PRIVATE;
    size_t size;

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

    size = n * sizeof(int);

    id = shmget(key, size, IPC_CREAT | 0666);
    sarray = (int *) shmat(id, NULL, 0);

    for(int i = 0; i < n; i++) {
        sarray[i] = array[i];
    }

    quickSort(sarray, 0, n-1);

    for(int i = 0; i < n; i++) {
        printf("%d ", sarray[i]);
    }

    printf("\n");

    shmdt(sarray);
    shmctl(id, IPC_RMID, NULL);

    return 0;
}
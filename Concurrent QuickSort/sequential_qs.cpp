/***********************************
Randomized Quick Sort Algorithm
Random Shuffling Implementation
***********************************/
#include<iostream>
#include<ctime>
#include<cstdlib>

using namespace std;

void shuffle(int *array, int start, int end){

    srand(time(NULL));

    int i,j, temp;
    for(int i= end; i > 0; i--){
        
        j = rand() % (i+1);
        swap(array[i], array[j]);
    }
}

int partition(int *array, int start, int end){

    int i = start - 1;
    int j = start;
    int pivot = array[end];
    for( ;j < end; j++){
        if(array[j] <= pivot){
            i++;
            swap(array[i], array[j]);
        }
    }
    
    swap(array[i+1], array[end]);
    return i + 1;
}

void quickSort(int *array, int start,int end){
    if(start >= end){
        return;
    }

    int p = partition(array, start, end);
    quickSort(array, start, p-1);
    quickSort(array, p+1, end);
}

int main(void){

    int *array, n = 0;
    array = (int *) malloc(1024 * sizeof(int));

    scanf("%d", &n);

    for(int i = 0; i < n; i++) {
        scanf("%d", array + i);
    }

    quickSort(array, 0, n-1);

    for(int i = 0; i < n; i++) {
        printf("%d ", array[i]);
    }

    printf("\n");

    return 0;
}
/*************************************************************
References:
https://codingblocks.com/resources/quicksort-II/
https://www.geeksforgeeks.org/quicksort-using-random-pivoting/

Naren Akash, R J - 2018111020
October 04, 2019 - Friday
International Institute of Information Technology
Hyderabad
*************************************************************/
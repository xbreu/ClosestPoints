#ifndef PointSortUtilities_h
#define PointSortUtilities_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <string.h>

// Definition Data Types
typedef struct {
    double x;
    double y;
} Point;

// Definition
void QuickPointSort(Point array[], int count, int sort_by_x);
void QuickPointSortRecursive(Point array[], int left_index, int right_index, int sort_by_x);
void SwapPoints(Point* a, Point* b);
int QuickPointSortPartitioner(Point array[], int left_index, int right_index, int sort_by_x);
void MergeTwoSortedPointArrays(Point arrA[], int arrA_count, Point arrB[], int arrB_count, Point merged_arr[], int sort_by_x);
int IsSortingPointsCorrect(Point array[], int arr_count, int* j, int sort_by_x);
void PrintPointArray(Point array[], int arr_count);

// Implementation

void QuickPointSort(Point array[], int count, int sort_by_x) {
    srand(time(NULL));
    QuickPointSortRecursive(array, 0, count - 1, sort_by_x);
}

void QuickPointSortRecursive(Point array[], int left_index, int right_index, int sort_by_x) {
    if (left_index < right_index) {
        int pivot_index = QuickPointSortPartitioner(array, left_index, right_index, sort_by_x);
        QuickPointSortRecursive(array, left_index, pivot_index - 1, sort_by_x);
        QuickPointSortRecursive(array, pivot_index + 1, right_index, sort_by_x);
    }
}

void SwapPoints(Point* a, Point* b) {
    Point temp;
    // Fill Temp
    temp.x = a->x;
    temp.y = a->y;
    // Fill a with b
    a->x = b->x;
    a->y = b->y;
    // Fill a with b
    b->x = temp.x;
    b->y = temp.y;
}

int QuickPointSortPartitioner(Point array[], int left_index, int right_index, int sort_by_x) {
    int pivot_index = left_index + rand() % (right_index - left_index + 1);
    SwapPoints(&array[pivot_index], &array[right_index]);
    pivot_index = right_index;
    Point pivot_element;
    pivot_element.x = array[pivot_index].x;
    pivot_element.y = array[pivot_index].y;

    int i, j;
    for (i = left_index, j = left_index; i < right_index; i++) {
        if ((sort_by_x && array[i].x < pivot_element.x) || (!sort_by_x && array[i].y < pivot_element.y)) {
            SwapPoints(&array[j], &array[i]);
            j++;
        }
    }
    SwapPoints(&array[pivot_index], &array[j]);

    return j;
}

void MergeTwoSortedPointArrays(Point arrA[], int arrA_count, Point arrB[], int arrB_count, Point merged_arr[], int sort_by_x) {
    int i = 0, j = 0, index = 0;

    while (i < arrA_count && j < arrB_count) {
        if ((sort_by_x && arrA[i].x <= arrB[j].x) || (!sort_by_x && arrA[i].y <= arrB[j].y)) {
            merged_arr[index].x = arrA[i].x;
            merged_arr[index].y = arrA[i].y;
            i++;
        } else {
            merged_arr[index].x = arrB[j].x;
            merged_arr[index].y = arrB[j].y;
            j++;
        }
        index++;
    }

    while (i < arrA_count) {
        merged_arr[index].x = arrA[i].x;
        merged_arr[index].y = arrA[i].y;
        i++;
        index++;
    }

    while (j < arrB_count) {
        merged_arr[index].x = arrB[j].x;
        merged_arr[index].y = arrB[j].y;
        j++;
        index++;
    }
}

int IsSortingPointsCorrect(Point array[], int arr_count, int* j, int sort_by_x) {
    if (j != NULL) 
        *j = -1;
        
    int i;
    for (i = 0; i < arr_count - 1; i++) {
        if ((sort_by_x && array[i].x > array[i + 1].x) || (!sort_by_x && array[i].y > array[i + 1].y)) {
            if (j != NULL) *j = i;
            return 0;
        }
    }
    return 1;
}

void PrintPointArray(Point array[], int arr_count) {
    if (array == NULL){
        printf("Array is NULL!\n");
        return;
    }
    int i;
    for (i = 0; i < arr_count; i++) {
        printf("arr[%d]: (%15.10lf, %15.10lf)\n", i, array[i].x, array[i].y);
    }
}


#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "omp.h"
#include <time.h>

#define MAX_SIZE 80000

void TopDownMergeSort(int * A, int* B, int n);
void TopDownMergeSort_p(int * A, int* B, int n);
void TopDownSplitMerge(int * A, int iBegin, int iEnd, int* B);
void TopDownSplitMerge_p(int * A, int iBegin, int iEnd, int* B);
void TopDownMerge(int * A, int iBegin, int iMiddle, int iEnd, int* B);
void CopyArray(int* B, int iBegin, int iEnd, int * A);

//產生亂數
void generate_list(int * x, int n) {
	int i, j, t;
	for (i = 0; i < n; i++) 
		x[i] = i;
	for (i = 0; i < n; i++) { //產生80000個亂數，其值與x[i]交換,以達到亂數不重複
		j = rand() % n;       
		t = x[i];             
		x[i] = x[j];
		x[j] = t;
	}
}
//列印陣列
void print_list(int * x, int n) {
	int i;
	for (i = 0; i < n; i++) {
		printf("%d ", x[i]);
	}
	printf("\n");
}


void TopDownMergeSort(int * A, int* B, int n)
{
	TopDownSplitMerge(A, 0, n, B);
}
// Array A[] has the items to sort; array B[] is a work array.
void TopDownMergeSort_p(int * A, int* B, int n)
{
	TopDownSplitMerge_p(A, 0, n, B);
}

void TopDownSplitMerge_p(int * A, int iBegin, int iEnd, int* B)
{
	if (iEnd - iBegin < 2)                       // if run size == 1
		return;                                 //   consider it sorted
	// recursively split runs into two halves until run size == 1,
	// then merge them and return back up the call chain
	int iMiddle = (iEnd + iBegin) / 2;              // iMiddle = mid point
#pragma omp parallel num_threads(2)
	{
#pragma omp sections
		{
#pragma omp section 
			TopDownSplitMerge(A, iBegin, iMiddle, B);  // split / merge left  half
#pragma omp section 
			TopDownSplitMerge(A, iMiddle, iEnd, B);  // split / merge right half
		}
	}
	TopDownMerge(A, iBegin, iMiddle, iEnd, B);  // merge the two half runs
	CopyArray(B, iBegin, iEnd, A);              // copy the merged runs back to A
}
// iBegin is inclusive; iEnd is exclusive (A[iEnd] is not in the set).
void TopDownSplitMerge(int * A, int iBegin, int iEnd, int* B)
{
	if (iEnd - iBegin < 2)                       // if run size == 1
		return;                                 //   consider it sorted
	// recursively split runs into two halves until run size == 1,
	// then merge them and return back up the call chain
	int iMiddle = (iEnd + iBegin) / 2;              // iMiddle = mid point
	TopDownSplitMerge(A, iBegin, iMiddle, B);  // split / merge left  half
	TopDownSplitMerge(A, iMiddle, iEnd, B);  // split / merge right half
	TopDownMerge(A, iBegin, iMiddle, iEnd, B);  // merge the two half runs
	CopyArray(B, iBegin, iEnd, A);              // copy the merged runs back to A
}

//  Left half is A[iBegin :iMiddle-1].
// Right half is A[iMiddle:iEnd-1   ].
void TopDownMerge(int * A,int iBegin,int iMiddle,int iEnd, int* B)
{
	int i = iBegin, j = iMiddle;

	// While there are elements in the left or right runs...
	for (int k = iBegin; k < iEnd; k++) {
		// If left run head exists and is <= existing right run head.
		if (i < iMiddle && (j >= iEnd || A[i] <= A[j])) {
			B[k] = A[i];
			i = i + 1;
		}
		else {
			B[k] = A[j];
			j = j + 1;
		}
	}
}

void CopyArray(int* B, int iBegin, int iEnd, int * A)
{
	for (int k = iBegin; k < iEnd; k++)
		A[k] = B[k];
}

int main()
{
	int n = 80000;
	double start, stop1, stop2, stop3, d1, d2, r;
	int data[MAX_SIZE], tmp[MAX_SIZE], data_s[MAX_SIZE];

	for (n; n > 7; n / 10)
	{
		printf( "資料筆數(資料筆數<100筆時顯示): %d \n", n);
		generate_list(data, n);
		for (int i = 0; i < n; i++)
		{
			data_s[i] = data[i];
		}
		if (n < 101)
		{
			printf("List Before Sorting...\n");
			print_list(data, n);
		}		
		start = omp_get_wtime();
		TopDownMergeSort_p(data, tmp, n);
		stop1 = omp_get_wtime();
		if (n < 101)
		{
			printf("\nList After Sorting by using parallel code...\n");
			print_list(data, n);
		}
		d1 = stop1 - start;
		printf("\nSorting by using parallel code Time: %2.8f", d1);

		stop2 = omp_get_wtime();
		TopDownMergeSort(data_s, tmp, n);
		stop3 = omp_get_wtime();
		d2 = stop3 - stop2;
		printf("\nSorting by using serial   code Time: %2.8f\n", d2);
		
		r = d2 / d1;
		printf("\n平行化快: %2.8f 倍\n\n\n", r);

		n = n / 10;
	}
	system("pause");
}
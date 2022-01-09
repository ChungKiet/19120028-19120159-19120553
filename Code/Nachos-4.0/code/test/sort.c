// /* sort.c 
//  *    Test program to sort a large number of integers.
//  *
//  *    Intention is to stress virtual memory system.
//  *
//  *    Ideally, we could read the unsorted array off of the file system,
//  *	and store the result back to the file system!
//  */


// /*
// #define UNIX
// #define UNIX_DEBUG
// */

// #ifdef UNIX
// #include <stdio.h>
// #define Exit exit
// #else
// #include "syscall.h"
// #endif /* UNIX */

// #define SIZE (1024)

// int A[SIZE];	/* size of physical memory; with code, we'll run out of space!*/

// int
// main()
// {
//     int i, j, tmp;

//     /* first initialize the array, in reverse sorted order */
//     for (i = 0; i < SIZE; i++) {
//         A[i] = (SIZE-1) - i;
//     }

//     /* then sort! */
//     for (i = 0; i < SIZE; i++) {
//         for (j = 0; j < (SIZE-1); j++) {
// 	   if (A[j] > A[j + 1]) {	/* out of order -> need to swap ! */
// 	      tmp = A[j];
// 	      A[j] = A[j + 1];
// 	      A[j + 1] = tmp;
//     	   }
//         }
//     }

// #ifdef UNIX_DEBUG
//     for (i=0; i<SIZE; i++) {
//         printf("%4d ", A[i]);
// 	if (((i+1) % 15) == 0) {
// 		printf("\n");
//         }
//         if (A[i] != i) {
//             fprintf(stderr, "Out of order A[%d] = %d\n", i, A[i]);
//             Exit(1);
//         }   
//     }
//     printf("\n");
// #endif /* UNIX_DEBUG */

//     for (i=0; i<SIZE; i++) {
//         if (A[i] != i) {
//             Exit(1);
//         }   
//     }

//     Exit(0);
// }

#include "syscall.h"
int main(){
    int n, choice,temp;
    int i,j;
    int A[100];
    PrintString("Nhap so nguyen duong n = ");
    n = ReadNum();
    while (n <= 0 || n > 100) {
        PrintString("Nhap 0 < n <= 100. n = ");
        n = ReadNum();
    }
    for(i = 0; i < n; ++i) {
        PrintString("A["); PrintNum(i); PrintString("] = ");
        A[i] = ReadNum();
    }
    PrintString("0: Tang dan/1: Giam dan?: ");
    choice = ReadNum();
    while (choice != 0 && choice != 1) {
        PrintString("0: Tang dan/1: Giam dan?: ");
        choice = ReadNum();
    }
    if(choice == 0){
        for(i = 0; i < n-1; ++i){
            for(j = 0; j < n-i-1; ++j){
                if(A[j] > A[j+1]) {
                    temp = A[j]; A[j] = A[j+1]; A[j+1] = temp;
                } 
            }
        }  
    } 
    else{
        for(i = 0; i < n-1; ++i){
            for(j = 0; j < n-i-1; ++j){
                if(A[j] < A[j+1]) {
                    temp = A[j]; A[j] = A[j+1]; A[j+1] = temp;
                }
            }
        }        
    }
        
    PrintString("Mang sap xep ");
    if(choice == 0) PrintString("tang dan:\n");
    else PrintString("giam dan:\n");
    for(i = 0; i<n; ++i) {
        PrintString("A["); PrintNum(i); PrintString("] = ");
        PrintNum(A[i]); PrintString("\n");
    }
    Halt();
}
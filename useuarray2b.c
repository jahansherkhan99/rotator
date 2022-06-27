
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <uarray2b.h>
#include "uarray2.h"
#include "uarray.h"

typedef long number;

const int DIM1 = 4;
const int DIM2 = 5;
const int ELEMENT_SIZE = 7;
const int MARKER = 99;

void check_and_print(int i, int j, UArray2b_T a, void *p1, void *p2) 
{
    (void) a;
    (void) p1;
    (void) p2;
    printf("%d = ", *(int *)p1);
    printf("ar[%d,%d]\n", i, j);

 }

int main(int argc, char *argv[])
{
        (void)argc;
        (void)argv;

        UArray2b_T test_array;
        bool OK = true;
        test_array = UArray2b_new(DIM1, DIM2, ELEMENT_SIZE, 3);


        OK = (UArray2b_width(test_array) == DIM1) &&
	     (UArray2b_height(test_array) == DIM2) &&
             (UArray2b_size(test_array) == ELEMENT_SIZE) && (UArray2b_blocksize(test_array) == 2);
    
        (void) OK;
        //fprintf(stderr, "%d ", OK);
    


        /* Note: we are only setting a value on the corner of the array */

        for (int i = 0; i < DIM2; i++) {
            for (int j = 0; j < DIM1; j++) {
                 *(int *)UArray2b_at(test_array, j, i) = 5;
            }
        }
        // for (int i = 0; i < DIM2; i++) {
        //     for (int j = 0; j < DIM1; j++) {
        //          printf("ar[%d,%d] = %d\n", j, i, *(int *)UArray2b_at(test_array, j, i));
        //     }
        // }
        // printf("%d", *(int *)UArray2b_at(test_array, 1, 2));
        // printf("%d", *(int *)UArray2b_at(test_array, 1, 3));
        UArray2b_map(test_array, check_and_print, NULL);

        UArray2b_T test2 = UArray2b_new_64K_block(DIM1, DIM2, ELEMENT_SIZE);
        printf("%d ", UArray2b_blocksize(test2));

        
        

        // UArray_T *temp = ((UArray_T *)UArray2b_at(test_array, DIM1 - 1, DIM2 - 1));
        // for (int i = 0; i < UArray_length(*temp); i++) {
        //     *(int *)UArray_at(*temp, i) = 5;
        //}
        // ((UArray_T *)UArray2b_at(test_array, DIM1 - 1, DIM2 - 1)) = temp;
        // UArray_T *temp2 = *((UArray_T *)UArray2b_at(test_array, DIM1 - 1, DIM2 - 1));
        //  for (int i = 0; i < UArray_length(*temp); i++) {
        //     printf("%d", UArray_at(temp2, i));
        // }

        // printf("Trying column major\n");
        // UArray2_map_col_major(test_array, check_and_print, &OK);

        // printf("Trying row major\n");
        // UArray2_map_row_major(test_array, check_and_print, &OK);

        UArray2b_free(&test_array);

        //printf("The array is %sOK!\n", (OK ? "" : "NOT "));

}
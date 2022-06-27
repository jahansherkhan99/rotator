/**************************************************************
 *
 *                     UArray2.c
 *
 *     Assignment: iii
 *     Authors:  Jahansher Khan (jkhan03), Tom Barnett-Young (tbarne02)
 *     Date:     10/04/2021
 *
 *      This file contains the implementation of the UArray2.c file.
 *      All functions and structs used in UArray2.c are thus defined here.
 *
 *    
 *
 **************************************************************/

#include "uarray.h"
#include "uarray2.h"

/*
 * UArray2_new
 * 
 * Creates a new UArray2_T object. Yses the UArray_T interface to achieve
 * this. Information about the width and height is loaded into a 
 * struct malloced on the heap to ensure all information is
 * readily accessible
 * 
 * Parameters: The width and height of the two-dimensional UArray,
 *             and the maximum size (in bytes) of elements stored
 *             therein.
 * 
 * Expectations: Width and height are >= 0. Size is >= 1. CRE if these
 *               requirements are not met.
 */
UArray2_T UArray2_new(int width, int height, int size)
{
    assert(width >= 0);
    assert(height >= 0);
    assert(size>0);
    struct UArray2_T *uarr2 = malloc(sizeof(struct UArray2_T));
    assert(uarr2 != NULL);
    uarr2->uarray2d = UArray_new(height, sizeof(uarr2->uarray2d));
    assert(uarr2->uarray2d);
    for (int i = 0; i < height; i++) {
        *(UArray_T *)UArray_at(uarr2->uarray2d, i) = UArray_new(width, size);
    }
    uarr2->width = width;
    uarr2->height = height;
    uarr2->size = size;
    return uarr2;
}
/*
 * UArray2_width
 * 
 * Returns the width of the two-dimensional array.
 * 
 * Parameters: Reference to a UArray2_T object.
 * 
 * Expectations: The reference input is valid - i.e., not a null pointer.
 *               UArray2_T referred to has width and height >= 0.
 *               The output width must be a positive integer >= 0.
 *               CRE if these aren't met.
 */
int UArray2_width(UArray2_T UA2D)
{
    assert(UA2D != NULL);
    return UA2D->width;
}
/*
 * UArray2_height
 * 
 * Returns the height of the two-dimensional array.
 * 
 * Parameters: Reference to a UArray2_T object.
 * 
 * Expectations: The reference input is valid - i.e., not a null pointer.
 *               UArray2_T referred to has width and height >= 0.
 *               The output height must be a positive integer >= 0.
 *               CRE if these aren't met.
 */
int UArray2_height(UArray2_T UA2D)
{
    assert(UA2D != NULL);
    return UA2D->height;
}
/*
 * UArray2_size
 * 
 * Returns the size of the two-dimensional array.
 * 
 * Parameters: Reference to a UArray2_T object.
 * 
 * Expectations: The reference is valid - i.e., not a null pointer.
 *               UArray2_T referred to has width and height >= 0.
 *               The output size must be a positive integer > 0.
 */
int UArray2_size(UArray2_T UA2D)
{
    assert(UA2D != NULL);
    return UA2D->size;
}
/*
 * UArray2_at
 * 
 * Returns the element at the specified location. The UArray_T
 * interface is once again used to return the spot in the 2D array
 * we are looking for since this representation is a UArray_T
 * of UArray_T's
 * 
 * Parameters: A UArray2_T object, and the width and height at which the
 *             sought element is located.
 * 
 * Expectations: The reference to the UArray2_T is valid - i.e., not a null
 *               pointer. The referenced UArray2_T has width and height >= 0.
 *              
 */
void *UArray2_at(UArray2_T UA2D, int col, int row) 
{
    assert(UA2D != NULL);
    assert(col >= 0 && col < UA2D->width);
    assert(row >= 0 && row < UA2D->height);
    assert(UA2D->uarray2d != NULL);
    UArray_T *temp = UArray_at(UA2D->uarray2d, row);
    return UArray_at(*temp, col);
}
/*
 * UArray2_map_col_major
 * 
 * Traverse through the two-dimensional UArray and apply the function in the
 * second parameter to the element in the specified index. Column-oriented
 * traversal, meaning row numbers change faster than columns numbers.
 * 
 * Parameters: A UArray2_T object, a function pointer that does work on a
 *             specific element of the 2D array, void pointer pointing to
 *             an accumulator.
 * 
 * Expectations: The reference to the UArray2_T is valid - i.e., not a null
 *               pointer The referenced UArray2_T has width and height >= 0.
 *               The width and height parameters provided are >= 0.
 */

void UArray2_map_col_major(UArray2_T UA2D, 
                            void apply(int width, int height, UArray2_T UA2D, 
                            void *element, void *acc),
                            void *acc) 
{
    assert(UA2D != NULL);
    assert(apply != NULL);
    for (int i = 0; i < UA2D->width; i++) {
        for (int j = 0; j < UA2D->height; j++) {
            void *element = UArray2_at(UA2D, i, j);
           apply(i, j, UA2D, element, acc);
        }
    }
}
/*
 * UArray2_map_row_major
 * 
 * Traverse through the two-dimensional UArray and apply the function in the
 * second parameter to the element in the specified index. Row-oriented
 * traversal, meaning column numbers change faster than row numbers.
 * 
 * Parameters: A UArray2_T object, a function pointer that does work on a
 *             specific element of the 2D array, void pointer pointing to
 *             an accumulator.
 * 
 * Expectations: The reference to the UArray2_T is valid - i.e., not a null
 *               pointer The referenced UArray2_T has width and height >= 0.
 *         
 */
void UArray2_map_row_major(UArray2_T UA2D, 
                            void apply(int width, int height, UArray2_T UA2D, 
                            void *element, void *acc),
                            void *acc) 
{
    assert(UA2D != NULL);
    assert(apply != NULL);
    for (int j = 0; j < UA2D->height; j++) {
        for (int i = 0; i < UA2D->width; i++) {
            void *element = UArray2_at(UA2D, i, j);
           apply(i, j, UA2D, element, acc);
        }
    }
}
/*
 * UArray2_free
 * 
 * Frees up the memory associated with the UArray2_T object.
 * 
 * Parameters: Reference to a UArray2_T object.
 * 
 * Expectations: The reference is valid - i.e., not a null pointer.
 *               UArray2_T referred to has width and height >= 0.
 *               There's no expectation on output since this function
 *               doesn't return anything but rather does the work required
 *               inside the function itself. 
 */
void UArray2_free(UArray2_T *UA2D)
{
    assert(UA2D != NULL);
    for (int i = 0; i < (*UA2D)->height; i++) {
        UArray_free((UArray_T *)UArray_at((*UA2D)->uarray2d, i));
    }

    UArray_free((UArray_T *)&((*UA2D)->uarray2d));
    free(*UA2D);

}

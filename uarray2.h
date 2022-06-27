/**************************************************************
 *
 *                     UArray2.h
 *
 *     Assignment: iii
 *     Authors:  Jahansher Khan (jkhan03), Tom Barnett-Young (tbarne02)
 *     Date:     9/27/2021
 *
 *      This file contains the interface of the UArray2.h file.
 *      All functions and structs used in UArray2.h are thus defined here
 *
 *    
 *
 **************************************************************/


#ifndef __UArray2__
#define __UArray2__


#include "uarray.h"
#include "assert.h"
#include <stdlib.h>
#include <stdio.h>


struct UArray2_T {
    UArray_T uarray2d;
    int width;
    int height;
    int size;    
};
typedef struct UArray2_T *UArray2_T;

/*
 * UArray2_new
 * 
 * Creates a new UArray2_T object.
 * 
 * Parameters: The width and height of the two-dimensional UArray,
 *             and the maximum size (in bytes) of elements stored
 *             therein.
 * 
 * Expectations: Width and height are >= 0. Size is >= 1. CRE if these
 *               requirements are not met.
 */
UArray2_T UArray2_new(int width, int height, int size);

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
int UArray2_width(UArray2_T UA2D);

/*
 * UArray2_height
 * 
 * Returns the height of the two-dimensional array.
 * 
 * Parameters: Reference to a UArray2_T object.
 * 
 * Expectations: The reference is valid - i.e., not a null pointer.
 *               UArray2_T referred to has width and height >= 0.
 *               The output height must be a positive integer >= 0.
 *               CRE if these aren't met.
 */
int UArray2_height(UArray2_T UA2D);

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
int UArray2_size(UArray2_T UA2D);

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
void UArray2_free(UArray2_T *UA2D);

/*
 * UArray2_at
 * 
 * Returns a pointer to the element at the specified location.
 * 
 * Parameters: A UArray2_T object, and the width and height at which the
 *             sought element is located.
 * 
 * Expectations: The reference to the UArray2_T is valid - i.e., not a null
 *               pointer. The referenced UArray2_T has width and height >= 0.
 *              
 */
void *UArray2_at(UArray2_T UA2D, int col, int row);


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
                                  void apply(int col, int row, UArray2_T UA2D, 
                                             void *element, void *acc),
                                  void *acc);

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
 *               pointer. The referenced UArray2_T has width and height >= 0.
 *         
 */
void UArray2_map_row_major(UArray2_T UA2D, 
                                  void apply(int col, int row, UArray2_T UA2D, 
                                             void *element, void *acc),
                                  void *acc);






#undef UArray2_T
#endif
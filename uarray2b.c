/*
 * uarray2b.c
 *
 * Implementation file for uarray2b, a two-dimensional blocked uarray.
 *
 * It is a checked run-time error to pass a NULL T to any function in this 
 * interface.
 *
 * By: Jahansher Khan (jkhan03) and Tom Barnett-Young (tbarne02)
 */ 

#include "uarray2b.h"
#include "uarray2.h"
#include "uarray.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define T UArray2b_T

struct T {
    UArray2_T matrix;
    int width;
    int height;
    int size;
    int blockwidth;
    int blockheight;
    int blocksize;
};

/*
 * UArray2b_new
 * 
 * Creates a new blocked two-dimensional UArray
 * 
 * Parameters: the width and height of the array, the size of the elements, and
 *             the size (each dimension) of the blocks.
 * 
 * Returns: a UArray2b object
 * 
 * Expectations: width, height, and size are valid values for the new array,
 *               blocksize = square root of # of cells per block.
 *            
 *               blocksize < 1 is a checked runtime error 
 */
extern T UArray2b_new (int width, int height, int size, int blocksize) 
{
    /* Asserts */
    assert(width > 0);
    assert(height > 0);
    assert(size > 0);
    assert(blocksize > 0);

    /* Malloc space for the blocked array */
    T blockarr = malloc(sizeof(struct T));
    assert(blockarr != NULL);
    
    /* set some of the member variables of the blocked array */
    blockarr->width = width;
    blockarr->height = height;
    blockarr->size = size;
    blockarr->blocksize = blocksize;
    
    /* Calculate the remaining member variables of the blocked array */
    if (width % blocksize == 0) {
        blockarr->blockwidth = width / blocksize;
    } else {
        blockarr->blockwidth = (width / blocksize) + 1;
    }
    if (height % blocksize == 0) {
        blockarr->blockheight = height / blocksize;
    } else {
        blockarr->blockheight = (height / blocksize) + 1;
    }
    
    /* Set the UArray2_T member variable equal to a new UArray2 */
    blockarr->matrix = UArray2_new(blockarr->blockwidth, 
                                    blockarr->blockheight, sizeof(UArray_T));

    /* Loop through blocked array and initialize one-dimensional UArrays */
    for (int i = 0; i < blockarr->blockheight; i++) {
        for (int j = 0; j < blockarr->blockwidth; j++) {
            *(UArray_T *)UArray2_at(blockarr->matrix, j,
            i) = UArray_new(blocksize * blocksize, size);
        }
    }

    /* Return the created blocked array */
    return blockarr;
}

/*
 * UArray2b_new_64K
 * 
 * Creates a new blocked two-dimensional UArray with maximum blocksize, where
 * block occupies at most 64KB.
 * 
 * Parameters: the width and height of the array, and the size of the elements.
 * 
 * Returns: a UArray2b object.
 * 
 * Expectations: width, height, and size are valid values for the new array.
 *            
 */
extern T UArray2b_new_64K_block(int width, int height, int size) 
{
    /* Asserts */
    assert(width > 0);
    assert(height > 0);
    assert(size > 0);

    /* Check whether the element size is bigger than the max we want */
    if (size > 65536) {
        return UArray2b_new(width, height, size, 1);
    } else {
        return UArray2b_new(width, height, size, (int)sqrt((65536) / (size)));
    }
}

/*
 * UArray2b_free
 * 
 * Frees the space associated with the UArray2b
 * 
 * Parameters: a pointer to the UArray2b that is to be freed
 * 
 * Expectations: the pointer is valid
 *            
 */
extern void UArray2b_free (T *array2b) 
{
    assert(array2b != NULL);
    
    /* Go through and free the elements of the two-dimensional array */
    for (int i = 0; i < (*array2b)->blockheight; i++) {
        for (int j = 0; j < (*array2b)->blockwidth; j++) {
            UArray_free(UArray2_at((*array2b)->matrix, j, i));
        }
    }
    
    /* Free the matrix */
    UArray2_free(&(*array2b)->matrix);
    
    free(*array2b);
}

/*
 * UArray2b_width
 * 
 * Returns the width of the UArray2b
 * 
 * Parameters: the UArray2b to get the width of.
 * 
 * Returns: array width
 * 
 * Expectations: the passed UArray2b is valid
 *            
 */
extern int UArray2b_width (T array2b) 
{
    assert(array2b != NULL);
    return array2b->width;
}

/*
 * UArray2b_height
 * 
 * Returns the height of the UArray2b
 * 
 * Parameters: the UArray2b to get the height of.
 * 
 * Returns: array height
 * 
 * Expectations: the passed UArray2b is valid
 *            
 */
extern int UArray2b_height (T array2b) 
{
    assert(array2b != NULL);
    return array2b->height;
}

/*
 * UArray2b_size
 * 
 * Returns the element size of the UArray2b
 * 
 * Parameters: the UArray2b to get the element size of.
 * 
 * Returns: array element size
 * 
 * Expectations: the passed UArray2b is valid
 *            
 */
extern int UArray2b_size (T array2b) 
{
    assert(array2b != NULL);
    return array2b->size;

}

/*
 * UArray2b_blocksize
 * 
 * Returns the blocksize of the UArray2b
 * 
 * Parameters: the UArray2b to get the blocksize of.
 * 
 * Returns: array blocksize
 * 
 * Expectations: the passed UArray2b is valid
 *            
 */
extern int UArray2b_blocksize(T array2b) 
{
    assert(array2b != NULL);
    return array2b->blocksize;
}

/*
 * UArray2b_at
 * 
 * Return a pointer to the cell in the given column and row.
 * 
 * Parameters: the UArray2b, and the column and row.
 * 
 * Returns: void pointer to the sought cell
 * 
 * Expectations: the passed UArray2b is valid. Column and row are valid,
 *               in-bounds values
 *
 * index out of range is a checked run-time error
 *            
 */
extern void *UArray2b_at(T array2b, int column, int row) 
{
    assert(array2b != NULL);
    assert(column < array2b->width && column >= 0);
    assert(row < array2b->height && row >= 0);

    /* Get the block */
    UArray_T *temp = UArray2_at(array2b->matrix, column / array2b->blocksize, 
                     row / array2b->blocksize);

    /* Get the element within the block */
    return UArray_at(*temp, (array2b->blocksize * (row % array2b->blocksize)) + 
           column % array2b->blocksize);
    }

/*
 * UArray2b_map
 * 
 * Traverses the UArray2b in block-major order (meaning it traverses the blocks
 * in row-major order).
 * 
 * Parameters: the UArray2b to traverse, an apply function, and the closure
 *             argument.
 * 
 * Expectations: the passed UArray2b is valid.
 */
extern void UArray2b_map(T array2b, void apply(int col, int row, T array2b,
                         void *elem, void *cl), void *cl) 
{
    assert(array2b != NULL);
    assert(apply != NULL);
    
    /* Go through the rows (of blocks) of the UArray2b */
    for (int brow = 0; brow < array2b->blockheight; brow++) {
        
        /* Go through the columns (of blocks) of the UArray2b */
        for (int bcol = 0; bcol < array2b->blockwidth; bcol++) {
            UArray_T *block = (UArray_T *)UArray2_at(array2b->matrix, bcol,
                                                     brow);
            
            /*
             * Go through the blocks themselves - i.e., go through the elements
             * within the blocks in row-major order.
             */
            for (int k = 0; k < UArray_length(*block); k++) {
                int col = (k % array2b->blocksize) + 
                          (bcol * array2b->blocksize);
                int row = (k / array2b->blocksize) + 
                          (brow * array2b->blocksize);
                
                /* If column and row are in-bounds, call the apply function */
                if (col < array2b->width && row < array2b->height) {
                    apply(col, row, array2b, UArray_at(*block, k), cl);
                }
            }
        }
    }
}

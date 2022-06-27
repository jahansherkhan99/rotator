/*
 * ppmtrans.c
 *
 * Performs rotations, and other alterations, on image files.
 *
 * By: Jahansher Khan (jkhan03) and Tom Barnett-Young (tbarne02)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "cputiming.h"

struct Package {
        A2Methods_T methods;
        A2Methods_UArray2 *finaluarr;
};

/* Define SET_METHODS for use in main */
#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)

/* Define the functions we use in this program */
Pnm_ppm rotate_file(Pnm_ppm ppm_original, A2Methods_T methods, 
                    A2Methods_mapfun *map, int rotation, struct Package *mail, 
                    CPUTime_T timer, double *time_taken, char flip_value);
void rotate90(int i, int j, A2Methods_UArray2 ppm_original, void *elem, 
              void *cl);
void rotate180(int i, int j, A2Methods_UArray2 ppm_original, void *elem, 
              void *cl);
void rotate270(int i, int j, A2Methods_UArray2 ppm_original, void *elem, 
               void *cl); 
void flip_horizontal(int i, int j, A2Methods_UArray2 ppm_original, void *elem,
                     void *cl);
void flip_vertical(int i, int j, A2Methods_UArray2 ppm_original, void *elem,
                     void *cl);
void transpose (int i, int j, A2Methods_UArray2 ppm_original, void *elem,
                     void *cl);
void timing_output(Pnm_ppm my_ppm_original, double time_taken, 
                   FILE *timings_fp, A2Methods_T methods);

/* Print a message to the user indicating the correct usage of the 
   executable */
static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

/*
 * main
 * 
 * Runs the program, calling helper functions
 * 
 * Parameters: the command line arguments
 * 
 * Expectations: valid command line arguments passed in
 */
int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        int   i;

        (void) time_file_name;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        FILE *fp = NULL;
        FILE *timings_fp = NULL;
        char *flip_direction = NULL;
        char flip_value = 'r';

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        flip_value = 'r';
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-flip") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        flip_direction = argv[++i];
                        if (strcmp(flip_direction, "horizontal") == 0) {
                                flip_value = 'h';
                        } else if (strcmp(flip_direction, "vertical") == 0) {
                                flip_value = 'v';
                        } else {
                                fprintf(stderr,"Invalid options\n");
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-transpose") == 0) {
                        flip_value = 't'; 
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                        timings_fp = fopen(time_file_name, "a");
                        assert(timings_fp != NULL);
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        fp = fopen(argv[i], "rb");
                        if (fp == NULL) {
                                fprintf(stderr,"File did not open\n");
                                return EXIT_FAILURE;
                        }
                        break;
                }
        }

        if (fp == NULL) {
                fp = stdin;
                if (fp == NULL) {
                        fprintf(stderr,"File did not open\n");
                        return EXIT_FAILURE;
                }
        }

        /* instance of ppm stores the original image */
        Pnm_ppm my_ppm_original =  Pnm_ppmread(fp, methods);

        /* variables for timing */
        double time_taken;
        double *time_taken_ptr = &time_taken;

        /* if rotating 0 degrees */
        if (rotation == 0 && flip_value == 'r') {
            Pnm_ppmwrite(stdout, my_ppm_original);
            *time_taken_ptr = 0;
            /* if there is a timing file provided */
             if (timings_fp != NULL) {
                timing_output(my_ppm_original, time_taken, timings_fp, 
                              methods);
              }
            Pnm_ppmfree(&my_ppm_original);
            fclose(fp);
            return EXIT_SUCCESS;
        }

        /* struct that's 'mailed' to each apply 
        function with a final UArray2b/UArray2 
        and the methods */
        struct Package *mail = malloc(sizeof(*mail));
        assert(mail != NULL);
        CPUTime_T timer = CPUTime_New();
        assert(timer != NULL);

        /* declares a final ppm object that's updated in rotate_file 
        and finally returned */
        Pnm_ppm ppm_final;
        ppm_final = rotate_file(my_ppm_original,methods, map, rotation, 
                                mail, timer, time_taken_ptr, flip_value);
        if (timings_fp != NULL) {
                timing_output(my_ppm_original, time_taken, timings_fp,
                methods);
        }

        /* writes to standard output */
        Pnm_ppmwrite(stdout, ppm_final);


        Pnm_ppmfree(&ppm_final);
        Pnm_ppmfree(&my_ppm_original);
        free(mail);

        CPUTime_Free(&timer);
        fclose(fp);

        return EXIT_SUCCESS;
}

/*
 * rotate_file
 * 
 * Acts as a liason to all the mapping apply functions that rotate 90 degrees,
 * rotate 180 degrees, rotate 270 degrees, flip horizontally, flip vertically,
 * and transpose the original image
 * 
 * Returns: A Pnm_ppm object with the final object
 * 
 * Parameters: the width and height of the pixel, the two-dimensional array
 *             holding the original iamge, the current element, and the closure
 *             argument
 * 
 * Expectations: i, j are in-bounds. ppm_original is a valid non-null UArray2. 
 */
Pnm_ppm rotate_file(Pnm_ppm ppm_original, A2Methods_T methods, 
                    A2Methods_mapfun *map, int rotation, struct Package *mail,
                    CPUTime_T timer, double *time_taken, char flip_value) 
{
      assert(mail != NULL);
      assert(ppm_original != NULL);  
      assert(time_taken != NULL);
      assert(methods != NULL);
      assert(map != NULL);
      assert(timer != NULL);

      /* malloc the final ppm object to be returned*/
      Pnm_ppm ppm_final = malloc(sizeof(*ppm_final));
      assert(ppm_final != NULL);

      mail->methods = methods;
      ppm_final->denominator = ppm_original->denominator;
      int size = methods->size(ppm_original->pixels);

      if ((rotation == 90 || rotation == 270) && flip_value == 'r') {
                ppm_final->width = ppm_original->height;
                ppm_final->height = ppm_original->width;
                ppm_final->pixels = methods->new(ppm_final->width,
                                                 ppm_final->height, size); 
                mail->finaluarr = ppm_final->pixels;
                if (rotation == 90) {
                        CPUTime_Start(timer);
                        map(ppm_original->pixels, rotate90, mail);
                        *time_taken = CPUTime_Stop(timer);
                } else {
                        CPUTime_Start(timer);
                        map(ppm_original->pixels, rotate270, mail); 
                        *time_taken = CPUTime_Stop(timer);
                }
      } else if (rotation == 180 && flip_value == 'r') {
                ppm_final->width = ppm_original->width;
                ppm_final->height = ppm_original->height;
                ppm_final->pixels = methods->new(ppm_final->width,
                                                 ppm_final->height, size); 
                mail->finaluarr = ppm_final->pixels;

                CPUTime_Start(timer);
                map(ppm_original->pixels, rotate180, mail);
                *time_taken = CPUTime_Stop(timer);
        /* flip vertically or horizontally */
      } else if (flip_value == 'v' || flip_value == 'h') {
               ppm_final->width = ppm_original->width;
               ppm_final->height = ppm_original->height;
               ppm_final->pixels = methods->new(ppm_final->width,
                                                 ppm_final->height, size); 
               mail->finaluarr = ppm_final->pixels;
               if (flip_value == 'v') {
                     CPUTime_Start(timer);
                     map(ppm_original->pixels, flip_vertical, mail);
                     *time_taken = CPUTime_Stop(timer);
               } else if (flip_value == 'h') {
                    CPUTime_Start(timer);
                     map(ppm_original->pixels, flip_horizontal, mail);
                     *time_taken = CPUTime_Stop(timer);
               }
        /* transpose the image */
      } else if (flip_value == 't') {
                ppm_final->width = ppm_original->height;
                ppm_final->height = ppm_original->width;
                ppm_final->pixels = methods->new(ppm_final->width,
                                                 ppm_final->height, size); 
                mail->finaluarr = ppm_final->pixels;

                CPUTime_Start(timer);
                map(ppm_original->pixels, transpose, mail);
                *time_taken = CPUTime_Stop(timer);
      }
      ppm_final->methods = ppm_original->methods;

      return ppm_final;
}

/*
 * rotate90
 * 
 * Rotates the image 90 degrees clockwise
 * 
 * Parameters: the width and height of the pixel, the two-dimensional array
 *             holding the original iamge, the current element, and the closure
 *             argument
 * 
 * Expectations: i, j are in-bounds. ppm_original is a valid non-null UArray2. 
 */
void rotate90(int i, int j, A2Methods_UArray2 ppm_original, void *elem,
              void *cl)
{
       assert(elem != NULL);
       assert (cl != NULL);
       assert(ppm_original != NULL);
       (void) ppm_original;

       A2Methods_UArray2 *UArray2_new = ((struct Package *)cl)->finaluarr;
       A2Methods_T methods = ((struct Package *)cl)->methods;
       int col = methods->width(UArray2_new) - j - 1;
       *(Pnm_rgb)methods->at(UArray2_new, col, i) = *(Pnm_rgb)elem;
}

/*
 * rotate180
 * 
 * Rotates the image 180 degrees clockwise
 * 
 * Parameters: the width and height of the pixel, the two-dimensional array
 *             holding the original iamge, the current element, and the closure
 *             argument
 * 
 * Expectations: i, j are in-bounds. ppm_original is a valid non-null UArray2. 
 */
void rotate180(int i, int j, A2Methods_UArray2 ppm_original, void *elem,
               void *cl)
{
       assert(elem != NULL);
       assert (cl != NULL);
       assert(ppm_original != NULL); 
       (void) ppm_original;

       A2Methods_UArray2 *UArray_temp = ((struct Package *)cl)->finaluarr;
       A2Methods_T methods_temp = ((struct Package *)cl)->methods;
       int col = methods_temp->width(UArray_temp) - i - 1;
       int row = methods_temp->height(UArray_temp) - j - 1;
       *(Pnm_rgb)methods_temp->at(UArray_temp, col, row) = *(Pnm_rgb)elem;

}

/*
 * rotate270
 * 
 * Rotates the image 270 degrees clockwise
 * 
 * Parameters: the width and height of the pixel, the two-dimensional array
 *             holding the original iamge, the current element, and the closure
 *             argument
 * 
 * Expectations: i, j are in-bounds. ppm_original is a valid non-null UArray2. 
 */
void rotate270(int i, int j, A2Methods_UArray2 ppm_original, void *elem,
               void *cl) 
{
       assert(elem != NULL);
       assert (cl != NULL);
       assert(ppm_original != NULL); 
       (void) ppm_original;

       A2Methods_UArray2 *UArray2_new = ((struct Package *)cl)->finaluarr;
       A2Methods_T methods = ((struct Package *)cl)->methods;
       int row = methods->height(UArray2_new) - i - 1;
       *(Pnm_rgb)methods->at(UArray2_new, j, row) = *(Pnm_rgb)elem;

}

/*
 * flip_horizontal
 * 
 * Flips the image horizontally
 * 
 * Parameters: the width and height of the pixel, the two-dimensional array
 *             holding the original iamge, the current element, and the closure
 *             argument
 * 
 * Expectations: i, j are in-bounds. ppm_original is a valid non-null UArray2. 
 */
void flip_horizontal(int i, int j, A2Methods_UArray2 ppm_original, void *elem,
                     void *cl)
{
       assert(elem != NULL);
       assert (cl != NULL);
       assert(ppm_original != NULL); 
       (void) ppm_original;

       A2Methods_UArray2 *UArray_temp = ((struct Package *)cl)->finaluarr;
       A2Methods_T methods_temp = ((struct Package *)cl)->methods;
       int col = methods_temp->width(UArray_temp) - i - 1;
       *(Pnm_rgb)methods_temp->at(UArray_temp, col, j) = *(Pnm_rgb)elem;
  
}

/*
 * flip_vertical
 * 
 * Flips the image vertically
 * 
 * Parameters: the width and height of the pixel, the two-dimensional array
 *             holding the original iamge, the current element, and the closure
 *             argument
 * 
 * Expectations: i, j are in-bounds. ppm_original is a valid non-null UArray2. 
 */
void flip_vertical(int i, int j, A2Methods_UArray2 ppm_original, void *elem,
                     void *cl)
{
       assert(elem != NULL);
       assert (cl != NULL);
       assert(ppm_original != NULL); 
       (void) ppm_original;

       A2Methods_UArray2 *UArray_temp = ((struct Package *)cl)->finaluarr;
       A2Methods_T methods_temp = ((struct Package *)cl)->methods;
       int row = methods_temp->height(UArray_temp) - j - 1;
       *(Pnm_rgb)methods_temp->at(UArray_temp, i, row) = *(Pnm_rgb)elem;

}

/*
 * transpose
 * 
 * Transposes image across upper-left to lower-right axis
 * 
 * Parameters: the width and height of the pixel, the two-dimensional array
 *             holding the original iamge, the current element, and the closure
 *             argument
 * 
 * Expectations: i, j are in-bounds. ppm_original is a valid non-null UArray2. 
 */
void transpose (int i, int j, A2Methods_UArray2 ppm_original, void *elem,
                     void *cl)
{
       assert(elem != NULL);
       assert (cl != NULL);
       assert(ppm_original != NULL); 
       (void) ppm_original;

       A2Methods_UArray2 *UArray_temp = ((struct Package *)cl)->finaluarr;
       A2Methods_T methods_temp = ((struct Package *)cl)->methods;
       *(Pnm_rgb)methods_temp->at(UArray_temp, j, i) = *(Pnm_rgb)elem;

}

/*
 * timing_output
 * 
 * Produces timing output for the file operations
 * 
 * Parameters: the original ppm file, the time taken, the file to
 *             which timing output is written, and the A2Methods_T object.
 * 
 * Expectations: all parameters passed in are valid. 
 */
void timing_output(Pnm_ppm my_ppm_original, double time_taken, 
                   FILE *timings_fp, A2Methods_T methods) {
        assert(my_ppm_original != NULL);
        assert(methods != NULL);
        assert(timings_fp != NULL);

        int width_count = methods->width(my_ppm_original->pixels);
        int height_count = methods->height(my_ppm_original->pixels);
        int pixel_count = width_count * height_count;
        double time_per_pixel = time_taken/pixel_count;

        fprintf(timings_fp, "Total Time Taken: %f\n", time_taken);
        fprintf(timings_fp, "Time Per Pixel: %f\n", time_per_pixel); 
        fclose(timings_fp);
}
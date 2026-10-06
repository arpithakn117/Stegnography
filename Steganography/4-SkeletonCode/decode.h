#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname; //store the output_img_fname
    FILE *fptr_stego_image; //file pointer for output image

    /* Output Image info */
    char *output_image_fname;  //store the out_image_filename
    FILE *fptr_out_image;   //file pointer for out_image                    
    //int image_file_size;

    int d_extn_size;
    int d_file_size;
    // char *d_secret_file;
    // FILE *fptr_sec_file;

} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_bmp_file(DecodeInfo *decInfo);

/* skip bmp image header */
Status skip_bmp_header(FILE *fptr_stego_image);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extension size   */
Status decode_secret_file_extn_size(int d_extn_size,DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(int file_size, DecodeInfo *decInfo);

/* Decode secret file data*/ 
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
char decode_bytes_from_lsb(char *buffer);

/* Decode the size to lsb   */
int decode_size_from_lsb(char *buffer);     


#endif
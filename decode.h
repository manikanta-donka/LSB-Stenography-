#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 10

typedef struct DecodeInfo
{
    /* Stego image info */
    char *stego_fname;          // To store the stego image file name
    FILE *fptr_stego;           // File pointer for stego image file

    /* Output file info */
    char out_fname[20];         // Output file name for decoded data
    int out_flag;               // Flag to indicate whether user is providing output file name or not
    FILE *fptr_output;          // File pointer for output file
    /* Secret file info */
    uint secret_extn_length;    // Secret file extn length 
    char secret_extn[10];       // Secret file extn
    uint secret_size;           // Secret file size

}DecodeInfo;
      //Datatype of the structure


/* Decoding function prototype */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status open_stego_file(DecodeInfo *decInfo);

Status open_output_file(DecodeInfo *decInfo);

Status skip_header(FILE *fptr);

Status decode_magic_string(const char *user_string, DecodeInfo *decInfo);

Status decode_secret_file_extn_size(DecodeInfo *decInfo);

Status decode_secret_file_extention(DecodeInfo *decInfo);

Status decode_secret_file_size(DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

char decode_lsb_to_byte(char *image_buffer);

uint decode_lsb_to_size(char *image_buffer);

#endif
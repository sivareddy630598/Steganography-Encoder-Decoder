#ifndef DECODE_H
#define DECODE_H
#include<stdio.h>
#include"types1.h"
#include"types.h"
#include"common.h"

typedef struct _DecodeInfo
{
    /* Stego image details */
    char *stego_image_fname;       // Name/path of the stego image file
    FILE *fptr_stego_image;       // File pointer to open stego image


    /* Output file details */
    char *output_fname;            // Name/path of the file where decoded secret data will be written
    FILE *fptr_output_file;       // File pointer to write the decoded secret data

    /* Decoded data */
    char extn_secret_file[10];  // Stores the decoded file extension (e.g., ".txt", ".c")
    int size_secret_file;      // Stores the decoded size of the secret file (in bytes)
} DecodeInfo;
//Validate command-line arguments for decoding
Status1 read_and_validate_decode_file(char* argv[],DecodeInfo*decInfo);
/* Main decoding controller function */
Status1 do_decoding(DecodeInfo *decInfo);

/* Opens required files (stego image and output file) */
Status1 open_files_decode(DecodeInfo *decInfo);

/* Skips BMP file header (first 54 bytes) to reach encoded data region */
Status1 skip_bmp_header(FILE *fptr_stego_image);

// Decodes a single byte from 8 LSBs of the image buffer
char decode_byte_from_lsb(char *image_buffer);
// Decodes an integer size (e.g., secret file size or extension length) from image data
int decode_size_from_lsb(char *image_buffer);
//  Decode the embedded magic string to verify stego file authenticity
Status1 decode_magic_string(DecodeInfo *decInfo);
//Decode the size of the secret file’s extension (e.g., 4 for ".txt")
Status1 decode_secret_file_extn_size(DecodeInfo *decInfo, int *extn_size);
// Decode the actual file extension string
Status1 decode_secret_file_extn(DecodeInfo *decInfo, int extn_size);
// Decode the size of the secret file data (in bytes)
Status1 decode_secret_file_size(DecodeInfo *decInfo);
// Decode the actual secret file data from the stego image
Status1 decode_secret_file_data(DecodeInfo *decInfo);

#endif
#include <stdio.h> 
#include "decode.h"
#include "types1.h"
#include<string.h>
#include "common.h"
// Function: read_and_validate_decode_file
// Description:
//   Validates command-line arguments for decoding. 
//   Ensures that the stego (source) file has a .bmp extension
//   and the output file has a valid text/code extension.
Status1 read_and_validate_decode_file(char* argv[],DecodeInfo* decInfo)
{
     // Validate source image extension (.bmp)
      // Find last '.' in file name
    char* source = (strrchr(argv[2],'.')); 
    if(source == NULL || strcmp(source,".bmp") != 0)
    {
        return d_failure;// Invalid or missing .bmp extension
    }
    else
    {
       decInfo->stego_image_fname = argv[2];// Store the image file name
    }
    // Validate output file extension (.txt, .c, .csv, .sh)
     char* secret = (strrchr(argv[3],'.'));//to check the last dot and store the address
    if(secret == NULL || (strcmp(secret, ".txt") != 0 
             && strcmp(secret, ".c") != 0 
             && strcmp(secret, ".csv") != 0
             && strcmp(secret, ".sh") != 0))
    {
        return d_failure;// Invalid extension for output file
    }
    else
    {
        decInfo->output_fname = argv[3];// Store the output file name
    }
    
   return d_success;
}
// Function: open_files_decode
// Description:
//   Opens the stego image file in binary read mode

Status1 open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname,"rb");
    if(decInfo->fptr_stego_image == NULL)
    {
         printf("Error: Cannot open stego image file %s\n", decInfo->stego_image_fname);
        return d_failure;
    }
    return d_success;
}
// Function: skip_bmp_header
// Description:
//   Skips the BMP header (first 54 bytes) before decoding data.
Status1 skip_bmp_header(FILE *fptr_stego_image)
{
    fseek(fptr_stego_image,54,SEEK_SET);
    if(ftell(fptr_stego_image) != 54)
    {
        return d_failure;
    }
    return d_success;
}
// Function: decode_byte_from_lsb
// Description:
// Extracts 1 byte of data from the LSBs of 8 image bytes.
char decode_byte_from_lsb(char *image_buffer)
{
    char data = 0;
    for(int i=0;i<8;i++)
    {
        int bit = image_buffer[i] & 1; // Extract LSB
        data=(data<<1) | bit;           // Append bit to data
    }
    return data;
}
// Function: decode_magic_string
// Description:
//   Decodes and verifies the hidden magic string to confirm
//   the image contains hidden data.
Status1 decode_magic_string(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char magic[strlen(MAGIC_STRING) + 1];
     // Decode each character of magic string
    for(int i=0;i<strlen(MAGIC_STRING);i++)
    {
        fread(image_buffer,1,8,decInfo->fptr_stego_image);
        magic[i] = decode_byte_from_lsb(image_buffer);

    }
    
    magic[strlen(MAGIC_STRING)] = '\0';
    // Compare decoded magic string with expected value
    if(strcmp(magic, MAGIC_STRING) == 0)
    {
        return d_success;
    }
    return d_failure;
}
// Function: decode_size_from_lsb
// Description:
//   Extracts a 32-bit integer value from 32 image bytes.
int decode_size_from_lsb(char* image_buffer)
{
    int data=0;
    for(int i=0;i<32;i++)
    {
        int bit = image_buffer[i] & 1;
        data = (data<<1) | bit;
    }
    return data;
}
// Function: decode_secret_file_extn_size
// Description:
//   Reads 32 bytes and decodes the size of secret file extension.
Status1 decode_secret_file_extn_size(DecodeInfo *decInfo, int *extn_size)
{
    char image_buffer[32];
    size_t bytesRead;
    bytesRead = fread(image_buffer,1,32,decInfo->fptr_stego_image);
    if(bytesRead != 32)
    {
        return d_failure;
    }
    *extn_size = decode_size_from_lsb(image_buffer);
    return d_success;
}
// Function: decode_secret_file_extn
// Description:
//   Decodes the actual extension of the secret file (.txt, .c etc.)
//   and creates the output file accordingly.
Status1 decode_secret_file_extn(DecodeInfo *decInfo, int extn_size)
{
    char image_buffer[8];
    int i;
    // Decode each character of extension
    for(i=0;i<extn_size;i++)
    {
        fread(image_buffer,1,8,decInfo->fptr_stego_image);
        decInfo->extn_secret_file[i] = decode_byte_from_lsb(image_buffer);
    }
    decInfo->extn_secret_file[i] = '\0';// Null-terminate string
     // Remove old extension from output file name
    char* dot = strrchr(decInfo->output_fname,'.');
    if(dot != NULL)
    {
        *dot = '\0';
    }
    // Append decoded extension
     strcat(decInfo->output_fname,decInfo->extn_secret_file);
      // Create new output file to store decoded data
    decInfo->fptr_output_file = fopen(decInfo->output_fname,"w");
    if(decInfo->fptr_output_file == NULL)
    {
        printf("Error: Cannot create file %s\n", decInfo->output_fname);
        return d_failure;
    }
    if(extn_size == strlen(decInfo->extn_secret_file))
    {
        return d_success;
    }
   
    return d_failure;
}
// Function: decode_secret_file_size
// Description:
//   Decodes the total size (in bytes) of the secret data.
Status1 decode_secret_file_size(DecodeInfo *decInfo)
{
    char image_buffer[32];
    size_t bytesRead;
   bytesRead = fread(image_buffer,1,32,decInfo->fptr_stego_image);
   if(bytesRead != 32)
    {
        return d_failure;
    }
    decInfo->size_secret_file = decode_size_from_lsb(image_buffer);
    return d_success;

}
// Function: decode_secret_file_data
// Description:
//   Decodes the actual hidden data from image and writes it
//   into the output file character by character.
Status1 decode_secret_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8];
    size_t bytesRead;
    for(int i=0;i<decInfo->size_secret_file;i++)
    {
       bytesRead = fread(image_buffer,1,8,decInfo->fptr_stego_image);
        if (bytesRead != 8)
        {
            return d_failure;  
        }

        char ch = decode_byte_from_lsb(image_buffer);

        if (fputc(ch, decInfo->fptr_output_file) == EOF)
        {
            return d_failure;  
        }

    }
    return d_success;
}
// Function: do_decoding
// Description:
//   Main controller function that coordinates all decoding steps.
Status1 do_decoding(DecodeInfo *decInfo)
{
    // Step 1: Open stego image file
     Status1 res = open_files_decode(decInfo);  
     if(res == d_failure)
     {
        printf("Error: File does not exist!\n");
        return d_failure;  
     } 
      // Step 2: Skip BMP header
    res = skip_bmp_header(decInfo->fptr_stego_image);
     if(res == d_failure)
     {
        printf("Error: skip_bmp_header is failure!\n");
        return d_failure;  
     }
     else
     {
        printf("BMP header skipped successfully!\n");

     }
      // Step 3: Decode and verify magic string
     res = decode_magic_string(decInfo);
     if(res == d_failure)
     {
        printf("Error: Magic string does not match!\n");
        return d_failure;
     }
     else
     {
        printf("Success: Magic string matched!\n");
     }
     // Step 4: Decode secret file extension size
     int extn_size=0;
      
     res = decode_secret_file_extn_size(decInfo,&extn_size);
     if(res == d_failure)
     {
        printf("Error: decode_secret_file_extn_size failure!\n");
        return d_failure;
     }
     else
     {
        printf("Success: decode_secret_file_extn_size!\n");
     }
      // Step 5: Decode secret file extension
     res = decode_secret_file_extn(decInfo,extn_size);
      if(res == d_failure)
     {
        printf("Error: decode_secret_file_extn failure!\n");
        return d_failure;
     }
     else
     {
        printf("Success: decode_secret_file_extn!\n");
     }
     // Step 6: Decode secret file size
     res = decode_secret_file_size(decInfo);
     if(res == d_failure)
     {
        printf("Error: decode_secret_file_size failure!\n");
        return d_failure;
     }
     else
     {
        printf("Success: decode_secret_file_size!\n");
     }
     // Step 7: Decode secret file data
     res = decode_secret_file_data(decInfo);
      if(res == d_failure)
     {
        printf("Error: decode_secret_file_data failure!\n");
        return d_failure;
     }
     else
     {
        printf("Success: decode_secret_file_data!\n");
     }
      // Close opened files
     fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output_file);
     return d_success;
}



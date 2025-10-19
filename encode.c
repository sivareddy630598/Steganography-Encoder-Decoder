#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);
    //After to calculate to reset pointer 
   fseek(fptr_image, 0, SEEK_SET);
    // Return image capacity
    return width * height * 3;
}
// Function to calculate the size of a file in bytes
uint get_file_size(FILE *fptr)
{
    char ch; // variable to hold each character read from the file
    // counter to count the total number of bytes
    int count = 0 ;  //fseek(fptr,0,SEEK_END);uint size = ftell(fptre) return size;
     // Read each character one by one until end of file (EOF)
    while((ch = fgetc(fptr)) != EOF)
    {
        count++;// increment count for each character read
    }
     // After reading, file pointer is now at the end of file.
    // Move it back to the beginning so other functions can read again.
    fseek(fptr, 0, SEEK_SET);
     // Return total number of bytes read (i.e., file size)
    return count;
    
}

// Function Name : read_and_validate_encode_args
// Description    : Validates the command-line arguments passed to the encoder
// Parameters     : 
//     char *argv[]      → command-line argument list
//     EncodeInfo *encInfo → structure to store file names
// Returns        : e_success (if all arguments are valid)
//                  e_failure (if any validation fails)

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // 1. Validate the source image file (argv[2])
    // Extract file extension from the source image name
    char* source = (strrchr(argv[2],'.'));
    // If there is no extension OR it's not ".bmp", fail
    if(source == NULL || strcmp(source,".bmp") != 0)
    {
        return e_failure;
    }
    else
    {
        // Store the valid image file name in structure
        encInfo->src_image_fname = argv[2];
    }
    // 2. Validate the secret file (argv[3])

    char* secret = (strrchr(argv[3],'.'));
    // Check if secret file extension is one of allowed types
    if(secret == NULL || (strcmp(secret, ".txt") != 0 
        &&  strcmp(secret, ".c") != 0 
        && strcmp(secret, ".csv") != 0 
        && strcmp(secret, ".sh") != 0))
    {
          // Invalid secret file format
        return e_failure;
    }
    else
    {
         // Store the valid secret file name
        encInfo->secret_fname = argv[3];
    }
    // 3. Validate or assign default stego image file (argv[4])
     // If user did not specify output file, use default name
    if(argv[4] == NULL)
    {
        
        encInfo->stego_image_fname = "default.bmp";
    }
    else
    {
         // Extract the extension of destination file
        char* des = (strrchr(argv[4],'.'));
          // Ensure destination file has a .bmp extension
        if(des == NULL || strcmp(des,".bmp") != 0)
        {
            return e_failure;
        }
        else
        {
             // Store the valid output file name
            encInfo->stego_image_fname = argv[4];
        }
    }
      // If all validations pass, return success
    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}
// Function Name : check_capacity
// Description    : Checks whether the source image has enough capacity to store
//                  all the data of the secret file (including metadata and magic string)
// Parameters     : EncodeInfo *encInfo → structure containing all encoding information
// Returns        : e_success (if capacity is sufficient)
//                  e_failure (if image capacity is insufficient)
Status check_capacity(EncodeInfo *encInfo)
{
    // 1. Get the size (capacity) of the BMP image file
     // This function calculates the total number of bytes in the BMP image.
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
     // Example: For a 1024x768 24-bit BMP → image_capacity = width * height * 3 = 2,359,296 bytes
    
      // 2. Get the size of the secret file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    // Counts how many bytes are in the secret file (e.g., text, .c, .csv, etc.)
    // 3. Extract the file extension of the secret file
     // Finds the last occurrence of '.' in the filename (like ".txt" or ".c")
    char *extn = strrchr(encInfo->secret_fname,'.');
     // Copies the extension into the structure for later encoding
    strcpy(encInfo->extn_secret_file, extn);

    // 4. Calculate total number of bytes required to encode
    // Breakdown of what needs to be stored inside the image:
    //   - 54 bytes : BMP header (not encoded; starts from pixel data)
    //   - strlen(MAGIC_STRING)*8 bits : magic string (to identify encoded image)
    //   - 4 bytes : size of secret file extension
    //   - strlen(extn)*8 bits : extension itself (.txt, .c, etc.)
    //   - 4 bytes : size of secret file
    //   - secret file data (each byte needs 8 bits to encode)
    // Multiply by 8 because each bit of data is stored in one byte of image data (LSB).
    uint total_required_bytes = 54 + ((encInfo->size_secret_file 
        + 4 
        + (strlen(encInfo->extn_secret_file))
        + 4 
        + strlen(MAGIC_STRING)) * 8);
    
      // 5. Check if image can hold all required data
    if(encInfo->image_capacity < total_required_bytes)
    {
        // Not enough space in BMP to hide the data
        return e_failure;
    }
     // Sufficient capacity — encoding can proceed
    return e_success;
}

// Function Name : copy_bmp_header
// Description    : Copies the 54-byte header from the source BMP image file
//                  to the destination (stego) BMP image file.
//                  This preserves image metadata (dimensions, bit depth, etc.)
// Parameters     : 
//      fptr_src_image   - File pointer to the original BMP image
//      fptr_stego_image - File pointer to the new BMP image (output)
// Returns        : 
//      e_success - If header copied successfully
//      e_failure - If copy fails (mismatch in bytes read/written)
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // 1. Declare a buffer to hold the BMP header (54 bytes)
    unsigned char header[54];
    size_t bytesRead, bytesWritten;
      // 2. Read the header from the source BMP file
      // fread() reads 1 block of 54 bytes from fptr_src_image into header[]
    bytesRead = fread(header,54,1,fptr_src_image);
     // 3. Write the same header to the destination (stego) BMP file
      // fwrite() writes 1 block of 54 bytes from header[] into fptr_stego_image
     bytesWritten = fwrite(header,54,1,fptr_stego_image);
     // If the number of blocks read ≠ number written → failure
    if(bytesWritten !=  bytesRead)
    {
        return e_failure;
    }
    // Header successfully copied
    return e_success;
}
// Function Name : encode_byte_to_lsb
// Description    : Encodes (hides) one byte of data into 8 bytes of image data 
//                  using Least Significant Bit (LSB) substitution technique.
// Parameters     : 
//      data          → The secret byte (8 bits) to encode.
//      image_buffer  → Pointer to 8 bytes of image data (each pixel byte).
// Returns        : 
//      e_success     → If encoding completes successfully.
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Loop through each bit of the data byte (from MSB to LSB)
    for(int i=0;i<8;i++)
    {
        // (data >> (7 - i)) shifts the bits so the required bit comes to LSB position.
        // & 1 isolates that bit.
        int bit=(data>>(7-i))&1;
         // image_buffer[i] & ~1 sets the last bit to 0 (clear it)
        int clear=image_buffer[i] & ~1;
         // Add the secret bit to LSB position using OR operation
        clear|=bit;
         //Store the modified byte back into image_buffer
        image_buffer[i]=clear;
    }
    // Successfully encoded one byte of data into 8 bytes of image data
    return e_success;
}
// Function Name : encode_size_to_lsb
// Description    : Encodes a 4-byte integer (size) into 32 bytes of image data
//                  using Least Significant Bit (LSB) substitution.
// Parameters     : 
//      size          → Integer value to encode (e.g., file size or string length)
//      image_buffer  → Pointer to 32 bytes of image data
// Returns        : 
//      e_success     → If encoding completes successfully
Status encode_size_to_lsb(int size, char *image_buffer)
{
    // Loop through all 32 bits of the integer (4 bytes = 32 bits)
    for(int i=0;i<32;i++)
    {
        // Shifting ensures we start from the Most Significant Bit (MSB)
        // '& 1' isolates that bit (either 0 or 1)
        int bit=(size>>(31-i))&1;
         // '& ~1' masks out the LSB (sets it to 0)
        int clear=image_buffer[i] & ~1;
         // Set the LSB of the image byte to the extracted bit
        clear|=bit;
         // Store the modified byte back into image buffer
        image_buffer[i]=clear;
    }
     // Successfully encoded integer value into 32 image bytes
    return e_success;
}
/**
 * Function: encode_magic_string
 * -----------------------------
 * Embeds a special identifier string (magic string) into the image file.
 * This string helps the decoder identify that the image contains hidden (encoded) data.
 *
 * Parameters:
 *   magic_string - A constant string that uniquely identifies the presence of hidden data.
 *   encInfo      - Pointer to the EncodeInfo structure containing file pointers
 *                  for the source image and the output (stego) image.
 *
 * Returns:
 *   e_success - If the magic string is successfully encoded.
 *   e_failure - If there is any mismatch in reading or writing image data.
 */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
     // Temporary buffer to hold 8 bytes from the image
     char image_buffer[8];
     size_t bytesRead, bytesWritten;
     // Loop through each character of the magic string
    for(int i=0;magic_string[i] != '\0';i++)
    {
         // Step 1: Read 8 bytes from the source image file
       bytesRead = fread(image_buffer,8,1,encInfo->fptr_src_image);
       // Step 2: Encode the current character into the 8 bytes
        // Each bit of the character is stored in the least significant bit of each byte
       encode_byte_to_lsb(magic_string[i],image_buffer);
        // Step 3: Write the modified 8 bytes into the stego (output) image
       bytesWritten = fwrite(image_buffer,8,1,encInfo->fptr_stego_image);
        // Step 4: Verify if the read and write operations were successful
        if(bytesWritten != bytesRead)
        {
            return e_failure;// Return failure if data mismatch occurs
        }
    }
     // If all characters are encoded successfully, return success
    return e_success;
}
/**
 * Function: encode_secret_file_extn_size
 * --------------------------------------
 * Encodes the size (length) of the secret file’s extension (like ".txt", ".c", etc.)
 * into the least significant bits (LSBs) of the image data.
 *
 * Parameters:
 *   size     - Integer value representing the length of the secret file extension.
 *   encInfo  - Pointer to the EncodeInfo structure containing file pointers for
 *              the source (original) and stego (encoded) image files.
 *
 * Returns:
 *   e_success - If encoding is successful.
 *   e_failure - If file read/write operation fails.
 */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
     // Buffer to hold 32 bytes of image data
        char image_buffer[32];
        // Step 1: Read 32 bytes from the source image
       // We need 32 bytes because an integer has 32 bits (1 bit stored in each byte)
        size_t bytesRead, bytesWritten;
       bytesRead = fread(image_buffer,32,1,encInfo->fptr_src_image);
       // Step 2: Encode the 'size' value (integer) into the 32 bytes
      // Each bit of 'size' is hidden in the least significant bit of one byte
       encode_size_to_lsb(size,image_buffer);
       // Step 3: Write the modified 32 bytes into the stego (output) image
        bytesWritten = fwrite(image_buffer,32,1,encInfo->fptr_stego_image);
        // Step 4: Verify if read and write operations matched successfully
        if(bytesWritten != bytesRead)
        {
            return e_failure;// Return failure if data mismatch occurs
        }
            // Step 5: Return success when encoding completes properly
        return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
     // Buffer to hold 8 bytes of image data
     char image_buffer[8];
     size_t bytesRead, bytesWritten;
     // Loop through each character in the secret file extension
    for(int i=0;file_extn[i] != '\0';i++)
    {
        // Step 1: Read 8 bytes from the source image
        // Each character (8 bits) requires 8 bytes to store its bits in the LSBs
       bytesRead = fread(image_buffer,8,1,encInfo->fptr_src_image);
        // Step 2: Encode the current character into the image buffer
        // Each bit of the character is hidden in the least significant bit of one byte
       encode_byte_to_lsb(file_extn[i],image_buffer);
        // Step 3: Write the modified bytes to the stego (output) image
       bytesWritten = fwrite(image_buffer,8,1,encInfo->fptr_stego_image);
        // Step 4: Check for read/write consistency
        if(bytesWritten != bytesRead)
        {
            return e_failure;// Return failure if read/write mismatch occurs
        }
    }
     // Step 5: Return success if all characters are encoded successfully
    return e_success;
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    // Buffer to hold 32 bytes from the image
    char image_buffer[32];
     // Step 1: Read 32 bytes from the source image
    // Each bit of the 32-bit integer will be stored in the LSB of each byte
        size_t bytesRead, bytesWritten;
       bytesRead = fread(image_buffer,32,1,encInfo->fptr_src_image);
       // Step 2: Encode the integer 'file_size' into the 32 bytes
       encode_size_to_lsb(file_size,image_buffer);
       // Step 3: Write the modified 32 bytes into the stego image
        bytesWritten = fwrite(image_buffer,32,1,encInfo->fptr_stego_image);
        // Step 4: Verify that read and write counts match
        if(bytesWritten != bytesRead)
        {
            return e_failure;// Return failure if mismatch occurs
        }
        // Step 5: Return success if encoding completed properly
        return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // Temporary buffer to hold 8 bytes of image data
    char image_buffer[8];
     size_t bytesRead, bytesWritten;
     // Loop through each byte of the secret file
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        // Step 1: Read 8 bytes from the source image
        // Each byte of secret data requires 8 image bytes (1 bit per byte)
       bytesRead = fread(image_buffer,8,1,encInfo->fptr_src_image);
       // Step 2: Encode one byte of secret data into the 8 image bytes
       encode_byte_to_lsb(encInfo->secret_data[i],image_buffer);
        // Step 3: Write the modified 8 bytes into the stego (output) image
       bytesWritten = fwrite(image_buffer,8,1,encInfo->fptr_stego_image);
       // Step 4: Verify that read/write counts match
        if(bytesWritten != bytesRead)
        {
            return e_failure;// Return failure if mismatch occurs
        }
    }
    // Step 5: Return success after all secret bytes are encoded
    return e_success;
}
// Function: copy_remaining_img_data
// Purpose: Copies the remaining data from source image file to destination image file
// Returns: e_success if copying is successful, otherwise e_failure

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    // Temporary buffer to store data chunks while copying
   char buffer[1024];
    // Read data from source file in chunks of 1024 bytes
   size_t bytesRead, bytesWritten;
   while((bytesRead = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0)
   {
    // Write the same data chunk into the destination file
        bytesWritten = fwrite(buffer, 1, bytesRead, fptr_dest);
        // Check if number of bytes written equals number of bytes read
        if(bytesWritten != bytesRead)
        {
            return e_failure;   // If not equal, an error occurred during writing
        }
   }
   // If all data is copied successfully, return success
   return e_success;
}
// Function: do_encoding
// Purpose : Performs the entire encoding process for steganography
// Input   : Pointer to EncodeInfo structure containing file pointers and secret data
// Returns : e_success if encoding completes successfully, otherwise e_failure

Status do_encoding(EncodeInfo *encInfo)
{
    // Step 1: Open all necessary files (source image, secret file, output image)
    Status res = open_files(encInfo);
    if(res == e_failure)
    {
        printf("Error: File does not exist!\n");
        return e_failure;  
    }
     // Step 2: Check if source image has enough capacity to hold secret data
    res = check_capacity(encInfo);
    if(res == e_failure)
    {
        printf("Error: Image file size should be greater than the secret file size!\n");
        return e_failure;
    }
    // Step 3: Copy BMP header (first 54 bytes) from source to output (stego) image
    res = copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    if( res == e_failure)
    {
        printf("Error: Header file does not store in output image file!\n");
        return e_failure;
    }
    else
    {
        printf("Header file stored successfully!\n");
    }
    // Step 4: Encode the MAGIC STRING into the image to mark the beginning of hidden data
    res = encode_magic_string(MAGIC_STRING,encInfo);
    if( res == e_failure)
    {
        printf("Error: Failed to encode magic string!\n");
        return e_failure;
    }
    else
    {
        printf("Magic string encoded successfully.\n");
    }
     // Step 5: Encode the size of the secret file’s extension
    res = encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo);
     if( res == e_failure)
    {
        printf("Error: Failed to encode secret file extrn size!\n");
        return e_failure;
    }
    else
    {
        printf("Secret file extrn size encoded successfully.\n");
    }
     // Step 6: Encode the actual secret file extension (e.g., ".txt", ".c")
    res = encode_secret_file_extn(encInfo->extn_secret_file,encInfo);
    if( res == e_failure)
    {
        printf("Error: Failed to encode secret file extrn !\n");
        return e_failure;
    }
    else
    {
        printf("Secret file extrn  encoded successfully.\n");
    }
    // Step 7: Encode the size (in bytes) of the secret file
    res = encode_secret_file_size(encInfo->size_secret_file,encInfo);
    if( res == e_failure)
    {
        printf("Error: Failed to encode secret file size !\n");
        return e_failure;
    }
    else
    {
        printf("Secret file size encoded successfully.\n");
    }
   /* int ch;
    int i=0;
    while((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
        encInfo->secret_data[i++] = (char)ch;   //fread to use store
    }*/
   // Step 8: Read the entire secret file content into memory
   fread(encInfo->secret_data,encInfo->size_secret_file,1,encInfo->fptr_secret);
    // Reset the file pointer of the secret file to the beginning
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    // Step 9: Encode the actual secret file data into the image
    res = encode_secret_file_data(encInfo);
    if( res == e_failure)
    {
        printf("Error: Failed to encode secret file data !\n");
        return e_failure;
    }
    else
    {
        printf("Secret file data encoded successfully.\n");
    }
    // Step 10: Copy remaining (unused) image data to the stego image
    res = copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image);
    if( res == e_failure)
    {
        printf("Error: Failed to encode remaining image data !\n");
        return e_failure;
    }
    else
    {
        printf("Remaining image data encoded successfully.\n");
    }
    // Step 11: Close all opened files to free resources
    if(encInfo->fptr_src_image)
        fclose(encInfo->fptr_src_image);
    if(encInfo->fptr_secret)
        fclose(encInfo->fptr_secret);
    if(encInfo->fptr_stego_image)
        fclose(encInfo->fptr_stego_image);

     // Step 12: Return success if all encoding steps completed successfully
   return e_success;
}

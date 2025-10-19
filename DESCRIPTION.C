/*
=====================================================================================
PROJECT TITLE  : IMAGE STEGANOGRAPHY USING LSB METHOD
FILE NAME      : description.c
AUTHOR         : K.SIVA PRASAD REDDY
DATE           : 11-10-2025
TECHNOLOGY     : C Programming, File Handling, Bitwise Operations
TOOLS USED     : Code::Blocks / VS Code / GCC Compiler
=====================================================================================

PROJECT OVERVIEW
----------------
This project implements a simple **Image Steganography system** in C language. 
It allows users to **hide (encode)** secret data (text/code files) inside a 
BMP image file and later **extract (decode)** that data from the same image. 

The technique used is **Least Significant Bit (LSB)** encoding, where the 
least significant bit of each byte in the image is modified to store the 
bits of the secret data. The change is imperceptible to the human eye.

=====================================================================================
FEATURES
--------
1. Encode secret data (text or code) into a 24-bit BMP image file.
2. Decode and retrieve the hidden secret data from the stego image.
3. Validates file extensions and ensures safe file handling.
4. Works with any small text, C, CSV, or shell file types.
5. Displays clear success/failure messages for every step.

=====================================================================================
MODULE DESCRIPTION
------------------

1. MAIN MODULE (main.c)
   - Entry point of the program.
   - Accepts command-line arguments.
   - Determines the operation type (-e for encode, -d for decode).
   - Calls corresponding functions from encode or decode modules.

   Example usage:
       For encoding: ./a.out -e beautiful.bmp secret.txt output.bmp
       For decoding: ./a.out -d output.bmp recovered.txt

-------------------------------------------------------------------------------------

2. ENCODE MODULE (encode.c / encode.h)
   - Reads the secret file and BMP image file.
   - Encodes the secret file data into the LSB of the BMP image.
   - Saves the new modified image as the stego image.

   Major functions:
       • read_and_validate_encode_args() — validates user inputs.
       • open_files_encode()              — opens required files.
       • check_capacity()                 — checks if image can store secret data.
       • encode_magic_string()            — encodes project signature.
       • encode_secret_file_extn()        — encodes file extension (.txt, .c etc.)
       • encode_secret_file_size()        — encodes secret file size.
       • encode_secret_file_data()        — encodes actual file data.
       • copy_remaining_img_data()        — copies remaining image bytes.
       • do_encoding()                    — main driver for encoding steps.

-------------------------------------------------------------------------------------

3. DECODE MODULE (decode.c / decode.h)
   - Performs the reverse operation of encoding.
   - Reads data hidden in the LSBs of the stego image and reconstructs
     the original secret file.

   Major functions:
       • read_and_validate_decode_file()  — validates decode arguments.
       • open_files_decode()              — opens stego image file.
       • skip_bmp_header()                — skips first 54 bytes (header).
       • decode_magic_string()            — checks if image has hidden data.
       • decode_secret_file_extn_size()   — retrieves file extension length.
       • decode_secret_file_extn()        — extracts and reconstructs file name.
       • decode_secret_file_size()        — retrieves total secret data size.
       • decode_secret_file_data()        — extracts and writes hidden data.
       • do_decoding()                    — main controller for decoding steps.

-------------------------------------------------------------------------------------

4. COMMON MODULE (common.h)
   - Contains project-wide macros, constants, and shared functions.
   - Example: MAGIC_STRING ("#*") is used to identify a valid stego image.

-------------------------------------------------------------------------------------

5. TYPES MODULE (types.h / types1.h)
   - Defines enumeration types for project status and operation control.

   Example:
       typedef enum {
           e_success,
           e_failure
       } Status;

       typedef enum {
           e_encode,
           e_decode,
           e_unsupported
       } OperationType;

=====================================================================================
WORKING FLOW
------------

ENCODING PHASE:
---------------
Step 1 : Read and validate input arguments.
Step 2 : Open source image and secret file.
Step 3 : Check if image has enough capacity.
Step 4 : Encode magic string.
Step 5 : Encode secret file extension size.
Step 6 : Encode secret file extension.
Step 7 : Encode secret file size.
Step 8 : Encode secret file data.
Step 9 : Copy remaining unmodified image bytes.
Step 10: Close files and display success message.

DECODING PHASE:
---------------
Step 1 : Read and validate decode arguments.
Step 2 : Open stego image and skip BMP header.
Step 3 : Decode and verify magic string.
Step 4 : Decode secret file extension size.
Step 5 : Decode secret file extension.
Step 6 : Decode secret file size.
Step 7 : Decode secret file data.
Step 8 : Close all files and display decoding success message.

=====================================================================================
DATA ENCODING PRINCIPLE (LSB METHOD)
------------------------------------
Each byte in a BMP image has three color components (R, G, B).
The least significant bit of each component is modified to store 
one bit of secret data.

Example:
    Original byte  : 10101100
    Secret bit     : 1
    Encoded byte   : 10101101

    → Only 1 bit change, visually unnoticeable.

=====================================================================================
COMMAND LINE USAGE
------------------
Encoding:
    ./stego -e source.bmp secret.txt output.bmp

Decoding:
    ./stego -d output.bmp recovered.txt

=====================================================================================
OUTPUT EXAMPLE
---------------
Encoding Output:
    BMP header skipped successfully!
    Magic string encoded successfully!
    Secret file extension encoded!
    Secret file size encoded!
    Secret data encoded successfully!
    Encoding the secret data successfully!

Decoding Output:
    BMP header skipped successfully!
    Success: Magic string matched!
    Success: Decoded secret file extension size!
    Success: Decoded secret file extension!
    Success: Decoded secret file size!
    Success: Decoded secret file data!
    Decoding successful!

=====================================================================================
CONCLUSION
----------
This project demonstrates how image steganography can be implemented using
simple C programming concepts like:
    - File handling
    - Bitwise operations
    - Structures
    - Conditional logic and modular design

It provides an educational understanding of how data can be securely hidden
within digital media using the Least Significant Bit (LSB) technique.

=====================================================================================
END OF FILE
=====================================================================================
*/

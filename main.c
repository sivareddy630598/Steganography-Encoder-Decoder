#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "types1.h"
#include<string.h>
#include"decode.h"
// Function prototype declaration
OperationType check_operation_type(char *);
// Step 1 : Check the argc >= 4 true - > step 2
    // Step 2 : Call the check_operation_type(argv[1]) == e_encode )) true - > step 3
    // Step 3 : Declare structure variable EncodeInfo enc_info
    // Step 4 : Call the read_and_validate_encode_args(argv,&enc_info)== e_success)
    //  true -> Step 5 , false - > terminate the program
    // Step 5 : Call the do_encoding (&encInfo);
int main(int argc, char *argv[])
{
    // Structure to hold encoding-related information
    EncodeInfo encInfo;
    // Step 1: Validate the number of command-line arguments
    // Encoding needs: ./a.out -e <source_image> <secret_file> <output_image>
    // Decoding needs: ./a.out -d <stego_image> <output_file>
     if (argc == 4 || argc == 5)
    {
         // Step 2: Identify operation type (encode/decode)
        OperationType res = check_operation_type(argv[1]);
        if(res == e_encode)
        {
            // Step 3: Validate and store encoding arguments (file names, paths, etc.)
           Status res = read_and_validate_encode_args(argv,&encInfo);
           if(res == e_success)
           {
            // Step 4: Perform the encoding process
              res = do_encoding(&encInfo);
              if(res == e_success)
              {
                printf("Encoding the secret data successfully!\n");
              }
              else
              {
               printf("Error: Encoding process failed!\n");
                return e_failure;
              } 
           }
           else
           {
             printf("Error: Invalid file extension or input file missing!\n");
             return e_failure;
           }
        }
        else if(res == e_decode)
        {
            // Structure to hold decoding-related information
           DecodeInfo decInfo;
           // Step 3 (for decoding): Validate and store decoding arguments
           Status1 res = read_and_validate_decode_file(argv,&decInfo);
           if(res == e_success )
           {
            // Step 4: Perform the decoding process
               res = do_decoding(&decInfo);
               if(res == e_success)
               {
                    printf("decoding successfull!\n");
               }
               else
               {
                    printf("Error:decode_secret_file_data failure!\n");
                    return e_failure;
               }
           }
           else
           {
                printf("Error: Invalid file extension or input file missing!\n");
                return e_failure;
           }
            
        }
        else
        {
            printf("Error: Invalid mode of operation entered.\n");
            printf("Use -e for encoding or -d for decoding.\n");
            return e_failure;
        }
    }
    else
    {
        // Step 5: If the number of arguments is invalid, display correct usage
        printf("Usage:\n");
        printf("  To Encode: %s -e <source_image> <secret_file> [output_image]\n", argv[0]);
        printf("  To Decode: %s -d <stego_image> <output_file>\n", argv[0]);
        return e_failure;
    }
    
}
// Function: check_operation_type
// Purpose : Determines whether the operation is encode, decode, or invalid
// Input   : symbol (string, e.g., "-e" or "-d")
// Returns : e_encode, e_decode, or e_unsupported
OperationType check_operation_type(char *symbol)
{
    // Step 1: Compare input symbol with "-e"
    if(strcmp("-e",symbol) == 0)
    {
        return  e_encode;// Encoding mode
    }
    // Step 2: Compare input symbol with "-d"
    else if(strcmp("-d",symbol) == 0)
    {
        return e_decode; // Decoding mode
    }
    else     // Step 3: If neither, return unsupported operation
    {
        return e_unsupported;
    }
}

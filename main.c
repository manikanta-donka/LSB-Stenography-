#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    if(argc < 2)
    {
        printf("%s: Encoding: %s - e <. bmp file> <. txt file> [output file]\n",argv[0],argv[0]);
        printf("%s: Decoding: %s -d .bmp file> [output file]\n",argv[0],argv[0]);
        return e_failure;
    }
    OperationType op_type = check_operation_type(argv[1]);
    if(op_type == e_encode)
    {
        if(argc < 4)
        {
            printf("%s: Encoding: %s - e <. bmp file> <. txt file> [output file]\n",argv[0],argv[0]);
            return e_failure;
        }
        if(read_and_validate_encode_args(argv, &encInfo) == e_failure)
        {
            printf("Error validating\n");
            return e_failure;
        }
        if(do_encoding(&encInfo) == e_failure)
        {
            printf("Error encoding\n");
            return e_failure;
        }
    }
    else if(op_type == e_decode)
    {

        if(argc < 3)
        {
            printf("%s: Encoding: %s - e <. bmp file> <. txt file> [output file]\n",argv[0],argv[0]);
            return e_failure;
        }
        if(read_and_validate_decode_args(argv, &decInfo) == e_failure)
        {
            printf("Error validating.\n");
            return e_failure;
        }
        if(do_decoding(&decInfo) == e_failure)
        {
            printf("Error decoding.\n");
            return e_failure;
        }
    }
    else
    {
        printf("Error and stop the process");
    }
    
    //STEP1: Check op_type e_encode

    //STEP2: If yes Start encode, No Goto STEP3

    //STEP3: Check op_type is e_decode

    //STEP4: If yes Start decode, No Goto STEPS

    //STEPS: Print error and stop the process
    return 0;
}
OperationType check_operation_type(char *argv)
{
    if(!strcmp(argv, "-e"))
    {
        return e_encode;  
    }
    else if(!strcmp(argv, "-d"))
    {
        return e_decode;
    }  
    else
    {
        return e_unsupported;
    }
        //Step1: Compare argv with -e
    //Step2:If yes -> return e_decode, No goto Step3

    //Step3: Compare argv with -d
    //Step4:IF yes -> return e_decode, No goto Step5
    
    //Step5: return e_unsupported

}
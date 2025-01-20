#include <stdio.h>
#include <string.h>
#include "types.h"
#include "decode.h"


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check if the stego file has a valid extension
    char *src_extn = strchr(argv[2], '.');
    // Check if extn is there or not  and extn is .bmp
    if (src_extn == NULL || strcmp(src_extn, ".bmp") != 0)
    {
        printf("INFO:Validation Error.\n");
        return e_failure;
    }
    // Storing the stego file name in the structure
    decInfo->stego_fname = argv[2];
    // Checking if output file name is passed or not
    if (argv[3] != NULL)
    {
        // Copy everything before the dot
        char *out_extn = strchr(argv[3], '.');
        // if extn is given store the name only
        if (out_extn != NULL)
        {
            strncpy(decInfo->out_fname, argv[3], out_extn - argv[3]);
            decInfo->out_fname[out_extn - argv[3]] = '\0';  
        }
        // Copy the output file name as it is if not extn
        else
        {
            strcpy(decInfo->out_fname, argv[3]);
        }
    }
    // Copy default name as output name
    else
    {
        decInfo->out_flag=1;
        strcpy(decInfo->out_fname,"decoded");
    }
    return e_success;
}

/* Function definition of do decoding */
Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started. ##\n");
    if(open_stego_file(decInfo) == e_failure)
    {
        printf("Error opening files\n");
        return e_failure;
    }
    if(skip_header(decInfo->fptr_stego) == e_failure)
    {
        printf("Error skipping header data\n");
        return e_failure;
    }
    char magic_string[10];
    printf("Enter the magic string:\n");
    scanf("%s",magic_string);
    if(decode_magic_string(magic_string,decInfo) == e_failure)
    {
        printf("Magic String not decoded\n");
        return e_failure;
    }
    if(decode_secret_file_extn_size(decInfo) == e_failure)
    {
        printf("Error decoding secret file extension length\n");
        return e_failure;
    }
    if(decode_secret_file_extention(decInfo) == e_failure)
    {
        printf("Error decoding secret file extension\n");
        return e_failure;
    }
    if(open_output_file(decInfo) == e_failure)
    {
        printf("Error opening output file\n");
        return e_failure;
    }
    printf("INFO: Done. Opened all required files.\n");
    if(decode_secret_file_size(decInfo) == e_failure)
    {
        printf("Error decoding secret file size\n");
        return e_failure;
    }
    if(decode_secret_file_data(decInfo) == e_failure)
    {
        printf("Error decoding secret data\n");
        return e_failure;
    }
    printf("INFO: ## Decoding Done Successfully.\n");
    return e_success;
}

Status open_stego_file(DecodeInfo *decInfo)
{
    printf("INFO: Opeing required files.\n");
    decInfo->fptr_stego = fopen(decInfo->stego_fname, "r");
    if(decInfo->fptr_stego == NULL)
    {
        return e_failure;
    }
    printf("INFO: Opened %s.\n", decInfo->stego_fname);
    return e_success;
}
Status open_output_file(DecodeInfo *decInfo)
{
    decInfo->fptr_output = fopen(decInfo->out_fname,"w");
    if(decInfo->fptr_output == NULL)
    {   
        return e_failure;
    }
    if(decInfo->out_flag)
    {
        printf("INFO: Output File not mentioned. Creating decoded.txt as default.\n");
    }
    printf("INFO: Opened %s.\n", decInfo->out_fname);
    return e_success;
}
Status skip_header(FILE *fptr)
{
    fseek(fptr, 54, SEEK_SET);
    return e_success;
}
Status decode_magic_string(const char *user_string, DecodeInfo *decInfo)
{
    printf("INFO: Decoding Magic String Signature.\n");
    char buffer[8],magic[4];
    // Decode magic string from the file
    for(int i = 0;i < 2;i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego);
        magic[i]=decode_lsb_to_byte(buffer);
    }
    
    // Compare the user entered magic string with the decoded string
    if( strcmp(user_string,magic) == 0 )
    {
        printf("INFO: Done\n");
        return e_success;
    }
    return e_failure;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer,32,1,decInfo->fptr_stego);
    decInfo->secret_extn_length = decode_lsb_to_size(buffer);
    return e_success;

}
Status decode_secret_file_extention(DecodeInfo *decInfo)
{
    char extention[8];
    int i;
    for(i = 0;i < decInfo->secret_extn_length;i++)
    {
        fread(extention, 8, 1, decInfo->fptr_stego);
        decInfo->secret_extn[i] = decode_lsb_to_byte(extention);
    }
    decInfo->secret_extn[i]='\0';
    strcat(decInfo->out_fname,decInfo->secret_extn);
    return e_success;
}
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    printf("INFO: Decoding %s File Size.\n", decInfo->out_fname);
    char buffer[32];
    fread(buffer,32,1,decInfo->fptr_stego);
    decInfo->secret_size = decode_lsb_to_size(buffer);
    printf("INFO: Done.\n");
    return e_success;
}
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("INFO: Decoding %s File Data.\n", decInfo->out_fname);
    rewind(decInfo->fptr_output);
    char buffer[8],ch;
    for(int i = 0;i < decInfo->secret_size;i++)
    {
        fread(buffer, 8, 1,decInfo->fptr_stego);
        ch = decode_lsb_to_byte(buffer);
        fwrite(&ch, 1, 1,decInfo->fptr_output);
    }
    printf("INFO: Done.\n");
    return e_success;
    
}

char decode_lsb_to_byte(char *image_buffer)
{
    char ch = 0;
    for(int i =0;i < 8;i++)
    {
        char bit = image_buffer[i] & 0x01;
        ch = ch | (bit << (7 - i));
       
    }
    return ch;
}
uint decode_lsb_to_size(char *image_buffer)
{
    uint size = 0;
    for(int i =0;i < 32;i++)
    {
        char bit = image_buffer[i] & 0x01;
        size = size | (bit << (31 - i));
        

    }
    return size;
}
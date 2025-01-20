#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

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

    // Return image capacity
    return width * height * 3;
}
uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files.\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: can't open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    
    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: can't open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: can't open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    printf("INFO: Opened beautiful.bmp\n");
    printf("INFO: Opened secret.txt\n");
    printf("INFO: Opened %s\n",encInfo->stego_image_fname);
    printf("INFO: Done\n");
    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char bmp[10],txt[10],out[10];
    strcpy(bmp, strchr(argv[2], '.'));
    strcpy(txt, strchr(argv[3], '.'));
    if(argv[4] == NULL)
    {
        printf("INFO: Output file not mentioned. Creating steged_img.bmp as default.\n");
        encInfo->stego_image_fname = "steged_img.bmp";
    }
    else
    {
        strcpy(out, strchr(argv[4], '.'));
        if(strcmp(out,".bmp"))
        {
            return e_failure;
        }
        encInfo->stego_image_fname=argv[4];
    }
    if(strcmp(bmp,".bmp"))
    {
        return e_failure;
    }
    if(strcmp(txt,".txt"))
    {
        return e_failure;   
    }
    
    strcpy(encInfo->extn_secret_file,txt);
    encInfo->src_image_fname = argv[2];
    encInfo->secret_fname=argv[3];
    return e_success;
    
}

Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_failure)
    {
        printf("INFO: Files are not opened.\n");
        return e_failure;
    }
    printf("INFO: ## Encoding Procedure Started. ##\n");
    if(check_capacity(encInfo) == e_failure)
    {
        printf("INFO: There is no enough space\n");
        return e_failure;
    }
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("INFO:Header not copied\n");
        return e_failure;
    }
    if(encode_magic_string(MAGIC_STRING,encInfo) == e_failure)
    {
        printf("INFO:Error encoding magic string\n");
        return e_failure;
    }
    if(encode_extention_size(strlen(encInfo->extn_secret_file), encInfo) == e_failure)
    {
        printf("INFO:Error encoding extention size.\n");
        return e_failure;
    }
    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_failure)
    {
        printf("INFO:Error encoding secret file extention.\n");
        return e_failure;
    }
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_failure)
    {
        printf("INFO:Error encoding secret file size.\n");
        return e_failure;
    }
    if(encode_secret_file_data(encInfo) == e_failure)
    {
        printf("INFO:Error encoding secret data.\n");
        return e_failure;
    }
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        printf("INFO:Error copying the remaing data to output image.\n");
        return e_failure;
    }
    printf("INFO: ## Enocding Done successfully.\n");
    return e_success;

}

Status check_capacity(EncodeInfo *encInfo)
{
    // Get Secret file size
    printf("INFO: Checking for %s size\n",encInfo->secret_fname);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if(encInfo->size_secret_file != 0)
    {
        printf("INFO: Done. Not Empty\n");
    }
    // Get Image size
    printf("INFO: Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    // Get Magic string length
    uint magic_string_length = strlen(MAGIC_STRING);
    // Calculate Required file size
    uint req_size = 54 + ((magic_string_length + 4 +strlen(encInfo->extn_secret_file) + 4 + encInfo->size_secret_file) * 8);
    if(encInfo->image_capacity < req_size)
    {
        return e_failure;
    }
    else 
    {
        printf("INFO: Done. Found OK\n");
        return e_success;
    }
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    printf("INFO: Copying Image Header.\n");
    rewind(fptr_src_image);

    char temp[54];
    fread(temp, 54, 1, fptr_src_image);
    fwrite(temp, 54, 1, fptr_stego_image);
    printf("INFO: Done\n");
    return e_success;
    

}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char magic[4];
    strcpy(magic,magic_string);
    printf("INFO: Encoding Magic string \n");
    encode_data_to_image(magic,encInfo->fptr_src_image, encInfo->fptr_stego_image);   
    printf("INFO: Done\n");
    return e_success;
}
Status encode_extention_size(char size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_int_to_lsb(buffer,size);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s Extention.\n",encInfo->secret_fname);
    char image_buffer[8];
    strcpy(image_buffer, file_extn);
    encode_data_to_image(image_buffer,encInfo->fptr_src_image,encInfo->fptr_stego_image);
    printf("INFO: Done\n");
    return e_success;
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s file size.\n",encInfo->secret_fname);
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_int_to_lsb(buffer,file_size);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    printf("INFO: Done\n");
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s file data.\n",encInfo->secret_fname);
    rewind(encInfo->fptr_secret);
    char ch;
    char buffer[8];
    while((ch = getc(encInfo->fptr_secret)) != EOF)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(ch,buffer);
        fwrite(buffer,8,1, encInfo->fptr_stego_image);
    }
printf("INFO: Done.\n");
return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO: Copying Left over Data.\n");
    int c;
  
   while((c=fgetc(fptr_src )) !=EOF)
   {
    fputc(c,(fptr_dest));
   }
    printf("INFO: Done.\n");
    return e_success;
}
Status encode_data_to_image(char *data,FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    //Read 8 bytes of data is read from beautiful.bmp and store it in an array.
    //Call encode_byte_to_lsb and pass the array and data[0]
    for(int i = 0;i < strlen(data); i++)
    {
        fread(buffer, 1, 8, fptr_src_image);
        encode_byte_to_lsb(data[i],buffer);
        fwrite (buffer, 1, 8, fptr_stego_image);
    }
    return e_success;
    //write the encoded 8 bytes of data to output.bmp
    //repeat for length of the data times. 
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    
   for(int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE);//Clear LSB bit of image buffer[0]
        char c = (unsigned)(data & (1 << (7 - i))) >>(7 - i); //Get one bit from data
        image_buffer[i] = (image_buffer[i] | c);    //replace the bit

    }
   return e_success;
}
Status encode_int_to_lsb(char *image_buffer, int data)
{
    
   for(int i = 0; i < 32; i++)
    {
        // Clearing lsb bit of all image datas
        image_buffer[i] = ( image_buffer[i] & 0XFE );
        // Extracting each bit of data and storing at the lsb of image data
        image_buffer[i] = ( image_buffer[i] | ((unsigned)(data&(1<<(31-i)))>>(31-i)) );

    }   //Repeat the process for 32 times.
    return e_success;

}

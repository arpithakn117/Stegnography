#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
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

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
      printf("INFO: Opened beautiful.bmp\n");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    printf("INFO: opened secret.txt\n");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    printf("INFO: Opened stego.bmp\n");
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

OperationType check_operation_type(char *argv[])
{
    //IF => checking argv[1] is "-e" ==> return e_encode
    if(strcmp(argv[1],"-e")==0)
    return e_encode;
    //ELSE_IF => checking argv[1] is "-d" ==> return e_decode
    else if(strcmp(argv[1],"-d")==0)
    return e_decode;
    //ELSE => return e_unsupported
    else
    return e_unsupported;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(strcmp(strchr(argv[2],'.'),".bmp")==0)
    {
        encInfo->src_image_fname=argv[2];
        
        if((strcmp(strchr(argv[3],'.'),".txt")==0) || (strcmp(strchr(argv[3],'.'),".c")==0) || (strcmp(strchr(argv[3],'.'),".sh")==0))
        {
            encInfo->secret_fname=argv[3];
            strcpy(encInfo->extn_secret_file,strchr(argv[3],'.'));
            if(argv[4]!=NULL)
            {
                if(strcmp(strchr(argv[4],'.'),".bmp")==0)
                {
                    encInfo->stego_image_fname=argv[4];
                    return e_success;
                }
                else
                {
                    printf("pass .bmp file\n");
                    return e_failure;
                }
            }
            else
            {
                encInfo->stego_image_fname="output_fname.bmp";
                return e_success;
            }
        }
        else
        {
            printf("give correct extn\n");
            return e_failure;
        }
    }
    else
    {
        printf("pass the .bmp file\n");
        return e_failure;
    }
}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: ## Encoding Procedure Started ##\n");
    printf("INFO: Opening required files\n");
    if(open_files(encInfo)==e_success)
    {
        printf("INFO: Done\n");
        if(check_capacity(encInfo)==e_success)
        {
            printf("INFO: Done. Found. OK\n");
            printf("INFO: Copying Image Header\n");
            if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
            {
                printf("INFO: Done\n");
                printf("INFO: Encoding Magic String Signature\n");
                if(encode_magic_string(MAGIC_STRING, encInfo)==e_success)
                {
                    printf("INFO: Done\n");
                    printf("INFO: Encoding Secret.txt File Extension Size\n");
                    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo)==e_success)
                    {
                        printf("INFO: Done\n");
                        printf("INFO: Encoding Secret.txt File Extension\n");
                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
                        {
                            printf("INFO: Done\n");
                            printf("INFO: Encoding Secret.txt File Size\n");
                            if(encode_secret_file_size(encInfo->secret_file_size,encInfo)==e_success)
                            {
                                printf("INFO: Done\n");
                                printf("INFO: Encoding Secret.txt File Data\n");
                                if(encode_secret_file_data(encInfo)==e_success)
                                {
                                    printf("INFO: Done\n");
                                    printf("INFO: Copying Left Over Data\n");
                                    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
                                    {
                                        printf("INFO: Done\n");
                                        printf("INFO: Encoding Done Successfully\n");
                                    }
                                    else
                                    {
                                        printf("Error: remaining image data\n");
                                        return e_failure;
                                    }
                                }
                                else
                                {
                                    printf("Error: secret file data\n");
                                    return e_failure;
                                }
                            }
                            else
                            {
                                printf("Error: secret file size\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Error: secret file extension\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Error: secret file extension size\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Error: magic string\n");
                    return e_failure;
                }
            }
            else
            {
                printf("Error: bmp header\n");
                return e_failure;
            }
        }
        else
        {
            printf("Error: capacity checking\n");
            return e_failure;
        }
    }
    else
    {
        printf("Error: opening files\n");
        return e_failure;
    }
}


Status check_capacity(EncodeInfo *encInfo)
{
    int encoding_things = 0;
    printf("INFO: Checking for secret.txt size\n");
    encInfo->secret_file_size = get_file_size(encInfo->fptr_secret);
    printf("INFO: Done. Not Empty\n");
    printf("INFO: Checking for beautiful.bmp capacity to handle secret.txt\n");
    encInfo->image_file_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    encoding_things = 54 + (2 + 4 + strlen(encInfo->extn_secret_file) + 4 + encInfo->secret_file_size) * 8;

    if(encInfo->image_file_size > encoding_things)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    rewind(fptr_src_image);
    char buffer[54];
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_stego_image);
    if(ftell(fptr_src_image)==ftell(fptr_stego_image))
    return e_success;
    else
    return e_failure;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    //printf("%s\n",magic_string);
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_stego_image))
    return e_success;
    else
    return e_failure;
}

Status encode_data_to_image(char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char buffer[8];
    for(int i=0;i<strlen(data);i++)
    {
        fread(buffer,8,1,fptr_src_image);
        encode_byte_to_lsb(data[i],buffer);
        fwrite(buffer,8,1,fptr_stego_image);
    }
        if(ftell(fptr_src_image)==ftell(fptr_stego_image))
        return e_success;
        else
        return e_failure;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
        int get;
        for(int i=0;i<8;i++)
        {
        get = (data & 1<<i)>>i;
        image_buffer[i] = image_buffer[i] & (~1); 
        image_buffer[i] = image_buffer[i] | get;
        }
        return e_success;
}

Status encode_secret_file_extn_size(int extn_size,EncodeInfo *encInfo)
{
        char buffer[32];
        fread(buffer,32,1,encInfo->fptr_src_image);
        encode_size_to_lsb(extn_size,buffer);
        fwrite(buffer,32,1,encInfo->fptr_stego_image);
        //printf("%d\n",extn_size);
        if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_stego_image))
        return e_success;
        else
        return e_failure;
}

Status encode_size_to_lsb(int data,char *buffer)
{
    int get;
    for(int i=0;i<32;i++)
    {
        get = (data & 1<<i)>>i;
        buffer[i] = buffer[i] & (~1); 
        buffer[i] = buffer[i] | get; 
    }
    return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    //printf("%s\n",file_extn);
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_stego_image))
    return e_success;
    else
    return e_failure;
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_stego_image))
    return e_success;
    else
    return e_failure;
}

Status encode_secret_file_data(EncodeInfo *encInfo)    
{
    char file_data[encInfo->secret_file_size];
    // printf("%d\n",encInfo->secret_file_size);
    rewind(encInfo->fptr_secret);
    fread(file_data,encInfo->secret_file_size,1,encInfo->fptr_secret);
    file_data[encInfo->secret_file_size]='\0';
    //printf("%s",file_data);
    encode_data_to_image(file_data,encInfo->secret_file_size,encInfo->fptr_src_image,encInfo->fptr_stego_image);
    if(ftell(encInfo->fptr_src_image)==ftell(encInfo->fptr_stego_image))
    return e_success;
    else
    return e_failure; 
}

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[1];
    while(fread(buffer,1,1,fptr_src_image))
        fwrite(buffer,1,1,fptr_stego_image);
}
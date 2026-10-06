#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strcmp(strchr(argv[2],'.'),".bmp")==0)
    {
        decInfo->stego_image_fname=argv[2];
        if(argv[3]!=NULL)
        {
            decInfo->output_image_fname=argv[3];
            return e_success;
        }
        else
        {
            decInfo->output_image_fname="output";
            return e_success;
        }
    }
    else
    {
        printf("input file shoulbe be in .bmp extension\n");
        return e_failure;
    }
}

Status open_bmp_file(DecodeInfo *decInfo)
{
    // stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }
    // No failure return e_success
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: ## Decoding Procedure Started ##\n");
    printf("INFO: Opening required files\n");
    if(open_bmp_file(decInfo)==e_success)
    {
        printf("INFO: Opened stego_beautiful.bmp\n");
        if(skip_bmp_header(decInfo->fptr_stego_image)==e_success)
        {
            printf("INFO: Decoding Magic String Signature\n");
            if(decode_magic_string(decInfo)==e_success)
            {
                printf("INFO: Done\n");
                printf("INFO: Decoding Output File Extension Size\n");
                if(decode_secret_file_extn_size(decInfo->d_extn_size,decInfo)==e_success)
                {
                    printf("INFO: Done\n");
                    printf("INFO: Decoding Output File Extension\n");
                    if(decode_secret_file_extn(decInfo)==e_success)
                    {
                        printf("INFO: Done\n");
                        printf("INFO: Decoding Output File size\n");
                        if(decode_secret_file_size(decInfo->d_file_size,decInfo)==e_success)
                        {
                            printf("INFO: Done\n");
                            printf("INFO: Decoding Output File Data\n");
                            if(decode_secret_file_data(decInfo)==e_success)
                            {
                                printf("INFO: Done\n");
                            }
                            else
                            {
                                printf("ERROR : Secret file data\n");
                            }
                        }
                        else
                        {
                            printf("ERROR : Secret file size\n");
                        }
                    }
                    else
                    {
                        printf("ERROR : Secret file extension\n");
                    }
                }
                else
                {
                    printf("ERROR : Secret file extension size\n");
                }
            }
            else
            {
                printf("ERROR : Magic string decoding\n");
            }
        }
        else
        {
            printf("ERROR : Skipping the bmp header\n");
        }
    }
    else
    {
        printf("ERROR : Opening the file\n");
    }
}

Status skip_bmp_header(FILE *fptr_stego_image)
{
    fseek(fptr_stego_image,54,SEEK_SET);
    if(ftell(fptr_stego_image)==54)
    return e_success;
    else
    return e_failure;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    char magic_string[3],buffer[8];
    for(int i=0;i<2;i++)
    {
        fread(buffer,8,1,decInfo->fptr_stego_image);
        magic_string[i]=decode_bytes_from_lsb(buffer);
    }
    magic_string[2]='\0';
    //printf("%s\n",magic_string);

    char user_magicstring[3];
    r1: printf("Enter the magic string : ");
    scanf(" %[^\n]",user_magicstring);

    if(strcmp(magic_string,user_magicstring)==0)
        return e_success;
        else
        return e_failure;
        goto r1;
}

char decode_bytes_from_lsb(char *buffer)
{
    char ch=0;
    int get;
    for(int i=0;i<8;i++)
    {
        get = (buffer[i] & 1);
        ch = ch | (get<<i);
    }
    return ch;
    //after comparison it returns # * character
}

Status decode_secret_file_extn_size(int d_extn_size,DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer,32,1,decInfo->fptr_stego_image);
    decInfo->d_extn_size = decode_size_from_lsb(buffer);
    //printf("%d\n",decInfo->d_extn_size);
    return e_success;
}

int decode_size_from_lsb(char *buffer)
{
    int data=0;
    for(int i=0;i<32;i++)
    {
        int get = buffer[i] & 1;
        data = data | get<<i;
    }
     return data;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[8];
    char extension[decInfo->d_extn_size];
    for(int i=0;i<decInfo->d_extn_size;i++)
    {
        fread(buffer,8,1,decInfo->fptr_stego_image);
        extension[i]=decode_bytes_from_lsb(buffer);
    }
    extension[decInfo->d_extn_size]='\0';
    strcat(decInfo->output_image_fname,extension);

    //printf("%s\n",decInfo->output_image_fname);
    // output file
    decInfo->fptr_out_image = fopen(decInfo->output_image_fname, "w");
    // printf("INFO: Output File not mentioned. Creating decoded.txt as default\n");
    // printf("INFO: Opened decoded.txt\n");
    // Do Error handling
    if (decInfo->fptr_out_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_image_fname);

    	return e_failure;
    }
    // No failure return e_success
    return e_success;
}

Status decode_secret_file_size(int d_file_size, DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer,32,1,decInfo->fptr_stego_image);
    decInfo->d_file_size = decode_size_from_lsb(buffer);
    //printf("%d\n",decInfo->d_file_size);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8],ch=0;
    for(int i=0;i<decInfo->d_file_size;i++)
    {
        fread(buffer,8,1,decInfo->fptr_stego_image);
        ch = decode_bytes_from_lsb(buffer);
        //printf("%c",ch);
        fwrite(&ch,1,1,decInfo->fptr_out_image);
    }
     return e_success;
}
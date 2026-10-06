#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc,char* argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    if(argc<2)
    {
         printf("INFO : Missing arguments\n");
         printf("INFO : Encoding: ./a.out -e <.bmp file> <.txt file> [Output file]\n");
		     printf("INFO : Decoding: ./a.out -d <.bmp file> [output file]\n");
    }
    //check the operation type
       //call the check_operation_type function
       int var=check_operation_type(argv);

     //IF => e_encode
    if(var==e_encode)
    {
      if(argc==4)
      {
          printf("INFO: Output file not mentioned. Creating .bmp as default\n");
      }
      if(argc >= 4 && argc <= 5)
      {
          if(read_and_validate_encode_args(argv,&encInfo)==e_success)
          {
              do_encoding(&encInfo);
              printf("INFO: ## Encoding Done Successfully ##\n");
          }
      }
      else
      {
          printf("INFO: Missing arguments\n");
          printf("INFO: Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
      }
    }
       //IF => e_decode
    else if(var==e_decode)
    {
      if(argc >= 3 && argc <= 4)
      {
        if(read_and_validate_decode_args(argv,&decInfo)==e_success)
        {
            do_decoding(&decInfo);
            printf("INFO: ## Decoding Done Successfully ##\n");
        }
      }
      else
      {
        printf("Invalid number of arguments\n");
        printf("INFO: Decoding: ./a.out -d <.bmp file> [output file]\n");
      }
    return 0;
    }
}
  
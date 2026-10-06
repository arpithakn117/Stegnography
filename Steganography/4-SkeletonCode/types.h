#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_success,  //default first value in enum is 0
    e_failure  //1
} Status;

typedef enum
{
    e_encode,  //-e
    e_decode,   //-d
    e_unsupported  //others
} OperationType;

#endif

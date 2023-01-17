#ifndef __gendefs__h 
#define  __gendefs__h 1

// for 64-bits architecture 
#include <stdio.h>

typedef unsigned char byte;

typedef short int int2; 
typedef int int4; 
typedef long int int8; 

typedef unsigned short int uint2; 
typedef unsigned int uint4; 
typedef unsigned long uint8; 

typedef float float4; 
typedef double float8; 

void gendefs_assert();
void set_verbose(int value);
int verbose(); // get

#endif 


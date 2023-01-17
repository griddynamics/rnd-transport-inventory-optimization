#include <assert.h> // do we need it
#include "gendefs.h" // do we need it

int __verbose = 0;

//======================
void gendefs_assert()
//======================
{
  assert(sizeof(byte) == 1);
  assert(sizeof(int2) == 2);
  assert(sizeof(uint2) == 2);
  assert(sizeof(int4) == 4);
  assert(sizeof(uint4) == 4);
  assert(sizeof(int8) == 8);
  assert(sizeof(uint8) == 8);
  assert(sizeof(float4) == 4);
  assert(sizeof(float8) == 8);

};


//=============================
void set_verbose(int value)
//=============================
{
   __verbose = value;
};

//=============================
int verbose()
//=============================
{
  return  __verbose;
};



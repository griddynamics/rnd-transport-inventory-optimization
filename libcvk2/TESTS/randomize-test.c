#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "randomize.h"

void test0()
{

   int nrep = 100;

   int i;
   for(i=0; i<nrep; i++)
   {
       unsigned long int rs = randomize();  
       printf("   RS = %ld\n\n", rs);
   }; // for(i=0; i<n; i++)
};


int main(int argc, char* argv[])
{

  test0();

  return 0;
}


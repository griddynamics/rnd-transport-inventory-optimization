#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <time.h>
// COMPILE: cc -o a -lrt clock_nanosleep.c

//==============================
unsigned long int randomize()
//==============================
{
   static unsigned long int count = 0;

       //
       // get seconds 
       //

        // get nano
   struct timespec ts;
   int ret = clock_gettime(CLOCK_REALTIME, &ts);
   ret = ret;
   assert(ret == 0);

   unsigned long int sec = ts.tv_sec;
   unsigned long int nsec = ts.tv_nsec;

   count += 1;

   return  sec * 10 + count*100 + nsec;

};



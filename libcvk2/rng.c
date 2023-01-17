#include <assert.h>
#include "rng.h"

#include "rng.h"
#include "randomize.h"


//=========================
gsl_rng* rng_create(unsigned long int seed)  // 0 - randomize
//=========================
{
    gsl_rng* rng = gsl_rng_alloc(gsl_rng_mt19937); // 
    if(rng == NULL) 
    {
       fprintf(stderr, " *** gsl_rng_alloc() failed: in rng_create()\n"); 
       assert(0);
       exit(1);
    };

    if(seed == 0)
       seed = randomize();

    gsl_rng_set(rng, seed);

    return rng;
};


//==============================
void rng_delete(gsl_rng* rng)
//==============================
{
   assert(rng != NULL);

   gsl_rng_free(rng);
}



///////////////  old version: some allocation problems were observed

// static unsigned long int  random_seed();


////=========================
//gsl_rng* rng_create(unsigned long int seed)  // 0 - randomize
////=========================
//{
//    gsl_rng* rng = gsl_rng_alloc(gsl_rng_mt19937); // 
//
//    if(rng == NULL) 
//    {
//       fprintf(stderr, " *** gsl_rng_alloc() failed\n"); 
//       exit(1);
//    };
//
//    if(seed == 0)
//       seed = random_seed();
//
//    gsl_rng_set(rng, seed);
//
//    return rng;
//};
//
//
////==============================
//void rng_delete(gsl_rng* rng)
////==============================
//{
//   assert(rng != NULL);
//
//   gsl_rng_free(rng);
//}
//
//

//#include <time.h>
////+++++++++++++++++++++++++++++++++++
//unsigned long int  random_seed()
////+++++++++++++++++++++++++++++++++++
//{
//   static int count = 0;
//   time_t t; 
//   time(&t);
//   struct tm tmm;
//   struct tm* tm_p = localtime_r(&t, &tmm);
//   unsigned long int seed = 
//         tm_p->tm_hour*3600 + tm_p->tm_min * 60 + tm_p->tm_sec + count;
//   count += 17;
//   return seed;
//};
//



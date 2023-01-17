#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "mmem.h"

typedef void (*release_fields_t)(void* mptr);


//================================
void* mmgetnew(uint4 size)
//================================
{
//   int head_size = sizeof(void*) + sizeof(uint4);
//   void* mem = malloc(head_size + size); 
//   if(mem != NULL)
//   {
//     void** del_func_ptr = (void**)mem;
//     uint4* count_ptr = (uint4*) ( (char*)mem + sizeof(void*) );
//     void* data_ptr = (void*) ((char*)mem + head_size);
//     return data_ptr;
//   }
//   else
//     return NULL;
    return mmgetnew2(size, NULL);
};

//====================================================================
void* mmgetnew2(uint4 size, void (*release_fields_func)(void* mptr))
//====================================================================
{
   int head_size = sizeof(void*) + sizeof(uint4);
   void* mem = malloc(head_size + size); 

#ifdef __MM_DEBUG
    extern FILE* __mmdebug_file;
    extern const char* __mmdebug_mark;

    if(__mmdebug_mark != NULL)
    {
       if(__mmdebug_file != NULL)
       {
          fprintf(__mmdebug_file, " mmdebug: %p  %s\n", mem, __mmdebug_mark);
          fflush(__mmdebug_file);
       } 
       else
          fprintf(stderr, " mmdebug: %p  %s\n", mem, __mmdebug_mark);
    };
#endif
       //printf(" mmgetnew2(): mem = %p\n", mem); //!!!!

   if(mem != NULL)
   {
     void (**release_fields_ptr)(void* ptr) = (release_fields_t*)mem;

     uint4* count_ptr = (uint4*) ( (char*)mem + sizeof(void*) );
     void* mptr = (void*) ((char*)mem + head_size);

     *count_ptr = 1;
     *release_fields_ptr = release_fields_func; 

//       //!!!!!!!!!!!!!!!
//       printf("    mmgetnew2(): release_fields_ptr = %p\n", 
//           release_fields_ptr); //!!!!
//       printf("    mmgetnew2(): count_ptr = %p\n", count_ptr); 
//       printf("    mmgetnew2(): mptr = %p\n", mptr); 
//       printf("    mmrefcount(mptr) = %u\n", mmrefcount(mptr)); 
//
//       mmgetref(mptr);
//       printf("    mmrefcount(mptr) after mmgetref = %u\n", mmrefcount(mptr)); 
//       mmrelease(mptr);
//       printf("    mmrefcount(mptr) after mmrelease = %u\n", mmrefcount(mptr)); 
//
//       release_fields_t* release_fields_ptr2 = 
//          (release_fields_t*)((char*)mptr - sizeof(uint4)- sizeof(void*));
//       printf("     release_fields_ptr2 = %p\n", release_fields_ptr2);
//       printf("     *release_fields_ptr2 = %p\n", *release_fields_ptr2);
//       //!!!!!!!!!!!!!!!



     return mptr;
   }
   else
     return NULL;

};



//=======================
void* mmgetref(const void* mptr)
//=======================
{
   assert(mptr != NULL);
   uint4* refcount = (uint4*)((char*)mptr - sizeof(uint4));
   assert(*refcount > 0);
   *refcount += 1;
   return (void*)mptr;
};

//=========================
void mmrelease(void* mptr)
//=========================
{
   assert(mptr != NULL);
   uint4* refcount = (uint4*)((char*)mptr - sizeof(uint4));
      //  printf(" mmrelease_(): mptr = %p\n", mptr); //!!!
      //  printf("               refcount_ptr = %p\n", refcount); //!!!

   if(*refcount > 1)
       *refcount -= 1;
   else  // refcount == 1
   {
      release_fields_t* release_fields_ptr = 
          (release_fields_t*)((char*)mptr - sizeof(uint4)- sizeof(void*));
      if( *release_fields_ptr != NULL)
                (**release_fields_ptr)(mptr);
      free(release_fields_ptr); // release_fields_ptr=mmem
   }
};


//==============================
uint4 mmrefcount(const void* mptr)
//==============================
{
   assert(mptr != NULL);
   return *(uint4*)((char*)mptr - sizeof(uint4));
};

//===========================
void mmrelease_(void** mptr)
//===========================
{
   assert(mptr != NULL);
   assert(*mptr != NULL);
   mmrelease(*mptr);
   *mptr = NULL;
};



//==================================================
void* mmgetnew_copy(const void* mptr, uint4 size)
//==================================================
{
   assert(mptr != NULL);
   assert(size > 0);

   void* retv = mmgetnew(size);
   assert(retv != NULL);

   memcpy(retv, mptr, size);

   return retv;
};


//======================================
char* mmgetnew_str(const char* str)
//======================================
{
   assert(str != NULL);
   return (char*)mmgetnew_copy(str, strlen(str) + 1);
};

    //********************************
    // debugging
    //********************************

FILE* __mmdebug_file = NULL;
const char* __mmdebug_mark = NULL;

//=========================================
void mmdebug_open_file(const char* path)
//=========================================
{
   __mmdebug_file = fopen(path, "w"); 
   assert(__mmdebug_file != NULL);
};

//============================
void mmdebug_close_file()
//============================
{
   assert(__mmdebug_file != NULL);
   fclose(__mmdebug_file);
};

//===================================
void mmdebug_mark(const char* mark)
//===================================
{
   __mmdebug_mark = mark; 
};

//=====================
void mmdebug_clear()
//=====================
{
   __mmdebug_mark = NULL;
};

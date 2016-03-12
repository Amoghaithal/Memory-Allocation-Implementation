/*
 * =============================================================================
 *
 *       Filename:  mem_alloc.hpp
 *
 *    Description:  Memory allocator and manager 
 *
 *        Version:  1.0
 *        Created:  09/03/16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:	Amogha K.G 
 *
 * =============================================================================
 */

#include <stdlib.h>

/*
 * ===============================================================================
 *
 * This structure is used to keep track of entire list of memory. Every allocation 
 * of memory should be this memory + actual memory. So that we can keep track of which
 * block is free.
 *
 * ===============================================================================
 * */
typedef struct meta_data {

    size_t size;             // indicates the size of the memory allocated .     
    struct meta_data *next; // here I am using linked list , you are free to use any data structure.
    int free;                // Indicates this memory is free . Instead of really freeing the memory, we can just overwrite them if free is 1.
    // many more if req.

} meta_data;


#define META_DATA_SIZE sizeof(meta_data)


/*
 *=================================================================================
 *
 * This base ptr points to starting point of the list. This is initialized when memory 
 * is first given to the process.
 *
 * ================================================================================
 * */
meta_data * base_ptr = NULL;

/* 
 * ==============================================================================
 * This function is called by the process that uses the mem_alloc. There wont be any main() 
 * function defined for this library .Since if you define a main any program/process cannot 
 *  use your functions, because they will become two different processes. You have to 
 *  link this library to the process. This we will see it later.
 *
 *
 *  Mem_Init is called only once by a process using your routines. sizeOfRegion is the number
 *  of bytes that you should request from the OS using mmap.
 *
 *  for mmap prototype please  see man mmap
 *
 *  And you have to use only this memory for all internal purposes i.e. you cannot use malloc 
 *  in this library.Also you cannot use global array , you can use global structure pointer to head
 *  of free list.
 *
 * ================================================================================
 * */
int mem_init(int sizeOfRegion);


/*
 * ================================================================================
 *
 *
 * mem_alloc() is similar to the library function malloc(). Mem_Alloc takes as input the size 
 * in bytes of the object to be allocated and returns a pointer to the start of that object. 
 * The function returns NULL if there is not enough free space within sizeOfRegion allocated 
 * by mem_init to satisfy this request.
 *
 * ================================================================================
*/
 void * mem_alloc(int size);


/*
 * =================================================================================
 *
 * mem_free frees the memory object that ptr falls within.  like  free(), 
 * if ptr is NULL, then no operation is performed. The function returns 0 on success 
 * and -1 if ptr to does not fall within a currently allocated object
 *
 * =================================================================================
 * */

 int mem_free(void *ptr);


 /*
  * =================================================================================
  *
  * This function returns 1 if ptr falls within a currently allocated object and 0 if it does not.
  *
  * =================================================================================
  * */

 int mem_is_valid(void *ptr);

 /*
  * =================================================================================
  *
  * If ptr falls within the range of a currently allocated object, then this function 
  * returns the size in bytes of that object; otherwise, the function returns -1.
  *
  * =================================================================================
  * */

 int mem_get_size(void *ptr);


 /*
  *
  * ================================================================================
  *
  * returns the pointer to memory for requested size.Null if not possible.
  *
  * */
 meta_data *find_free_block(meta_data **last, size_t size);


 /*
  * ==================================================================================
  *
  * Add two adjacent free memory blocks to a single block.
  *
  * ==================================================================================
  * */

 void add_blocks (struct meta_data ** first);


/*
 * ==================================================================================
 *
 * These we can see later. They have to just use mem_alloc.
 *
 * ==================================================================================
 * */
void *mem_realloc(void *ptr, size_t size);

void *mem_calloc(size_t elem, size_t elem_size);

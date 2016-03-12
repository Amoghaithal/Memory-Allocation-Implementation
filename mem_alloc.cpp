/*
 * =============================================================================
 *
 *       Filename:  mem_alloc.cpp
 *
 *    Description:  Memory allocator and manager 
 *
 *        Version:  1.0
 *        Created:  09/03/16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author: Amogha K.G
 *
 * =============================================================================
 */

#include "mem_alloc.hpp"
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h> // used for rand()

int max_size_allocated=0;
static int size_allocated_till_now =0;
meta_data *last ;

/*
 *
 * open a region for memory.
 * map it to write it to that region.
 * I havent checked it in windows. May be /dev/zero 
 * might not be there in windows.better to use linux.
 *
 * */

int mem_init(int sizeOfRegion) {
    int fd = open("/dev/zero",O_RDWR);
    base_ptr = (meta_data *)mmap(NULL,sizeOfRegion,PROT_READ | PROT_WRITE,MAP_PRIVATE,fd,0);
    if (base_ptr == MAP_FAILED) {
        perror("mmap failed to allocate memory");
        return -1;
    }
    max_size_allocated = sizeOfRegion;
    last =(meta_data *)base_ptr;
    close(fd);  
    return 0;

}

/*
 *merge consecutive blocks to single block.
 * 
 * */

void merge_blocks(void) {
	meta_data *current =(meta_data *)base_ptr;
	meta_data *prev =(meta_data *)base_ptr;
	bool free =false;
	while(current) {
		if(current->free) {
			if(free) {
				prev->size +=(current->size);
				prev->next =current->next;
			}
			else {
				free =true;
				prev =current;
			}
		}
		else {
			free =false;
			prev =current;
		}
		current =current->next;
	}	
}

/*
 *insert a new block after resizing.
 * 
 * */

void insert_block(meta_data * current,int size) {
	if(size < META_DATA_SIZE) {
		return ;
	}
	meta_data * temp =current->next;
	meta_data * new_block =(meta_data *)((char *)(current+1) + current->size);
  	new_block->size =size-META_DATA_SIZE;
	new_block->free =1;
	new_block->next =temp;
	current->next =new_block;
	return ;	
}

/*
 *
 * find the free memory from already allocated blocks.
 *
 * */

meta_data * find_free_block(size_t size) {
    
   meta_data * current=base_ptr;
   while(current) {
   if ( current->size >= size && current->free==1 ) {
         int current_size =current->size;
	     current->size =size;
         current->free =0;
         insert_block(current,current_size -size);    
	     return current+1;
        }
        current =current->next;
    }
  return NULL;
}

/*
 * idea is to first allocate the memory till end of the sizeOfRegion.If it is 
 * already full, then try to find the free block if mem_free is used, if not 
 * found return NULL.
 *  
 * */

void * mem_alloc(int size) {
    
    if (size_allocated_till_now ==0){
        base_ptr->size =size;
        base_ptr->free =0;
        base_ptr->next =NULL;
        size_allocated_till_now = size_allocated_till_now+ size + META_DATA_SIZE;
        last = base_ptr;
        return base_ptr+1;
    }
    if (size_allocated_till_now + size + META_DATA_SIZE < max_size_allocated) {
        meta_data * current;
        current = (meta_data *)(((char *)(last+1))+ last->size);
        current->size = size;
        current->free =0;
        size_allocated_till_now = size_allocated_till_now + size + META_DATA_SIZE;
        current->next =NULL;
        last->next =current;
        last = current;
        return current+1;
    }

    return (void *)find_free_block(size);
}

/*
 * pointer moves by sizeof the data type it is pointing to
 *
 * */

meta_data * get_meta_data(void * ptr) {
    return (meta_data *)ptr -1 ;
}

/*
 * I have assumed that ptr given is valid one.
 * In current implementation there is no way to check this.
 * And also this cannot be used to free memory in middle of the block
 * i.e if memory is allocated fro ptr to ptr+50. we cannot give ptr+30.
 * This is because there is no way I can know ptr from ptr+30. 
 * 
 *
 * */

int mem_free(void *ptr) {
    meta_data * cur_ptr = get_meta_data(ptr);
    cur_ptr->free =1;
    merge_blocks();
    return 1;
}

/*
 * just used for testing make this a shared library, so that you can link it
 * to any process. 
 * */

int main(){
    int sizeOfRegion;
    scanf("%d",&sizeOfRegion);
    if(mem_init(sizeOfRegion)!=-1) {
        for(int i=1;i<20;i++) {
	    int random_value = rand()%sizeOfRegion +1; 
            char * str1=(char *)mem_alloc(random_value*sizeof(char));
            if(str1!=NULL) {
                printf("Allocated memory for : %d\n",random_value);
	        if(i%2==0) {
                   mem_free(str1);
		   printf("Freed memory for %d\n",random_value);
                }
            }
            else {
                printf("failed to allocate for: %d\n",random_value);
            }
        }
    }
    else {
        printf("failed\n");
    }
}

/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"


//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//

//=====================================================
// 1) GET BLOCK SIZE (including size of its meta data):
//=====================================================
__inline__ uint32 get_block_size(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (*curBlkMetaData) & ~(0x1);
}

//===========================
// 2) GET BLOCK STATUS:
//===========================
__inline__ int8 is_free_block(void* va)
{
	uint32 *curBlkMetaData = ((uint32 *)va - 1) ;
	return (~(*curBlkMetaData) & 0x1) ;
}

//===========================
// 3) ALLOCATE BLOCK:
//===========================

void *alloc_block(uint32 size, int ALLOC_STRATEGY)
{
	void *va = NULL;
	switch (ALLOC_STRATEGY)
	{
	case DA_FF:
		va = alloc_block_FF(size);
		break;
	case DA_NF:
		va = alloc_block_NF(size);
		break;
	case DA_BF:
		va = alloc_block_BF(size);
		break;
	case DA_WF:
		va = alloc_block_WF(size);
		break;
	default:
		cprintf("Invalid allocation strategy\n");
		break;
	}
	return va;
}

//===========================
// 4) PRINT BLOCKS LIST:
//===========================

void print_blocks_list(struct MemBlock_LIST list)
{
	cprintf("=========================================\n");
	struct BlockElement* blk ;
	cprintf("\nDynAlloc Blocks List:\n");
	LIST_FOREACH(blk, &list)
	{
		cprintf("(size: %d, isFree: %d)\n", get_block_size(blk), is_free_block(blk)) ;
	}
	cprintf("=========================================\n");

}
//
////********************************************************************************//
////********************************************************************************//

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

bool is_initialized = 0;
//==================================
// [1] INITIALIZE DYNAMIC ALLOCATOR:
//==================================
void initialize_dynamic_allocator(uint32 daStart, uint32 initSizeOfAllocatedSpace)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (initSizeOfAllocatedSpace % 2 != 0) initSizeOfAllocatedSpace++; //ensure it's multiple of 2
		if (initSizeOfAllocatedSpace == 0)
			return ;
		is_initialized = 1;
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #04] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("initialize_dynamic_allocator is not implemented yet");
	//Your Code is Here...

}
//==================================
// [2] SET BLOCK  & FOOTER:
//==================================
void set_block_data(void* va, uint32 totalSize, bool isAllocated)
{
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("set_block_data is not implemented yet");
    if (totalSize < DYN_ALLOC_MIN_BLOCK_SIZE)
    {
	  cprintf("The total size is not enough");
    }

   int*header = (int*) va;
   int*footer = (int*)(char*)va + totalSize - 2*sizeof(int);

   header[0]= totalSize;
   header[1]= isAllocated;
   footer[0]= totalSize;
   footer[1]= isAllocated;

}


//=========================================
// [3] ALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *alloc_block_FF(uint32 size)
{
	//==================================================================================
	//DON'T CHANGE THESE LINES==========================================================
	//==================================================================================
	{
		if (size % 2 != 0) size++;	//ensure that the size is even (to use LSB as allocation flag)
		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
			size = DYN_ALLOC_MIN_BLOCK_SIZE ;
		if (!is_initialized)
		{
			uint32 required_size = size + 2*sizeof(int) /* & footer*/ + 2*sizeof(int) /*da begin & end*/ ;
			uint32 da_start = (uint32)sbrk(ROUNDUP(required_size, PAGE_SIZE)/PAGE_SIZE);
			uint32 da_break = (uint32)sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("alloc_block_FF is not implemented yet");
	//Your Code is Here...

}
//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("alloc_block_BF is not implemented yet");

//	struct MemBlock_LIST *bestfitblock = NULL;
//	uint32 bestfitblocksize = DYN_ALLOC_MAX_BLOCK_SIZE;
//
//    if (size == 0){
//    	return NULL;
//    }
//    if (size % 2 != 0) size++;	//ensure that the size is even
//    		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
//    			size = DYN_ALLOC_MIN_BLOCK_SIZE ;
//
//    uint32 required_size = size + 2*sizeof(int)+ 2*sizeof(int);
//
//    struct MemBlock_LIST *current = freeBlocksList.lh_first;
//    while(current != NULL){
//      uint32 blocksize = current->size;
//
//      if (blocksize >= required_size){
//         if(blocksize < bestfitblocksize){
//    	  bestfitblock = current;
//    	  bestfitblocksize = blocksize;
//         }
//      }
//      set_block_data(bestfitblock,current->size,1);
//      current = current->___ptr_next;
//    }
//
//
//  return (char*)bestfitblock - 2*sizeof(int);
}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	//TODO: [PROJECT'24.MS1 - #07] [3] DYNAMIC ALLOCATOR - free_block
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("free_block is not implemented yet");
	//Your Code is Here...
}

//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size)
{
	/*PLEASE NOTICE that new_size does not include meta data (header,footer) , shimaa*/
	//[1] Test calling realloc with VA = NULL. It should call alloc
	/* Try to allocate set of blocks with different sizes*/
	/* Try to allocate a block with a size equal to the size of the first existing free block*/
	//[2] Test realloc by passing size = 0. It should call free //return null
	//test calling it with va & ZERO
	//test calling it with NULL & ZERO




	bool IS_FIT = 1;
	uint32 block_size = get_block_size(va);
	uint32 all_new_size = new_size + 8;
	struct BlockElement* next_block_addr = (struct BlockElement*)(va + (block_size - 4) + 4);//the addr of the next block , are you sure about datatypes?
	struct BlockElement* new_next_block_addr = (struct BlockElement*)(va + (all_new_size-4) + 4);//the addr of the next block after reallocation ,after header, are you sure about datatypes?

	//[]Test if that block is the last block in heap ..shimaa -> which is [5] in test
	if(get_block_size(next_block_addr) == 1 && !is_free_block(next_block_addr)){
		//return alloc_block_FF(new_size);
		//sbrk(the_extra_size/(4*1024));
		return NULL;
	}


	if(all_new_size > block_size) {
		//[3] Test realloc with increased sizes
		uint32 the_extra_size = all_new_size - block_size;//the difference between new size and old size


		uint32 next_block_size = get_block_size(next_block_addr);



		if(is_free_block(next_block_addr)) {

			if(the_extra_size < next_block_size) {
				//[3.1] reallocate in same place (NO relocate - split)(shimaa:will take part of next block)
				//check return address
				//check the new address of the next free block,shimaa
				uint32 the_remaining_size = next_block_size-the_extra_size;//the remaining_size in free block after taking from it to our block

				if(the_remaining_size >= 16) {

					set_block_data(va, all_new_size, 1);
					set_block_data(new_next_block_addr, the_remaining_size, 0);


					free_block(new_next_block_addr);
					//LIST_INSERT_AFTER(&freeBlocksList, next_block_addr,new_next_block_addr);
				}
				else {
					//fragment
					//check by nouran
					set_block_data(va, block_size+next_block_size,1);//will exist internal fragment (free unused mem in block)
				}

				LIST_REMOVE(&freeBlocksList, next_block_addr);
				return va;

			}else if(the_extra_size == next_block_size){
				//[3.2] reallocate in same place (NO relocate - NO split) (shimaa:will take all the next block)
				set_block_data(va, all_new_size,1);//will exist internal fragment (free unused mem in block)
				LIST_REMOVE(&freeBlocksList, next_block_addr);
				return va;

			}else {
				//reallocate in another place
				//IS_FIT = 0;
				//ask raazan how to realloc with data
				//ask razan will va be void or int or what?
				uint32* new_va = (uint32*)alloc_block_FF(new_size);
				uint32 tmp_block_size = block_size - 8;//check
				while (tmp_block_size--) {
					*new_va++ = *((uint32*)va++);
				}
				return new_va;
			}
		}else {
			uint32* new_va = alloc_block_FF(new_size);
			uint32 tmp_block_size = block_size - 8;//check
			while (tmp_block_size--) {
				*new_va++ = *((uint32*)va++);
			}
			return new_va;
		}
		/*dont forget in 3.3,3.4 to move with the same content */
		//[3.3] reallocate in another place (relocate - NO split)shimaa
		//will change location and take full block
		//if the size is more than the next free block
		/*block after ours is free but not enough*/
		/*block after ours is not free*/
		//check if the new block we will go to will let remaining size smaller than 16?
		//check if the new block we will go to will let remaining size greater than 16
		//[3.4] reallocate in another place (relocate - split)shimaa
		//will change location and take half block
		//[3.5] no enough space (NO relocate - NO split)-> search till the end but no space -> return null ,shimaa


	}else if (all_new_size < block_size){
		//shrinking the block and update the free block list
		if (all_new_size % 2 != 0) all_new_size++; //ensure it's multiple of 2
		uint32 remainig_size = block_size-all_new_size;

		//[4] Test realloc with decreased sizes
		//[4.0] newsizze<16
		if(is_free_block(new_next_block_addr)){
			//[4.2] next block is empty (coalesce)shimaa
				/*divide the block to [1.full block] [2. free block will join to the next free block ,
				 *  dont forget to update the addr of the beginning of the free block in free block list]*/

			if(all_new_size < 16){
				set_block_data(va, 16 , 1);//check if second param is all size our without metadata
				set_block_data(new_next_block_addr, block_size-16 , 0);//is it needed before freeblock or not

				free_block(new_next_block_addr);

			}else{
				set_block_data(va,all_new_size , 1);
				set_block_data(new_next_block_addr,remainig_size  , 0);//is it needed before freeblock or not

				free_block(new_next_block_addr);
			}

		}else{
			//[4.1] next block is full (NO coalesce)
			 //1.divide the block to [1.full block] [2. free block if it is more than 16]
			 //2.[Internal Framgmentation]
			if(all_new_size < 16){

			}else{

			}

			if(remainig_size < 16){
				return va;
			}else{
				set_block_data(va,all_new_size , 1);
				set_block_data(new_next_block_addr,remainig_size  , 0);//is it needed before freeblock or not

				free_block(new_next_block_addr);
			}
		}


	}else {
		//[6] realloc with the same size
		return va;
	}






	//TODO: [PROJECT'24.MS1 - #08] [3] DYNAMIC ALLOCATOR - realloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("realloc_block_FF is not implemented yet");
	//Your Code is Here...

	return va;//not the actual return value , but to avoid any error while running

}

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/
//=========================================
// [7] ALLOCATE BLOCK BY WORST FIT:
//=========================================
void *alloc_block_WF(uint32 size)
{
	panic("alloc_block_WF is not implemented yet");
	return NULL;
}

//=========================================
// [8] ALLOCATE BLOCK BY NEXT FIT:
//=========================================
void *alloc_block_NF(uint32 size)
{
	panic("alloc_block_NF is not implemented yet");
	return NULL;
}

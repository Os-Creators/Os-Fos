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

	//2022170629
    if (totalSize < DYN_ALLOC_MIN_BLOCK_SIZE)
    {
	  cprintf("The total size is not enough to set the data");
	  return;
    }
	//2022170629
   int*meta_header = ((uint32 *)va - 1);
   int*meta_footer = (int*)(char*)va + totalSize - sizeof(uint32);
    //2022170629
   meta_header[0]= totalSize;
   meta_header[1]= isAllocated? 1 : 0;
   meta_footer[0]= totalSize;
   meta_footer[1]= isAllocated? 1 : 0;

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
   return 0;
}
//=========================================
// [4] ALLOCATE BLOCK BY BEST FIT:
//=========================================
void *alloc_block_BF(uint32 size)
{
	//TODO: [PROJECT'24.MS1 - BONUS] [3] DYNAMIC ALLOCATOR - alloc_block_BF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("alloc_block_BF is not implemented yet");

	//2022170629
	struct MemBlock_LIST *mbestfitblock = NULL;
	uint32 mbestfitblocksize = DYN_ALLOC_MAX_BLOCK_SIZE;
	//2022170629
    if (size == 0){
    	return NULL;
    }
    if (size % 2 != 0) size++;
    		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
    			size = DYN_ALLOC_MIN_BLOCK_SIZE ;
    uint32 size_needed_by_blocks = size + sizeof(uint32)+ sizeof(uint32);

    //2022170629

    struct MemBlock_LIST *now = LIST_FIRST(freeBlocksList);
    while(now != NULL){
      uint32 mblocksize = now->size;

      if (mblocksize >= size_needed_by_blocks){
         if(mblocksize < mbestfitblocksize){
    	  mbestfitblock = now;
    	  mbestfitblocksize = mblocksize;
         }
      }
      now = now->___ptr_next;
    }
      if (mbestfitblock == NULL) {
    	  uint32 pagas_size = getpagesize();
    	  uint32 pagas_needed = (size_needed_by_blocks + pagas_size - 1) / pagas_size;

    	  mbestfitblock = (struct MemBlock_LIST *)sbrk(pagas_needed * pagas_size);
    	  if (mbestfitblock == (void *)-1)
    	  {
    	      return NULL;
    	  }
    	  mbestfitblock->size = size_needed_by_blocks;

        } else {
            uint32 remaining_size = mbestfitblocksize - size_needed_by_blocks;

            if (remaining_size > 0)
            {
                if (remaining_size >= (DYN_ALLOC_MIN_BLOCK_SIZE + sizeof(uint32) + sizeof(uint32))) {
                struct MemBlock_LIST *new_block = (struct MemBlock_LIST *)((char *)bestfitblock + size_needed_by_blocks);
                set_block_data(new_block, remaining_size, 0);
                new_block->___ptr_next = mbestfitblock->___ptr_next; // Point new block to the next block
                mbestfitblock->___ptr_next = new_block;
             }
            set_block_data(mbestfitblock,size_needed_by_blocks,1);
        }
    if (!is_initialized)
          		{
          			uint32 required_size = size + 2*sizeof(int) + 2*sizeof(int);
          			uint32 da_start = mbestfitblock;
          			uint32 da_break = (uint32)sbrk(0);
          			initialize_dynamic_allocator(da_start, da_break - da_start);
          		}
 }
      return (void*) mbestfitblock;
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
	//TODO: [PROJECT'24.MS1 - #08] [3] DYNAMIC ALLOCATOR - realloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("realloc_block_FF is not implemented yet");
	//Your Code is Here...
	return 0;
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

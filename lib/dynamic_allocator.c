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
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #04] [3] DYNAMIC ALLOCATOR - initialize_dynamic_allocator
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
    //panic("initialize_dynamic_allocator is not implemented yet");
	//Your Code is Here...
	//2022170597
		//==================================================================================
			//DON'T CHANGE THESE LINES==========================================================
			//==================================================================================
			{
				if (initSizeOfAllocatedSpace % 2 != 0) initSizeOfAllocatedSpace++; //ensure it's multiple of 2
				if (initSizeOfAllocatedSpace == 0)
					return ;
				is_initialized = 1;
			}


    uint32* BEGblock597 = (uint32*) daStart ;
    uint32* ENDBlock = (uint32*) (daStart +  initSizeOfAllocatedSpace - sizeof(int));
	struct BlockElement* first597block = (struct BlockElement*)daStart;
	set_block_data(first597block,initSizeOfAllocatedSpace-8,0);
	freeBlocksList.lh_first = first597block;
	freeBlocksList.lh_last = first597block;
	((struct BlockElement*)BEGblock597)->prev_next_info.le_next = first597block;
	first597block->prev_next_info.le_prev = (struct BlockElement*)BEGblock597;
	first597block->prev_next_info.le_next = (struct BlockElement*)ENDBlock;
	((struct BlockElement*)ENDBlock)->prev_next_info.le_prev = first597block;

}
//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================
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
    uint32 header = (uint32)va;


        *(uint32*)(header) = totalSize;
        *(uint32*)(header + 4) = isAllocated ? 1 : 0;


        uint32 footer = header + totalSize - 8;


        *(uint32*)(footer) = totalSize;
        *(uint32*)(footer + 4) = isAllocated ? 1 : 0;
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
			uint32 required_size = size + 2*sizeof(int) /*header & footer*/ + 2*sizeof(int) /*da begin & end*/ ;
			uint32 da_start = (uint32)sbrk(ROUNDUP(required_size, PAGE_SIZE)/PAGE_SIZE);
			uint32 da_break = (uint32)sbrk(0);
			initialize_dynamic_allocator(da_start, da_break - da_start);
		}
	}
	//==================================================================================
	//==================================================================================

	//TODO: [PROJECT'24.MS1 - #06] [3] DYNAMIC ALLOCATOR - alloc_block_FF
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
    //panic("alloc_block_FF is not implemented yet");
	//Your Code is Here...
    //2022170597
	 if (size == 0)
		    {
			  return NULL;
		    }
	struct BlockElement * mfirst597fitblock = NULL;
	uint32 mfirst597fitblocksize;
	uint32 size_needed_by_blocks = size + 2*sizeof(uint32);
	 struct BlockElement *now;
	    LIST_FOREACH(now ,&freeBlocksList) {
	        uint32 mblocksize = sizeof(*now);

	        if (mblocksize >= size_needed_by_blocks) {

	        	 mfirst597fitblock = now;
	                mfirst597fitblocksize = mblocksize;

	        }
	    }
	    if ( mfirst597fitblock == NULL) {

	    	 mfirst597fitblock = (struct BlockElement *)sbrk(ROUNDUP(size_needed_by_blocks + sizeof( mfirst597fitblock), PAGE_SIZE));
	           if ( mfirst597fitblock == (void *)-1) {
	               return NULL;
	           }
	           *((uint32 *) mfirst597fitblock) = size_needed_by_blocks;
	                  mfirst597fitblock = (struct BlockElement *)((char *) mfirst597fitblock + sizeof(uint32));
	           } else {
	               uint32 remaining_size = mfirst597fitblocksize - size_needed_by_blocks;

	               if (remaining_size > 0)
	               {
	                   if (remaining_size >= (DYN_ALLOC_MIN_BLOCK_SIZE + sizeof(uint32) + sizeof(uint32))) {
	                   struct BlockElement *new_block = (struct BlockElement *)((char *) mfirst597fitblock + size_needed_by_blocks);
	                   set_block_data(new_block, remaining_size, 0);
	                   new_block->prev_next_info.le_next =  mfirst597fitblock->prev_next_info.le_next;
	                   if (new_block->prev_next_info.le_next != NULL) {
	                       new_block->prev_next_info.le_next->prev_next_info.le_prev = new_block;
	                   }

	                   mfirst597fitblock->prev_next_info.le_next = new_block;
	                   new_block->prev_next_info.le_prev = mfirst597fitblock;
	                }
	               set_block_data( mfirst597fitblock,size_needed_by_blocks,1);
	           }
	           }
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
	struct BlockElement *mbestfitblock = NULL;
	uint32 mbestfitblocksize = DYN_ALLOC_MAX_BLOCK_SIZE;

	if (!is_initialized) {
	        uint32 required_size = size + 2 * sizeof(uint32) + 2 * sizeof(uint32);
	        uint32 da_start = (uint32)sbrk(0);
	        void *allocated_memory = sbrk(ROUNDUP(required_size, PAGE_SIZE));
	        if (allocated_memory == (void *)-1) {
	            return NULL;
	        }

	        initialize_dynamic_allocator(da_start, (uint32)(sbrk(0) - da_start));

	        is_initialized = 1;
	    }
	//2022170629
    if (size == 0){
    	return NULL;
    }
    if (size % 2 != 0) size++;
    		if (size < DYN_ALLOC_MIN_BLOCK_SIZE)
    			size = DYN_ALLOC_MIN_BLOCK_SIZE ;
    uint32 size_needed_by_blocks = size + 2*sizeof(uint32);

    //2022170629

    struct BlockElement *now;
    LIST_FOREACH(now ,&freeBlocksList) {
        uint32 mblocksize = sizeof(*now);

        if (mblocksize >= size_needed_by_blocks) {
            if (mblocksize < mbestfitblocksize) {
                mbestfitblock = now;
                mbestfitblocksize = mblocksize;
            }
        }
    }

    if (mbestfitblock == NULL) {

        mbestfitblock = (struct BlockElement *)sbrk(ROUNDUP(size_needed_by_blocks + sizeof(mbestfitblock), PAGE_SIZE));
        if (mbestfitblock == (void *)-1) {
            return NULL;
        }
        *((uint32 *)mbestfitblock) = size_needed_by_blocks;
               mbestfitblock = (struct BlockElement *)((char *)mbestfitblock + sizeof(uint32));
        } else {
            uint32 remaining_size = mbestfitblocksize - size_needed_by_blocks;

            if (remaining_size > 0)
            {
                if (remaining_size >= (DYN_ALLOC_MIN_BLOCK_SIZE + sizeof(uint32) + sizeof(uint32))) {
                struct BlockElement *new_block = (struct BlockElement *)((char *)mbestfitblock + size_needed_by_blocks);
                set_block_data(new_block, remaining_size, 0);
                new_block->prev_next_info.le_next = mbestfitblock->prev_next_info.le_next;
                if (new_block->prev_next_info.le_next != NULL) {
                    new_block->prev_next_info.le_next->prev_next_info.le_prev = new_block;
                }

                mbestfitblock->prev_next_info.le_next = new_block;
                new_block->prev_next_info.le_prev = mbestfitblock;
             }
            set_block_data(mbestfitblock,size_needed_by_blocks,1);
        }
        }
   return 0;
}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{
	//TODO: [PROJECT'24.MS1 - #07] [3] DYNAMIC ALLOCATOR - free_block
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
    //panic("free_block is not implemented yet");
	//Your Code is Here...
    void merge_blocks(struct BlockElement *MLFprev_block, struct BlockElement *MLFcurrent_block)
    {

	    uint32 combined_size = get_block_size(MLFprev_block) + get_block_size(MLFcurrent_block);

	    set_block_data(MLFprev_block, combined_size, 0);

	     MLFprev_block->prev_next_info.le_next = MLFcurrent_block->prev_next_info.le_next;

	  }

	if (va == NULL){
		cprintf("The virtual address is Null please try agin later");
		return;
	}

    if (is_free_block(va))
	{
		cprintf("The given Block is already Free");
		return;
	}

    struct BlockElement *MLFblock597 = (struct BlockElement *)((char *)va - sizeof(struct BlockElement));
    uint32 MLFblockSize597 = get_block_size(va);
    set_block_data(va, MLFblockSize597, 0);

    //free(MLFblock597);

    struct BlockElement *MLFprev_block597 = MLFblock597->prev_next_info.le_prev;
    struct BlockElement *MLFnext_block597 = MLFblock597->prev_next_info.le_next;

    if (MLFprev_block597 && is_free_block(MLFprev_block597)) {
            merge_blocks(MLFprev_block597, MLFblock597);
        }

        if (MLFnext_block597 && is_free_block(MLFnext_block597)) {
            merge_blocks(MLFblock597, MLFnext_block597);
        }

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

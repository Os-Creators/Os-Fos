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
	// each block in the linked list
		struct block597 {
		    uint32 size;
		    struct block597* next;
		    struct block597* prev;
		};
		// Free block list structure
		struct block597_LIST {
		    struct block597* head;        // First free block in the list
		    struct block597* tail;        // Last free block in the list
		};
		// Free block list declaration
		struct block597_LIST Freeblock597List;
		// Function to initialize the list
		void lamiaa597 (struct block597_LIST* list) {
		    list->head = NULL;
		    list->tail = NULL;
		}
		// Function to insert a block at the head of the list
		void insertBlock597Fun(struct block597_LIST* list, struct block597* block) {
		    block->next = list->head;
		    block->prev = NULL;
		    if (list->head != NULL) {
		        list->head->prev = block;
		    }
		    list->head = block;
		    if (list->tail == NULL) {
		        list->tail = block;
		    }
		}
		// Function to insert a block at the end of the list
		void insertEndblock597Fun(struct block597_LIST* list, struct block597* block) {
		    block->next = NULL;
		    block->prev = list->tail;
		    if (list->tail != NULL) {
		        list->tail->next = block;
		    }
		    list->tail = block;
		    if (list->head == NULL) {
		        list->head = block;
		    }
		}
		//==================================================================================
			//DON'T CHANGE THESE LINES==========================================================
			//==================================================================================
			{
				if (initSizeOfAllocatedSpace % 2 != 0) initSizeOfAllocatedSpace++; //ensure it's multiple of 2
				if (initSizeOfAllocatedSpace == 0)
					return ;
				is_initialized = 1;
			}
	//a pointer to point on a first block by starting address
	struct block597* firstblock597 = (struct block597 *) daStart;
	//Set the BEG block size and mark it as free
	//(first "0" for "size" second for "free")
	firstblock597->size = initSizeOfAllocatedSpace | 0x0;
	//Set up the END block after the first block
		uint32 endblock597 = daStart + initSizeOfAllocatedSpace;  // Address of the end block
	    struct block597* endBlock = (struct block597 *) endblock597;
	    //The END block should have a size of 0
	    endBlock->size = 0;
	//Initialize the free block list
	lamiaa597(&Freeblock597List);
	//Insert the BEG block into the free block list
	insertBlock597Fun(&Freeblock597List, firstblock597);

    //Insert the END block at the end of the list
    insertEndblock597Fun(&Freeblock597List, endBlock);  // Insert the end block at the end

}
//==================================
// [2] SET BLOCK HEADER & FOOTER:
//==================================
void set_block_data(void* va, uint32 totalSize, bool isAllocated)
{
	//TODO: [PROJECT'24.MS1 - #05] [3] DYNAMIC ALLOCATOR - set_block_data
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("set_block_data is not implemented yet");
	//Your Code is Here...
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
	//Your Code is Here...

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

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


    uint32* BEGblock = (uint32*) daStart ;
    uint32* ENDBlock = (uint32*) (daStart +  initSizeOfAllocatedSpace - sizeof(int));

    *(BEGblock) = 1;
    *(ENDBlock) = 1;

	struct BlockElement* firstblock = (struct BlockElement*)(daStart+(2*sizeof(int)));

	set_block_data(firstblock, initSizeOfAllocatedSpace - 8 ,0);

	LIST_INSERT_HEAD(&freeBlocksList, firstblock);

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


    if (totalSize < DYN_ALLOC_MIN_BLOCK_SIZE)
    {
	  cprintf("The total size is not enough to set the data");
	  return;
    }

    uint32* header = (uint32*)(va-sizeof(int));
    uint32* footer = (uint32*)(va+totalSize-(2*sizeof(int)));


    uint32 Size_withAlloc = (totalSize & ~0x1) | (isAllocated ? 0x1 : 0x0);

    *(header) = *(footer) = Size_withAlloc;

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
	uint32 size_needed_by_blocks = size + 2*sizeof(int);
	struct BlockElement *now;

	LIST_FOREACH(now ,&freeBlocksList) {
		uint32 mblocksize = get_block_size(now);

		if (mblocksize >= size_needed_by_blocks) {

			mfirst597fitblock = now;
			mfirst597fitblocksize = mblocksize;
			break;

		}
	}

	if ( mfirst597fitblock == NULL) {

		//see ROUNDUP -> sprk-----------------
		mfirst597fitblock = (struct BlockElement *)sbrk(ROUNDUP(size_needed_by_blocks + sizeof( mfirst597fitblock), PAGE_SIZE));
		if ( mfirst597fitblock == (void *)-1) {
			return NULL;
		}

	   //*((uint32 *) mfirst597fitblock) = size_needed_by_blocks;
		//mfirst597fitblock = (struct BlockElement *)((char *) mfirst597fitblock + sizeof(uint32));
	} else {
	   uint32 remaining_size = mfirst597fitblocksize - size_needed_by_blocks;


	   //block fits and have more
	   if (remaining_size >= (DYN_ALLOC_MIN_BLOCK_SIZE + sizeof(uint32) + sizeof(uint32))/*16*/) {
		   //no fragment
		   struct BlockElement *new_block = (struct BlockElement *)((char *) mfirst597fitblock + size_needed_by_blocks);

		   set_block_data(new_block, remaining_size, 0);

		   //free_block(new_block);
	       LIST_INSERT_AFTER(&freeBlocksList,mfirst597fitblock , new_block);
	       LIST_REMOVE(&freeBlocksList, mfirst597fitblock);


	       set_block_data( mfirst597fitblock,size_needed_by_blocks,1);

	   }else{
		   //fragment
		   set_block_data(mfirst597fitblock, mfirst597fitblocksize, 1);
		   LIST_REMOVE(&freeBlocksList, mfirst597fitblock);
	   }


	}
	//cprintf("header in alloc. Actual addr H:%d H2:%p\n", *((uint32*)mfirst597fitblock-1), (uint32*)(mfirst597fitblock-4));

	return mfirst597fitblock;
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
	bool is_in_free_list(void* block){
		struct BlockElement* tmp_block;

		LIST_FOREACH (tmp_block, &freeBlocksList){

			if((struct BlockElement*)block == tmp_block){
				return 1;
			}

		}
		return 0;
	}


	void insert_block_in_order(struct BlockElement* block) {
	    struct BlockElement* current = freeBlocksList.lh_first;

	    // If list is empty, insert as the only element
	    if ( LIST_SIZE(&freeBlocksList) == 0) {
	        LIST_INSERT_HEAD(&freeBlocksList,block);
	        return;
	    }

	    // Traverse the list to find the correct position

	    struct BlockElement* tmp_block;
	    bool found_place = 0;

	    LIST_FOREACH (tmp_block, &freeBlocksList){

	    	if(tmp_block > block){
	    		LIST_INSERT_BEFORE(&freeBlocksList, tmp_block, block );
	    		found_place = 1;
	    		break;
			}
		}

	    if(!found_place)
	    	LIST_INSERT_TAIL(&freeBlocksList, block);

	}

    if (va == NULL){
        cprintf("The virtual address is Null please try again later");
        return;
    }

    uint32 size_of_prev_block = get_block_size((uint32 *)(va - sizeof(int)));//by footer, get_block_size will minus another 4byte
	struct BlockElement* brev_block_addr = (struct BlockElement *)((uint32 *)(va - size_of_prev_block));//footer of prev block

	uint32 block_size = get_block_size(va);
	uint32 size_needed_by_block = get_block_size(va);
	struct BlockElement *new_block = (struct BlockElement *)((uint32 *)(va + size_needed_by_block));

	struct BlockElement* begin_of_the_free_block = va;

	//brev is free?
	if (is_free_block(brev_block_addr)){
		block_size += get_block_size(brev_block_addr);
		set_block_data(brev_block_addr, block_size, 0);
		begin_of_the_free_block = brev_block_addr;

		//if VA already was free and in list , remove it (cause the begin changed)
		if(is_in_free_list(va))LIST_REMOVE(&freeBlocksList, (struct BlockElement*)va);

		//will not insert any thing in list because (brev block) already was

	}else{
		set_block_data(va, block_size, 0);
		if(!is_in_free_list(va)) insert_block_in_order((struct BlockElement*)va);
	}

	//next is free?
	if (is_free_block(new_block)) {

		block_size += get_block_size(new_block);
		set_block_data(begin_of_the_free_block, block_size, 0);

		LIST_REMOVE(&freeBlocksList, new_block);

	}



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
	if(va == NULL){
		return alloc_block_FF(new_size);
	}
	//[2] Test realloc by passing size = 0. It should call free //return null
	//test calling it with va & ZERO
	if(new_size == 0 && va != NULL){
		free_block(va);
		return NULL;
	}
	//test calling it with NULL & ZERO
	if(new_size == 0 && va != NULL){
		return alloc_block_FF(0);
	}




	bool IS_FIT = 1;
	uint32 block_size = get_block_size(va);
	uint32 all_new_size = new_size + 8;
	struct BlockElement* next_block_addr = (struct BlockElement*)(va + (block_size - 4) + 4);//the addr of the next block , are you sure about datatypes?
	struct BlockElement* new_next_block_addr = (struct BlockElement*)(va + (all_new_size-4) + 4);//the addr of the next block after reallocation ,after header, are you sure about datatypes?

	//[]Test if that block is the last block in heap ..shimaa -> which is [5] in test
	if(get_block_size(next_block_addr) == 0 && !is_free_block(next_block_addr)){
		return alloc_block_FF(new_size);
		//sbrk(the_extra_size/(4*1024));
		//return NULL;
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
		//[4.0] new size<16
		//[4.1] next block is full (NO coalesce)
		 //1.divide the block to [1.full block] [2. free block if it is more than 16]
		 //2.[Internal Framgmentation]
		//[4.2] next block is empty (coalesce)shimaa
			/*divide the block to [1.full block] [2. free block will join to the next free block ,
			 *  dont forget to update the addr of the beginning of the free block in free block list]*/

		if(all_new_size < 16)
		{
			new_next_block_addr = va + 16;
			remainig_size = block_size -16;
			all_new_size=16;

		}
		if(remainig_size>=16)
		{
			//merge
			set_block_data(va, all_new_size , 1);
			set_block_data(new_next_block_addr,remainig_size,0); //check free implementaion

			free_block(new_next_block_addr);

			//call free or continue coding
		}
		return va;



	}else {
		//[6] realloc with the same size
		return va;
	}



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

/*
 * dynamic_allocator.c
 *
 *  Created on: Sep 21, 2023
 *      Author: HP
 */
#include <inc/assert.h>
#include <inc/string.h>
#include "../inc/dynamic_allocator.h"
//#include "../kern/mem/kheap.h"

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

	//2022170597 we need to create the begin and end blocks of the freeblockslist
    uint32* LMBEGblock597 = (uint32*) daStart ;
    uint32* LMENDBlock597 = (uint32*) (daStart +  initSizeOfAllocatedSpace - sizeof(int));

    //2022170597 we need to make them equal to 1 like the dr said in the video
    *(LMBEGblock597) = 1;
    *(LMENDBlock597) = 1;

    //2022170597 we need to make the first block of the list
	struct BlockElement* LMfirstblock597 = (struct BlockElement*)(daStart+(2*sizeof(int)));

    //2022170597 then we are setting the metadata of the firstblock with the size passed to the function
	set_block_data(LMfirstblock597, initSizeOfAllocatedSpace - 8 ,0);

    //2022170597 then initialize the freeblocklist and insert the first block to it
	LIST_INIT(&freeBlocksList);
	LIST_INSERT_HEAD(&freeBlocksList, LMfirstblock597);

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

        //2022170629 check if the size given to the function is large enough to be able to set the block
	    if (totalSize < DYN_ALLOC_MIN_BLOCK_SIZE)
	    {
		  cprintf("The total size is not enough to set the data");
		  return;
	    }

	    //2022170629 create the variables for the header and footer so we can save the data in

	    uint32* MHheader629 = (uint32*)(va-sizeof(int));
	    uint32* MHfooter629 = (uint32*)(va+totalSize-(2*sizeof(int)));

	    //2022170629 create a variable with the data that is supposed to be saved by masking them
	    uint32 MHSize_withAlloc629 = (totalSize & ~0x1) | (isAllocated ? 0x1 : 0x0);

	    //2022170629 give the data to the header & footer of the block
	    *(MHheader629) = *(MHfooter629) = MHSize_withAlloc629;

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
				//(uint32*) (da_start +  (da_break - da_start) - sizeof(int));

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
		struct BlockElement * lfirst597fitblock = NULL;
		uint32 lfirst597fitblocksize;
		uint32 size_needed_by_blocks597 = size + 2*sizeof(int);
		struct BlockElement *now597;

		LIST_FOREACH(now597 ,&freeBlocksList) {
			uint32 l597blocksize = get_block_size(now597);

			if (l597blocksize >= size_needed_by_blocks597) {

				lfirst597fitblock = now597;
				lfirst597fitblocksize = l597blocksize;
				break;

			}
		}

		if ( lfirst597fitblock == NULL) {

			//see ROUNDUP -> sprk-----------------
			lfirst597fitblock = (struct BlockElement *)sbrk(ROUNDUP(size_needed_by_blocks597, PAGE_SIZE)/PAGE_SIZE);
			if ( lfirst597fitblock == (void *)-1) {
				return NULL;
			}else{
				//uint32* end_block = (uint32*)((char*)segment_break-sizeof(int)) ;
				uint32* end_block = (uint32*)lfirst597fitblock ;

			    if(is_free_block(end_block-sizeof(int))){
			    	uint32 prev_block_size =get_block_size(end_block-sizeof(int));
			    	uint32* prev_block_va = (uint32*)(((char*)end_block - prev_block_size) + sizeof(int));
			    	set_block_data( prev_block_va,prev_block_size + size_needed_by_blocks597,0);
			    }else{
			    	set_block_data( end_block,size_needed_by_blocks597,0);
			    	LIST_INSERT_TAIL(&freeBlocksList,(struct BlockElement *)end_block);
			    	//lfirst597fitblock = (struct BlockElement*)end_block;

			    }
			    end_block = (uint32*)((char*)end_block+size_needed_by_blocks597);
				*(end_block) = 1;


			}

		   //*((uint32 *) mfirst597fitblock) = size_needed_by_blocks;
			//mfirst597fitblock = (struct BlockElement *)((char *) mfirst597fitblock + sizeof(uint32));
		}
	   uint32 remaining_size55 = lfirst597fitblocksize - size_needed_by_blocks597;

	   //we dont have to check if after me is empty or not , because it will never be (if it was it would be already merged before)

	   //block fits and have more
	   if (remaining_size55 >= (DYN_ALLOC_MIN_BLOCK_SIZE + sizeof(uint32) + sizeof(uint32))/*16*/) {
		   //no fragment
		   struct BlockElement *new_block55 = (struct BlockElement *)((char *) lfirst597fitblock + size_needed_by_blocks597);

		   set_block_data(new_block55, remaining_size55, 0);

		   //free_block(new_block);
		   LIST_INSERT_AFTER(&freeBlocksList,lfirst597fitblock , new_block55);
		   LIST_REMOVE(&freeBlocksList, lfirst597fitblock);


		   set_block_data( lfirst597fitblock,size_needed_by_blocks597,1);

	   }else{
		   //fragment
		   set_block_data(lfirst597fitblock, lfirst597fitblocksize, 1);
		   LIST_REMOVE(&freeBlocksList, lfirst597fitblock);
	   }



		//cprintf("header in alloc. Actual addr H:%d H2:%p\n", *((uint32*)mfirst597fitblock-1), (uint32*)(mfirst597fitblock-4));

		return lfirst597fitblock;

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


	if (!is_initialized) {
	        uint32 required_size = size + 2 * sizeof(int) + 2 * sizeof(int);
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

    uint32 size_needed_by_blocks629 = size + 2*sizeof(int);

    //2022170629
    struct BlockElement *m629bestfitblock = NULL;
    uint32 m629bestfitblocksize = UINT_MAX;
    struct BlockElement *now629;
    LIST_FOREACH(now629 ,&freeBlocksList) {
    uint32 m629blocksize = get_block_size(now629);

        if (m629blocksize >= size_needed_by_blocks629 && m629blocksize < m629bestfitblocksize) {
                   m629bestfitblock = now629;
                   m629bestfitblocksize = m629blocksize;
               }
    }

    if (m629bestfitblocksize == UINT_MAX) {

    	//see ROUNDUP -> sbrk-----------------
        m629bestfitblock = (struct BlockElement *)sbrk(ROUNDUP(size_needed_by_blocks629 + m629bestfitblocksize , PAGE_SIZE));
        if (m629bestfitblock == (void *)-1) {
            return NULL;
        }
        set_block_data(m629bestfitblock, size_needed_by_blocks629, 1);
        } else {
        	//block fits and have more
        uint32 remaining_size = m629bestfitblocksize - size_needed_by_blocks629;
        //we dont have to check if after me is empty or not , because it will never be (if it was it would be already merged before)
         //block fits and have more
         if (remaining_size >= (DYN_ALLOC_MIN_BLOCK_SIZE + sizeof(uint32) + sizeof(uint32))/*16*/) {
        //no fragment
         struct BlockElement *new_block629 = (struct BlockElement *)((char *) m629bestfitblock + size_needed_by_blocks629);
         set_block_data(new_block629, remaining_size, 0);
         //free_block(new_block);
        LIST_INSERT_AFTER(&freeBlocksList,m629bestfitblock, new_block629);
        set_block_data( m629bestfitblock,size_needed_by_blocks629,1);
        LIST_REMOVE(&freeBlocksList, m629bestfitblock);
        }else{
          //fragment
        set_block_data(m629bestfitblock, m629bestfitblocksize, 1);
        LIST_REMOVE(&freeBlocksList, m629bestfitblock);
    }
    }

	return m629bestfitblock;
}

//===================================================
// [5] FREE BLOCK WITH COALESCING:
//===================================================
void free_block(void *va)
{

  bool is_in_free_list(void* block){

		struct BlockElement* tmp_blockmlf;

		LIST_FOREACH (tmp_blockmlf, &freeBlocksList){

			if((struct BlockElement*)block == tmp_blockmlf){
				return 1;
			}

		}
		return 0;
	}


	void insert_block_in_order(struct BlockElement* block) {
	    struct BlockElement* currentmlf = freeBlocksList.lh_first;

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

////////////
	if(get_block_size(va)==0){
		 cprintf("The virtual address is already free or it is a BEGIN/END block");
		return;
	}
///////////
    uint32 size_of_prev_block_mlf = get_block_size((uint32 *)(va - sizeof(int)));//by footer, get_block_size will minus another 4byte
	struct BlockElement* brev_block_addr_mlf = (struct BlockElement *)((uint32 *)(va - size_of_prev_block_mlf));//footer of prev block

	uint32 block_size = get_block_size(va);
	uint32 size_needed_by_block = get_block_size(va);
	struct BlockElement *new_blockmlf = (struct BlockElement *)((uint32 *)(va + size_needed_by_block));

	struct BlockElement* begin_of_the_free_block = va;

	//brev is free?
	if (is_free_block(brev_block_addr_mlf)){
		block_size += get_block_size(brev_block_addr_mlf);
		set_block_data(brev_block_addr_mlf, block_size, 0);
		begin_of_the_free_block = brev_block_addr_mlf;

		//if VA already was free and in list , remove it (cause the begin changed)
		if(is_in_free_list(va))LIST_REMOVE(&freeBlocksList, (struct BlockElement*)va);

		//will not insert any thing in list because (brev block) already was

	}else{
		set_block_data(va, block_size, 0);
		if(!is_in_free_list(va)) insert_block_in_order((struct BlockElement*)va);
	}

	//next is free?
	if (is_free_block(new_blockmlf)) {

		block_size += get_block_size(new_blockmlf);
		set_block_data(begin_of_the_free_block, block_size, 0);

		LIST_REMOVE(&freeBlocksList, new_blockmlf);

	}

}



//=========================================
// [6] REALLOCATE BLOCK BY FIRST FIT:
//=========================================
void *realloc_block_FF(void* va, uint32 new_size)
{
	// corner cases

	    if(new_size == 0 && va == NULL){
			return NULL;
		}

	    if(new_size != 0 && va == NULL){
	    	return alloc_block_FF(new_size);
		}

		if(new_size==0 && va != NULL){
				free_block(va);
				return NULL;
		}

		// new size is same as current size
		/*PLEASE NOTICE that new_size does not include meta data (header,footer) , shimaa*/
		uint32 curr_size_metadata=get_block_size(va);

		if(new_size == curr_size_metadata-8){
			return va;
		}

		// update newsize

		if(new_size < 8) new_size=8; // min 16
		else if(new_size&1) new_size++;  //odd case X

		/////////////////////////
		void* leave(void* va,uint32 size,uint32 new_size)
		{
			void* new_va = (void*)alloc_block_FF(new_size);

			if(new_va !=NULL){
			memcpy(new_va, va, size);  //without meta data
			cprintf("va in realloc after %p",va);

			set_block_data(va, size+8, 0); // should remove
			free_block(va);
		    cprintf("here");
			}
			return new_va;

		}
		/////////////////////////

		void* next_block_addr = va+curr_size_metadata;
		uint32 next_size_metadata=get_block_size(next_block_addr);

		// increasing size

		if(new_size > curr_size_metadata-8)
		{
			if(next_size_metadata == 0 || !is_free_block(next_block_addr)){
				return leave(va,curr_size_metadata-8,new_size);
			}

			uint32 extra_size=new_size-curr_size_metadata-8;

			// not extend

			if(extra_size > next_size_metadata)
			{
				return leave(va,curr_size_metadata-8,new_size);
			}
			// extend
			else
			{
					uint32 remains = curr_size_metadata + next_size_metadata - (new_size+8);
					if(remains >= 16) {
						set_block_data(va, new_size+8, 1);
						set_block_data(va+new_size+8, remains, 0);
						free_block(va+new_size+8);
				    }
					else {
						set_block_data(va, curr_size_metadata + next_size_metadata,1);
					}

					LIST_REMOVE(&freeBlocksList,(struct BlockElement*)next_block_addr);

				  return va;
			}

		}
		// decreasing size
		else
		{
			uint32 free_size=curr_size_metadata - (new_size+8);

			if(!is_free_block(next_block_addr)){

			 if(free_size >= 16) { // free block
			 	set_block_data(va, new_size+8, 1);
				set_block_data(va+new_size+8, free_size, 0);
				free_block(va+new_size+8);
		     }
			}
			else{
			 	set_block_data(va, new_size+8, 1);
				set_block_data(va+new_size+8, free_size, 0);
				free_block(va+new_size+8);
			}

			return va;
		}


	// /*PLEASE NOTICE that new_size does not include meta data (header,footer) , shimaa*/
	// 	//[2] Test realloc by passing size = 0. It should call free //return null
	// 	//test calling it with va & ZERO

	// 	if(new_size == 0 && va != NULL){
	// 		free_block(va);
	// 		return NULL;
	// 	}
	// 	//test calling it with NULL & ZERO
	// 	if(new_size == 0 && va == NULL){
	// 		return NULL;
	// 	}

	// 	//[1] Test calling realloc with VA = NULL. It should call alloc
	// 	/* Try to allocate set of blocks with different sizes*/
	// 	/* Try to allocate a block with a size equal to the size of the first existing free block*/

	// 	if(va == NULL){
	// 		return alloc_block_FF(new_size);
	// 	}

	// 	bool IS_FIT = 1;
	// 	uint32 block_size = get_block_size(va);
	// 	uint32 all_new_size = ROUNDUP(new_size + 8, 2);
	// 	if (all_new_size % 2 != 0) all_new_size++; //ensure it's multiple of 2

	// 	struct BlockElement* next_block_addr = (struct BlockElement*)(va + (block_size - 4) + 4);//the addr of the next block , are you sure about datatypes?
	// 	struct BlockElement* new_next_block_addr = (struct BlockElement*)(va + (all_new_size-4) + 4);//the addr of the next block after reallocation ,after header, are you sure about datatypes?

	// 	//[]Test if that block is the last block in heap ..shimaa -> which is [5] in test
	// 	if(get_block_size(next_block_addr) == 0 && !is_free_block(next_block_addr)){
	// 		return alloc_block_FF(new_size);
	// 		//sbrk(the_extra_size/(4*1024));
	// 		//return NULL;
	// 	}


	// 	if(all_new_size > block_size) {
	// 		//[3] Test realloc with increased sizes
	// 		uint32 the_extra_size = all_new_size - block_size;//the difference between new size and old size


	// 		uint32 next_block_size = get_block_size(next_block_addr);



	// 		if(is_free_block(next_block_addr)) {

	// 			if(the_extra_size < next_block_size) {
	// 				//[3.1] reallocate in same place (NO relocate - split)(shimaa:will take part of next block)
	// 				//check return address
	// 				//check the new address of the next free block,shimaa
	// 				uint32 the_remaining_size = next_block_size-the_extra_size;//the remaining_size in free block after taking from it to our block

	// 				if(the_remaining_size >= 16) {

	// 					set_block_data(va, all_new_size, 1);
	// 					set_block_data(new_next_block_addr, the_remaining_size, 0);


	// 					free_block(new_next_block_addr);
	// 					//LIST_INSERT_AFTER(&freeBlocksList, next_block_addr,new_next_block_addr);
	// 				}
	// 				else {
	// 					//fragment
	// 					//check by nouran
	// 					set_block_data(va, block_size+next_block_size,1);//will exist internal fragment (free unused mem in block)
	// 				}

	// 				LIST_REMOVE(&freeBlocksList, next_block_addr);
	// 				return va;

	// 			}else if(the_extra_size == next_block_size){
	// 				//[3.2] reallocate in same place (NO relocate - NO split) (shimaa:will take all the next block)
	// 				set_block_data(va, all_new_size,1);//will exist internal fragment (free unused mem in block)
	// 				LIST_REMOVE(&freeBlocksList, next_block_addr);
	// 				return va;

	// 			}else {
	// 				//reallocate in another place
	// 				//IS_FIT = 0;
	// 				//ask raazan how to realloc with data
	// 				//ask razan will va be void or int or what?
	// 				uint32* new_va = (uint32*)alloc_block_FF(new_size);
	// 				uint32 tmp_block_size = block_size - 8;//check
	// 				uint32* tmp_new_va = new_va;
	// 				while (tmp_block_size--) {
	// 					*tmp_new_va++ = *((uint32*)va++);
	// 				}
	// 				cprintf("here");
	// 				return new_va;
	// 			}
	// 		}else {
	// 			uint32* new_va = alloc_block_FF(new_size);
	// 			uint32 tmp_block_size = block_size - 8;//check
	// 			uint32* tmp_new_va = new_va;
	// 			while (tmp_block_size--) {
	// 				*tmp_new_va++ = *((uint32*)va++);
	// 			}
	// 			return new_va;
	// 		}
	// 		/*dont forget in 3.3,3.4 to move with the same content */
	// 		//[3.3] reallocate in another place (relocate - NO split)shimaa
	// 		//will change location and take full block
	// 		//if the size is more than the next free block
	// 		/*block after ours is free but not enough*/
	// 		/*block after ours is not free*/
	// 		//check if the new block we will go to will let remaining size smaller than 16?
	// 		//check if the new block we will go to will let remaining size greater than 16
	// 		//[3.4] reallocate in another place (relocate - split)shimaa
	// 		//will change location and take half block
	// 		//[3.5] no enough space (NO relocate - NO split)-> search till the end but no space -> return null ,shimaa


	// 	}else if (all_new_size < block_size){
	// 		//shrinking the block and update the free block list
	// 		//if (all_new_size % 2 != 0) all_new_size++; //ensure it's multiple of 2 ,,already in begin of the code
	// 		uint32 remainig_size = block_size-all_new_size;

	// 		//[4] Test realloc with decreased sizes
	// 		//[4.0] new size<16
	// 		//[4.1] next block is full (NO coalesce)
	// 		 //1.divide the block to [1.full block] [2. free block if it is more than 16]
	// 		 //2.[Internal Framgmentation]
	// 		//[4.2] next block is empty (coalesce)shimaa
	// 			/*divide the block to [1.full block] [2. free block will join to the next free block ,
	// 			 *  dont forget to update the addr of the beginning of the free block in free block list]*/

	// 		if(all_new_size < 16)
	// 		{
	// 			new_next_block_addr = va + 16;
	// 			remainig_size = block_size -16;
	// 			all_new_size=16;

	// 		}
	// 		if(remainig_size>=16)
	// 		{
	// 			//merge
	// 			set_block_data(va, all_new_size , 1);
	// 			set_block_data(new_next_block_addr,remainig_size,0); //check free implementaion

	// 			free_block(new_next_block_addr);

	// 			//call free or continue coding
	// 		}else{
	// 			if(is_free_block(next_block_addr)){
	// 				//merge with free
	// 				set_block_data(va, all_new_size , 1);
	// 				set_block_data(new_next_block_addr,remainig_size+get_block_size(next_block_addr),0);
	// 				LIST_INSERT_AFTER(&freeBlocksList,next_block_addr , new_next_block_addr);
	// 				LIST_REMOVE(&freeBlocksList, next_block_addr);

	// 			}
	// 		}
	// 		return va;



	// 	}else {
	// 		//[6] realloc with the same size
	// 		return va;
	// 	}

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

//=========================================
// [9]Make some delicious Potato:
//=========================================
void *delicious_Potato (uint32 potato)
{
	uint32 salad;
	uint32 tomato = 0;
	salad = tomato + potato;
	return (void*) salad;
}


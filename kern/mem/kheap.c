#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

typedef LIST_ENTRY(PageInfo) Free_page_LIST_entry_t;
struct PageInfo {

	uint32 start_page_va;    // data type? (store address)
	uint32 end_page_va;      // inclusive AKA free page

	bool is_first_addr;      // is first address in all allocated blocks?
	bool is_last_addr;
    bool is_free;

    uint32 number_of_pages;
	Free_page_LIST_entry_t prev_next_info;
};

//const uint32 page_allocator_pages =(KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE;   //(KERNEL_HEAP_MAX-((uint32)hardlimit + PAGE_SIZE))/PAGE_SIZE;
struct PageInfo pages_arr[(KERNEL_HEAP_MAX-KERNEL_HEAP_START)/PAGE_SIZE];

struct sleeplock k_sleeplock;

inline void acquireSleep()
{
	//if(!holding_sleeplock(&k_sleeplock))
		acquire_sleeplock(&k_sleeplock);
}

inline void releaseSleep()
{
	//if(holding_sleeplock(&k_sleeplock))
		release_sleeplock(&k_sleeplock);
}

//[PROJECT'24.MS2] Initialize the dynamic allocator of kernel heap with the given start address, size & limit
//All pages in the given range should be allocated
//Remember: call the initialize_dynamic_allocator(..) to complete the initialization
//Return:
//	On success: 0
//	Otherwise (if no memory OR initial size exceed the given limit): E_NO_MEM
int initialize_kheap_dynamic_allocator(uint32 daStart, uint32 initSizeToAllocate, uint32 daLimit)
{
    //TODO: [PROJECT'24.MS2 - #01] [1] KERNEL HEAP - initialize_kheap_dynamic_allocator

     start=(uint32*)daStart;
     hardlimit=(uint32*)daLimit;
     segment_break = (uint32*)((char*)daStart + initSizeToAllocate);

         if (start > hardlimit || segment_break> hardlimit)
         {
          	panic("exceed the limit...!!");
         }

         uint32* tmp_start=start;
    	 while(tmp_start<segment_break)
    	 {
         	allocate_page_to_frame(tmp_start);
            tmp_start = (uint32*)((char*)tmp_start+PAGE_SIZE);
         }

    	 initialize_dynamic_allocator(daStart,initSizeToAllocate);
    	 init_free_list();
    	 init_sleeplock(&k_sleeplock,"kernel sleep lock");

       return 0;
}

void* sbrk(int numOfPages)
{
	/* numOfPages > 0: move the segment break of the kernel to increase the size of its heap by the given numOfPages,
	 * 				you should allocate pages and map them into the kernel virtual address space,
	 * 				and returns the address of the previous break (i.e. the beginning of newly mapped memory).
	 * numOfPages = 0: just return the current position of the segment break
	 *
	 * NOTES:
	 * 	1) Allocating additional pages for a kernel dynamic allocator will fail if the free frames are exhausted

	 * 		or the break exceed the limit of the dynamic allocator. If sbrk fails, kernel should panic(...)
	 */

	//MS2: COMMENT THIS LINE BEFORE START CODING====
	//return (void*)-1 ;
	//====================================================
	//cprintf("\n before sbrk");

    if (numOfPages == 0)
    {
        return (void*)segment_break;
    }

    uint32* begin_alloc = segment_break, *prev_break=segment_break;

    uint32 increment = numOfPages * PAGE_SIZE;
    uint32* new_brk = (uint32*)((char*)segment_break + increment);

    	if(new_brk>hardlimit)  // =?
    	{
    		return (void *)-1;
    	}

        while(numOfPages--)
        {
            allocate_page_to_frame(begin_alloc);
            begin_alloc=(uint32*)((char*)begin_alloc+PAGE_SIZE);
        }

        segment_break=new_brk;

		//cprintf("TOTAL sbrk = 1\n");

        return (void*) prev_break;
}

void* kmalloc(unsigned int size)
{
	//[PROJECT'24.MS2] Implement this function
	//Write your code here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");
	acquireSleep();
	// Block Allocator
	if(size <= DYN_ALLOC_MAX_BLOCK_SIZE)
	{
		void* addr = alloc_block_FF(size);
		releaseSleep();

		return addr;
	}
	// Page Allocator
	if(isKHeapPlacementStrategyFIRSTFIT() != 1)
	{
		releaseSleep(&k_sleeplock);
		return NULL;
	}
	uint32 num_of_pages = ROUNDUP(size , PAGE_SIZE)/ PAGE_SIZE; //check calculation
	uint32 page_allocator_pages = (KERNEL_HEAP_MAX-((uint32)hardlimit + PAGE_SIZE))/PAGE_SIZE;

	if(num_of_pages > page_allocator_pages)
	{
		releaseSleep(&k_sleeplock);
		return NULL;
	}

//	struct freeFramesCounters counters9 = calculate_available_frames();
//	int total1 = counters9.freeBuffered + counters9.freeNotBuffered;
	//cprintf("in page = %d \n", counters9.freeBuffered + counters9.freeNotBuffered);

	bool page_found = 0;
	uint32 return_addr;

	for(int i = 0 ; i < page_allocator_pages-1 ; i++){

		if(pages_arr[i].is_free && pages_arr[i].number_of_pages >= num_of_pages){

			if(pages_arr[i].number_of_pages > num_of_pages){
				//split
				uint32 splited_free_page_index = (i + num_of_pages);

				//(like header)
				pages_arr[splited_free_page_index].number_of_pages = pages_arr[i].number_of_pages -num_of_pages;
				pages_arr[splited_free_page_index].is_free = 1;
				pages_arr[splited_free_page_index].start_page_va = ((uint32)hardlimit + PAGE_SIZE) + (splited_free_page_index*PAGE_SIZE);
				pages_arr[splited_free_page_index].is_first_addr = 1;

				//(like footer)
				pages_arr[splited_free_page_index+pages_arr[splited_free_page_index].number_of_pages-1].number_of_pages = pages_arr[i].number_of_pages -num_of_pages;
				pages_arr[splited_free_page_index+pages_arr[splited_free_page_index].number_of_pages-1].is_free = 1;

			}


			//like header of the allocated block
			pages_arr[i].is_free = 0;
			pages_arr[i].number_of_pages = num_of_pages;
			pages_arr[i].start_page_va = ((uint32)hardlimit + PAGE_SIZE) + (i*PAGE_SIZE);
			return_addr = pages_arr[i].start_page_va;
			pages_arr[i].is_first_addr = 1;

			//like footer
			pages_arr[i+num_of_pages-1].is_last_addr =1;
			pages_arr[i+num_of_pages-1].is_free = 0;
			pages_arr[i+num_of_pages-1].number_of_pages=num_of_pages;


			page_found = 1;

			//mapping
			uint32 tmp_num_of_pages=num_of_pages;
			uint32 start_page_va = pages_arr[i].start_page_va;
			while(tmp_num_of_pages--)
			{
				if(allocate_page_to_frame((uint32*)(start_page_va)) != 0)
				{
					releaseSleep(&k_sleeplock);
					return NULL;
				}
				start_page_va = (start_page_va +PAGE_SIZE);
			}

			break;

		}else{

			//jump
			i = (i + pages_arr[i].number_of_pages) -1;//for loop will increment it again and remove (-1)

		}
	}


//	struct freeFramesCounters counters10 = calculate_available_frames();
//		int total2 = counters10.freeBuffered + counters10.freeNotBuffered;
//		cprintf("TOTAL in page  = %d \n", total1 - total2);

	if(!page_found)
	{
		releaseSleep(&k_sleeplock);
		return NULL; // couldn't allocate
	}
	releaseSleep(&k_sleeplock);
	return (uint32*)return_addr;
}

void kfree(void* virtual_address)
{
	//[PROJECT'24.MS2] Implement this function
	//Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");
	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

	acquireSleep();

	//block allocator
	if((uint32)virtual_address >= KERNEL_HEAP_START && (uint32*)virtual_address <= hardlimit)
	{
		free_block(virtual_address);
	}
	//page allocator
	else if((uint32*)virtual_address >= (uint32*)((char*)hardlimit+PAGE_SIZE) && (uint32)virtual_address <= KERNEL_HEAP_MAX)
	{
		uint32* va = ROUNDDOWN(virtual_address,PAGE_SIZE);
		uint32 pageIndex = ((uint32)va - ((uint32)hardlimit+PAGE_SIZE))/PAGE_SIZE;
		uint32 num_of_pages = pages_arr[pageIndex].number_of_pages;

		if(pages_arr[pageIndex].is_free || !pages_arr[pageIndex].is_first_addr)
		{
			//cprintf("already free Virtual address or not first address in allocated pages...");
			releaseSleep();
			return;
		}

		//header of this block of pages
		pages_arr[pageIndex].is_free = 1;

		//footer of this block of pages
		pages_arr[pageIndex + num_of_pages -1].is_free = 1;
		pages_arr[pageIndex + num_of_pages -1].number_of_pages = num_of_pages;
		pages_arr[pageIndex + num_of_pages -1].is_first_addr = 0;

		//un_map the pages
		uint32 tmp_num_of_pages = num_of_pages;
		while(tmp_num_of_pages--)
		{
			deallocate_page_to_frame(va);
			va = (uint32*)((char*)va+ PAGE_SIZE);
		}

		//before my block is empty
	if(pageIndex != 0)
		if(pages_arr[pageIndex-1].is_free)
		{
			//merge

			//update pageindex to the begin
			pages_arr[pageIndex].is_first_addr = 0;

			uint32 num_of_old_pages = num_of_pages;
			 num_of_pages = pages_arr[pageIndex-1].number_of_pages;

			 //page index know is the index of the previous page
			 //absolute -> i dont know why function didn't make it right , i am tired
			int tmppageIndex = (int)(pageIndex - pages_arr[pageIndex-1].number_of_pages);
			pageIndex = tmppageIndex >= 0 ? tmppageIndex : -tmppageIndex;

			//update data of first page of prev block (header)
			pages_arr[pageIndex].number_of_pages = num_of_old_pages+num_of_pages;

			//updating last page in all block (prev + curr)
			pages_arr[pageIndex+num_of_old_pages+num_of_pages-1].number_of_pages = num_of_old_pages+num_of_pages;
			pages_arr[pageIndex+num_of_old_pages+num_of_pages-1].is_free = 1;

			num_of_pages = pages_arr[pageIndex].number_of_pages;

		}

		//after my block of pages is empty
	if(pageIndex < KERNEL_HEAP_MAX - PAGE_SIZE)
		if(pages_arr[pageIndex+num_of_pages].is_free){
			//merge
			pages_arr[pageIndex+num_of_pages].is_first_addr = 0;

			uint32 added_size = pages_arr[pageIndex+num_of_pages].number_of_pages;
			pages_arr[pageIndex].number_of_pages += added_size;

			//update last page in the block
			pages_arr[pageIndex+pages_arr[pageIndex].number_of_pages-1].number_of_pages = pages_arr[pageIndex].number_of_pages;
			pages_arr[pageIndex+pages_arr[pageIndex].number_of_pages-1].is_free = 1;

		}
	}
	else
	{
		panic("Invalid Virtual address...");
	}

	releaseSleep();
}

unsigned int kheap_virtual_address(unsigned int physical_address){

	acquireSleep();

	struct FrameInfo* frame=to_frame_info(physical_address);

    if(frame == NULL || frame->references == 0)
    {
    	releaseSleep();
    	return 0;
    }

    uint32 offset = physical_address & 0xFFF;
    uint32 va = (frame->page_num<<12) | offset;

	releaseSleep();
    return va;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	acquireSleep();

	uint32* ptr_page_table;
	struct FrameInfo* frame = get_frame_info(ptr_page_directory,virtual_address,&ptr_page_table);

	if(frame == NULL)
	{
		releaseSleep();
	    return 0;
	}
	uint32 offset = virtual_address & 0xFFF;
	uint32 pa = (to_frame_number(frame)<<12) | offset;

	releaseSleep();
	return pa;
}


//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//[PROJECT'24.MS2 BONUS2] Kernel Heap Realloc
	// Write your code here, remove the panic and write your code
	//return NULL;
	//panic("krealloc() is not implemented yet...!!");

	if(new_size == 0 && virtual_address == NULL)
	{
		return NULL;
	}

	if(virtual_address == NULL)
	{
		return kmalloc(new_size);
	}

	if(new_size == 0)
	{
		kfree(virtual_address);
		return NULL;
	}


	if((uint32)virtual_address >= KERNEL_HEAP_START && (uint32*)virtual_address <= hardlimit)
	{
		void* addr;
		//in block allocator
		if(new_size <= DYN_ALLOC_MAX_BLOCK_SIZE)
		{
			acquireSleep();
			addr = realloc_block_FF(virtual_address,new_size);
			releaseSleep();

			if(addr == virtual_address)
				addr = NULL;

			return addr;

		}else{

			addr = block_to_page_allocator(virtual_address,get_block_size(virtual_address)-8,new_size);// old size is not valid
			return addr;
		}
	}

	if((uint32*)virtual_address >= (uint32*)((char*)hardlimit+PAGE_SIZE) && (uint32)virtual_address <= KERNEL_HEAP_MAX)
	{
		//in page allocator

		acquireSleep();

		virtual_address = ROUNDDOWN(virtual_address,PAGE_SIZE);// double check
		uint32 pageIndex = ((uint32)virtual_address - ((uint32)hardlimit+PAGE_SIZE))/PAGE_SIZE;
		uint32 num_of_pages = pages_arr[pageIndex].number_of_pages;
		uint32 old_size = num_of_pages * PAGE_SIZE;
		void* addr;

		if(new_size <= DYN_ALLOC_MAX_BLOCK_SIZE)
		{
			addr = page_to_block_allocator(virtual_address,old_size,new_size);
			if(addr == NULL) releaseSleep();
			return addr;

		}else{

			new_size = ROUNDUP(new_size,PAGE_SIZE);// double check

			// equal case
			if(old_size == new_size)
			{
				releaseSleep();
				return virtual_address;
			}

			// increasing case
			if(old_size < new_size)
			{
				uint32 extraPagesReq=(new_size-old_size)/PAGE_SIZE;

				// not extend  [ not free OR not enough free space infront of me]
				if(!pages_arr[pageIndex + num_of_pages].is_free
						|| pages_arr[pageIndex + num_of_pages].number_of_pages < extraPagesReq)
				{
					releaseSleep();
					addr = kleave(virtual_address,old_size,new_size);
					return addr;
				}
				// extend
				else
				{
					uint32* nxt_freeBlock_va=(uint32*)pages_arr[pageIndex + num_of_pages].start_page_va;
					uint32 nxt_freeBlock_pagesNum = pages_arr[pageIndex + num_of_pages].number_of_pages;
					uint32 remaining_free_pages = nxt_freeBlock_pagesNum - extraPagesReq;

					//Split data
					uint32* va = ROUNDDOWN(virtual_address,PAGE_SIZE);

					uint32* Split_va=(uint32*)((char*)va+new_size);
					uint32 Split_pageIndex =  ((uint32)Split_va - ((uint32)hardlimit+PAGE_SIZE))/PAGE_SIZE;

					//set new header for the current block
					pages_arr[pageIndex].number_of_pages = num_of_pages+extraPagesReq;

					// set new footer for the current block
					pages_arr[Split_pageIndex-1].is_last_addr =1;
					pages_arr[Split_pageIndex-1].is_free = 0;
					pages_arr[Split_pageIndex-1].number_of_pages=num_of_pages+extraPagesReq;

					//set new header for the next block
					pages_arr[Split_pageIndex].number_of_pages = remaining_free_pages;
					pages_arr[Split_pageIndex].is_first_addr = 1;
					pages_arr[Split_pageIndex].is_free = 1;
					pages_arr[Split_pageIndex].start_page_va = (uint32)nxt_freeBlock_va + (extraPagesReq*PAGE_SIZE);

					// set new footer for the next block
					pages_arr[Split_pageIndex+remaining_free_pages-1].is_last_addr =1;
					pages_arr[Split_pageIndex+remaining_free_pages-1].is_free = 1;
					pages_arr[Split_pageIndex+remaining_free_pages-1].number_of_pages=remaining_free_pages;
					pages_arr[Split_pageIndex+remaining_free_pages-1].start_page_va = (uint32)nxt_freeBlock_va + (extraPagesReq*PAGE_SIZE);

					//mapping
					uint32* old_free_va=(uint32*)((char*)va+old_size);
					uint32 old_free_pageIndex =  ((uint32)old_free_va - ((uint32)hardlimit+PAGE_SIZE))/PAGE_SIZE;

					uint32 tmp_num_of_pages=extraPagesReq;
					uint32 start_page_va = pages_arr[old_free_pageIndex].start_page_va;
					while(tmp_num_of_pages--)
					{
						if(allocate_page_to_frame((uint32*)(start_page_va)) != 0)
						{
							releaseSleep();
							return NULL;
						}
						start_page_va = (start_page_va +PAGE_SIZE);
					}

					releaseSleep();
					return virtual_address;
				}

			}

			// decreasing case
			if(old_size > new_size)
			{

				uint32 new_pages = (new_size)/PAGE_SIZE;

				//split data
				uint32 Split_pageIndex = pageIndex + new_pages;

				uint32 remaining_free_pages = num_of_pages-new_pages;

				//set new header for the current block
				pages_arr[pageIndex].number_of_pages = new_pages;

				// set new footer for the current block
				pages_arr[Split_pageIndex-1].is_last_addr =1;
				pages_arr[Split_pageIndex-1].is_free = 0;
				pages_arr[Split_pageIndex-1].number_of_pages=new_pages;

				//set new header for the next block
				pages_arr[Split_pageIndex].number_of_pages = remaining_free_pages;
				pages_arr[Split_pageIndex].is_first_addr = 1;
				pages_arr[Split_pageIndex].is_free = 0;//make it not free because we will call kfree
				pages_arr[Split_pageIndex].start_page_va = pages_arr[pageIndex].start_page_va + new_size;


				// set new footer for the next block
				pages_arr[Split_pageIndex+remaining_free_pages-1].is_last_addr =1;
				pages_arr[Split_pageIndex+remaining_free_pages-1].is_free = 0;//make it not free because we will call kfree
				pages_arr[Split_pageIndex+remaining_free_pages-1].number_of_pages=remaining_free_pages;
				pages_arr[Split_pageIndex+remaining_free_pages-1].start_page_va = pages_arr[pageIndex].start_page_va + new_size;

				// let kfree do the merging
				releaseSleep();
				kfree((uint32*)pages_arr[Split_pageIndex].start_page_va);
				return virtual_address;
			}

		}
	}

	return NULL;
}


//=================================================================================//
//============================== OUR HELPER FUNCTIONS =============================//
//=================================================================================//
void* kleave(void* va,uint32 size,uint32 new_size) // sizes in bytes
{
	// use in increase size only
	void* new_va = (void*)kmalloc(new_size);

	if(new_va !=NULL){
		memcpy(new_va, va, size);
		kfree(va);
	}
	return new_va;

}
void* page_to_block_allocator(void* va,uint32 size,uint32 new_size)
{
	void* new_va = (void*)alloc_block_FF(new_size);

	if(new_va !=NULL){
		memcpy(new_va, va, new_size); //new size because old size will be bigger than the block we allocated;
		releaseSleep();
		kfree(va);
	}
	return new_va;
}

void* block_to_page_allocator(void* va,uint32 size,uint32 new_size)
{
	// size without meta data

	void* new_va = (void*)kmalloc(new_size);

	if(new_va !=NULL){

		memcpy(new_va, va, size); //size because old newsize will be bigger than the block we allocated;
		acquireSleep();
		free_block(va);
		releaseSleep();
	}

	return new_va;
}

int allocate_page_to_frame(void * page_VA){

	//Allocate new frame
	struct FrameInfo* ptr_frame_info ;
	allocate_frame(&ptr_frame_info);  //it panics if there is no memory

	//Map the given va to the allocated frame
	int ret = map_frame(ptr_page_directory, ptr_frame_info, (uint32)page_VA, PERM_WRITEABLE);
	ptr_frame_info->page_num = (uint32)page_VA>>12;

	if (ret != 0)
	{
		free_frame(ptr_frame_info);
		return -1;
	}

	return 0;
}

void deallocate_page_to_frame(void * page_VA){

	uint32 *ptr_page_table;
	struct FrameInfo* ptr_frame_info=get_frame_info(ptr_page_directory,(uint32)page_VA,&ptr_page_table);
	unmap_frame(ptr_page_directory, (uint32) page_VA);
}


void init_free_list()
{
		struct PageInfo p_alloc;
		p_alloc.is_first_addr =1;
		p_alloc.start_page_va =(uint32)((char*)hardlimit + PAGE_SIZE);
		p_alloc.end_page_va = KERNEL_HEAP_MAX - PAGE_SIZE;
		p_alloc.number_of_pages =(KERNEL_HEAP_MAX-(p_alloc.start_page_va)) / PAGE_SIZE;
		p_alloc.is_free =1;

		pages_arr[0]=p_alloc;

	    /*struct PageInfo* p_alloc = (struct PageInfo*)(ptr_page_directory);
	    //round down in any of these?
	    p_alloc -> start_page_va = (uint32)((char*)hardlimit + PAGE_SIZE);
	    p_alloc -> end_page_va = KERNEL_HEAP_MAX - PAGE_SIZE;
	    p_alloc -> number_of_pages = (KERNEL_HEAP_MAX-(p_alloc->start_page_va)) / PAGE_SIZE;

		LIST_INSERT_HEAD(&free_page_list,p_alloc);*/
}

///// not used
//int numOfAllocPages_busyList(void* va)
//{
//	    struct PageInfo* busy_Page;
//		LIST_FOREACH(busy_Page, &(busy_page_list))
//		{
//			if((busy_Page->start_page_va) == (uint32)va)
//			{
//				return busy_Page->number_of_pages;
//			}
//		}
//		return 0;
//}
//void merge_freeList(void* va, int num_of_pages)
//{
//
//	struct PageInfo* new_free_Page = (struct PageInfo*)va;
//	new_free_Page ->start_page_va = (uint32)va;
//	new_free_Page-> number_of_pages = num_of_pages;
//	new_free_Page -> end_page_va = (uint32)((char*)va + (num_of_pages*PAGE_SIZE) -  PAGE_SIZE); //beginning of last page
//
//
//	// get previous & next free block
//
//	struct PageInfo* free_Page, *prev_free_block=NULL, *next_free_block=NULL;
//	LIST_FOREACH(free_Page, &(free_page_list))
//	{
//		if(free_Page->start_page_va > new_free_Page->start_page_va)
//		{
//			next_free_block=free_Page;
//			prev_free_block=LIST_PREV(next_free_block);
//			break;
//		}
//	}
//
//	if(next_free_block==NULL) prev_free_block=LIST_LAST(&free_page_list);
//
//
//	// merge
//
//	int merge_prev(struct PageInfo* prev_free_block,struct PageInfo* new_free_Page)
//	{
//		if(prev_free_block!=NULL && ((prev_free_block->end_page_va + PAGE_SIZE) == new_free_Page->start_page_va))
//		{
//				prev_free_block -> number_of_pages += new_free_Page ->number_of_pages;
//				prev_free_block -> end_page_va = new_free_Page ->end_page_va;
//				new_free_Page=prev_free_block;
//
//				if(next_free_block!=NULL && ((next_free_block->start_page_va - PAGE_SIZE) == new_free_Page->end_page_va))
//				{
//					next_free_block -> start_page_va = new_free_Page ->start_page_va;
//					next_free_block -> number_of_pages += new_free_Page ->number_of_pages;
//
//					LIST_REMOVE(&free_page_list,new_free_Page);
//					return 1;
//				}
//
//				return 2;
//		}
//
//		return 0; // no merging or prev = NULL
//	}
//
//	int merge_next(struct PageInfo* next_free_block,struct PageInfo* new_free_Page)
//	{
//		if(next_free_block!=NULL && ((next_free_block->start_page_va - PAGE_SIZE) == new_free_Page->end_page_va))
//		{
//			next_free_block -> start_page_va = new_free_Page ->start_page_va;
//			next_free_block -> number_of_pages += new_free_Page ->number_of_pages;
//			return 1;
//		}
//		return 0; // no merging or next = NULL
//	}
//
//
//	if(merge_prev(prev_free_block,new_free_Page)==0 && merge_next(next_free_block,new_free_Page)==0) // no merging
//	{
//		if(prev_free_block==NULL)LIST_INSERT_HEAD(&free_page_list,new_free_Page);
//		else LIST_INSERT_AFTER(&free_page_list,new_free_Page,prev_free_block);
//	}
//
//}
//int my_abs(int x)
//{
//    return x >= 0 ? x : -x;
//}

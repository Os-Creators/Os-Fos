#include "kheap.h"

#include <inc/memlayout.h>
#include <inc/dynamic_allocator.h>
#include "memory_manager.h"

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

            if (start > hardlimit){
            	//return E_NO_MEM ;
            	panic("exced the limit...!!");
            }
            if(segment_break> hardlimit){
               // return E_NO_MEM;
            	panic("exced the limit...!!");
            }


         uint32* tmp_start=start;
    	 while(tmp_start<segment_break)
    	 {
    	     struct FrameInfo *ptr_frame_info;
             allocate_frame(&ptr_frame_info);

             int ret=map_frame(ptr_page_directory,ptr_frame_info,(uint32)tmp_start,PERM_WRITEABLE);
         	if (ret != 0)
         	{
               free_frame(ptr_frame_info) ;
               return E_NO_MEM;
            }

            tmp_start = (uint32*)((char*)tmp_start+PAGE_SIZE);
         }

    	 initialize_dynamic_allocator(daStart,initSizeToAllocate);
    	 init_free_list();

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
            struct FrameInfo *ptr_frame_info;
            allocate_frame(&ptr_frame_info);
            int retur=map_frame(ptr_page_directory,ptr_frame_info,(uint32)begin_alloc,PERM_WRITEABLE);
            if(retur!=0)
            {
                 free_frame(ptr_frame_info);
                 return (void *)-1;  // no memory
            }
            begin_alloc=(uint32*)((char*)begin_alloc+PAGE_SIZE);
        }

        segment_break=new_brk;

        return (void*) prev_break;
}

void* kmalloc(unsigned int size)
{
	//[PROJECT'24.MS2] Implement this function
	//Write your code here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");
	// Block Allocator
	if(size <= DYN_ALLOC_MAX_BLOCK_SIZE)
		return alloc_block_FF(size);

	// Page Allocator
	if(isKHeapPlacementStrategyFIRSTFIT() != 1)
	{
		return NULL;
	}
	uint32 num_of_pages = ROUNDUP(size , PAGE_SIZE)/ PAGE_SIZE; //check calculation
	//cprintf("free list size = %d , first element start address %p , first element num pages: %d , requested num of pages %d\n",LIST_SIZE(&free_page_list),LIST_FIRST(&free_page_list)->start_page_va,LIST_FIRST(&free_page_list)->number_of_pages,num_of_pages);

	bool page_found = 0;
	uint32 page_allocator_pages = (KERNEL_HEAP_MAX-((uint32)hardlimit + PAGE_SIZE))/PAGE_SIZE;

	if(num_of_pages > page_allocator_pages)return NULL;

	uint32 return_addr;
	for(int i =0 ; i < page_allocator_pages ; i++){
		if(pages_arr[i].is_free && pages_arr[i].number_of_pages >= num_of_pages){
			//cprintf("i = %d , va %p \n",i,(uint32*)pages_arr[i].start_page_va);

			if(pages_arr[i].number_of_pages > num_of_pages){

				uint32 splited_free_page_index = (i + num_of_pages);

				pages_arr[splited_free_page_index].number_of_pages = pages_arr[i].number_of_pages -num_of_pages;
				pages_arr[splited_free_page_index].is_free = 1;
				pages_arr[splited_free_page_index].start_page_va = ((uint32)hardlimit + PAGE_SIZE) + (splited_free_page_index*PAGE_SIZE);
				pages_arr[splited_free_page_index].is_first_addr = 1;


				pages_arr[splited_free_page_index+pages_arr[splited_free_page_index].number_of_pages-1].number_of_pages = pages_arr[i].number_of_pages -num_of_pages;
				pages_arr[splited_free_page_index+pages_arr[splited_free_page_index].number_of_pages-1].is_free = 1;

			}


			pages_arr[i].is_free = 0;
			pages_arr[i].number_of_pages = num_of_pages;
			pages_arr[i].start_page_va = ((uint32)hardlimit + PAGE_SIZE) + (i*PAGE_SIZE);
			return_addr = pages_arr[i].start_page_va;
			pages_arr[i].is_first_addr = 1;


			pages_arr[i+num_of_pages-1].is_last_addr =1;
			pages_arr[i+num_of_pages-1].is_free = 0;
			pages_arr[i+num_of_pages-1].number_of_pages=num_of_pages;


			page_found = 1;

			//mapping
			uint32 tmp_num_of_pages=num_of_pages;
			uint32 start_page_va = pages_arr[i].start_page_va;
			while(tmp_num_of_pages--)
			{
				if(allocate_page_to_frame((uint32*)(start_page_va)) != 0) return NULL;
				start_page_va = (start_page_va +PAGE_SIZE);
			}

			break;

		}else{
			i = (i + pages_arr[i].number_of_pages) -1;//for loop will increment it again and remove (-1)
		}
	}

	if(!page_found) return NULL; // couldn't allocate

	return (uint32*)return_addr;

	/*// Search for FF in free_list & Allocate
    struct PageInfo* page_VA, *st_page_VA=NULL;
    uint32 start_page_va;

		LIST_FOREACH(page_VA, &(free_page_list))
		{
			if(page_VA->number_of_pages >= num_of_pages)
			{
				st_page_VA=page_VA;
				start_page_va= page_VA -> start_page_va;
				cprintf("page_VA -> start_page_va %d his pages %d \n",page_VA -> number_of_pages,st_page_VA->number_of_pages);
				uint32 tmp_num_of_pages=num_of_pages;
				while(tmp_num_of_pages--)
				{
					if(allocate_page_to_frame((uint32*)(page_VA ->start_page_va)) != 0) return NULL;
					page_VA ->start_page_va = (page_VA ->start_page_va +PAGE_SIZE);
				}
				cprintf("page_VA -> start_page_va %d his pages %d \n",page_VA -> number_of_pages,st_page_VA->number_of_pages);

				break;
			}

		}
		cprintf("atart %p\n",(uint32*)start_page_va);
		if(st_page_VA==NULL) return NULL; // couldn't allocate

		cprintf("st_page_va = %x \n",st_page_VA);
		cprintf("page_VA -> start_page_va %d his pages %d \n",page_VA -> number_of_pages,st_page_VA->number_of_pages);

		// add in busy_list
		struct PageInfo* busy_va =(struct PageInfo*)st_page_VA;
		busy_va->number_of_pages=num_of_pages;
		busy_va->start_page_va=st_page_VA->start_page_va;
		busy_va->end_page_va=(uint32)page_VA;

		LIST_INSERT_HEAD(&busy_page_list,busy_va); //list is unordered

		cprintf("st_page_va = %x \n",st_page_VA);

		// remove from free_list
		cprintf("our pages %d his pages %d \n",num_of_pages,st_page_VA->number_of_pages);
		if(st_page_VA->number_of_pages == num_of_pages)
		{
			LIST_REMOVE(&free_page_list, st_page_VA);
		}
		else
		{
			cprintf("here\n");
			st_page_VA -> number_of_pages -= num_of_pages;
			st_page_VA -> start_page_va = start_page_va; //final page address after allocation
			// end address will be the same
		}

		cprintf("return st_page_va = %x \n",st_page_VA);
		//cprintf("free list size = %d , first element start address %p , first element num pages: %d\n",LIST_SIZE(&free_page_list),LIST_FIRST(&free_page_list)->start_page_va,LIST_FIRST(&free_page_list)->number_of_pages);

		return (uint32*)(st_page_VA->start_page_va); //will the address be the same after removal?*/

	    //check data in the list (for realloc) [copying data]
}

void kfree(void* virtual_address)
{
	//[PROJECT'24.MS2] Implement this function
	//Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");
	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

	//block allocator
	if((uint32)virtual_address >= KERNEL_HEAP_START && (uint32*)virtual_address <= hardlimit)
	{
		/// va in the middle of the block?
		free_block(virtual_address);
	}
	else if((uint32*)virtual_address >= (uint32*)((char*)hardlimit+PAGE_SIZE) && (uint32)virtual_address <= KERNEL_HEAP_MAX)
	{
		uint32* va = ROUNDDOWN(virtual_address,PAGE_SIZE);
		//if(va == (uint32*)pages_arr[0].start_page_va)cprintf("equal");
		uint32 pageIndex = ((uint32)va - ((uint32)hardlimit+PAGE_SIZE))/PAGE_SIZE;

		uint32 num_of_pages = pages_arr[pageIndex].number_of_pages;
		//cprintf(" index %d ,size %d",pageIndex,num_of_pages);

		if(pages_arr[pageIndex].is_free || !pages_arr[pageIndex].is_first_addr){
			cprintf("already free Virtual address or not first address in allocated pages...");return;
		}


		pages_arr[pageIndex].is_free = 1;

//		pages_arr[pageIndex + num_of_pages -1].is_free = 1;
//		pages_arr[pageIndex + num_of_pages -1].number_of_pages = num_of_pages;
//		pages_arr[pageIndex + num_of_pages -1].is_first_addr = 0;


		//cprintf(" last index %d ,size %d",pageIndex + num_of_pages -1);

		//un_map the pages
		uint32 tmp_num_of_pages = num_of_pages;
		while(tmp_num_of_pages--)
		{
			unmap_frame(ptr_page_directory, (uint32) va);
			va = (uint32*)((char*)va+ PAGE_SIZE);
		}


		if(pageIndex != 0)
		if(pages_arr[pageIndex-1].is_free){
			//merge
			//update pageindex to the begin
			uint32 num_of_old_pages = num_of_pages;
			uint32 num_of_pages = pages_arr[pageIndex-1].number_of_pages;

			int tmppageIndex = (int)(pageIndex - pages_arr[pageIndex-1].number_of_pages);
			pageIndex = tmppageIndex >= 0 ? tmppageIndex : -tmppageIndex;

			pages_arr[pageIndex].number_of_pages = num_of_old_pages+num_of_pages;

			pages_arr[pageIndex+num_of_old_pages+num_of_pages-1].number_of_pages = num_of_old_pages+num_of_pages;//updating last page in block of pages
			pages_arr[pageIndex+num_of_old_pages+num_of_pages-1].is_free = 1;

//			cprintf(" merge bfr index %d ,size %d",pageIndex,num_of_old_pages+num_of_pages);
//			cprintf("merged before last index %d\n",pageIndex+num_of_old_pages+num_of_pages-1);

		}

		if(pageIndex < KERNEL_HEAP_MAX - PAGE_SIZE)
		if(pages_arr[pageIndex+num_of_pages].is_free){
			//merge
			uint32 added_size = pages_arr[pageIndex+num_of_pages].number_of_pages;
			pages_arr[pageIndex].number_of_pages += added_size;

			//update last page in the block
			pages_arr[pageIndex+pages_arr[pageIndex].number_of_pages-1].number_of_pages += pages_arr[pageIndex].number_of_pages;
			pages_arr[pageIndex+pages_arr[pageIndex].number_of_pages-1].is_free = 1;

//			cprintf(" merge aftr index %d ,size %d",pageIndex,pages_arr[pageIndex+pages_arr[pageIndex].number_of_pages-1].number_of_pages);
//			cprintf("merged aftr last index %d\n",pageIndex+pages_arr[pageIndex].number_of_pages-1);


		}
		//if(!not)pages_arr[pageIndex + num_of_pages -1].is_free = 1;;
		/*/// would he give me an address that in the middle of a free block?

		uint32* va = ROUNDDOWN(virtual_address,PAGE_SIZE);
		uint32 pageIndex = ((uint32)va - ((uint32)hardlimit+PAGE_SIZE))/PAGE_SIZE;
		uint32 num_of_pages = numOfAllocPages_busyList(va);//

		if(num_of_pages==0) return; // already free

		//un_map the pages
		uint32 tmp_num_of_pages = num_of_pages;
		while(tmp_num_of_pages--)
		{
			unmap_frame(ptr_page_directory, (uint32) va);
			va = (uint32*)((char*)va+ PAGE_SIZE);
		}

		//remove from busy list
		va=ROUNDDOWN(virtual_address,PAGE_SIZE);  // re-assign
		LIST_REMOVE(&busy_page_list,(struct PageInfo*)va);

		//add in free list & merge
		merge_freeList(va, num_of_pages);*/

	}
	else
	{
		panic("Invalid Virtual address...");
	}

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	//[PROJECT'24.MS2] [KERNEL HEAP] kheap_virtual_address
	// Write your code here, remove the panic and write your code
	panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	//[PROJECT'24.MS2] [KERNEL HEAP] kheap_physical_address
	// Write your code here, remove the panic and write your code
	panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//EFFICIENT IMPLEMENTATION ~O(1) IS REQUIRED ==================
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
	return NULL;
	panic("krealloc() is not implemented yet...!!");
}


//=================================================================================//
//============================== OUR HELPER FUNCTIONS =============================//
//=================================================================================//

int allocate_page_to_frame(void * page_VA){

	//[1] Check if the page exists or not?
	uint32 *ptr_table = NULL;
	struct FrameInfo* ptr_frame_info ;/*= get_frame_info(ptr_page_directory, (uint32)page_VA, &ptr_table);
	if (ptr_frame_info != NULL) return 0; // already allocated*/

	//struct FrameInfo *ptr_frame_info;

	//[2] Allocate new frame
	allocate_frame(&ptr_frame_info);  //it panics if there is no memory

	//[3] Map the given va to the allocated frame
	int ret = map_frame(ptr_page_directory, ptr_frame_info, (uint32)page_VA, PERM_WRITEABLE);
	if (ret != 0) {
		cprintf("couldn't map!\n");
		free_frame(ptr_frame_info);
		return -1;
	}
	return 0;
}


void init_free_list()
{

//	    // ??????????????
//		//check for not found frame in the next code
//		struct FrameInfo* ptr_frame_info;
//		allocate_frame(&ptr_frame_info);
//		map_frame(ptr_page_directory,ptr_frame_info,(uint32)((char*)hardlimit+PAGE_SIZE),PERM_WRITEABLE);
//		//---

		struct PageInfo p_alloc;
		p_alloc .is_first_addr =1;
		p_alloc .start_page_va =(uint32)((char*)hardlimit + PAGE_SIZE);
		p_alloc . end_page_va = KERNEL_HEAP_MAX - PAGE_SIZE;
		p_alloc .number_of_pages =(KERNEL_HEAP_MAX-(p_alloc.start_page_va)) / PAGE_SIZE;
		p_alloc .is_free =1;


		pages_arr[0]=p_alloc;



	    /*struct PageInfo* p_alloc = (struct PageInfo*)(ptr_page_directory);
	    //round down in any of these?
	    p_alloc -> start_page_va = (uint32)((char*)hardlimit + PAGE_SIZE);
	    p_alloc -> end_page_va = KERNEL_HEAP_MAX - PAGE_SIZE;
	    p_alloc -> number_of_pages = (KERNEL_HEAP_MAX-(p_alloc->start_page_va)) / PAGE_SIZE;

		LIST_INSERT_HEAD(&free_page_list,p_alloc);*/
}


int numOfAllocPages_busyList(void* va)
{
	    struct PageInfo* busy_Page;
		LIST_FOREACH(busy_Page, &(busy_page_list))
		{
			if((busy_Page->start_page_va) == (uint32)va)
			{
				return busy_Page->number_of_pages;
			}
		}
		return 0;
}

void merge_freeList(void* va, int num_of_pages)
{

	struct PageInfo* new_free_Page = (struct PageInfo*)va;
	new_free_Page ->start_page_va = (uint32)va;
	new_free_Page-> number_of_pages = num_of_pages;
	new_free_Page -> end_page_va = (uint32)((char*)va + (num_of_pages*PAGE_SIZE) -  PAGE_SIZE); //beginning of last page


	// get previous & next free block

	struct PageInfo* free_Page, *prev_free_block=NULL, *next_free_block=NULL;
	LIST_FOREACH(free_Page, &(free_page_list))
	{
		if(free_Page->start_page_va > new_free_Page->start_page_va)
		{
			next_free_block=free_Page;
			prev_free_block=LIST_PREV(next_free_block);
			break;
		}
	}

	if(next_free_block==NULL) prev_free_block=LIST_LAST(&free_page_list);


	// merge

	int merge_prev(struct PageInfo* prev_free_block,struct PageInfo* new_free_Page)
	{
		if(prev_free_block!=NULL && ((prev_free_block->end_page_va + PAGE_SIZE) == new_free_Page->start_page_va))
		{
				prev_free_block -> number_of_pages += new_free_Page ->number_of_pages;
				prev_free_block -> end_page_va = new_free_Page ->end_page_va;
				new_free_Page=prev_free_block;

				if(next_free_block!=NULL && ((next_free_block->start_page_va - PAGE_SIZE) == new_free_Page->end_page_va))
				{
					next_free_block -> start_page_va = new_free_Page ->start_page_va;
					next_free_block -> number_of_pages += new_free_Page ->number_of_pages;

					LIST_REMOVE(&free_page_list,new_free_Page);
					return 1;
				}

				return 2;
		}

		return 0; // no merging or prev = NULL
	}

	int merge_next(struct PageInfo* next_free_block,struct PageInfo* new_free_Page)
	{
		if(next_free_block!=NULL && ((next_free_block->start_page_va - PAGE_SIZE) == new_free_Page->end_page_va))
		{
			next_free_block -> start_page_va = new_free_Page ->start_page_va;
			next_free_block -> number_of_pages += new_free_Page ->number_of_pages;
			return 1;
		}
		return 0; // no merging or next = NULL
	}


	if(merge_prev(prev_free_block,new_free_Page)==0 && merge_next(next_free_block,new_free_Page)==0) // no merging
	{
		if(prev_free_block==NULL)LIST_INSERT_HEAD(&free_page_list,new_free_Page);
		else LIST_INSERT_AFTER(&free_page_list,new_free_Page,prev_free_block);
	}

	int my_abs(int x) {
	    return x >= 0 ? x : -x;
	}

//
//	struct PageInfo* free_Page;
//	LIST_FOREACH(free_Page, &(free_page_list))
//	{
//		if(free_Page->start_page_va < new_free_Page->start_page_va){
//
//			if((free_Page -> end_page_va) + PAGE_SIZE == (uint32)new_free_Page ->start_page_va){
//				//merging
//				free_Page -> number_of_pages += new_free_Page ->number_of_pages;
//				free_Page -> end_page_va += new_free_Page ->end_page_va;
//				break;
//			}else continue;
//			//if end_page_va > va then already free --> our va is between start and end of free block
//
//		}else{
//			if((free_Page -> start_page_va) - PAGE_SIZE == (uint32)new_free_Page ->end_page_va){
//				//merging
//				free_Page = (struct PageInfo*)new_free_Page ->start_page_va;//check that line...not easy thing to delete
//				free_Page -> start_page_va = new_free_Page ->start_page_va;
//				free_Page -> number_of_pages += new_free_Page ->number_of_pages;
//			     break;
//			}else {
//				//store in the list normally
//				LIST_INSERT_BEFORE(&(free_page_list),free_Page,new_free_Page);
//
//				break;
//			}
//		}
//
//
//	  }

}



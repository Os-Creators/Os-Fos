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
    // Write your code here, remove the panic and write your code
    //panic("initialize_kheap_dynamic_allocator() is not implemented yet...!!");
     start=daStart;

		hardlimit=daLimit;
		segment_break=daStart+initSizeToAllocate;
		if(segment_break>hardlimit){
			return E_NO_MEM;
		}
		uint32 num_pages;
		uint32 page_address=start;
		uint32* ptr_page_table = NULL;
		struct FrameInfo **ptr_frame_info;
		//struct FrameInfopointer_frame_info;
		//num_pages=(segment_break-start)/PAGE_SIZE;
		uint32 check_allocations = 0;
		num_pages = ROUNDUP(initSizeToAllocate, PAGE_SIZE); num_pages /= PAGE_SIZE ;
		while(num_pages--){

    // ptr_page_table=get_page_table(ptr_page_directory, page_address,&ptr_page_table);

     struct FrameInfo* ptr_frame_info=get_frame_info(ptr_page_directory,page_address,&ptr_page_table);


     allocate_frame(&ptr_frame_info);

     map_frame(ptr_page_directory,ptr_frame_info,page_address, PERM_USER | PERM_PRESENT | PERM_WRITEABLE );
		 page_address += PAGE_SIZE;
		  check_allocations ++;
	 }
    initialize_dynamic_allocator( daStart,initSizeToAllocate);
    //page_allocator ----------------------------------------------

	struct PageInfo* page_allocator = (struct PageInfo*)((char*)hardlimit+PAGE_SIZE);//any thing to not making it null

	//check for not found frame in the next code
	struct FrameInfo* ptr_frame_info2=get_frame_info(ptr_page_directory,hardlimit+PAGE_SIZE,&ptr_page_table);
	allocate_frame(&ptr_frame_info2);
	map_frame(ptr_page_directory,ptr_frame_info2,hardlimit+PAGE_SIZE, PERM_AVAILABLE|PERM_USER | PERM_WRITEABLE );
	//---

	//initialize data
	page_allocator -> start_page_va = (uint32)((char*)hardlimit + PAGE_SIZE);
	page_allocator -> end_page_va = KERNEL_HEAP_MAX - PAGE_SIZE; //beginning of last page
	page_allocator -> number_of_pages = (KERNEL_HEAP_MAX / PAGE_SIZE);
	max_merged_pages_size = (KERNEL_HEAP_MAX / PAGE_SIZE);

	LIST_INSERT_HEAD(&free_Page_list,page_allocator);

	//------------------------------------------------------------

	//cprintf("in initialize list size %d,first element size %d",LIST_SIZE(&free_Page_list),LIST_FIRST(&free_Page_list)->number_of_pages);

	if(check_allocations==num_pages){
	   return 0;
	}


        return E_NO_MEM;




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
	/*uint32 previous_break = segment_break;
		if (numOfPages == 0 )
		{
			return (void*) segment_break;
	//		cprintf("\nsegment_break = %u\n",segment_break);

		}
		else if(numOfPages>0){
			   segment_break += numOfPages * PAGE_SIZE;
			   //allocate and mapped

			   return (void*)previous_break;
		}*/

	return (void*)-1 ;
	//====================================================

	//[PROJECT'24.MS2] Implement this function
	// Write your code here, remove the panic and write your code
	//panic("sbrk() is not implemented yet...!!");
}


void* kmalloc(unsigned int size)
{
	//[PROJECT'24.MS2] Implement this function
	// Write your code here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	if(size <= DYN_ALLOC_MAX_BLOCK_SIZE)
		return alloc_block_FF(size);

	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy
	if(isKHeapPlacementStrategyFIRSTFIT() != 1){
		panic("the strategy is not first fit");
	}
	//cprintf("in initialize list size %d,first element size %d",LIST_SIZE(&free_Page_list),LIST_FIRST(&free_Page_list)->number_of_pages);

	uint32 num_of_pages = ROUNDUP(size / PAGE_SIZE, PAGE_SIZE);
	struct PageInfo* page_VA;

	if(max_merged_pages_size >= num_of_pages){

		//see first element that fits all size
		LIST_FOREACH(page_VA, &(free_Page_list))
		{
			if( (page_VA -> number_of_pages) >= num_of_pages){
				while(num_of_pages--){
					if(allocate_page_to_frame(page_VA) != 0)return NULL;
					page_VA = (struct PageInfo*)((char*)page_VA +PAGE_SIZE);
				}
				break;
			}
		}

		//removing from the list
		if(page_VA -> number_of_pages == num_of_pages)
			LIST_REMOVE(&free_Page_list, page_VA);
		else{
			page_VA -> number_of_pages -= num_of_pages;
			page_VA -> start_page_va = (uint32) (char*)(page_VA ->start_page_va)+(num_of_pages * PAGE_SIZE);
		}

		return page_VA;
	}else{
		return NULL;
	}
	//check data in the list (for realloc)
}

void kfree(void* virtual_address)
{
	//[PROJECT'24.MS2] Implement this function
	// Write your code here, remove the panic and write your code
	panic("kfree() is not implemented yet...!!");

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

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
//============================== OUR HELPER FUNCTIONS ===================================//
//=================================================================================//

int allocate_page_to_frame(struct PageInfo * page_VA){
	//[1] Check if the page exists or not?
	uint32 *ptr_table = NULL;
	struct FrameInfo* ptr_frame_info = get_frame_info(ptr_page_directory, (uint32)page_VA, &ptr_table);
	if (ptr_frame_info != NULL) return 0;

	//[2] Allocate new frame
	int ret = allocate_frame(&ptr_frame_info) ;
	if (ptr_frame_info == NULL || ret != 0) {
		cprintf("No enough memory for page itself!\n");
		return -1;
	}

	//[3] Map the given va to the allocated frame
	//check perms
	ret = map_frame(ptr_page_directory, ptr_frame_info, (uint32)page_VA, PERM_USER|PERM_WRITEABLE|PERM_PRESENT);
	if (ret != 0) {
		cprintf("No enough memory for page table!\n"); //free the allocated frame
		free_frame(ptr_frame_info) ;
		return -1;
	}
	return 0 ;



}

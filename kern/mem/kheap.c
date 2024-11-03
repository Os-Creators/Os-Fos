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
            uint32 ptr_page_table = NULL;
            struct FrameInfo **ptr_frame_info;
            //struct FrameInfopointer_frame_info;
            //num_pages=(segment_break-start)/PAGE_SIZE;
            uint32 check_allocations = 0;
            num_pages = ROUNDUP(initSizeToAllocate, PAGE_SIZE); num_pages /= PAGE_SIZE ;
      while(num_pages--){

    // ptr_page_table=get_page_table(ptr_page_directory, page_address,&ptr_page_table);

     struct FrameInfoptr_frame_info=get_frame_info(ptr_page_directory,page_address,&ptr_page_table);

     allocate_frame(&ptr_frame_info);

     map_frame(ptr_page_directory,ptr_frame_info,page_address, PERM_USER | PERM_PRESENT | PERM_WRITEABLE );
     page_address += PAGE_SIZE;
      check_allocations ++;
            }
    initialize_dynamic_allocator( daStart,initSizeToAllocate);

       if(check_allocations==num_pages){
        return 0;
        }
        return E_NO_MEM;
//           char* start_addr= (char*)(hardlimit + PAGE_SIZE);
//
//            while(start_addr!= KERNEL_HEAP_MAX)
//            {
//            	LIST_INSERT_TAIL(&free_Page_list,(struct PageInfo*)start_addr);  //might get exception
//            	start_addr+=PAGE_SIZE;
//            }
//

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
	uint32 previous_break = segment_break;
		if (numOfPages == 0 )
		{
			return (void*) segment_break;
	//		cprintf("\nsegment_break = %u\n",segment_break);

		}
		else if(numOfPages>0){
			   segment_break += numOfPages * PAGE_SIZE;
			   //allocate and mapped

			   return (void*)previous_break;
		}

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
	kpanic_into_prompt("kmalloc() is not implemented yet...!!");

	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy

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

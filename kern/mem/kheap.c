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
             int ret1=allocate_frame(&ptr_frame_info);
             if (ptr_frame_info == NULL || ret1 != 0) {
                  cprintf("No enough memory for page itself!\n");
                  return E_NO_MEM;
             }

            int ret2=map_frame(ptr_page_directory,ptr_frame_info,(uint32)tmp_start,PERM_WRITEABLE |PERM_PRESENT);
         	if (ret2 != 0)
         	{
               cprintf("No enough memory for page table!\n");
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
            int retur=map_frame(ptr_page_directory,ptr_frame_info,(uint32)begin_alloc,PERM_WRITEABLE | PERM_PRESENT);
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

void kfree(void* virtual_address)
{
	//[PROJECT'24.MS2] Implement this function
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");

	//block allocator ?
	if(virtual_address > (void*)start && virtual_address < (void*)(hardlimit+PAGE_SIZE)){
		free_block(virtual_address);
	}

	uint32* va = ROUNDDOWN(virtual_address,PAGE_SIZE);
	uint32 num_of_pages = 0;

	//getting how many pages to free
	struct PageInfo* busy_Page;
	LIST_FOREACH(busy_Page, &(busy_Page_list))
	{
		if( (busy_Page -> start_page_va) == (uint32)va){
			num_of_pages = busy_Page -> number_of_pages;
			break;
		}
	}
	if(num_of_pages == 0){
		panic("page is already free !");
	}

	//unmapping
	uint32 tmp_num_of_pages = num_of_pages;
	while(tmp_num_of_pages--){
		unmap_frame(ptr_page_directory, (uint32) va);
		va += PAGE_SIZE;
	}
	va=ROUNDDOWN(virtual_address,PAGE_SIZE); //because va changed in the line before this


	//making the pointer that will be added to the list
	struct PageInfo* new_free_Page = (struct PageInfo*)va;
	new_free_Page ->start_page_va = (uint32)va;
	new_free_Page-> number_of_pages = num_of_pages;
	new_free_Page -> end_page_va = (uint32)( (char*)va + (num_of_pages*PAGE_SIZE) -  PAGE_SIZE); //beginning of last page

	//updating free page list and merging if possible
	struct PageInfo* free_Page;
	LIST_FOREACH(free_Page, &(free_Page_list))
	{
		if( (free_Page -> start_page_va) < (uint32)new_free_Page ->start_page_va){

			if((free_Page -> end_page_va) + PAGE_SIZE == (uint32)new_free_Page ->start_page_va){
				//merging
				free_Page -> number_of_pages += new_free_Page ->number_of_pages;
				free_Page -> end_page_va += new_free_Page ->end_page_va;
				if(max_merged_pages_size < free_Page -> number_of_pages )max_merged_pages_size = free_Page -> number_of_pages;

				break;
			}else continue;
			//if end_page_va > va then already free --> our va is between start and end of free block

		}else{
			if((free_Page -> start_page_va) - PAGE_SIZE == (uint32)new_free_Page ->end_page_va){
				//merging
				free_Page = (struct PageInfo*)new_free_Page ->start_page_va;//check that line...not easy thing to delete
				free_Page -> start_page_va = new_free_Page ->start_page_va;
				free_Page -> number_of_pages += new_free_Page ->number_of_pages;

				if(max_merged_pages_size < free_Page -> number_of_pages )max_merged_pages_size = free_Page -> number_of_pages;
				break;
			}else {
				//store in the list normally
				LIST_INSERT_BEFORE(&(free_Page_list),free_Page,new_free_Page);
				if(max_merged_pages_size < free_Page -> number_of_pages )max_merged_pages_size = free_Page -> number_of_pages;

				break;
			}
		}
	}

	//what if va is not in beginning of address we allocated in kmlloc

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
	ret = map_frame(ptr_page_directory, ptr_frame_info, (uint32)page_VA, PERM_WRITEABLE|PERM_PRESENT|PERM_AVAILABLE);
	if (ret != 0) {
		cprintf("No enough memory for page table!\n"); //free the allocated frame
		free_frame(ptr_frame_info) ;
		return -1;
	}
	return 0 ;
}

void init_free_list()
{

	    struct PageInfo* page_allocator = (struct PageInfo*)((char*)hardlimit+PAGE_SIZE);//any thing to not making it null

		//check for not found frame in the next code
		struct FrameInfo* ptr_frame_info2;
		allocate_frame(&ptr_frame_info2);
		map_frame(ptr_page_directory,ptr_frame_info2,(uint32)((char*)hardlimit+PAGE_SIZE), PERM_PRESENT|  PERM_AVAILABLE | PERM_WRITEABLE );
		//---

		//initialize data
		page_allocator -> start_page_va = (uint32)((char*)hardlimit + PAGE_SIZE);
		page_allocator -> end_page_va = KERNEL_HEAP_MAX - PAGE_SIZE; //beginning of last page
		page_allocator -> number_of_pages = (KERNEL_HEAP_MAX / PAGE_SIZE);
		max_merged_pages_size = (KERNEL_HEAP_MAX / PAGE_SIZE);

		LIST_INSERT_HEAD(&free_Page_list,page_allocator);

}


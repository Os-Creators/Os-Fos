#include <inc/lib.h>
//#include <kern/proc/user_environment.c>

struct UserPageInfo {

	uint32 start_page_va;    // data type? (store address)
	uint32 end_page_va;   // inclusive AKA free page

	bool is_first_addr:1;      // is first address in all allocated blocks?
	bool is_last_addr;
    bool is_free:1;
    int ID_shared;

    uint32 number_of_pages;
};

struct UserPageInfo user_pages_arr[(USER_HEAP_MAX-USER_HEAP_START)/PAGE_SIZE];

bool firstTime = 1;
//init();

//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//

//=============================================
// [1] CHANGE THE BREAK LIMIT OF THE USER HEAP:
//=============================================
/*2023*/
void* sbrk(int increment)
{
	return (void*) sys_sbrk(increment);
}

//=================================
// [2] ALLOCATE SPACE IN USER HEAP:
//=================================
void* malloc(uint32 size)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #12] [3] USER HEAP [USER SIDE] - malloc()
	// Write your code here, remove the panic and write your code
	//panic("malloc() is not implemented yet...!!");
	//return NULL;
	//Use sys_isUHeapPlacementStrategyFIRSTFIT() and	sys_isUHeapPlacementStrategyBESTFIT()
	//to check the current strategy


	// Block Allocator ------> ask about it
	if(size <= DYN_ALLOC_MAX_BLOCK_SIZE)
		return alloc_block_FF(size);


	if(!sys_isUHeapPlacementStrategyFIRSTFIT() && !sys_isUHeapPlacementStrategyBESTFIT())
		panic("NOT FIRST FIT STRATEGY");

	//[BLOCK ALLOCATOR]
	//[PAGE ALLOCATOR]
	//SAME AS KMALLOC ARRAY
	//after finding the address call sys_allocate_user_mem(uint32 virtual_address, uint32 size)
	//without mapping

	uint32 num_of_pages = ROUNDUP(size , PAGE_SIZE)/ PAGE_SIZE; //check calculation
	uint32 page_allocator_pages = (USER_HEAP_MAX-((uint32)myEnv->hardlimit + PAGE_SIZE))/PAGE_SIZE;

	if(num_of_pages > page_allocator_pages) return NULL;

	bool page_found = 0;
	uint32 return_addr;

	//initializing if it's the first time
	if(firstTime){
		struct UserPageInfo p_alloc;
		p_alloc.is_first_addr =1;
		p_alloc.start_page_va =(uint32)((char*)myEnv->hardlimit + PAGE_SIZE);
		p_alloc.end_page_va = USER_HEAP_MAX - PAGE_SIZE;
		p_alloc.number_of_pages =(USER_HEAP_MAX-(p_alloc.start_page_va)) / PAGE_SIZE;
		p_alloc.is_free =1;

		user_pages_arr[0]=p_alloc;
		firstTime = 0;

	}

	for(int i = 0 ; i < page_allocator_pages-1 ; i++){


		if(user_pages_arr[i].is_free && user_pages_arr[i].number_of_pages >= num_of_pages){
			if(user_pages_arr[i].number_of_pages > num_of_pages){
				//split
				uint32 splited_free_page_index = (i + num_of_pages);

				//(like header)
				user_pages_arr[splited_free_page_index].number_of_pages = user_pages_arr[i].number_of_pages -num_of_pages;
				user_pages_arr[splited_free_page_index].is_free = 1;
				user_pages_arr[splited_free_page_index].start_page_va = ((uint32)myEnv->hardlimit + PAGE_SIZE) + (splited_free_page_index*PAGE_SIZE);
				user_pages_arr[splited_free_page_index].is_first_addr = 1;

				//(like footer)
				user_pages_arr[splited_free_page_index+user_pages_arr[splited_free_page_index].number_of_pages-1].number_of_pages = user_pages_arr[i].number_of_pages -num_of_pages;
				user_pages_arr[splited_free_page_index+user_pages_arr[splited_free_page_index].number_of_pages-1].is_free = 1;

			}

			//like header of the allocated block
			user_pages_arr[i].is_free = 0;
			user_pages_arr[i].number_of_pages = num_of_pages;
			user_pages_arr[i].start_page_va = ((uint32)myEnv->hardlimit + PAGE_SIZE) + (i*PAGE_SIZE);
			return_addr = user_pages_arr[i].start_page_va;
			user_pages_arr[i].is_first_addr = 1;

			//like footer
			user_pages_arr[i+num_of_pages-1].is_last_addr =1;
			user_pages_arr[i+num_of_pages-1].is_free = 0;
			user_pages_arr[i+num_of_pages-1].number_of_pages=num_of_pages;


			page_found = 1;

			//marking
			sys_allocate_user_mem(return_addr,size);

			break;

		}else{
			//jump
			i = (i + user_pages_arr[i].number_of_pages) -1;//for loop will increment it again and remove (-1)

		}
	}

	if(!page_found) return NULL; // couldn't allocate

	return (uint32*)return_addr;
}

//=================================
// [3] FREE SPACE FROM USER HEAP:
//=================================
void free(void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #14] [3] USER HEAP [USER SIDE] - free()
	// Write your code here, remove the panic and write your code
	//panic("free() is not implemented yet...!!");

	//[BLOCK ALLOCATOR]
	//[PAGE ALLOCATOR]
	//SAME AS KMALLOC ARRAY
	//after finding the address call sys_allocate_user_mem(uint32 virtual_address, uint32 size)
	//without umapping

	//initializing if it's the first time
	if(firstTime){
		struct UserPageInfo p_alloc;
		p_alloc.is_first_addr =1;
		p_alloc.start_page_va =(uint32)((char*)myEnv->hardlimit + PAGE_SIZE);
		p_alloc.end_page_va = USER_HEAP_MAX - PAGE_SIZE;
		p_alloc.number_of_pages =(USER_HEAP_MAX-(p_alloc.start_page_va)) / PAGE_SIZE;
		p_alloc.is_free =1;

		user_pages_arr[0]=p_alloc;
		firstTime = 0;

	}
	//block allocator
	if((uint32)virtual_address >= USER_HEAP_START && (uint32*)virtual_address <= myEnv->hardlimit)
	{
		/// va in the middle of the block?
		free_block(virtual_address);
	}
	//page allocator
	else if((uint32*)virtual_address >= (uint32*)((char*)myEnv->hardlimit+PAGE_SIZE) && (uint32)virtual_address <= USER_HEAP_MAX)
	{
		uint32* va = ROUNDDOWN(virtual_address,PAGE_SIZE);
		uint32 pageIndex = ((uint32)va - ((uint32)myEnv->hardlimit+PAGE_SIZE))/PAGE_SIZE;
		uint32 num_of_pages = user_pages_arr[pageIndex].number_of_pages;

		if(user_pages_arr[pageIndex].is_free || !user_pages_arr[pageIndex].is_first_addr)
		{
			cprintf("already free Virtual address or not first address in allocated pages...");return;
		}

		//header of this block of pages
		user_pages_arr[pageIndex].is_free = 1;

		//footer of this block of pages
		user_pages_arr[pageIndex + num_of_pages -1].is_free = 1;
		user_pages_arr[pageIndex + num_of_pages -1].number_of_pages = num_of_pages;
		user_pages_arr[pageIndex + num_of_pages -1].is_first_addr = 0;

		//un_map the pages
		cprintf("IN FREE %x\n",virtual_address);
		sys_free_user_mem((uint32)virtual_address,(user_pages_arr[pageIndex].number_of_pages*PAGE_SIZE));

		//before my block is empty
	if(pageIndex != 0)
		if(user_pages_arr[pageIndex-1].is_free)
		{
			//merge

			//update pageindex to the begin
			user_pages_arr[pageIndex].is_first_addr = 0;

			uint32 num_of_old_pages = num_of_pages;
			 num_of_pages = user_pages_arr[pageIndex-1].number_of_pages;

			 //page index know is the index of the previous page
			 //absolute -> i dont know why function didn't make it right , i am tired
			int tmppageIndex = (int)(pageIndex - user_pages_arr[pageIndex-1].number_of_pages);
			pageIndex = tmppageIndex >= 0 ? tmppageIndex : -tmppageIndex;

			//update data of first page of prev block (header)
			user_pages_arr[pageIndex].number_of_pages = num_of_old_pages+num_of_pages;

			//updating last page in all block (prev + curr)
			user_pages_arr[pageIndex+num_of_old_pages+num_of_pages-1].number_of_pages = num_of_old_pages+num_of_pages;
			user_pages_arr[pageIndex+num_of_old_pages+num_of_pages-1].is_free = 1;

			num_of_pages = user_pages_arr[pageIndex].number_of_pages;

		}

		//after my block of pages is empty
	if(pageIndex < USER_HEAP_MAX - PAGE_SIZE)
		if(user_pages_arr[pageIndex+num_of_pages].is_free){
			//merge
			user_pages_arr[pageIndex+num_of_pages].is_first_addr = 0;

			uint32 added_size = user_pages_arr[pageIndex+num_of_pages].number_of_pages;
			user_pages_arr[pageIndex].number_of_pages += added_size;

			//update last page in the block
			user_pages_arr[pageIndex+user_pages_arr[pageIndex].number_of_pages-1].number_of_pages = user_pages_arr[pageIndex].number_of_pages;
			user_pages_arr[pageIndex+user_pages_arr[pageIndex].number_of_pages-1].is_free = 1;

		}
	}
	else
	{
		panic("Invalid Virtual address...");
	}
}


//=================================
// [4] ALLOCATE SHARED VARIABLE:
//=================================
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
	//==============================================================
	//DON'T CHANGE THIS CODE========================================
	if (size == 0) return NULL ;
	//==============================================================
	//TODO: [PROJECT'24.MS2 - #18] [4] SHARED MEMORY [USER SIDE] - smalloc()
	// Write your code here, remove the panic and write your code
	//panic("smalloc() is not implemented yet...!!");
	//return NULL;
	/*void* va = new_malloc(size);

	if(va != NULL){
		int ret = sys_createSharedObject(sharedVarName,size,isWritable,va);

		if(ret == E_SHARED_MEM_NOT_EXISTS || ret == E_NO_SHARE)
			return NULL;
	}

	return va;*/


	uint32 num_of_pages = ROUNDUP(size , PAGE_SIZE)/ PAGE_SIZE; //check calculation
	//if(num_of_pages == 0) num_of_pages = 1;

	uint32 page_allocator_pages = (USER_HEAP_MAX-((uint32)myEnv->hardlimit + PAGE_SIZE))/PAGE_SIZE;

	if(num_of_pages > page_allocator_pages) return NULL;

	bool page_found = 0;
	uint32 return_addr;

	//initializing if it's the first time
	if(firstTime){
		struct UserPageInfo p_alloc;
		p_alloc.is_first_addr =1;
		p_alloc.start_page_va =(uint32)((char*)myEnv->hardlimit + PAGE_SIZE);
		p_alloc.end_page_va = USER_HEAP_MAX - PAGE_SIZE;
		p_alloc.number_of_pages =(USER_HEAP_MAX-(p_alloc.start_page_va)) / PAGE_SIZE;
		p_alloc.is_free =1;

		user_pages_arr[0]=p_alloc;
		firstTime = 0;

	}

	for(int i = 0 ; i < page_allocator_pages-1 ; i++){


		if(user_pages_arr[i].is_free && user_pages_arr[i].number_of_pages >= num_of_pages){

			uint32* va = (uint32*)(((uint32)myEnv->hardlimit + PAGE_SIZE) + (i*PAGE_SIZE));
			if(va != NULL){
				sys_acquire_shared_sleep();
				int ret = sys_createSharedObject(sharedVarName,size,isWritable,va);

				if(ret < 0 )
					return NULL;

				user_pages_arr[i].ID_shared = ret;
				//cprintf("ID in smalloc %d\n",ret);
				//cprintf("array id in smalloc %d\n",user_pages_arr[i].ID_shared);
				//cprintf("index in smalloc %d\n",i);
			}

			if(user_pages_arr[i].number_of_pages > num_of_pages){
				//split
				uint32 splited_free_page_index = (i + num_of_pages);

				//(like header)
				user_pages_arr[splited_free_page_index].number_of_pages = user_pages_arr[i].number_of_pages -num_of_pages;
				user_pages_arr[splited_free_page_index].is_free = 1;
				user_pages_arr[splited_free_page_index].start_page_va = ((uint32)myEnv->hardlimit + PAGE_SIZE) + (splited_free_page_index*PAGE_SIZE);
				user_pages_arr[splited_free_page_index].is_first_addr = 1;

				//(like footer)
				user_pages_arr[splited_free_page_index+user_pages_arr[splited_free_page_index].number_of_pages-1].number_of_pages = user_pages_arr[i].number_of_pages -num_of_pages;
				user_pages_arr[splited_free_page_index+user_pages_arr[splited_free_page_index].number_of_pages-1].is_free = 1;

			}

			//like header of the allocated block
			user_pages_arr[i].is_free = 0;
			user_pages_arr[i].number_of_pages = num_of_pages;
			user_pages_arr[i].start_page_va = ((uint32)myEnv->hardlimit + PAGE_SIZE) + (i*PAGE_SIZE);
			return_addr = user_pages_arr[i].start_page_va;
			user_pages_arr[i].is_first_addr = 1;

			//like footer
			user_pages_arr[i+num_of_pages-1].is_last_addr =1;
			user_pages_arr[i+num_of_pages-1].is_free = 0;
			user_pages_arr[i+num_of_pages-1].number_of_pages=num_of_pages;


			page_found = 1;

			//marking
			sys_allocate_user_mem(return_addr,size);

			break;

		}else{
			//jump
			i = (i + user_pages_arr[i].number_of_pages) -1;//for loop will increment it again and remove (-1)

		}
	}

	if(!page_found) return NULL; // couldn't allocate

	return (uint32*)return_addr;
}

//========================================
// [5] SHARE ON ALLOCATED SHARED VARIABLE:
//========================================
void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//TODO: [PROJECT'24.MS2 - #20] [4] SHARED MEMORY [USER SIDE] - sget()
	// Write your code here, remove the panic and write your code
	//panic("sget() is not implemented yet...!!");
	//return NULL;

	int size = sys_getSizeOfSharedObject(ownerEnvID,sharedVarName);

	if(size == E_SHARED_MEM_NOT_EXISTS)
		return NULL;

	/*void* va = new_malloc(size);

	if(va != NULL){
		int ret = sys_getSharedObject(ownerEnvID,sharedVarName,va);

		if(ret == E_SHARED_MEM_NOT_EXISTS)
			return NULL;
	}

	return va;*/


	uint32 num_of_pages = ROUNDUP(size , PAGE_SIZE)/ PAGE_SIZE; //check calculation

	uint32 page_allocator_pages = (USER_HEAP_MAX-((uint32)myEnv->hardlimit + PAGE_SIZE))/PAGE_SIZE;

	if(num_of_pages > page_allocator_pages) return NULL;

	bool page_found = 0;
	uint32 return_addr;

	//initializing if it's the first time
	if(firstTime){
		struct UserPageInfo p_alloc;
		p_alloc.is_first_addr =1;
		p_alloc.start_page_va =(uint32)((char*)myEnv->hardlimit + PAGE_SIZE);
		p_alloc.end_page_va = USER_HEAP_MAX - PAGE_SIZE;
		p_alloc.number_of_pages =(USER_HEAP_MAX-(p_alloc.start_page_va)) / PAGE_SIZE;
		p_alloc.is_free =1;

		user_pages_arr[0]=p_alloc;
		firstTime = 0;

	}

	for(int i = 0 ; i < page_allocator_pages-1 ; i++){


		if(user_pages_arr[i].is_free && user_pages_arr[i].number_of_pages >= num_of_pages){

			uint32* va = (uint32*)(((uint32)myEnv->hardlimit + PAGE_SIZE) + (i*PAGE_SIZE));
			if(va != NULL){
				int ret = sys_getSharedObject(ownerEnvID,sharedVarName,va);
				if(ret < 0)
					return NULL;

				user_pages_arr[i].ID_shared=ret;
			}

			if(user_pages_arr[i].number_of_pages > num_of_pages){
				//split
				uint32 splited_free_page_index = (i + num_of_pages);

				//(like header)
				user_pages_arr[splited_free_page_index].number_of_pages = user_pages_arr[i].number_of_pages -num_of_pages;
				user_pages_arr[splited_free_page_index].is_free = 1;
				user_pages_arr[splited_free_page_index].start_page_va = ((uint32)myEnv->hardlimit + PAGE_SIZE) + (splited_free_page_index*PAGE_SIZE);
				user_pages_arr[splited_free_page_index].is_first_addr = 1;

				//(like footer)
				user_pages_arr[splited_free_page_index+user_pages_arr[splited_free_page_index].number_of_pages-1].number_of_pages = user_pages_arr[i].number_of_pages -num_of_pages;
				user_pages_arr[splited_free_page_index+user_pages_arr[splited_free_page_index].number_of_pages-1].is_free = 1;

			}

			//like header of the allocated block
			user_pages_arr[i].is_free = 0;
			user_pages_arr[i].number_of_pages = num_of_pages;
			user_pages_arr[i].start_page_va = ((uint32)myEnv->hardlimit + PAGE_SIZE) + (i*PAGE_SIZE);
			return_addr = user_pages_arr[i].start_page_va;
			user_pages_arr[i].is_first_addr = 1;

			//like footer
			user_pages_arr[i+num_of_pages-1].is_last_addr =1;
			user_pages_arr[i+num_of_pages-1].is_free = 0;
			user_pages_arr[i+num_of_pages-1].number_of_pages=num_of_pages;


			page_found = 1;

			//marking
			sys_allocate_user_mem(return_addr,size);

			break;

		}else{
			//jump
			i = (i + user_pages_arr[i].number_of_pages) -1;//for loop will increment it again and remove (-1)

		}
	}

	if(!page_found) return NULL; // couldn't allocate

	return (uint32*)return_addr;
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=================================
// FREE SHARED VARIABLE:
//=================================
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	  //TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [USER SIDE] - sfree()
		    // Write your code here, remove the panic and write your code
		    //panic("sfree() is not implemented yet...!!");

	uint32 va = ROUNDDOWN((uint32)virtual_address,PAGE_SIZE);
	uint32 pageIndex = (va - ((uint32)myEnv->hardlimit+PAGE_SIZE))/PAGE_SIZE;

	int ID = user_pages_arr[pageIndex].ID_shared;
	user_pages_arr[pageIndex].ID_shared = -1;

	free((void*)va);
	cprintf("ID in sfree %d\n",ID);
	//cprintf("index in sfree %d\n",pageIndex);
	//va &= 0x7FFFFFFF;

	sys_freeSharedObject(ID,(void*)va);

}


//=================================
// REALLOC USER SPACE:
//=================================
//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_move_user_mem(...)
//		which switches to the kernel mode, calls move_user_mem(...)
//		in "kern/mem/chunk_operations.c", then switch back to the user mode here
//	the move_user_mem() function is empty, make sure to implement it.
void *realloc(void *virtual_address, uint32 new_size)
{
	//[PROJECT]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");
	return NULL;

}


//==================================================================================//
//========================== MODIFICATION FUNCTIONS ================================//
//==================================================================================//

void expand(uint32 newSize)
{
	panic("Not Implemented");

}
void shrink(uint32 newSize)
{
	panic("Not Implemented");

}
void freeHeap(void* virtual_address)
{
	panic("Not Implemented");

}

//==================================================================================//
//========================== OUR HELPER FUNCTIONS ================================//
//==================================================================================//
//void* new_malloc(uint32 size){
//
//	uint32 num_of_pages = ROUNDUP(size , PAGE_SIZE)/ PAGE_SIZE; //check calculation
//	if(num_of_pages == 0) num_of_pages = 1;
//
//	uint32 page_allocator_pages = (USER_HEAP_MAX-((uint32)myEnv->hardlimit + PAGE_SIZE))/PAGE_SIZE;
//
//	if(num_of_pages > page_allocator_pages) return NULL;
//
//	bool page_found = 0;
//	uint32 return_addr;
//
//	//initializing if it's the first time
//	if(firstTime){
//		struct UserPageInfo p_alloc;
//		p_alloc.is_first_addr =1;
//		p_alloc.start_page_va =(uint32)((char*)myEnv->hardlimit + PAGE_SIZE);
//		p_alloc.end_page_va = USER_HEAP_MAX - PAGE_SIZE;
//		p_alloc.number_of_pages =(USER_HEAP_MAX-(p_alloc.start_page_va)) / PAGE_SIZE;
//		p_alloc.is_free =1;
//
//		user_pages_arr[0]=p_alloc;
//		firstTime = 0;
//
//	}
//
//	for(int i = 0 ; i < page_allocator_pages-1 ; i++){
//
//
//		if(user_pages_arr[i].is_free && user_pages_arr[i].number_of_pages >= num_of_pages){
//			if(user_pages_arr[i].number_of_pages > num_of_pages){
//				//split
//				uint32 splited_free_page_index = (i + num_of_pages);
//
//				//(like header)
//				user_pages_arr[splited_free_page_index].number_of_pages = user_pages_arr[i].number_of_pages -num_of_pages;
//				user_pages_arr[splited_free_page_index].is_free = 1;
//				user_pages_arr[splited_free_page_index].start_page_va = ((uint32)myEnv->hardlimit + PAGE_SIZE) + (splited_free_page_index*PAGE_SIZE);
//				user_pages_arr[splited_free_page_index].is_first_addr = 1;
//
//				//(like footer)
//				user_pages_arr[splited_free_page_index+user_pages_arr[splited_free_page_index].number_of_pages-1].number_of_pages = user_pages_arr[i].number_of_pages -num_of_pages;
//				user_pages_arr[splited_free_page_index+user_pages_arr[splited_free_page_index].number_of_pages-1].is_free = 1;
//
//			}
//
//			//like header of the allocated block
//			user_pages_arr[i].is_free = 0;
//			user_pages_arr[i].number_of_pages = num_of_pages;
//			user_pages_arr[i].start_page_va = ((uint32)myEnv->hardlimit + PAGE_SIZE) + (i*PAGE_SIZE);
//			return_addr = user_pages_arr[i].start_page_va;
//			user_pages_arr[i].is_first_addr = 1;
//
//			//like footer
//			user_pages_arr[i+num_of_pages-1].is_last_addr =1;
//			user_pages_arr[i+num_of_pages-1].is_free = 0;
//			user_pages_arr[i+num_of_pages-1].number_of_pages=num_of_pages;
//
//
//			page_found = 1;
//
//			//marking
//			sys_allocate_user_mem(return_addr,size);
//
//			break;
//
//		}else{
//			//jump
//			i = (i + user_pages_arr[i].number_of_pages) -1;//for loop will increment it again and remove (-1)
//
//		}
//	}
//
//	if(!page_found) return NULL; // couldn't allocate
//
//	return (uint32*)return_addr;
//}

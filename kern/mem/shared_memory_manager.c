#include <inc/memlayout.h>
#include "shared_memory_manager.h"

#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/queue.h>
#include <inc/environment_definitions.h>

#include <kern/proc/user_environment.h>
#include <kern/trap/syscall.h>
#include "kheap.h"
#include "memory_manager.h"

//==================================================================================//
//============================== GIVEN FUNCTIONS ===================================//
//==================================================================================//
struct Share* get_share(int32 ownerID, char* name);

//===========================
// [1] INITIALIZE SHARES:
//===========================
//Initialize the list and the corresponding lock
void sharing_init()
{
#if USE_KHEAP
	LIST_INIT(&AllShares.shares_list) ;
	init_spinlock(&AllShares.shareslock, "shares lock");
#else
	panic("not handled when KERN HEAP is disabled");
#endif
}

//==============================
// [2] Get Size of Share Object:
//==============================
int getSizeOfSharedObject(int32 ownerID, char* shareName)
{
	//[PROJECT'24.MS2] DONE
	// This function should return the size of the given shared object
	// RETURN:
	//	a) If found, return size of shared object
	//	b) Else, return E_SHARED_MEM_NOT_EXISTS
	//
	struct Share* ptr_share = get_share(ownerID, shareName);
	if (ptr_share == NULL)
		return E_SHARED_MEM_NOT_EXISTS;
	else
		return ptr_share->size;

	return 0;
}

//===========================================================


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
//===========================
// [1] Create frames_storage:
//===========================
// Create the frames_storage and initialize it by 0
//lamiaa_mahmoud 2022170597
inline struct FrameInfo** create_frames_storage(int numOfFrames)
{
   struct FrameInfo** framesStorage = kmalloc(numOfFrames * sizeof(struct FrameInfo*));
   if (framesStorage == NULL)
   {
	        return NULL;
   }
	memset(framesStorage, 0, numOfFrames * sizeof(struct FrameInfo*));
	return framesStorage;

}


//=====================================
// [2] Alloc & Initialize Share Object:
//=====================================
//Allocates a new shared object and initialize its member
//It dynamically creates the "framesStorage"
//Return: allocatedObject (pointer to struct Share) passed by reference
struct Share* create_share(int32 ownerID, char* shareName, uint32 size, uint8 isWritable)
{
	//TODO: [PROJECT'24.MS2 - #16] [4] SHARED MEMORY - create_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
    //panic("create_share is not implemented yet");
	//Your Code is Here...
	//lamiaa_mahmoud 2022170597
	struct Share* new_share = kmalloc(sizeof(struct Share));
	if (new_share == NULL)
	{
		return NULL;
	}

	memset(new_share, 0, sizeof(struct Share));
	new_share->ownerID = ownerID;
	strncpy(new_share->name, shareName, sizeof(new_share->name) - 1);
	new_share->name[sizeof(new_share->name) - 1] = '\0';
	new_share->size = size;
	new_share->references = 1;
	new_share->isWritable = isWritable;

	uint32 numFrames = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	new_share->framesStorage = create_frames_storage(numFrames);
	if (new_share->framesStorage == NULL)
	{
	   kfree(new_share);
	   return NULL;
	 }

	 new_share->ID = (int32)new_share;
	 new_share->ID &= 0x7FFFFFFF;
	 LIST_INSERT_HEAD(&AllShares.shares_list, new_share);

	 return new_share;
}

//=============================
// [3] Search for Share Object:
//=============================
//Search for the given shared object in the "shares_list"
//Return:
//	a) if found: ptr to Share object
//	b) else: NULL
struct Share* get_share(int32 ownerID, char* name)
{
	//TODO: [PROJECT'24.MS2 - #17] [4] SHARED MEMORY - get_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
    //panic("get_share is not implemented yet");
	//Your Code is Here...
	//lamiaa_mahmoud 2022170597

	// Search for share object in static list
#if USE_KHEAP == 0

    for (int i = 0; i < MAX_SHARES; i++) {
        if (shares[i].ownerID == ownerID && strcmp(shares[i].name, name) == 0) {
            return &shares[i];
        }
    }
#else
    // Search for share object in dynamic list
    struct Share* current_share = AllShares.shares_list.lh_first;
    while (current_share != NULL) {
            if (current_share->ownerID == ownerID && strcmp(current_share->name, name) == 0) {
                return current_share;
            }
            current_share = current_share->prev_next_info.le_next;
        }
#endif

    return NULL;
}

//=========================
// [4] Create Share Object:
//=========================
int createSharedObject(int32 ownerID, char* shareName, uint32 size, uint8 isWritable, void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #19] [4] SHARED MEMORY [KERNEL SIDE] - createSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("createSharedObject is not implemented yet");
	//Your Code is Here...

	struct Env* myenv = get_cpu_proc(); //The calling environment
	struct Share* object=create_share(ownerID,shareName,size,1);

	if(object==NULL){
		return	E_NO_SHARE;
	}
	//LIST_INSERT_TAIL(&AllShares,object);
	LIST_INSERT_TAIL(&AllShares.shares_list,object);
	uint32* start=virtual_address;
	size = ROUNDUP(size, PAGE_SIZE);
	uint32 allocate=size/PAGE_SIZE;
	struct FrameInfo *ptr_frame_info;
	struct FrameInfo** framesStorage;
	for(uint32 i=0;i<allocate;i++)
	{
		allocate_page_to_frame(start);
		uint32 mem = allocate_frame(&ptr_frame_info);
	    map_frame(myenv->env_page_directory, ptr_frame_info, (uint32)start, PERM_WRITEABLE);

        //add to frames storage
	    //framesStorage[i] = object;
		start=start+PAGE_SIZE;
	}
	struct Share*tmp;
	//check tmp if exist in shares_list
	LIST_FOREACH(tmp,&(AllShares.shares_list))
	{
	if(tmp==object)return E_SHARED_MEM_EXISTS;
	}
	struct Share * s;
    //return s->ID;
	return 0;
 }

//======================
// [5] Get Share Object:
//======================
int getSharedObject(int32 ownerID, char* shareName, void* virtual_address)
{
	//TODO: [PROJECT'24.MS2 - #21] [4] SHARED MEMORY [KERNEL SIDE] - getSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("getSharedObject is not implemented yet");
	//Your Code is Here...
	struct Env* myenv = get_cpu_proc(); //The calling environment
	struct Share* shared_obj = get_share(ownerID, shareName);
	if(shared_obj == NULL)
	{
	    return E_SHARED_MEM_NOT_EXISTS;
	}
	uint32 sizeOfPage = getSizeOfSharedObject(ownerID,shareName)/PAGE_SIZE;
	struct FrameInfo** phys_frames = shared_obj->framesStorage;
	uint32 va = (uint32)virtual_address;
	for (uint32 i = 0; i < sizeOfPage; i++)
	{
	   int perm = PERM_USER | PERM_PRESENT;
	   if (shared_obj->isWritable)
	   {
	       perm |= PERM_WRITEABLE;
	   }
       map_frame(myenv->env_page_directory, phys_frames[i], (va + i * PAGE_SIZE), perm);
	 }

	shared_obj->references++;
	if(shared_obj->references == 1)
	{
	shared_obj->ID = va & 0x7FFFFFFF;
	}
	return shared_obj->ID;
}

//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//==========================
// [B1] Delete Share Object:
//==========================
//delete the given shared object from the "shares_list"
//it should free its framesStorage and the share object itself
void free_share(struct Share* ptrShare)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - free_share()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("free_share is not implemented yet");
	//Your Code is Here...

}
//========================
// [B2] Free Share Object:
//========================
int freeSharedObject(int32 sharedObjectID, void *startVA)
{
	//TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - freeSharedObject()
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("freeSharedObject is not implemented yet");
	//Your Code is Here...

}

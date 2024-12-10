#include <inc/memlayout.h>
#include "shared_memory_manager.h"

#include <inc/mmu.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>
#include <inc/queue.h>
#include <inc/environment_definitions.h>

#include <kern/disk/pagefile_manager.h>
#include <kern/proc/user_environment.h>
#include <kern/trap/syscall.h>
#include "kheap.h"
#include "memory_manager.h"


inline void releaseSharedSleep()
{
	if(holding_sleeplock(&shared_sleeplock))
		release_sleeplock(&shared_sleeplock);
}

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
	init_sleeplock(&shared_sleeplock, "shared sleep lock");
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
	new_share->size = ROUNDUP(size,PAGE_SIZE);
	new_share->references = 1;
	new_share->isWritable = isWritable;

	uint32 numFrames = ROUNDUP((size + PAGE_SIZE - 1),PAGE_SIZE) / PAGE_SIZE;
	new_share->framesStorage = create_frames_storage(numFrames);

	if (new_share->framesStorage == NULL)
	{
	   kfree(new_share);
	   return NULL;
	 }

	 new_share->ID = (uint32)new_share;
	 new_share->ID &= 0x7FFFFFFF;

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
	if(!holding_spinlock(&AllShares.shareslock)) acquire_spinlock(&AllShares.shareslock);

	struct Share* current_share = AllShares.shares_list.lh_first;
    LIST_FOREACH(current_share,&(AllShares.shares_list))
    {
      if (current_share->ownerID == ownerID && strcmp(current_share->name, name) == 0)
      {
    	 if(holding_spinlock(&AllShares.shareslock)) release_spinlock(&AllShares.shareslock);
    	 return current_share;

       }
    }

    if(holding_spinlock(&AllShares.shareslock)) release_spinlock(&AllShares.shareslock);
    return NULL;

#endif


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
		if(get_share(ownerID,shareName)!=NULL){

			releaseSharedSleep();
			return	E_SHARED_MEM_EXISTS;
		}

		struct Share* object=create_share(ownerID,shareName,size,isWritable);

		if(object==NULL){

			releaseSharedSleep();
			return	E_NO_SHARE;
		}


		uint32 start=(uint32)virtual_address;
		size = ROUNDUP(size, PAGE_SIZE);
		uint32 allocate=size/PAGE_SIZE;

		struct FrameInfo **frames=object->framesStorage;
		int ret;
		for(uint32 i=0;i<allocate;i++)
		{

			allocate_frame(&(frames[i]));

			ret = map_frame(myenv->env_page_directory, frames[i], start, PERM_WRITEABLE|PERM_USER);

			start=start+PAGE_SIZE;
		}

		if(!holding_spinlock(&(AllShares.shareslock))) acquire_spinlock(&(AllShares.shareslock));
		LIST_INSERT_TAIL(&(AllShares.shares_list),object);
		if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));

		releaseSharedSleep();

		return object->ID;

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

	if(!holding_spinlock(&(AllShares.shareslock))) acquire_spinlock(&(AllShares.shareslock));


	uint32 sizeOfPage = ROUNDUP(shared_obj->size,PAGE_SIZE)/PAGE_SIZE;
	struct FrameInfo** phys_frames = shared_obj->framesStorage;
	uint32 va = (uint32)virtual_address;
	uint32* tmp_va = virtual_address;

	shared_obj->references++;
	if(shared_obj->references == 1)
	{
		shared_obj->ID = va & 0x7FFFFFFF;
	}

	//map and update perms
	for (uint32 i = 0; i < sizeOfPage; i++)
	{
	   if (shared_obj->isWritable)
	   {
	       map_frame(myenv->env_page_directory, phys_frames[i], (va + i * PAGE_SIZE), PERM_WRITEABLE|PERM_USER);
	   }else{
		   map_frame(myenv->env_page_directory, phys_frames[i], (va + i * PAGE_SIZE), PERM_USER);
	   }
	 }

	if(holding_spinlock(&(AllShares.shareslock))) release_spinlock(&(AllShares.shareslock));

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
  //panic("free_share is not implemented yet");
  //Your Code is Here...
  if(holding_spinlock(&(AllShares.shareslock))==0)
    acquire_spinlock(&(AllShares.shareslock));

  LIST_REMOVE(&AllShares.shares_list,ptrShare);

  if(holding_spinlock((&AllShares.shareslock))==1)
    release_spinlock((&AllShares.shareslock));

  kfree(ptrShare->framesStorage);
  kfree(ptrShare);

}

//========================
// [B2] Free Share Object:
//========================
int freeSharedObject(int32 sharedObjectID, void *startVA)
{
  //TODO: [PROJECT'24.MS2 - BONUS#4] [4] SHARED MEMORY [KERNEL SIDE] - freeSharedObject()
  //COMMENT THE FOLLOWING LINE BEFORE START CODING
  //panic("freeSharedObject is not implemented yet");
  //Your Code is Here...

	  if(holding_spinlock(&(AllShares.shareslock))==0) acquire_spinlock(&(AllShares.shareslock));

	  struct Share* sObject;

	  LIST_FOREACH(sObject, &(AllShares.shares_list))
	  {
	    if (sObject->ID == sharedObjectID)
	      {
	          break;
	      }

	  }
	  // corner case
	  if(sObject == NULL || sObject->references == 0)
	  {
		  if(holding_spinlock(&(AllShares.shareslock))==1)release_spinlock(&(AllShares.shareslock));
		  return -1;
	  }

	  if(holding_spinlock(&(AllShares.shareslock))==1)release_spinlock(&(AllShares.shareslock));


	  uint32 start = ROUNDDOWN((uint32)startVA,PAGE_SIZE);
	  uint32 allocate = ROUNDUP(sObject->size,PAGE_SIZE)/PAGE_SIZE;

	  struct Env* myenv = get_cpu_proc();

	  for(uint32 i=0; i < allocate; i++)
	    {

	      uint32 *ptr_page_table;
	      int ret = get_page_table(myenv->env_page_directory,start,&ptr_page_table);

	       if(myenv->env_page_directory[PDX((uint32*)start)] != 0)
	       {
	    	  bool empty = 1;

			   for(uint32 j=0; j<1024; j++)
			  {
				if((ptr_page_table[j] & (~PERM_AVAILABLE)) != 0)
				{

				   empty = 0;
				   break;
				}
			  }

			  if(empty)
			  {
				  cprintf("after disk1\n");

				  pf_remove_env_page(myenv,(uint32)ptr_page_table); //remove page table from disk
				  kfree(ptr_page_table);
				  pd_clear_page_dir_entry(myenv->env_page_directory,(uint32)start); //remove page table entry from page directory

				  cprintf("after disk2\n");
			 }
	     }

	      start = start + PAGE_SIZE;
	    }

	  if(holding_spinlock(&(AllShares.shareslock))==0) acquire_spinlock(&(AllShares.shareslock));

	   sObject->references--;

	   if(sObject->references == 0)
		   free_share(sObject);

	   tlbflush();

	   if(holding_spinlock(&(AllShares.shareslock))==1)release_spinlock(&(AllShares.shareslock));

	   return 0;
}


//==================================================================================//
//============================== OUR HELPER FUNCTIONS ===================================//
//==================================================================================//
int compare_shares(struct Share *a, struct Share *b) {
    return a->ownerID == b->ownerID &&
           strcmp(a->name, b->name) == 0 &&
           a->size == b->size;
}

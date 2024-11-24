#ifndef FOS_KERN_KHEAP_H_
#define FOS_KERN_KHEAP_H_

#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#include <inc/types.h>
#include <inc/queue.h>
#include <inc/memlayout.h>


/*2017*/
uint32 _KHeapPlacementStrategy;
//Values for user heap placement strategy
#define KHP_PLACE_CONTALLOC 0x0
#define KHP_PLACE_FIRSTFIT 	0x1
#define KHP_PLACE_BESTFIT 	0x2
#define KHP_PLACE_NEXTFIT 	0x3
#define KHP_PLACE_WORSTFIT 	0x4

static inline void setKHeapPlacementStrategyCONTALLOC(){_KHeapPlacementStrategy = KHP_PLACE_CONTALLOC;}
static inline void setKHeapPlacementStrategyFIRSTFIT(){_KHeapPlacementStrategy = KHP_PLACE_FIRSTFIT;}
static inline void setKHeapPlacementStrategyBESTFIT(){_KHeapPlacementStrategy = KHP_PLACE_BESTFIT;}
static inline void setKHeapPlacementStrategyNEXTFIT(){_KHeapPlacementStrategy = KHP_PLACE_NEXTFIT;}
static inline void setKHeapPlacementStrategyWORSTFIT(){_KHeapPlacementStrategy = KHP_PLACE_WORSTFIT;}

static inline uint8 isKHeapPlacementStrategyCONTALLOC(){if(_KHeapPlacementStrategy == KHP_PLACE_CONTALLOC) return 1; return 0;}
static inline uint8 isKHeapPlacementStrategyFIRSTFIT(){if(_KHeapPlacementStrategy == KHP_PLACE_FIRSTFIT) return 1; return 0;}
static inline uint8 isKHeapPlacementStrategyBESTFIT(){if(_KHeapPlacementStrategy == KHP_PLACE_BESTFIT) return 1; return 0;}
static inline uint8 isKHeapPlacementStrategyNEXTFIT(){if(_KHeapPlacementStrategy == KHP_PLACE_NEXTFIT) return 1; return 0;}
static inline uint8 isKHeapPlacementStrategyWORSTFIT(){if(_KHeapPlacementStrategy == KHP_PLACE_WORSTFIT) return 1; return 0;}

//***********************************

void* kmalloc(unsigned int size);
void kfree(void* virtual_address);
void *krealloc(void *virtual_address, unsigned int new_size);

unsigned int kheap_virtual_address(unsigned int physical_address);
unsigned int kheap_physical_address(unsigned int virtual_address);

int numOfKheapVACalls ;


//[PROJECT'24.MS2] add suitable code here

uint32* start;
uint32* hardlimit;
uint32* segment_break;

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
uint32 frame_page[1048576];

//=================================================================================//
//============================== OUR HELPER FUNCTIONS ===================================//
//=================================================================================//
void* kleave(void* va,uint32 size,uint32 new_size);
void* page_to_block_allocator(void* va,uint32 size,uint32 new_size);
void* block_to_page_allocator(void* va,uint32 size,uint32 new_size);

int allocate_page_to_frame(void * page_VA);
void deallocate_page_to_frame(void * page_VA);
void init_free_list();

// not used
//int numOfAllocPages_busyList(void* va);
//void merge_freeList(void* va, int num_of_pages);
//int my_abs(int x);

#endif // FOS_KERN_KHEAP_H_


#ifndef FOS_KERN_KHEAP_H_
#define FOS_KERN_KHEAP_H_

#ifndef FOS_KERNEL
# error "This is a FOS kernel header; user programs should not #include it"
#endif

#include <inc/types.h>
#include <inc/queue.h>

uint32* start;
//unsigned int end;
uint32* hardlimit;
uint32* segment_break;


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

LIST_HEAD(PageInfo_List, PageInfo);
typedef LIST_ENTRY(PageInfo) Free_page_LIST_entry_t;
struct PageInfo {
	uint32 start_page_va;    // data type?
	uint32 end_page_va;
	uint32 number_of_pages;
	Free_page_LIST_entry_t prev_next_info;
};

struct PageInfo_List free_Page_list;

uint32 max_merged_pages_size;


LIST_HEAD(BusyPageInfo_List, PageInfo);
struct BusyPageInfo_List busy_Page_list;

//LIST_HEAD(BusyPageInfo_List, BusyPageInfo);
//typedef LIST_ENTRY(BusyPageInfo) Busy_page_LIST_entry_t;
//struct BusyPageInfo {
//	uint32 va;
//	uint32 number_of_pages;
//	Busy_page_LIST_entry_t prev_next_info;
//};
//
//struct BusyPageInfo_List busy_Page_list;




//=================================================================================//
//============================== OUR HELPER FUNCTIONS ===================================//
//=================================================================================//
int allocate_page_to_frame(struct PageInfo * page_VA);
void init_free_list();

#endif // FOS_KERN_KHEAP_H_


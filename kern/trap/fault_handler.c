/*
 * fault_handler.c
 *
 *  Created on: Oct 12, 2022
 *      Author: HP
 */

#include "trap.h"
#include <kern/proc/user_environment.h>
#include <kern/cpu/sched.h>
#include <kern/cpu/cpu.h>
#include <kern/disk/pagefile_manager.h>
#include <kern/mem/memory_manager.h>
#include <kern/mem/kheap.h>

//2014 Test Free(): Set it to bypass the PAGE FAULT on an instruction with this length and continue executing the next one
// 0 means don't bypass the PAGE FAULT
uint8 bypassInstrLength = 0;

//===============================
// REPLACEMENT STRATEGIES
//===============================
//2020
void setPageReplacmentAlgorithmLRU(int LRU_TYPE)
{
	assert(LRU_TYPE == PG_REP_LRU_TIME_APPROX || LRU_TYPE == PG_REP_LRU_LISTS_APPROX);
	_PageRepAlgoType = LRU_TYPE ;
}
void setPageReplacmentAlgorithmCLOCK(){_PageRepAlgoType = PG_REP_CLOCK;}
void setPageReplacmentAlgorithmFIFO(){_PageRepAlgoType = PG_REP_FIFO;}
void setPageReplacmentAlgorithmModifiedCLOCK(){_PageRepAlgoType = PG_REP_MODIFIEDCLOCK;}
/*2018*/ void setPageReplacmentAlgorithmDynamicLocal(){_PageRepAlgoType = PG_REP_DYNAMIC_LOCAL;}
/*2021*/ void setPageReplacmentAlgorithmNchanceCLOCK(int PageWSMaxSweeps){_PageRepAlgoType = PG_REP_NchanceCLOCK;  page_WS_max_sweeps = PageWSMaxSweeps;}

//2020
uint32 isPageReplacmentAlgorithmLRU(int LRU_TYPE){return _PageRepAlgoType == LRU_TYPE ? 1 : 0;}
uint32 isPageReplacmentAlgorithmCLOCK(){if(_PageRepAlgoType == PG_REP_CLOCK) return 1; return 0;}
uint32 isPageReplacmentAlgorithmFIFO(){if(_PageRepAlgoType == PG_REP_FIFO) return 1; return 0;}
uint32 isPageReplacmentAlgorithmModifiedCLOCK(){if(_PageRepAlgoType == PG_REP_MODIFIEDCLOCK) return 1; return 0;}
/*2018*/ uint32 isPageReplacmentAlgorithmDynamicLocal(){if(_PageRepAlgoType == PG_REP_DYNAMIC_LOCAL) return 1; return 0;}
/*2021*/ uint32 isPageReplacmentAlgorithmNchanceCLOCK(){if(_PageRepAlgoType == PG_REP_NchanceCLOCK) return 1; return 0;}

//===============================
// PAGE BUFFERING
//===============================
void enableModifiedBuffer(uint32 enableIt){_EnableModifiedBuffer = enableIt;}
uint8 isModifiedBufferEnabled(){  return _EnableModifiedBuffer ; }

void enableBuffering(uint32 enableIt){_EnableBuffering = enableIt;}
uint8 isBufferingEnabled(){  return _EnableBuffering ; }

void setModifiedBufferLength(uint32 length) { _ModifiedBufferLength = length;}
uint32 getModifiedBufferLength() { return _ModifiedBufferLength;}

//===============================
// FAULT HANDLERS
//===============================

//==================
// [1] MAIN HANDLER:
//==================
/*2022*/
uint32 last_eip = 0;
uint32 before_last_eip = 0;
uint32 last_fault_va = 0;
uint32 before_last_fault_va = 0;
int8 num_repeated_fault  = 0;

struct Env* last_faulted_env = NULL;
void fault_handler(struct Trapframe *tf)
{
	/******************************************************/
	// Read processor's CR2 register to find the faulting address
	uint32 fault_va = rcr2();
	//	cprintf("\n************Faulted VA = %x************\n", fault_va);
	//	print_trapframe(tf);
	/******************************************************/

	//If same fault va for 3 times, then panic
	//UPDATE: 3 FAULTS MUST come from the same environment (or the kernel)
	struct Env* cur_env = get_cpu_proc();
	if (last_fault_va == fault_va && last_faulted_env == cur_env)
	{
		num_repeated_fault++ ;
		if (num_repeated_fault == 3)
		{
			print_trapframe(tf);
			panic("Failed to handle fault! fault @ at va = %x from eip = %x causes va (%x) to be faulted for 3 successive times\n", before_last_fault_va, before_last_eip, fault_va);
		}
	}
	else
	{
		before_last_fault_va = last_fault_va;
		before_last_eip = last_eip;
		num_repeated_fault = 0;
	}
	last_eip = (uint32)tf->tf_eip;
	last_fault_va = fault_va ;
	last_faulted_env = cur_env;
	/******************************************************/
	//2017: Check stack overflow for Kernel
	int userTrap = 0;
	if ((tf->tf_cs & 3) == 3) {
		userTrap = 1;
	}
	if (!userTrap)
	{
		struct cpu* c = mycpu();
		//cprintf("trap from KERNEL\n");
		if (cur_env && fault_va >= (uint32)cur_env->kstack && fault_va < (uint32)cur_env->kstack + PAGE_SIZE)
			panic("User Kernel Stack: overflow exception!");
		else if (fault_va >= (uint32)c->stack && fault_va < (uint32)c->stack + PAGE_SIZE)
			panic("Sched Kernel Stack of CPU #%d: overflow exception!", c - CPUS);
#if USE_KHEAP
		if (fault_va >= KERNEL_HEAP_MAX)
			panic("Kernel: heap overflow exception!");
#endif
	}
	//2017: Check stack underflow for User
	else
	{
		//cprintf("trap from USER\n");
		if (fault_va >= USTACKTOP && fault_va < USER_TOP)
			panic("User: stack underflow exception!");
	}

	//get a pointer to the environment that caused the fault at runtime
	//cprintf("curenv = %x\n", curenv);
	struct Env* faulted_env = cur_env;
	if (faulted_env == NULL)
	{
		print_trapframe(tf);
		panic("faulted env == NULL!");
	}
	//check the faulted address, is it a table or not ?
	//If the directory entry of the faulted address is NOT PRESENT then
	if ( (faulted_env->env_page_directory[PDX(fault_va)] & PERM_PRESENT) != PERM_PRESENT)
	{
		// we have a table fault =============================================================
		//		cprintf("[%s] user TABLE fault va %08x\n", curenv->prog_name, fault_va);
		//		print_trapframe(tf);

		faulted_env->tableFaultsCounter ++ ;

		table_fault_handler(faulted_env, fault_va);
	}
	else
	{
		if (userTrap)
		{
			/*============================================================================================*/
			//TODO: [PROJECT'24.MS2 - #08] [2] FAULT HANDLER I - Check for invalid pointers
			//(e.g. pointing to unmarked user heap page, kernel or wrong access rights),
			//your code is here
		   int permissions= pt_get_page_permissions(faulted_env->env_page_directory,fault_va);
		   int perm_available=0x800;
		   int user_access= permissions & PERM_USER;
		   int marked_page= permissions & perm_available;
		   int read_access= permissions & PERM_WRITEABLE;
		   int present= permissions & PERM_PRESENT;

		   if(!(fault_va>=0 && fault_va<USER_LIMIT) && user_access!=PERM_USER) //User
		   {
			   //cprintf("1");
			  env_exit();
		   }
		   if(fault_va>= USER_HEAP_START && fault_va<USER_HEAP_MAX && marked_page!=perm_available) // unmarked
		   {
			   //cprintf("2");
			 env_exit();
		   }
		   if(present==PERM_PRESENT && read_access!=PERM_WRITEABLE) // read
		   {
			  // cprintf("3");
			 env_exit();
		   }
			/*============================================================================================*/
		}

		/*2022: Check if fault due to Access Rights */
		int perms = pt_get_page_permissions(faulted_env->env_page_directory, fault_va);
		if (perms & PERM_PRESENT)
			panic("Page @va=%x is exist! page fault due to violation of ACCESS RIGHTS\n", fault_va) ;
		/*============================================================================================*/


		// we have normal page fault =============================================================
		faulted_env->pageFaultsCounter ++ ;

		//		cprintf("[%08s] user PAGE fault va %08x\n", curenv->prog_name, fault_va);
		//		cprintf("\nPage working set BEFORE fault handler...\n");
		//		env_page_ws_print(curenv);

		if(isBufferingEnabled())
		{
			__page_fault_handler_with_buffering(faulted_env, fault_va);
		}
		else
		{
			//page_fault_handler(faulted_env, fault_va);
			page_fault_handler(faulted_env, fault_va);
		}
		//		cprintf("\nPage working set AFTER fault handler...\n");
		//		env_page_ws_print(curenv);


	}

	/*************************************************************/
	//Refresh the TLB cache
	tlbflush();
	/*************************************************************/
}

//=========================
// [2] TABLE FAULT HANDLER:
//=========================
void table_fault_handler(struct Env * curenv, uint32 fault_va)
{
	//panic("table_fault_handler() is not implemented yet...!!");
	//Check if it's a stack page
	uint32* ptr_table;
#if USE_KHEAP
	{
		ptr_table = create_page_table(curenv->env_page_directory, (uint32)fault_va);
	}
#else
	{
		__static_cpt(curenv->env_page_directory, (uint32)fault_va, &ptr_table);
	}
#endif
}

//=========================
// [3] PAGE FAULT HANDLER:
//=========================
void page_fault_handler(struct Env * faulted_env, uint32 fault_va)
{
#if USE_KHEAP

		struct WorkingSetElement *victimWSElement = NULL;
		uint32 wsSize = LIST_SIZE(&(faulted_env->page_WS_list));
#else
		int iWS =faulted_env->page_last_WS_index;
		uint32 wsSize = env_page_ws_get_size(faulted_env);
#endif

   if(wsSize < (faulted_env->page_WS_max_size))
   {
	//cprintf("PLACEMENT=========================WS Size = %d\n", wsSize );
	//TODO: [PROJECT'24.MS2 - #09] [2] FAULT HANDLER I - Placement
	// Write your code here, remove the panic and write your code
	//panic("page_fault_handler().PLACEMENT is not implemented yet...!!");
    // Functions to check if my page is stack or heap
	// allocate , map -> ws ele(kmalloc -> alloc,map)

	 struct WorkingSetElement *new_element = env_page_ws_list_create_element(faulted_env , fault_va);
	 LIST_INSERT_TAIL(&(faulted_env->page_WS_list), new_element);

	 uint32 size = LIST_SIZE(&(faulted_env->page_WS_list));
	 if (size == faulted_env->page_WS_max_size)
	 {
	 	 faulted_env->page_last_WS_element = LIST_FIRST(&(faulted_env->page_WS_list));
	 }
	 else
	 {
	 	 faulted_env->page_last_WS_element = NULL;
	 }

	 int status= pf_read_env_page(faulted_env,(void*)fault_va);

	 uint32 *ptr_page_table;
	 struct FrameInfo *Frame_Info;
	 uint32 faulted_page = allocate_frame(&Frame_Info);

	 if(status == E_PAGE_NOT_EXIST_IN_PF)
	 {
   	   if (is_stack_address(fault_va) == 1 || is_heap_address(fault_va) == 1)
	   {
   		   map_frame(faulted_env->env_page_directory,Frame_Info,fault_va,PERM_WRITEABLE | PERM_USER);
	   }
	  else
	   {
		  //cprintf("4");
		  env_page_ws_invalidate(faulted_env,fault_va);
	      env_exit();
	   }
	 }
	 else
	 {
		 map_frame(faulted_env->env_page_directory,Frame_Info,fault_va,PERM_WRITEABLE | PERM_USER);
	 }
   }
	else
	{
		//cprintf("REPLACEMENT=========================WS Size = %d\n", wsSize );
		//TODO: [PROJECT'24.MS3] [2] FAULT HANDLER II - Replacement

		//setPageReplacmentAlgorithmNchanceCLOCK(-2);

		// 0 --> Normal mode (clean)
		// 1 --> Modified mode (dirty)

		bool mode = (page_WS_max_sweeps >= 0)? 0 : 1;

//			cprintf("before N = %d \n",page_WS_max_sweeps);
//			env_page_ws_print(faulted_env);
//			cprintf("\n");

			// find the victim and the needed sweeps
			uint32 victim_sweeps;
			struct WorkingSetElement* victim = findVictim(mode,faulted_env,&victim_sweeps);

//			cprintf(" victim sweeps = %u \n",victim_sweeps);

			// update data
			update_WS_data(mode,faulted_env,victim,&victim_sweeps);
			faulted_env->page_last_WS_element = victim;

			// replace the victim
			replace(faulted_env,fault_va); // it updates the pointer

//			cprintf("after N = %d \n",page_WS_max_sweeps);
//			env_page_ws_print(faulted_env);

	}
}

void __page_fault_handler_with_buffering(struct Env * curenv, uint32 fault_va)
{
	//[PROJECT] PAGE FAULT HANDLER WITH BUFFERING
	// your code is here, remove the panic and write your code
	panic("__page_fault_handler_with_buffering() is not implemented yet...!!");
}


/////////////////////////////////////////////////////// Bonus Nth chance helper functions

struct WorkingSetElement* findVictim(bool mode,struct Env * faulted_env,uint32* sweeps)
{
	struct WorkingSetElement* victim;
	uint32 min_sweeps = abs(page_WS_max_sweeps) + 6;

	struct WorkingSetElement* tmp_ws_ele = faulted_env->page_last_WS_element;

	// one sweep needed to find the victim page

	while(1 == 1)
	{
		uint32 s = countNeededSweeps(mode,faulted_env,tmp_ws_ele);

		//cprintf(" s = %u \n",s);

		if( s < min_sweeps)
		{
			victim = tmp_ws_ele;
		    min_sweeps = s;
		}

		update_pointer(faulted_env,&tmp_ws_ele);

		if(tmp_ws_ele == faulted_env->page_last_WS_element)
			break;
	}

	*sweeps = min_sweeps;
	return victim;
}
uint32 countNeededSweeps(bool mode,struct Env * faulted_env,struct WorkingSetElement* ws_ele)
{
	int perms = pt_get_page_permissions(faulted_env->env_page_directory,ws_ele->virtual_address);

	// if used bit = 1, then it will take N + 1 sweeps
	// why in the MAX we compare with 1 not 0?
	// if the needed sweeps is 1, then it will loop then +1 sweeps then become victim
	// if the needed sweeps is 0, then it will STILL NEED TO LOOP then become victim without having to add +1 to sweeps

	if(mode == 1) // Modified mode
	{
		if((perms & PERM_USED) == PERM_USED)
		{
			if((perms & PERM_MODIFIED) == PERM_MODIFIED) return (abs(page_WS_max_sweeps) + 1) + 1;
			else return abs(page_WS_max_sweeps) + 1;
		}
		else
		{
			if((perms & PERM_MODIFIED) == PERM_MODIFIED) return MAX(abs(page_WS_max_sweeps) + 1 - ws_ele->sweeps_counter,1);
			else return MAX(abs(page_WS_max_sweeps) - ws_ele->sweeps_counter,1);
		}
	}
	else // Normal mode
	{
		if((perms & PERM_USED) == PERM_USED)
		{
			return abs(page_WS_max_sweeps) + 1;
		}
		else
		{
			return MAX(abs(page_WS_max_sweeps) - ws_ele->sweeps_counter,1);
		}
	}
}
void update_WS_data(bool mode,struct Env * faulted_env,struct WorkingSetElement* victim,uint32* sweeps)
{
	    //cprintf("sweeps in update before %u",*sweeps);

		struct WorkingSetElement* tmp_ws_ele = faulted_env->page_last_WS_element;

		// the elements before the victim pointer will take a (victim sweeps)
		// the elements after the victim pointer will take (victim sweeps - 1)

		while(1 == 1)
		{
			if(tmp_ws_ele == victim) (*sweeps)--;
			if(*sweeps <= 0) break;  // no updates so break

			//cprintf("sweeps in update after %u",*sweeps);

			int perms = pt_get_page_permissions(faulted_env->env_page_directory,tmp_ws_ele->virtual_address);

			uint32 swe = *sweeps;
			if((perms & PERM_USED) == PERM_USED)
			{
				swe--;
				pt_set_page_permissions(faulted_env->env_page_directory, tmp_ws_ele->virtual_address, 0, PERM_USED);
				tmp_ws_ele->sweeps_counter = 0;
			}

			tmp_ws_ele->sweeps_counter += swe;

			update_pointer(faulted_env,&tmp_ws_ele);

			if(tmp_ws_ele == faulted_env->page_last_WS_element)
				break;
		}
}
void replace(struct Env* faulted_env,uint32 fault_va)
{
	struct WorkingSetElement* victim = faulted_env->page_last_WS_element;

	remove_victim(faulted_env,victim);

	// mapping in the new va

//	cprintf("check perms of victim old va that should be all cleared \n");
//    env_page_ws_print(faulted_env);

	// update data
    // unmap function clears all perms including the used bit
	// pt_set_page_permissions(faulted_env->env_page_directory, victim->virtual_address, PERM_USED, 0);
	victim->sweeps_counter = 0;
	victim ->virtual_address = fault_va;

	struct FrameInfo *Frame_Info;
	uint32 faulted_page = allocate_frame(&Frame_Info);
	map_frame(faulted_env->env_page_directory,Frame_Info,fault_va,PERM_WRITEABLE | PERM_USER);

	// read from disk
	int ret = pf_read_env_page(faulted_env,(void*)fault_va);

	 if(ret == E_PAGE_NOT_EXIST_IN_PF)
	 {
		  if (is_stack_address(fault_va) != 1 && is_heap_address(fault_va) != 1)
		  {
			  //cprintf("EXIST \n");
			  env_exit();
		  }
	 }

//	if(ret == E_PAGE_NOT_EXIST_IN_PF)
//	{
//	   cprintf("didn't read from disk \n");
//	}
//	else
//	{
//	   cprintf("read from disk \n");
//	}

//		cprintf("check perms of faulted va \n");
//	    env_page_ws_print(faulted_env);


	update_pointer(faulted_env, &(faulted_env->page_last_WS_element));

}
void remove_victim(struct Env* faulted_env, struct WorkingSetElement* victim)
{
	int perms = pt_get_page_permissions(faulted_env->env_page_directory,victim->virtual_address);

	//Write on disk
	if((perms & PERM_MODIFIED) == PERM_MODIFIED || is_stack_address(victim->virtual_address) == 1 || is_heap_address(victim->virtual_address) == 1)
	{

		uint32 * ptr_page_table;
		struct FrameInfo* modified_page_frame_info= get_frame_info(faulted_env->env_page_directory, victim->virtual_address, &ptr_page_table);

		int ret = pf_update_env_page(faulted_env, victim->virtual_address, modified_page_frame_info);
		// un_map function clears all perms except available bit
		// meaning it clears the modified bit as well
		// pt_set_page_permissions(faulted_env->env_page_directory, victim->virtual_address, 0, PERM_MODIFIED);

		//cprintf("MODIFIED \n");
	}

	unmap_frame(faulted_env->env_page_directory, victim->virtual_address);
}

void update_pointer(struct Env* faulted_env,struct WorkingSetElement** pointer)
{
	 struct WorkingSetElement* last_element = LIST_LAST(&(faulted_env->page_WS_list));

	 if (last_element == *pointer)
	 {
		 *pointer = (LIST_FIRST(&(faulted_env->page_WS_list)));
	 }
	 else
	 {
		 *pointer = LIST_NEXT(*pointer);
	 }
}
int is_stack_address(uint32 address)
 {
    if (address < USTACKTOP && address >= USTACKBOTTOM)
    {
	   return 1;
    }
   else
   {
	   return 0;
   }
 }
int is_heap_address(uint32 address)
 {
     if (address >= USER_HEAP_START && address < USER_HEAP_MAX)
     {
	    return 1;
	 }
    else
    {
 	   return 0;
	}

  }
int abs(int x)
{
    return x >= 0 ? x : (-1*x);
}

/////////////////////////////////////////////////////// Nth chance functions
//bool NthChance(bool mode,struct Env * faulted_env, uint32 fault_va)
//{
////			cprintf("before Nth, N = %d \n",page_WS_max_sweeps);
////			env_page_ws_print(faulted_env);
////			cprintf("\n");
//
//	while(1==1)
//	{
//		struct WorkingSetElement* current_ws = faulted_env->page_last_WS_element;
//		int permissions = pt_get_page_permissions(faulted_env->env_page_directory,current_ws->virtual_address);
//		//cprintf("Outside CONDITION \n");
//		if(checkVictimPage(mode,faulted_env,permissions))
//		{
//			//cprintf("INSIDE CONDITION 1 \n");
//			replace(faulted_env, permissions, fault_va);
//			return 1;
//		}
//		else
//		{
//			//cprintf("INSIDE CONDITION 2 \n");
//
//			if((permissions & PERM_USED) == PERM_USED) // used bit = 1
//			{
//				//cprintf("INSIDE CONDITION 3 \n");
//				pt_set_page_permissions(faulted_env->env_page_directory, current_ws->virtual_address, 0, PERM_USED);
//				permissions &=~(PERM_USED);
//				current_ws->sweeps_counter = 0;
//
//			}
//			else // used bit = 0
//			{
//				//cprintf("INSIDE CONDITION 4 \n");
//				faulted_env->page_last_WS_element->sweeps_counter++;
//			}
//
//			if(checkVictimPage(mode,faulted_env,permissions))
//			{
//				//cprintf("INSIDE CONDITION 5 \n");
//				replace(faulted_env, permissions, fault_va);
//				//cprintf("INSIDE CONDITION 6 \n");
//				return 1;
//			}
//		}
//
//		update_pointer(faulted_env,&(faulted_env->page_last_WS_element));
//
//		if(LIST_FIRST(&(faulted_env->page_WS_list)) == faulted_env->page_last_WS_element)
//		{
//			return 0;
//		}
//
//		//cprintf("durning \n");
//		//env_page_ws_print(faulted_env);
//		//cprintf("\n");
//	}
//
////	cprintf("After Nth, N = %d \n",page_WS_max_sweeps);
////	env_page_ws_print(faulted_env);
////	cprintf("\n");
//}
//bool checkVictimPage(bool mode,struct Env* faulted_env, int perms)
//{
//
//	if((perms & PERM_USED) == PERM_USED)
//	{
//		return 0;
//	}
//
//	uint32 sweeps = faulted_env->page_last_WS_element->sweeps_counter;
//
//	if(mode == 1) // Modified mode
//	{
//		if(((perms & PERM_MODIFIED) == PERM_MODIFIED) && (sweeps == abs(page_WS_max_sweeps) + 1))
//		{
//			return 1;
//		}
//		else if(!((perms & PERM_MODIFIED) == PERM_MODIFIED) && (sweeps == abs(page_WS_max_sweeps)))
//		{
//			return 1;
//		}
//		else
//		{
//			return 0;
//		}
//	}
//	else // Normal mode
//	{
//		if(sweeps == abs(page_WS_max_sweeps))
//		{
//			return 1;
//		}
//		else
//		{
//			return 0;
//		}
//	}
//}
//

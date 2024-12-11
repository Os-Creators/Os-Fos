// Kernel-level Semaphore

#include "inc/types.h"
#include "inc/x86.h"
#include "inc/memlayout.h"
#include "inc/mmu.h"
#include "inc/environment_definitions.h"
#include "inc/assert.h"
#include "inc/string.h"
#include "ksemaphore.h"
#include "channel.h"
#include "../cpu/cpu.h"
#include "../proc/user_environment.h"

void init_ksemaphore(struct ksemaphore *ksem, int value, char *name)
{
	//[PROJECT'24.MS3]
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("init_ksemaphore is not implemented yet");
	//Your Code is Here...
	if (ksem == NULL) {
	        panic("Invalid ksemaphore");
	    }

	    ksem->count = value;

	    strncpy(ksem->name, name, sizeof(ksem->name) - 1);
	    ksem->name[sizeof(ksem->name) - 1] = '\0';
        init_spinlock(&(ksem->lk),"semaphores lock");
        init_channel(&(ksem->chan),"semaphores channel");
}

void wait_ksemaphore(struct ksemaphore *ksem)
{
	//[PROJECT'24.MS3]
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("wait_ksemaphore is not implemented yet");
	//Your Code is Here...
	if (ksem == NULL)
	{
		    panic("Invalid semaphore");
		}
	if(holding_spinlock(&(ksem->lk))==0) acquire_spinlock(&(ksem->lk));
	sleep(&(ksem->chan),&(ksem->lk));
	if(holding_spinlock(&(ksem->lk))==1) release_spinlock(&(ksem->lk));

}

void signal_ksemaphore(struct ksemaphore *ksem)
{
	//[PROJECT'24.MS3]
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("signal_ksemaphore is not implemented yet");
	//Your Code is Here...
	if (ksem == NULL)
	{
			    panic("Invalid semaphore");
	}
	if(holding_spinlock(&(ksem->lk))==0) acquire_spinlock(&(ksem->lk));
	wakeup_one(&(ksem->chan));
	if(holding_spinlock(&(ksem->lk))==1) release_spinlock(&(ksem->lk));
}



// Sleeping locks

#include "inc/types.h"
#include "inc/x86.h"
#include "inc/memlayout.h"
#include "inc/mmu.h"
#include "inc/environment_definitions.h"
#include "inc/assert.h"
#include "inc/string.h"
#include "sleeplock.h"
#include "channel.h"
#include "../cpu/cpu.h"
#include "../proc/user_environment.h"

void init_sleeplock(struct sleeplock *lk, char *name)
{
	init_channel(&(lk->chan), "sleep lock channel");
	init_spinlock(&(lk->lk), "lock of sleep lock");
	strcpy(lk->name, name);
	lk->locked = 0;
	lk->pid = 0;
}
int holding_sleeplock(struct sleeplock *lk)
{
	int r;
	acquire_spinlock(&(lk->lk));
	r = lk->locked && (lk->pid == get_cpu_proc()->env_id);
	release_spinlock(&(lk->lk));
	return r;
}
//==========================================================================

void acquire_sleeplock(struct sleeplock *lk)
{
	//TODO: [PROJECT'24.MS1 - #13] [4] LOCKS - acquire_sleeplock
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("acquire_sleeplock is not implemented yet");
	//Your Code is Here...

	if(holding_spinlock(&(lk->lk))==0) acquire_spinlock(&(lk->lk));//2022170473

		while (lk->locked == 1)    //if(holding_sleeplock(lk)) //2022170473
		{
		  sleep(&(lk->chan),&(lk->lk));  // go to sleep //2022170473
		}

		lk->locked = 1;//2022170473
		lk->pid = get_cpu_proc()->env_id;   //related to above comment

	if(holding_spinlock(&(lk->lk))==1) release_spinlock(&(lk->lk));//2022170473

}

void release_sleeplock(struct sleeplock *lk)
{
	  //TODO: [PROJECT'24.MS1 - #14] [4] LOCKS - release_sleeplock
	  //COMMENT THE FOLLOWING LINE BEFORE START CODING
	  //panic("release_sleeplock is not implemented yet");
	  //Your Code is Here...
	if(holding_spinlock(&(lk->lk))==0) acquire_spinlock(&(lk->lk));//2022170432

	      if(lk->pid==get_cpu_proc()->env_id)//2022170432
	      {
	    	
	    	 	   wakeup_all(&(lk->chan));//2022170432
	    	 	   lk->locked=0; //free //2022170432
	    	 	   lk->pid=-1;//2022170432
	      }
	      
	if(holding_spinlock(&(lk->lk))==1) release_spinlock(&(lk->lk)); //2022170432
}






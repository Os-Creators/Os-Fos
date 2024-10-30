/*
 * channel.c
 *
 *  Created on: Sep 22, 2024
 *      Author: HP
 */
#include "channel.h"
#include <kern/proc/user_environment.h>
#include <kern/cpu/sched.h>
#include <inc/string.h>
#include <inc/disk.h>

//===============================
// 1) INITIALIZE THE CHANNEL:
//===============================
// initialize its lock & queue
void init_channel(struct Channel *chan, char *name)
{
	strcpy(chan->name, name);
	init_queue(&(chan->queue));
}

//===============================
// 2) SLEEP ON A GIVEN CHANNEL:
//===============================
// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
// Ref: xv6-x86 OS code
void sleep(struct Channel *chan, struct spinlock* lk)
{
	//TODO: [PROJECT'24.MS1 - #10] [4] LOCKS - sleep
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("sleep is not implemented yet");
	//Your Code is Here...

	//2022170473
	struct Env* NHcurr_env473=get_cpu_proc();

	//2022170473
	if(holding_spinlock(&ProcessQueues.qlock)==0) acquire_spinlock(&ProcessQueues.qlock);

        if(holding_spinlock(lk)==1) release_spinlock(lk); // guard //2022170473

        NHcurr_env473->env_status = ENV_BLOCKED;//2022170473
    	enqueue(&(chan->queue),NHcurr_env473);//2022170473

    	sched();//2022170473

       // while(NHcurr_env473 != get_cpu_proc());//2022170473
        if(holding_spinlock(lk)==0) acquire_spinlock(lk);//2022170473

    if(holding_spinlock(&ProcessQueues.qlock)==1) release_spinlock(&ProcessQueues.qlock);

}

//==================================================
// 3) WAKEUP ONE BLOCKED PROCESS ON A GIVEN CHANNEL:
//==================================================
// Wake up ONE process sleeping on chan.
// The qlock must be held.
// Ref: xv6-x86 OS code
// chan MUST be of type "struct Env_Queue" to hold the blocked processes
void wakeup_one(struct Channel *chan)
{
	//TODO: [PROJECT'24.MS1 - #11] [4] LOCKS - wakeup_one
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("wakeup_one is not implemented yet");

	//2022170629
	if(holding_spinlock(&ProcessQueues.qlock)==0) acquire_spinlock(&ProcessQueues.qlock);

    struct Env *Mays_waked_process629 = dequeue(&(chan->queue));//2022170629
    if(Mays_waked_process629!=NULL)//2022170629
    {
    	sched_insert_ready0(Mays_waked_process629);//2022170629
    }

    if(holding_spinlock(&ProcessQueues.qlock)==1) release_spinlock(&ProcessQueues.qlock);

}

//====================================================
// 4) WAKEUP ALL BLOCKED PROCESSES ON A GIVEN CHANNEL:
//====================================================
// Wake up all processes sleeping on chan.
// The queues lock must be held.
// Ref: xv6-x86 OS code
// chan MUST be of type "struct Env_Queue" to hold the blocked processes

void wakeup_all(struct Channel *chan)
{
	//TODO: [PROJECT'24.MS1 - #12] [4] LOCKS - wakeup_all
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("wakeup_all is not implemented yet");
	//Your Code is Here...

	//202210213 do a while loop on wakeup_one for the all function
	while (LIST_LAST(&(chan->queue))!=NULL)//202210213
	{
	   wakeup_one(chan);//202210213
	}//202210213

}


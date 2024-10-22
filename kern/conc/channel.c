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
	panic("sleep is not implemented yet");
	//Your Code is Here...

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
		init_spinlock(&ProcessQueues.qlock, "MaChannelSpinlock");
		acquire_spinlock(&ProcessQueues.qlock);

		// If the queue is empty then no processes to wake up
		if (LIST_FIRST(chan->queue) == NULL) {
			release_spinlock(&ProcessQueues.qlock);
			return;
		}
	    struct Env *Mays_waked_process = LIST_FIRST(chan->queue);

	    // wake the process
	    Mays_waked_process->env_status = ENV_READY;
	    remove_from_queue(chan->queue,Mays_waked_process);

	 release_spinlock(&ProcessQueues.qlock);
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
		ProcessQueues.qlock;//202210213
		init_spinlock(&ProcessQueues.qlock, "chan_spinlock");//2022170213
		acquire_spinlock(&ProcessQueues.qlock);//2022170213

		struct Env* waited_process;//2022170213
		LIST_FOREACH(waited_process, &chan->queue) { //2022170213
			sched_insert_ready0(waited_process);//make it ready 2022170213
			LIST_REMOVE(&chan->queue, waited_process);//remove from blocked
		}//2022170213

		release_spinlock(&ProcessQueues.qlock);//2022170213

}


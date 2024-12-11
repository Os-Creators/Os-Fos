// User-level Semaphore

#include "inc/lib.h"

struct semaphore create_semaphore(char *semaphoreName, uint32 value)
{
	//TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
    //panic("create_semaphore is not implemented yet");
	//Your Code is Here...
	//2022170597 lamiaa_mahmoud

	struct __semdata* semdata = (struct __semdata*)smalloc(semaphoreName, sizeof(struct __semdata), 1);
	if (semdata == NULL) {
		panic("Failed to allocate memory for semaphore data");
	}

	sys_init_queue(&(semdata->queue));
	semdata->count = value;
	strncpy(semdata->name, semaphoreName, sizeof(semdata->name) - 1);
	semdata->name[sizeof(semdata->name) - 1] = '\0';

	struct semaphore sem597;
	semdata->lock = 0;
	sem597.semdata = semdata;

	return sem597;
}
struct semaphore get_semaphore(int32 ownerEnvID, char* semaphoreName)
{
	//TODO: [PROJECT'24.MS3 - #03] [2] USER-LEVEL SEMAPHORE - get_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("get_semaphore is not implemented yet");
	//Your Code is Here...
	// 2022170597 lamiaa_mahmoud

    struct __semdata* semdata = (struct __semdata*)sget(ownerEnvID, semaphoreName);
    if (semdata == NULL)
    {

        panic("Semaphore not found in shared memory");
    }

    struct semaphore sem;
    sem.semdata = semdata;

    return sem;


}

void wait_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #04] [2] USER-LEVEL SEMAPHORE - wait_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("wait_semaphore is not implemented yet");
	//Your Code is Here...
    if(sem.semdata == NULL)
    {
    	panic("Invalid semaphore");
    }
	while (1)
	{
		int key = xchg((uint32*)&sem.semdata->lock, 1);
		if (key == 0)
		{
		    break;
		 }
	}

    // decrement the count
	 sem.semdata->count--;
	 // If process is not blocked
	 sem.semdata->lock = 0;

	    if (sem.semdata->count < 0)
	    {
	    	// Block process P
	    	sys_wait_ksemaphore(&sem);
	        // Release the lock after blocking
	        sem.semdata->lock = 0;
	        return;
	    }

}

void signal_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #05] [2] USER-LEVEL SEMAPHORE - signal_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("signal_semaphore is not implemented yet");
	//Your Code is Here...
	if (sem.semdata == NULL)
	{
	    panic("Invalid semaphore");
	}
	while (1)
	{
	int key = xchg((uint32*)&sem.semdata->lock, 1);
	if (key == 0)
	{
	    break;
	 }
	    }
	sem.semdata->count++;
	if (sem.semdata->count <= 0)
	{
		// remove process P from queue and unblock and insert to ready
			sys_signal_ksemaphore(&sem);
		    sem.semdata->lock = 0;
			return;
	}

		sem.semdata->lock = 0;
}
int semaphore_count(struct semaphore sem)
{
	return sem.semdata->count;
}

// User-level Semaphore

#include "inc/lib.h"

struct semaphore create_semaphore(char *semaphoreName, uint32 value)
{
	//TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
    //panic("create_semaphore is not implemented yet");
	//Your Code is Here...
	//2022170597 lamiaa_mahmoud

	struct __semdata* semdata = (struct __semdata*)malloc(sizeof(struct __semdata));
	if (semdata == NULL) {
		panic("Failed to allocate memory for semaphore data");
	}

	sys_init_queue(&semdata->queue);
	semdata->count = value;
	semdata->lock = 0;

	strncpy(semdata->name, semaphoreName, sizeof(semdata->name) - 1);
	semdata->name[sizeof(semdata->name) - 1] = '\0';

	struct semaphore sem597;
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

	struct __semdata* semdata = find_semaphore_by_name(ownerEnvID, semaphoreName);
	if (semdata == NULL) {
		panic("Semaphore not found");
	}

	struct semaphore sem597;
	sem597.semdata = semdata;

	return sem597;
}

void wait_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #04] [2] USER-LEVEL SEMAPHORE - wait_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("wait_semaphore is not implemented yet");
	//Your Code is Here...
	int keyw = 1;
	do
	{
		xchg((uint32*)&keyw,sem.semdata->lock);
	} while (keyw != 0);
	sem.semdata->count--;
//	if (sem.semdata->count < 0)
//	{
//		struct Env* myenv = cur_env;
//		// place this process in s.queue
//		enqueue(&(sem.semdata->queue), myenv);
//		// block this process
//		myenv->env_status = ENV_BLOCKED;
//		// release the lock if the process is blocked
//		sem.semdata->lock = 0;
//	}
	/*If process is not blocked */
	sem.semdata->lock = 0;
}

void signal_semaphore(struct semaphore sem)
{
	//TODO: [PROJECT'24.MS3 - #05] [2] USER-LEVEL SEMAPHORE - signal_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("signal_semaphore is not implemented yet");
	//Your Code is Here...
	int keys = 1;
	do
	{
		xchg((uint32*)&keys,sem.semdata->lock);
	} while (keys != 0);
	sem.semdata->count++;
//	if (sem.semdata->count <= 0)
//	{
//		//remove a process P from s.queue
//		struct Env* env = dequeue(&(sem.semdata->queue));
//		if(env != NULL)
//		{
//		//Unbock process P from blocked process and place process P on ready list//
//        env->env_status = ENV_READY;
//		sched_insert_ready(env);
//		}

	 sem.semdata->lock = 0;
}

int semaphore_count(struct semaphore sem)
{
	return sem.semdata->count;
}
int semaphore_count(struct semaphore sem)
{
	return sem.semdata->count;
}

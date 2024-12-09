#include <kern/proc/priority_manager.h>
#include <inc/assert.h>
#include <kern/proc/user_environment.h>
#include <kern/cmd/command_prompt.h>
#include <kern/disk/pagefile_manager.h>
#include "../mem/memory_manager.h"

extern int sys_calculate_free_frames();

uint8 firstTime = 1;
int test_num = 1;
void test_priority_normal_and_higher()
{
#if USE_KHEAP


	cprintf("\n which case to test?\n");
	test_num = getchar();
	cputchar(test_num);
	test_num -= '0';

	char run_command[100] = "runall";
	switch(test_num){
	case 1 :
		cprintf("\n[1] Run 2 instance of Fibonacci with different priorities AND small threshold\n");
		cprintf("********************************\n");
		cprintf("TO DO: when asking fib index ENTER 30 , second time will be fine if 2(as you like)\n");
		cprintf("********************************\n");
		cprintf("EXPECTED: first fib1 must by interrupted by fib2 and both results printed at the end\n");
		cprintf("********************************\n");


		char command0[100] = "schedPRIRR 5 10 9";
		execute_command(command0);
		char command[100] = "load fib 300 1";
		execute_command(command);
		char command2[100] = "load fib 300 0";
		execute_command(command2);
//		char print_com[100] = "printall";
//				execute_command(print_com);
		//char run_command[100] = "runall";
		execute_command(run_command);

		break;
	case 2 :
			cprintf("\n[2] Run 2 instance of Fibonacci with different priorities AND large threshold\n");
			cprintf("********************************\n");
			cprintf("TO DO: when asking fib index ENTER 30 , second time can be any index \n");
			cprintf("********************************\n");
			cprintf("EXPECTED: first fib1 will continue till the end then fib2 \n");
			cprintf("********************************\n");


			char c1[100] = "schedPRIRR 5 10 1000";
			execute_command(c1);
			char c2[100] = "load fib 100 0";
			execute_command(c2);
			char c3[100] = "load fib 100 1";
			execute_command(c3);

			execute_command(run_command);

			break;
	case 3 :
			cprintf("\n[3] Run 2 instance of Fibonacci with same priorities\n");
			cprintf("********************************\n");
			cprintf("TO DO & EXPECTED: with entering any large index the first process will be interrupted,, if too small then no interrupt\n");
			cprintf("********************************\n");

			char c9[100] = "schedPRIRR 5 10 1000";
			execute_command(c9);
			char c10[100] = "load fib 300 1";
			execute_command(c10);
			char c11[100] = "load fib 300 1";
			execute_command(c11);

			execute_command(run_command);
			break;
	case 4 :
			cprintf("\n[4] Run 2 instance of Fibonacci with big different priorities AND small threshold\n");
			cprintf("********************************\n");
			cprintf("TO DO: when asking fib index ENTER 30 , second time will be fine if 2(as you like)\n");
			cprintf("********************************\n");
			cprintf("EXPECTED: first fib1 must by interrupted by fib2 and both results printed at the end\n");
			cprintf("********************************\n");


			char c20[100] = "schedPRIRR 5 10 9";
			execute_command(c20);
			char c21[100] = "load fib 300 4";
			execute_command(c21);
			char c22[100] = "load fib 300 0";
			execute_command(c22);

			execute_command(run_command);


			break;
	case 5 :
			cprintf("\n[5] Run 2 different processes with different priorities AND large threshold\n");
			cprintf("********************************\n");
			cprintf("TO DO: when asking fib index ENTER 30 , second time can be any index \n");
			cprintf("********************************\n");
			cprintf("EXPECTED: first fib1 will continue till the end then fib2 \n");
			cprintf("********************************\n");

			char c23[100] = "schedPRIRR 5 10 1000";
			execute_command(c23);
			char c24[100] = "load fib 300 0";
			execute_command(c24);
			char c25[100] = "load fact 300 1";
			execute_command(c25);

			execute_command(run_command);
			break;
	case 6 :
			cprintf("\n[6] Run 2 different processes with different priorities AND small threshold\n");
			cprintf("********************************\n");
			cprintf("TO DO: when asking fib index ENTER 30 , second time will be fine if 2(as you like)\n");
			cprintf("********************************\n");
			cprintf("EXPECTED: first fib1 must by interrupted by fib2 and both results printed at the end\n");
			cprintf("********************************\n");

			char c26[100] = "schedPRIRR 5 10 9";
			execute_command(c26);
			char c27[100] = "load fib 300 0";
			execute_command(c27);
			char c28[100] = "load fact 300 1";
			execute_command(c28);

			execute_command(run_command);
			break;
	case 7 :
			cprintf("\n[7] Run 2 different processes with same priorities\n");
			cprintf("********************************\n");
			cprintf("TO DO & EXPECTED: with entering any large index the first process will be interrupted,, if too small then no interrupt\n");
			cprintf("********************************\n");

			char c29[100] = "schedPRIRR 5 10 1000";
			execute_command(c29);
			char c30[100] = "load fib 300 0";
			execute_command(c30);
			char c31[100] = "load fact 300 0";
			execute_command(c31);

			execute_command(run_command);
			break;
	default:
		cprintf("\nCongratulations!! test priority 1 completed successfully.\n");
		break;

	}

#else
	if(firstTime)
	{
		uint32 add_WS[20];
		uint32 add_TimeStamp[20];
		uint32 fact_WS[15];
		uint32 fact_TimeStamp[15];
		uint32 hello_WS[10];
		uint32 hello_TimeStamp[10];

		firstTime = 0;
		char command[100] = "load fos_add 20";
		execute_command(command);
		char command2[100] = "load fact 15";
		execute_command(command2);
		char command3[100] = "load fos_helloWorld 10";
		execute_command(command3);

		struct Env * addEnv;
		struct Env * factEnv;
		struct Env * helloEnv;
		envid2env(16, &addEnv, 0);
		envid2env(17, &factEnv, 0);
		envid2env(18, &helloEnv, 0);

		if(addEnv == NULL || factEnv == NULL || helloEnv == NULL)
			panic("Loading programs failed\n");

		if(addEnv->page_WS_max_size != 20 || factEnv->page_WS_max_size != 15 || helloEnv->page_WS_max_size != 10)
			panic("The programs should be initially loaded with the given working set size. add: %d, fact: %d, hello: %d\n", addEnv->page_WS_max_size , factEnv->page_WS_max_size , helloEnv->page_WS_max_size );

		for(int i = 0; i < 20; i++)
		{
			add_WS[i] = addEnv->ptr_pageWorkingSet[i].virtual_address;
			add_TimeStamp[i] = addEnv->ptr_pageWorkingSet[i].time_stamp;
		}

		for(int i = 0; i < 15; i++)
		{
			fact_WS[i] = factEnv->ptr_pageWorkingSet[i].virtual_address;
			fact_TimeStamp[i] = factEnv->ptr_pageWorkingSet[i].time_stamp;
		}

		for(int i = 0; i < 10; i++)
		{
			hello_WS[i] = helloEnv->ptr_pageWorkingSet[i].virtual_address;
			hello_TimeStamp[i] = helloEnv->ptr_pageWorkingSet[i].time_stamp;
		}

		int freeFrames = sys_calculate_free_frames();
		int freeDiskFrames = pf_calculate_free_frames();

		// Set Priority To Normal // Should change nothing
		set_program_priority(addEnv, 3);
		set_program_priority(factEnv, 3);
		set_program_priority(helloEnv, 3);

		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Old working set should be removed properly\n");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Old working set should be removed properly\n");

		if(addEnv->page_WS_max_size != 20 || factEnv->page_WS_max_size != 15 || helloEnv->page_WS_max_size != 10)
			panic("The programs' working set size should not change after setting priority to normal\n");

		freeFrames = sys_calculate_free_frames();
		freeDiskFrames = pf_calculate_free_frames() ;

		// Set Priority To Above Normal // Should change only helloWorld as none of the rest is full
		set_program_priority(addEnv, 4);
		set_program_priority(factEnv, 4);
		set_program_priority(helloEnv, 4);

		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Old working set should be removed properly\n");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Old working set should be removed properly%x\n");

		if(addEnv->page_WS_max_size != 20 || factEnv->page_WS_max_size != 15 || helloEnv->page_WS_max_size != 20)
			panic("The programs' working set size should be doubled only if it is full\n");

		freeFrames = sys_calculate_free_frames();
		freeDiskFrames = pf_calculate_free_frames() ;

		// Set priority to above normal --> nothing should change (1 time only)
		set_program_priority(helloEnv, 4);

		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Old working set should be removed properly\n");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Old working set should be removed properly\n");

		if(addEnv->page_WS_max_size != 20 || factEnv->page_WS_max_size != 15 || helloEnv->page_WS_max_size != 20)
			panic("The programs' working set size should be doubled only once\n");


		// Manipulate WS to seem as full
		for(int i = 10; i < 20; i++)
		{
			helloEnv->ptr_pageWorkingSet[i].empty = 0;
		}

		freeFrames = sys_calculate_free_frames();
		freeDiskFrames = pf_calculate_free_frames() ;

		set_program_priority(helloEnv, 4);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Old working set should be removed properly\n");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Old working set should be removed properly\n");

		if(addEnv->page_WS_max_size != 20 || factEnv->page_WS_max_size != 15 || helloEnv->page_WS_max_size != 20)
			panic("The programs' working set size should be doubled only once\n");

		for(int i = 10; i < 15; i++)
		{
			factEnv->ptr_pageWorkingSet[i].empty = 0;
		}

		freeFrames = sys_calculate_free_frames();
		freeDiskFrames = pf_calculate_free_frames() ;
		set_program_priority(factEnv, 5);
		set_program_priority(helloEnv, 5);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Old working set should be removed properly\n");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Old working set should be removed properly\n");

		if(addEnv->page_WS_max_size != 20 || factEnv->page_WS_max_size != 30 || helloEnv->page_WS_max_size != 40)
			panic("The programs' working set size should be doubled if full\n");

		for(int i = 20; i < 40; i++)
		{
			helloEnv->ptr_pageWorkingSet[i].empty = 0;
		}

		freeFrames = sys_calculate_free_frames();
		freeDiskFrames = pf_calculate_free_frames() ;
		set_program_priority(factEnv, 5);
		set_program_priority(helloEnv, 4);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Old working set should be removed properly\n");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Old working set should be removed properly\n");

		if(addEnv->page_WS_max_size != 20 || factEnv->page_WS_max_size != 30 || helloEnv->page_WS_max_size != 40)
			panic("The programs' working set size should be doubled if full\n");

		for(int i = 15; i < 30; i++)
		{
			factEnv->ptr_pageWorkingSet[i].empty = 0;
		}

		freeFrames = sys_calculate_free_frames();
		freeDiskFrames = pf_calculate_free_frames() ;
		set_program_priority(factEnv, 5);
		set_program_priority(helloEnv, 4);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Old working set should be removed properly\n");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Old working set should be removed properly\n");

		if(addEnv->page_WS_max_size != 20 || factEnv->page_WS_max_size != 60 || helloEnv->page_WS_max_size != 40)
			panic("The programs' working set size should be doubled if full\n");

		for(int i = 15; i < 60; i++)
		{
			factEnv->ptr_pageWorkingSet[i].empty = 1;
		}
		for(int i = 10; i < 40; i++)
		{
			helloEnv->ptr_pageWorkingSet[i].empty = 1;
		}

		for(int i = 0; i < 20; i++)
		{
			if(add_WS[i] != addEnv->ptr_pageWorkingSet[i].virtual_address)
				panic("Working set should be moved properly to the new one");

			if(add_TimeStamp[i] != addEnv->ptr_pageWorkingSet[i].time_stamp)
				panic("Working set should be moved properly to the new one");
		}

		for(int i = 0; i < 15; i++)
		{
			if(fact_WS[i] != factEnv->ptr_pageWorkingSet[i].virtual_address)
				panic("Working set should be moved properly to the new one");

			if(fact_TimeStamp[i] != factEnv->ptr_pageWorkingSet[i].time_stamp)
				panic("Working set should be moved properly to the new one");
		}

		for(int i = 0; i < 10; i++)
		{
			if(hello_WS[i] != helloEnv->ptr_pageWorkingSet[i].virtual_address)
				panic("Working set should be moved properly to the new one");

			if(hello_TimeStamp[i] != helloEnv->ptr_pageWorkingSet[i].time_stamp)
				panic("Working set should be moved properly to the new one");
		}

		char command4[100] = "runall";
		execute_command(command4);
	}
	else
	{
		cprintf("\nCongratulations!! test priority 1 completed successfully.\n");
	}
#endif
}

void test_priority_normal_and_lower()
{
#if USE_KHEAP
	panic("not handled yet");
#else
	if(firstTime)
	{
		uint32 add_WS[20];
		uint32 add_TimeStamp[20];
		uint32 fact_WS[30];
		uint32 fact_TimeStamp[30];
		uint32 hello_WS[40];
		uint32 hello_TimeStamp[40];

		firstTime = 0;
		char command[100] = "load fos_add 20";
		execute_command(command);
		char command2[100] = "load fact 30";
		execute_command(command2);
		char command3[100] = "load fos_helloWorld 40";
		execute_command(command3);

		struct Env * addEnv;
		struct Env * factEnv;
		struct Env * helloEnv;
		envid2env(4096, &addEnv, 0);
		envid2env(4097, &factEnv, 0);
		envid2env(4098, &helloEnv, 0);

		if(addEnv == NULL || factEnv == NULL || helloEnv == NULL)
			panic("Loading programs failed\n");

		if(addEnv->page_WS_max_size != 20 || factEnv->page_WS_max_size != 30 || helloEnv->page_WS_max_size != 40)
			panic("The programs should be initially loaded with the given working set size\n");

		for(int i = 0; i < 20; i++)
		{
			add_WS[i] = addEnv->ptr_pageWorkingSet[i].virtual_address;
			add_TimeStamp[i] = addEnv->ptr_pageWorkingSet[i].time_stamp;
		}

		for(int i = 0; i < 30; i++)
		{
			fact_WS[i] = factEnv->ptr_pageWorkingSet[i].virtual_address;
			fact_TimeStamp[i] = factEnv->ptr_pageWorkingSet[i].time_stamp;
		}

		for(int i = 0; i < 40; i++)
		{
			hello_WS[i] = helloEnv->ptr_pageWorkingSet[i].virtual_address;
			hello_TimeStamp[i] = helloEnv->ptr_pageWorkingSet[i].time_stamp;
		}

		int freeFrames = sys_calculate_free_frames();
		int freeDiskFrames = pf_calculate_free_frames() ;

		// Set Priority To Normal // Should change nothing
		set_program_priority(addEnv, 3);
		set_program_priority(factEnv, 3);
		set_program_priority(helloEnv, 3);

		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Old working set should be removed properly\n");
		if ((freeFrames - sys_calculate_free_frames()) != 0) panic("Old working set should be removed properly\n");

		if(addEnv->page_WS_max_size != 20 || factEnv->page_WS_max_size != 30 || helloEnv->page_WS_max_size != 40)
			panic("The programs' working set size should not change after setting priority to normal\n");

		freeFrames = sys_calculate_free_frames();
		freeDiskFrames = pf_calculate_free_frames() ;

		// Set Priority To Below Normal // Should change only if half is free
		set_program_priority(addEnv, 2);
		set_program_priority(factEnv, 2);
		set_program_priority(helloEnv, 2);

		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Old working set should be removed properly\n");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Old working set should be removed properly%x\n", (freeFrames - sys_calculate_free_frames()));

		if(addEnv->page_WS_max_size != 10 || factEnv->page_WS_max_size != 15 || helloEnv->page_WS_max_size != 20)
			panic("The programs' working set size should be doubled only if it is full\n");

		freeFrames = sys_calculate_free_frames();
		freeDiskFrames = pf_calculate_free_frames() ;

		// Set Priority To Below Normal // Should change only if half is free
		set_program_priority(addEnv, 2);
		set_program_priority(factEnv, 2);
		set_program_priority(helloEnv, 2);

		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Old working set should be removed properly\n");
		if ((sys_calculate_free_frames() - freeFrames) != 0) panic("Old working set should be removed properly%x\n", (freeFrames - sys_calculate_free_frames()));

		if(addEnv->page_WS_max_size != 10 || factEnv->page_WS_max_size != 15 || helloEnv->page_WS_max_size != 10)
			panic("The programs' working set size should be doubled only if it is full\n");

		for(int i = 0; i < 10; i++)
		{
			if(add_WS[i] != addEnv->ptr_pageWorkingSet[i].virtual_address)
				panic("Working set should be moved properly to the new one");

			if(add_TimeStamp[i] != addEnv->ptr_pageWorkingSet[i].time_stamp)
				panic("Working set should be moved properly to the new one");
		}

		for(int i = 0; i < 15; i++)
		{
			if(fact_WS[i] != factEnv->ptr_pageWorkingSet[i].virtual_address)
				panic("Working set should be moved properly to the new one");

			if(fact_TimeStamp[i] != factEnv->ptr_pageWorkingSet[i].time_stamp)
				panic("Working set should be moved properly to the new one");
		}

		for(int i = 0; i < 10; i++)
		{
			if(hello_WS[i] != helloEnv->ptr_pageWorkingSet[i].virtual_address)
				panic("Working set should be moved properly to the new one");

			if(hello_TimeStamp[i] != helloEnv->ptr_pageWorkingSet[i].time_stamp)
				panic("Working set should be moved properly to the new one");
		}

		// Set priority to low
		freeFrames = sys_calculate_free_frames();
		freeDiskFrames = pf_calculate_free_frames() ;
		set_program_priority(addEnv, 1);
		set_program_priority(factEnv, 1);
		set_program_priority(helloEnv, 1);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Old working set should be removed properly\n");
		if ((sys_calculate_free_frames() - freeFrames) != (5+3+5)) panic("Old working set and extra pages in WS should be removed properly %d\n");

		if(addEnv->page_WS_max_size != 5 || factEnv->page_WS_max_size != 7 || helloEnv->page_WS_max_size != 5)
			panic("The programs' working set size should be half\n");


		freeFrames = sys_calculate_free_frames();
		freeDiskFrames = pf_calculate_free_frames() ;
		set_program_priority(addEnv, 1);
		if ((pf_calculate_free_frames() - freeDiskFrames) != 0) panic("Old working set should be removed properly\n");
		if ((sys_calculate_free_frames() - freeFrames) != 3) panic("Old working set should be removed properly\n");

		if(addEnv->page_WS_max_size != 2 || factEnv->page_WS_max_size != 7 || helloEnv->page_WS_max_size != 5)
			panic("The programs' working set size should be doubled if full\n");

		for(int i = 0; i < 2; i++)
		{
			if(add_WS[i+8] != addEnv->ptr_pageWorkingSet[i].virtual_address)
				panic("Working set should be moved properly to the new one, removed pages are chosen based on replacement policy");

			if(add_TimeStamp[i+8] != addEnv->ptr_pageWorkingSet[i].time_stamp)
				panic("Working set should be moved properly to the new one, removed pages are chosen based on replacement policy");
		}

		for(int i = 0; i < 7; i++)
		{
			if(fact_WS[i+3] != factEnv->ptr_pageWorkingSet[i].virtual_address)
				panic("%d Working set should be moved properly to the new one, removed pages are chosen based on replacement policy", i);

			if(fact_TimeStamp[i+3] != factEnv->ptr_pageWorkingSet[i].time_stamp)
				panic("Working set should be moved properly to the new one, removed pages are chosen based on replacement policy");
		}

		for(int i = 0; i < 5; i++)
		{
			if(hello_WS[i+5] != helloEnv->ptr_pageWorkingSet[i].virtual_address)
				panic("Working set should be moved properly to the new one, removed pages are chosen based on replacement policy");

			if(hello_TimeStamp[i+5] != helloEnv->ptr_pageWorkingSet[i].time_stamp)
				panic("Working set should be moved properly to the new one, removed pages are chosen based on replacement policy");
		}

		set_program_priority(addEnv, 5);
		set_program_priority(addEnv, 5);

		char command4[100] = "runall";
		execute_command(command4);
	}
	else
	{
		cprintf("\nCongratulations!! test priority 2 completed successfully.\n");
	}
#endif
}

// Edited by Matt Wallace 
// mbw5861
// agm1564
// UL Lafayette CMPS 455 Fall 2012


// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.



#include "copyright.h"
#include "system.h"
#include "synch.h"
#include <ctype.h>

void TaskFourEntrance();
void TaskFiveEntrance();
void TaskSixEntrance();

void Philosopher(int ThreadID);

// GLOBAL VALUES
// -----------------------------------------------------------------
int NumThreads; // number of threads to create for task 1

int Philosophers=0; // number of philosophers that are dining
int EatAmount=0; // number of total times the philosophers may collectively eat
int InitialEats=0;
int SittingPhilosophers=0; // number of current philosophers at the table
int CompletePhilosophers=0; // number of philosophers who have completed execution

Semaphore ** Chopsticks; // will eventually be a dynamic array of chopstick semaphores
Semaphore DeleteTime("", 1); // guards the deletion for the chopstick array

bool EatTime = false; // guards the start of execution of philosopher threads until everyone is seated

bool *ChopstickFlags; // will be used as a dynamic boolean array to hold the chopstick flags

// ----------------------------------------------------------------

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void ThreadTest()
{
	switch(TaskNumber)
	{
		
		case 4:
			TaskFourEntrance();
			break;
			
		case 5:
			TaskFiveEntrance();
			break;
			
		default:
			printf("No tasks to perform, exiting...\n");
	}
}


void TaskFourEntrance()
{
	printf("Task Four\n");
	
	char* NumPhilsInput = new char[256]; // used to store number of philosophers 
	char* EatAmountInput = new char[256]; // used to store amount of food there is
	char* ThreadDebugString = new char[32]; // stores the debug string
	char TestChar;
	int InputResult;
	
	printf("How many philosophers will be dining tonight? (integer from 1-10): ");
	fgets(NumPhilsInput, 256, stdin);
	InputResult = sscanf(NumPhilsInput, "%d%c", &Philosophers, &TestChar);
	
	while ( InputResult != 2 || TestChar != '\n' || Philosophers < 1 || Philosophers > 10)
	{
		printf("Invalid input! Try again.\n");
	    printf("How many philosophers will be dining tonight? (integer from 1-10): ");
	    fgets(NumPhilsInput, 256, stdin);
	    InputResult = sscanf(NumPhilsInput, "%d%c", &Philosophers, &TestChar);
		
	}
	
	printf("How much food will there be? (integer from 1-10): ");
	fgets(EatAmountInput, 256, stdin);
	InputResult = sscanf(EatAmountInput, "%d%c", &EatAmount, &TestChar);
	
	while ( InputResult != 2 || TestChar != '\n' || EatAmount < 1 || EatAmount > 10)
	{
		printf("Invalid input! Try again.\n");
		printf("How much food will there be? (integer from 1-10): ");
		fgets(EatAmountInput, 256, stdin);
		InputResult = sscanf(EatAmountInput, "%d%c", &EatAmount, &TestChar);
	}
	
	InitialEats=EatAmount;
	// at this point, Philosophers and EatAmount hold correct values
	
	// init the Chopsticks array to be the same size as the amount of philosophers
	
	// if there's only one philosopher, make the chopstick array size 2, instead of size 1.
	if(Philosophers==1)
	{
		Chopsticks = new Semaphore* [2];
		Chopsticks[0] = new Semaphore("", 1);
		Chopsticks[1] = new Semaphore("", 1);
	
	}
	
	else
	{
		Chopsticks = new Semaphore* [Philosophers];
		
		for(int i =0;i<Philosophers;i++)
		{
			Chopsticks[i] = new Semaphore("", 1);
		}
	}
	
	DeleteTime.P();
	
	// forking
	
	for(int i=0;i<Philosophers;i++)
	{
		sprintf(ThreadDebugString, "Forked thread %d", i);
		Thread* T = new Thread(ThreadDebugString);
		T->Fork(Philosopher, i);
	}
	
	// busy-wait until every philosopher is at the table, then set eattime to true, which lets the philosophers continue with execution
	
	while(SittingPhilosophers < Philosophers)
	{
		currentThread->Yield();
	}
	
	EatTime = true;
	
	printf("Everyone's here, let's eat!\n");
	
	bool sentinel = true;
	
	// after forking, initial thread will sit in this loop until DeleteTime is freed up at the moment all philosophers
	// are finished eating. at that moment, the chopsticks dynamic array can be deleted
	// until then, it CANNOT be deleted or memory will be all kinds of fucked up.
	
	while (sentinel)
	{
		DeleteTime.P();
		delete [] Chopsticks;
		sentinel = false;
		DeleteTime.V();
	}
	
	
}
	
void TaskFiveEntrance()
{
	printf("Task Five\n");
	
	char* NumPhilsInput = new char[256]; // used to store number of philosphers 
	char* EatAmountInput = new char[256]; // used to store amount of food there is
	char* ThreadDebugString = new char[32]; // stores the debug string
	char TestChar;
	int InputResult;
	
	printf("How many philosophers will be dining tonight? (integer from 1-10): ");
	fgets(NumPhilsInput, 256, stdin);
	InputResult = sscanf(NumPhilsInput, "%d%c", &Philosophers, &TestChar);
	
	while ( InputResult != 2 || TestChar != '\n' || Philosophers < 1 || Philosophers > 10)
	{
		printf("Invalid input! Try again.\n");
	    printf("How many philosophers will be dining tonight? (integer from 1-10): ");
	    fgets(NumPhilsInput, 256, stdin);
	    InputResult = sscanf(NumPhilsInput, "%d%c", &Philosophers, &TestChar);
	}
	
	printf("How much food will there be? (integer from 1-10): ");
	fgets(EatAmountInput, 256, stdin);
	InputResult = sscanf(EatAmountInput, "%d%c", &EatAmount, &TestChar);
	
	
	while ( InputResult != 2 || TestChar != '\n' || EatAmount < 1 || EatAmount > 10)
	{
		printf("Invalid input! Try again.\n");
		printf("How much food will there be? (integer from 1-10): ");
		fgets(EatAmountInput, 256, stdin);
		InputResult = sscanf(EatAmountInput, "%d%c", &EatAmount, &TestChar);
	}
	// at this point, Philosophers and EatAmount hold correct values
	
	InitialEats=EatAmount;
	
	// init the Chopsticks flag array to be the same size as the amount of philosophers
	
	
	if(Philosophers == 1)
	{
		ChopstickFlags = new bool[2];
	}
	
	ChopstickFlags = new bool[Philosophers];
	
	
	DeleteTime.P();
	
	// forking
	
	for(int i=0;i<Philosophers;i++)
	{
		sprintf(ThreadDebugString, "Forked thread %d", i);
		Thread* T = new Thread(ThreadDebugString);
		T->Fork(PhilosopherBusyWait, i);
	}
	
	// busy-wait until every philosopher is at the table, then set eattime to true, which lets the philosophers continue with execution
	
	while(SittingPhilosophers < Philosophers)
	{
		currentThread->Yield();
	}
	
	EatTime = true;
	
	printf("Everyone's here, let's eat!\n");
	
	
	
	
}

void Philosopher(int ThreadID)
{
	// increase the amount of philosophers currently at the table
	SittingPhilosophers++;
	printf("Philosopher %d says: 'What's up dudes?'\n", ThreadID+1);
	// until every philosopher is sitting, wait
	while(EatTime == false)
	{
		currentThread->Yield();
	}
	
	while(EatAmount > 0)
	{
		// every time a philosopher goes through the loop, init the random times it will eat and think, 2-5 inclusive
		int RandomEat = ( Random() % 4 ) + 2;
		int RandomThink = ( Random() % 4 ) + 2;
		
		// pick up the chopsticks
		
		Chopsticks[ThreadID]->P();
		printf("Philosopher %d picked up his left chopstick.\n", ThreadID+1);
		
		
		if(Philosophers==1)
		{
			Chopsticks[ 1 ]->P();
		}
		else
		{
			Chopsticks[ (ThreadID+1) % Philosophers ]->P();
		}
		
		printf("Philosopher %d picked up his right chopstick.\n", ThreadID+1);
		
		
		// if the philosopher can eat, subtract one from the food pile and one from the amount of times the philosopher should eat
		
		if(EatAmount > 0)
		{
			EatAmount--;
				
			printf("Philosopher %d is nomming away.\n", ThreadID+1);
			
			printf("%d food has been eaten so far.\n", InitialEats-EatAmount);
			
			while(RandomEat > 0)
			{
				RandomEat--;
				currentThread->Yield();
			}
		}
		
		else
		{
			printf("There's no more food, so philosopher %d just sat there and wallowed in self-pity.\n", ThreadID+1);
			
			currentThread->Yield();
		}
		
		// put down chopsticks
		Chopsticks[ThreadID]->V();
		printf("Philosopher %d put down his left chopstick.\n", ThreadID+1);
		
		
		if(Philosophers==1)
		{
			Chopsticks[ 1 ]->V();
		}
		else
		{
			Chopsticks[ (ThreadID+1) % Philosophers ]->V();
		}
		
		printf("Philosopher %d put down his right chopstick.\n", ThreadID+1);
		//think, busy wait loop
		
		printf("Philosopher %d started thinking about ponies and rainbows.\n", ThreadID+1);
		
		while( RandomThink > 0 )
		{
			RandomThink--;
			
			currentThread->Yield();
		}
	}
	
	CompletePhilosophers++;
	
	while(CompletePhilosophers < Philosophers)
	{
		currentThread->Yield();
	}
	
	printf("Philosopher %d has left the building.\n", ThreadID+1);
	
	//enable deletion of chopstick array
	DeleteTime.V();
}	


void PhilosopherBusyWait(int ThreadID)
{
	// increase the amount of philosophers currently at the table
	SittingPhilosophers++;
	printf("Philosopher %d says: 'What's up dudes?'\n", ThreadID+1);
	// until every philosopher is sitting, wait
	while(EatTime == false)
	{
		currentThread->Yield();
	}
	
	while(EatAmount > 0)
	{
		// every time a philosopher goes through the loop, init the random times it will eat and think, 2-5 inclusive
		int RandomEat = ( Random() % 4 ) + 2;
		int RandomThink = ( Random() % 4 ) + 2;
		
		// pick up the chopsticks
		
		while(ChopstickFlags[ThreadID] == true)
		{
			currentThread->Yield();
		}
		
		ChopstickFlags[ThreadID] = true;
		printf("Philosopher %d picked up his left chopstick.\n", ThreadID+1);
		
		currentThread->Yield();
		if(Philosophers == 1)
		{
			while(ChopstickFlags[ 1 ] == true)
			{
				currentThread->Yield();
			}
		
		}
		else
		{
			while(ChopstickFlags[ (ThreadID+1) % Philosophers] == true)
			{
				currentThread->Yield();
			}
		}
		
		ChopstickFlags[ThreadID+1] = true;
		
		
		printf("Philosopher %d picked up his right chopstick.\n", ThreadID+1);
		
		
		
		// if the philosopher can eat, subtract one from the food pile and one from the amount of times the philosopher should eat
		
		if(EatAmount > 0)
		{
			EatAmount--;
				
			printf("Philosopher %d is nomming away.\n", ThreadID+1);
			printf("%d food has been eaten so far.\n", InitialEats-EatAmount);
			
			while(RandomEat > 0)
			{
				RandomEat--;
				
				currentThread->Yield();
			}
		}
		
		else
		{
			printf("There's no more food, so philosopher %d just sat there and wallowed in self-pity.\n", ThreadID+1);
			
			currentThread->Yield();
		}
		
		// put down chopsticks
		ChopstickFlags[ThreadID] = false;
		printf("Philosopher %d put down his left chopstick.\n", ThreadID+1);
		
		currentThread->Yield();
		
		ChopstickFlags[ThreadID+1] = false;
		printf("Philosopher %d put down his right chopstick.\n", ThreadID+1);
		//think, busy wait loop
		
		printf("Philosopher %d started thinking about ponies and rainbows.\n", ThreadID+1);
		
		while( RandomThink > 0 )
		{
			RandomThink--;
			
			currentThread->Yield();
		}
	}
	
	CompletePhilosophers++;
	
	while(CompletePhilosophers < Philosophers)
	{
		currentThread->Yield();
	}
	
	printf("Philosopher %d has left the building.\n", ThreadID+1);
	
}	
	states[ThreadID] = 0;
	
	readyRockers++;
	
	while( readyRockers < numRockers)
	{
		
		currentThread->Yield();
	}
	
	printf("Rocker %d has entered the building!\n", ThreadID);
	
	rockersIn++;
	
	while (readyToStart == false)
	{
		currentThread->Yield();	
	}
	
	
	while (songsPlayed < numSongs)
	{
		readyRestartRockers++;
		
		states[ThreadID] = 0;
		
		while(readyRestartRockers < numRockers)
		{
			
			currentThread->Yield();
		}
		
		int decision = Random() % 2;
		
		if( decision == 0)
		{
			states[ThreadID] = 2;
		}
		else
		{
			states[ThreadID] = 3;		
		}
		
		int choice;
		
		if(states[ThreadID] == 2)
		{	
			semInstr.P();
			choice = Random() % numInstruments;
			if(instruments[choice] == false)
			{
				instruments[choice] = true;				
				printf("Rocker %d has chosen instrument %d!\n", ThreadID, choice);
				states[ThreadID] = 1;
			}
			else
			{
				states[ThreadID] = 3;
			}
			semInstr.V();
		}
		
		if(states[ThreadID] == 3)
			spectators++;
		
		if(states[ThreadID] == 1)
		{
			players++;	
		}
		
		
		
		while((players + spectators) < numRockers)
		{
			
			currentThread->Yield();		
		}
		
		rockersReadyToRock++;
		
		while(readyToRock == false)
		{
			currentThread->Yield();		
		}
		
		while(playTime > 0)
		{
			
			playTime--;
			currentThread->Yield();
		}
		
		if(states[ThreadID] == 1)
		{
			instruments[choice] = false;
			printf("***Rocker %d has dropped instrument %d\n", ThreadID, choice);
			currentThread->Yield();
			states[ThreadID] = 0;
		}
		finishedPlayingRockers++;
		
	}

	finishedRockers++;
		
	while(finishedRockers < numRockers)
	{
		currentThread->Yield();	
	}
	
	printf("Rocker %d has left the building!\n", ThreadID);
	
	rockersLeft++;

}

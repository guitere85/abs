/*********************************************************************
*  File: kernel.h
*  Project: Assignment #2 for ENEL323
*  Brief: Header File for kernel.c
*  Authors: Paul Bowler & Aydin Arik
*  Last Modified: June 2011
**********************************************************************/

#ifndef KERNEL_H_
#define KERNEL_H_

/*---------------------------
   Header file inclusions
----------------------------*/
#include "tasks.h"

/*---------------------------
	Public type definitions
----------------------------*/

/*---------------------------
	Enumerations
----------------------------*/
enum functionFlags {INACTIVE = 0, ACTIVE, DISABLED};	//Possible states of the flags
enum background_tasks {AVERAGEPERIOD = 0, UPDATEDISPLAY, CHECKWHEELLOCKTASK, CLEARBUFFERTASK, MONITORWHEELS, MONITORPEDALTASK, BRAKETASK, AVERAGERATE}; //Index of background tasks in the function registry

/*---------------------------
	Public constants
----------------------------*/
#define NUM_TASKS 8		//Number of tasks or functions contained in the function registry
#define uint8_t unsigned char	//Data type for flags

/*---------------------------
	Public varaibles
----------------------------*/
/*n/a*/

/*---------------------------
  Public function prototypes
----------------------------*/

//*-------------------------------------------------------------
//*	Function name:		initialiseKernel
//*	Object:				initialises the flagRegistry array for 
//*						first start-up of ABS.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void initialiseKernel (void);

//*-------------------------------------------------------------
//*	Function name:		setFlag
//*	Object:				allows task control within the program. Tasks should
//*						only be made active as required.				
//*	Input Parameters:	flagToSet - the task flag of interest 
//*						flagState - what to set the task flag of interest
//*	Output Parameters:	none
//*-------------------------------------------------------------
void setFlag (uint8_t flagToSet, uint8_t flagState);

//*-------------------------------------------------------------
//*	Function name:		runKernel
//*	Object:				operates the system kernel in a round-robin fashion.
//*						Each function is called if it is set as active.
//*						This function sets a tasks inactive after the active 
//*						task is completely executed.				
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void runKernel(void);

#endif /*KERNEL_H_*/

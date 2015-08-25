/*********************************************************************
*  File: kernel.c
*  Project: Assignment #2 for ENEL323
*  Brief: The RTOS (Kernal) for the ABS
*  Authors: Paul Bowler & Aydin Arik
*  Last Modified: June 2011
**********************************************************************/

/*---------------------------
   Header file inclusions
----------------------------*/
#include "kernel.h"

/*---------------------------
    Private constants
----------------------------*/
/*n/a*/

/*---------------------------
  Private function prototypes
----------------------------*/
/*n/a*/



/*---------------------------
  Kernel Function Registry
----------------------------*/

/*Initialise function registry for tasks */
static void (*functionRegistry[NUM_TASKS])(void) = {
	averagePeriod, 
	UpdateDisplay,
	checkWheelLockTask,
	clearBufferTask, 
	monitorWheels, 
	monitorPedalTask, 
	brakeTask,
	averageRate
};
	
/*---------------------------
  Kernel Flag Registry
----------------------------*/

/*Initialise function flag array for tasks */
static uint8_t flagRegistry[NUM_TASKS];	


//*-------------------------------------------------------------
//*	Function name:		initialiseKernel
//*	Object:				initialises the flagRegistry array for 
//*						first start-up of ABS.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void initialiseKernel (void)
{
	uint8_t i;
	
	for (i = 0; i < NUM_TASKS; i++)
	{
		flagRegistry[i] = INACTIVE;
	}
}


//*-------------------------------------------------------------
//*	Function name:		setFlag
//*	Object:				allows task control within the program. Tasks should
//*						only be made active as required.				
//*	Input Parameters:	flagToSet - the task flag of interest 
//*						flagState - what to set the task flag of interest
//*	Output Parameters:	none
//*-------------------------------------------------------------
void setFlag (uint8_t flagToSet, uint8_t flagState)
{
	flagRegistry[flagToSet] = flagState;
}



//*-------------------------------------------------------------
//*	Function name:		runKernel
//*	Object:				operates the system kernel in a round-robin fashion.
//*						Each function is called if it is set as active.
//*						This function sets a tasks inactive after the active 
//*						task is completely executed.				
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void runKernel(void)
{
	uint8_t i;
	
	while (1)
	{
		/* cycle through each task and check if any are active.   */
		for (i = 0; i < NUM_TASKS; i++) 
		{
			if ((flagRegistry[i] == ACTIVE)) /* If active, run the task. Else, continue onto the next task */
			{
				(*functionRegistry[i])();
				
				flagRegistry[i] = INACTIVE; /* Deactivate task once completely executed */
			}
		}
	}
}



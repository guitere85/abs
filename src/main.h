/*********************************************************************
*  File: main.h
*  Project: Assignment #2 for ENEL323
*  Brief: Header file for main.c
*  Authors: Paul Bowler & Aydin Arik
*  Last Modified: June 2011
**********************************************************************/

#ifndef MAIN_H_
#define MAIN_H_

/*---------------------------
  	Global variables
----------------------------*/
extern volatile uint32_t left_tick, right_tick, left_curr, right_curr, 
							left_prev, right_prev, left_period, right_period;
extern volatile uint32_t left_timeout, right_timeout;
extern int PWMState;


/*---------------------------
  Public function prototypes
----------------------------*/

//*-------------------------------------------------------------
//*	Function name:		Timer0AIntHandler
//*	Object:				Required to correctly measure wheel periods when
//*						Timer 0A resets.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void Timer0AIntHandler(void);


//*-------------------------------------------------------------
//*	Function name:		PortBIntHandler
//*	Object:				Measures wheel periods.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void PortBIntHandler(void);

//*-------------------------------------------------------------
//*	Function name:		SysTickISR
//*	Object:				Allows for ADC sampling at 50/sec and increments a
//*						variable used to determine wheel lockup.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void SysTickISR(void);

#endif /*MAIN_H_*/

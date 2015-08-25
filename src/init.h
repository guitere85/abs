/*********************************************************************
*  File: init.h
*  Project: Assignment #2 for ENEL323
*  Brief: Header file for init.c
*  Authors: Paul Bowler & Aydin Arik
*  Last Modified: June 2011
**********************************************************************/

#ifndef INIT_H_
#define INIT_H_

/*---------------------------
   Header file inclusions
----------------------------*/

#include "utils/ustdlib.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_ints.h"
//#include "inc/lm3s1968.h"  // for the "xxx_R" port and register names
#include "inc/hw_types.h" // for using the HWREG() macro
//#include "inc/hw_nvic.h"  // for all the register enables etc.
#include "drivers/rit128x96x4.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"

/*---------------------------
  Public function prototypes
----------------------------*/

/*---------------------------
	Public constants
----------------------------*/
#define PWM_FREQ 5 //Frequency of sound emitted from speaker

//*-------------------------------------------------------------
//*	Function name:		initTimer0AInterupt
//*	Object:				initialises the Timer 0A interupt routines.
//*						An interupt is set to occur when the timer
//*						times out (reaches zero).
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void initTimer0AInterupt(void);

//*-------------------------------------------------------------
//*	Function name:		initPortBInterupt
//*	Object:				initialises the Port B interrupt routines. Specifically,
//*						interupts will be limited to PB0 and PB1 (wheel encoders).
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void initPortBInterupt(void);



//*-------------------------------------------------------------
//*	Function name:		initADC
//*	Object:				initialise the ADC routine. Required to get
//*						brake pedal position information.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void initADC(void);

	
//*-------------------------------------------------------------
//*	Function name:		initPWM
//*	Object:				initialise the PWM routine. Required to 
//*						generate braking.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------	 	
void initPWM(void);

#endif /*INIT_H_*/

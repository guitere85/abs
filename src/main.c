/*********************************************************************
*  File: main.c
*  Project: Assignment #2 for ENEL323
*  Brief: Main program entry point => includes ISRs.
*  Authors: Paul Bowler & Aydin Arik
*  Last Modified: June 2011
**********************************************************************/

/*---------------------------
	Public Constants
----------------------------*/
#define TOT_PERIOD 65536


/*---------------------------
   Header file inclusions
----------------------------*/
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h" // for using the HWREG() macro
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/systick.h"
#include "driverlib/adc.h"

/* Temporarily excluded header files */
//#include "inc/hw_timer.h"
//#include "inc/lm3s1968.h"  // for the "xxx_R" port and register names
//#include "inc/hw_nvic.h"  // for all the register enables etc.
//#include "driverlib/sysctl.h"
//#include "driverlib/pwm.h"

/* Include our own relevant header files */
#include "dcb.h"
#include "kernel.h"
#include "init.h"
#include "tasks.h"
#include "main.h"


/*---------------------------
  Global (to file) ISR variables
----------------------------*/
volatile uint32_t left_tick = 0, right_tick = 0, left_curr = 0, right_curr = 0, 
							left_prev = 0, right_prev = 0, left_period = 0, right_period = 0;
volatile uint32_t left_timeout = 0, right_timeout = 0;
int PWMState = OFF;

//*****************************************************************************
// The error routine that is called if the driver library encounters an error.
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif


//*-------------------------------------------------------------
//*	Function name:		SysTickISR
//*	Object:				Allows for ADC sampling at 50/sec and increments a
//*						variable used to determine wheel lockup.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void SysTickISR(void)
{ 
	
	/* increment both wheel 'timeout' counters (required for detecting wheel lockups) */
	left_timeout ++;
	right_timeout ++;
	
	/* Incrememnt the ABS counter tick */
	ABStick++;
		
	/* request an ADC conversion */
	ADCProcessorTrigger (ADC0_BASE, 3);	
	
	/* enabling a task to check whether there is are any wheel lockups */
	setFlag(CHECKWHEELLOCKTASK, ACTIVE);
	
	/*setting a flag for brake task to run*/
	setFlag(BRAKETASK, ACTIVE);	
	
	return;
}

//*-------------------------------------------------------------
//*	Function name:		Timer0AIntHandler
//*	Object:				Required to correctly measure wheel periods when
//*						Timer 0A resets.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void Timer0AIntHandler(void)
{
	 /* resetting a status flag so that a correct timer interrupt always occurs */
	 TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
     
     /* increment the tick counters only if the corresponding wheel ISN'T locked */
     if ((wheelMode != LEFT) && (wheelMode != BOTH))
     	left_tick++; 
     if ((wheelMode != RIGHT) && (wheelMode != BOTH))	    
     	right_tick++;
    
     return;
}

//*-------------------------------------------------------------
//*	Function name:		PortBIntHandler
//*	Object:				Measures wheel periods.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void PortBIntHandler(void)
{
	/* Gather the interrupt status from the input */
	long intr = GPIOPinIntStatus(GPIO_PORTB_BASE, 0);
	
	/* resetting a status flag so that a correct pin interrupt always occurs */
	GPIOPinIntClear (GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	
	/* if the interrupt is a result of PB0 OR both PB0 & PB1 */		
	if ((intr == 1) || (intr == 3))
    {
    	/* read the timer value */
    	left_curr = TimerValueGet(TIMER0_BASE, TIMER_A);
    	
    	/* calculate the period */
    	if (left_tick > 0) /* has there been a timer reset */
    	{
    		left_period = (TOT_PERIOD - left_curr) + left_prev + ((left_tick - 1) * TOT_PERIOD);
    	}
    	else /* no time reset */
    	{  
    		if (left_prev > left_curr)  	
    			left_period = left_prev - left_curr;
    		else 
    			left_period = 0;
    	}
    	
    	/* reset values */
    	left_tick = 0; /* required to correctly calculate the period for the next pin interupt */
    	left_timeout = 0; /* reset the wheel timeout timer for correct determination of wheel lockup */     	   	
    	
    	left_prev = left_curr; /* required to correctly calculate the period for the next pin interupt */
    	
    	/* write the period calculated into a buffer. Trigger the averagePeriod Task when buffer becomes full */
    	setFlag (AVERAGEPERIOD, DCB_write (left_period, &leftWheelBuff));
    	
    	
    }	
    /* if the interrupt is a result of PB1 OR both PB0 & PB1 */
    if ((intr == 2) || (intr == 3))
    {
    	/* read the timer value */
    	right_curr = TimerValueGet(TIMER0_BASE, TIMER_A);
    	
    	/* calculate the period */
    	if (right_tick > 0) /* has there been a timer reset */
    	{
    		right_period = (TOT_PERIOD - right_curr) + right_prev + ((right_tick - 1) * TOT_PERIOD) ;
    	}
    	else /* no timer reset */
    	{    	
    		if (right_prev > right_curr)
    			right_period = right_prev - right_curr;
    		else 
    			right_period = 0;
    	}
    	
    	right_tick = 0; /* required to correctly calculate the period for the next pin interupt */    
    	right_timeout = 0; /* reset the wheel timeout timer for correct determination of wheel lockup */
    		   	
    	right_prev = right_curr; /* required to correctly calculate the period for the next pin interupt */
    	
    	/* write the period calculated into a buffer. Trigger the averagePeriod Task when buffer becomes full */
    	setFlag (AVERAGEPERIOD, DCB_write (right_period, &rightWheelBuff));
    }

   return;
}

//*-------------------------------------------------------------
//*	Function name:		main
//*	Object:				Program entry point.
//*	Input Parameters:	none
//*	Output Parameters:	an int
//*-------------------------------------------------------------
int main(void)
{
	/**************************************
	 * 		Initialisations				  *
	 * ***********************************/
	
	/* Initialise a double circular buffer for the wheels */  
	DCB_init(&leftWheelBuff);
	DCB_init(&rightWheelBuff);
	 
  	/*Initialise system clock*/
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);	
        
    /* initialise interrupts */
    initTimer0AInterupt();
    initPortBInterupt();    
    IntMasterEnable ();
    
    /* initialise the OLED display */
	RIT128x96x4Init(1000000);
	
	/*initialise the kernel */
	initialiseKernel ();
	
	/*initialise the ADC */
	initADC();
	
	/*initialise the PWM */
	initPWM();
	
	/* initialise the SysTick interupt (required to be initialised after ADC) */
    SysTickPeriodSet (SysCtlClockGet()/ADC_FREQUENCY);
    SysTickEnable();
    SysTickIntEnable();
    
    /* initialise global varibles */
    brakeMode = NOBRAKE;
    ABStick = 0;
    
    /*initialise pin for oscilloscope test purposes */
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
   
   	/* Setting the direction and mode of PB0 and PB1 (both are direction mode in and rising edge) */
   	GPIODirModeSet (GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_DIR_MODE_OUT);
    
    /*run the kernel*/
	runKernel (); 	
}

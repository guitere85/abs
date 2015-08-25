/*********************************************************************
*  File: init.c
*  Project: Assignment #2 for ENEL323
*  Brief: Initialisation of interupts and peripherals
*  Authors: Paul Bowler & Aydin Arik
*  Last Modified: June 2011
**********************************************************************/

/*---------------------------
   Header file inclusions
----------------------------*/
#include "init.h"
#include "tasks.h"

/*---------------------------
  Public function prototypes
----------------------------*/

//*-------------------------------------------------------------
//*	Function name:		initTimer0AInterupt
//*	Object:				initialises the Timer 0A interupt routines.
//*						An interupt is set to occur when the timer
//*						times out (reaches zero).
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void initTimer0AInterupt(void)
{
   /* Initialise a 16-bit timer */
   SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    
   TimerDisable(TIMER0_BASE,TIMER_A);    /* disabling Timer 0A so that it can be configured */ 
   TimerConfigure(TIMER0_BASE, TIMER_CFG_16_BIT_PAIR | TIMER_CFG_A_PERIODIC); /* configuring Timer 0A to periodic mode */
   
   /* setting up, and enabling, a timer interupt for timeout mode */   
   TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT); 
   IntEnable(INT_TIMER0A);
   TimerEnable(TIMER0_BASE,TIMER_A);
   
   /* setting a priority to the timer interupt */
   IntPrioritySet(INT_TIMER0A, 0x01);
}

//*-------------------------------------------------------------
//*	Function name:		initPortBInterupt
//*	Object:				initialises the Port B interrupt routines. Specifically,
//*						interupts will be limited to PB0 and PB1 (wheel encoders).
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void initPortBInterupt(void)
{   
  /* Enable the GPIO port B that is used for the Timer input capture.*/    
   SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOB); 
   
   GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
   
   /* Setting the direction and mode of PB0 and PB1 (both are direction mode in and rising edge) */
   GPIODirModeSet (GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_DIR_MODE_IN);
   GPIOIntTypeSet (GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1,  GPIO_RISING_EDGE) ;
   
   /* enabling port B interupts */
   GPIOPinIntEnable(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
   IntEnable(INT_GPIOB);
   
   /* setting a priority to the Port B interupt */
   IntPrioritySet(INT_GPIOB, 0x00);   
}


//*-------------------------------------------------------------
//*	Function name:		initADC
//*	Object:				initialise the ADC routine. Required to get
//*						brake pedal position information.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void initADC(void)
{
	/* enabling ADC peripheral */
	SysCtlPeripheralEnable (SYSCTL_PERIPH_ADC0);
	
	/* configure trigger source */
	ADCSequenceConfigure (ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure (ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
	
	ADCSequenceEnable (ADC0_BASE, 3);
	
	/* enabling ADC interupts */
	ADCIntEnable (ADC0_BASE, 3);
	
	/* setting a priority to the ADC interupt */
	IntPrioritySet(INT_ADC0SS3, 0x02);	
}
	
//*-------------------------------------------------------------
//*	Function name:		initPWM
//*	Object:				initialise the PWM routine. Required to 
//*						generate braking.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------	 	
void initPWM(void)
{
	unsigned long ulPeriod;

    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

    
    // Enable the Port H and PWM peripherials 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);

    // Set GPIO H0 and H1 as PWM pins.  They are used to output the PWM2 and PWM3 signals.
    GPIOPinConfigure(GPIO_PH0_PWM2);
    GPIOPinConfigure(GPIO_PH1_PWM3);
    GPIOPinTypePWM(GPIO_PORTH_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Compute the PWM period (1/(5Hz)) based on the system clock.
    ulPeriod = SysCtlClockGet() / PWM_FREQ;

    // Set the PWM period to correspond to a frequency of 5Hz.
    PWMGenConfigure(PWM_BASE, PWM_GEN_1,PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM_BASE, PWM_GEN_1, ulPeriod);

    // Set PWM2 to a duty cycle of 25% and PWM3 to a duty cycle of 50%.
    PWMPulseWidthSet(PWM_BASE, PWM_OUT_2, ulPeriod / 2);
    //PWMPulseWidthSet(PWM_BASE, PWM_OUT_3, ulPeriod / 2);

    // Enable the PWM2 and PWM3 output signals.
    PWMOutputState(PWM_BASE, PWM_OUT_2_BIT, true);
    //PWMOutputState(PWM_BASE, PWM_OUT_3_BIT, true);
    PWMOutputState(PWM_BASE, PWM_OUT_3_BIT, false);
	
    // Enable the PWM generator 1.
    PWMGenEnable(PWM_BASE, PWM_GEN_1);
}

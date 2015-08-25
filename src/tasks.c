/*********************************************************************
*  File: tasks.c
*  Project: Assignment #2 for ENEL323
*  Brief: Source file for background tasks required in the ABS
*  Authors: Paul Bowler & Aydin Arik
*  Last Modified: June 2011
**********************************************************************/

/*---------------------------
   Header file inclusions
----------------------------*/
#include "tasks.h"
#include "main.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

/*---------------------------
   Global file inclusions
----------------------------*/
static uint32_t avgLeftWheelPeriod = 0, avgRightWheelPeriod = 0;
dcb_t leftWheelBuff, rightWheelBuff;
static unsigned long ADCValue = 0;
static dcb_t ADCBuffer;
static long avgBrakeRate;
volatile uint32_t brakeMode;
volatile uint32_t wheelMode;	//mode to represent if either wheel is locked (or both are locked)
uint16_t ABStick;

//*-------------------------------------------------------------
//*	Function name:		averagePeriod
//*	Object:				averages the periods from the wheel buffers.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void averagePeriod(void)
{
	int i;
	uint32_t leftWheel = 0, rightWheel = 0;

	if (leftWheelBuff.flag1 == SET) /* if Buffer 1 (left wheel) isn't being written to */
	{
		/* Sum the contents of the buffer */
		for (i = 0; i < BUFFLEN ; i++)
		{
			leftWheel += leftWheelBuff.buff1[i];
		}
	}
	else if (leftWheelBuff.flag2 == SET) /* if Buffer 2 (left wheel) isn't being written to */
	{
		/* Sum the contents of the buffer */
		for (i = 0; i < BUFFLEN ; i++)
		{
			leftWheel += leftWheelBuff.buff2[i];
		}
	}
	
	if (rightWheelBuff.flag1 == SET) /* if Buffer 1 (right wheel) isn't being written to */
	{
		/* Sum the contents of the buffer */
		for (i = 0; i < BUFFLEN ; i++)
		{
			rightWheel += rightWheelBuff.buff1[i];
		} 
	}
	else if (rightWheelBuff.flag2 == SET) /* if Buffer 2 (right wheel) isn't being written to */
	{
		/* Sum the contents of the buffer */
		for (i = 0; i < BUFFLEN ; i++)
		{
			rightWheel += rightWheelBuff.buff2[i];
		}
	}
	
	/* find the averages of the summed buffer values */
	avgLeftWheelPeriod = (leftWheel) / (BUFFLEN * FACTOR);
	avgRightWheelPeriod = (rightWheel) / (BUFFLEN * FACTOR);
	
	/* setting a task flag to determine whether any braking is necessary */
	setFlag (MONITORWHEELS, ACTIVE);
}

//*-------------------------------------------------------------
//*	Function name:		monitorPedalTask
//*	Object:				
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void monitorPedalTask(void)
{

}

//*-------------------------------------------------------------
//*	Function name:		checkWheelLockTask
//*	Object:				Checks to see whether there is any wheel locking
//*						occuring.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void checkWheelLockTask(void)
{
	unsigned long lockUpValue = (MAX_WHEEL_PERIOD * ADC_FREQUENCY / 1000);
	
	/* implements pulsed braking if value of the timeout counters are above a predefined value */
	if ((right_timeout >= lockUpValue) && (left_timeout >= lockUpValue))
	{
		if (wheelMode != (BOTH)) /* if it is a new event of both wheels being locked */
		{
			wheelMode = BOTH;	 /* both wheels are locked */
			
			/* setting a task flag to determine what braking is required */
			setFlag (MONITORWHEELS, ACTIVE);
			
			/* reset variables */
			left_tick = 0;
			right_tick = 0;
			left_curr = 0;
			left_prev = 0;
			right_curr = 0;
			right_prev = 0;
		}
	}
	else if (left_timeout >= lockUpValue)
	{
		if (wheelMode != LEFT)	/* if it is a new event of the left wheel being locked */
		{
			wheelMode = LEFT;   /* left wheels is locked */ 
			
			/* setting a task flag to determine whether any braking is necessary */
			setFlag (MONITORWHEELS, ACTIVE);
			
			/* reset variables */
			left_tick = 0;
			left_curr = 0;
			left_prev = 0;
		}
	}
	else if (right_timeout >= lockUpValue)
	{
		if (wheelMode != RIGHT)	/* if it is a new event of the right wheel being locked */
		{
			wheelMode = RIGHT;  /* right wheels is locked */
			
			/* setting a task flag to determine what braking is required */
			setFlag (MONITORWHEELS, ACTIVE);
			
			/* reset variables */
			right_tick = 0;
			right_curr = 0;
			right_prev = 0;
		}
	}
	else //((right_timeout <= (MAX_WHEEL_PERIOD * ADC_FREQUENCY / 1000)) && (left_timeout <= (MAX_WHEEL_PERIOD * ADC_FREQUENCY / 1000)))
		wheelMode = NONE;	
}

//*-------------------------------------------------------------
//*	Function name:		clearBufferTask
//*	Object:				clears the appropriate buffers by reinitialising them when a 
//*						new lockup event is detected.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void clearBufferTask(void)
{
	/* depending on which wheel (or wheels) is/are locked up, clear the appropriate buffers */
	switch(wheelMode)
	{
		case(LEFT):
		{
			DCB_init(&leftWheelBuff);
			avgLeftWheelPeriod = 0;
			
			break;
		}
		case(RIGHT):
		{
			DCB_init(&rightWheelBuff);
			avgRightWheelPeriod = 0;

			break;
		}
		case(BOTH):
		{
			DCB_init(&leftWheelBuff);
			DCB_init(&rightWheelBuff);
			avgLeftWheelPeriod = 0;
			avgRightWheelPeriod = 0;
		}
	}
	
	/*set a flag to monitor the braking required (if any) */
	setFlag (MONITORWHEELS, ACTIVE);
}

//*-------------------------------------------------------------
//*	Function name:		monitorWheels
//*	Object:				Applies appropriate braking depending on situation
//*						encountered.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void monitorWheels(void)
{
	/* declaring and defining local variables */
	uint32_t wheelToBrake= NONE;
	uint32_t diff = 0;
	
	/*set breakMode back to the default */
	brakeMode = NOBRAKE;	
	
	/* find the absolute difference between the wheel periods */
	if (avgLeftWheelPeriod > avgRightWheelPeriod)
	{
		diff = avgLeftWheelPeriod - avgRightWheelPeriod ;
		wheelToBrake = LEFT;
		//wheelToBrake = RIGHTBRAKE;
	}
	else if (avgLeftWheelPeriod < avgRightWheelPeriod)
	{
		diff = avgRightWheelPeriod - avgLeftWheelPeriod;
		wheelToBrake = RIGHT;
		//wheelToBrake = LEFTBRAKE;
	}
		
	/* determine whether the difference is greater than some predefined threshold for braking */
	if ((diff > DIFF_THRESHOLD))
	{
		//brakeMode = wheelToBrake;
		wheelMode = wheelToBrake;
	}
		
	/* set a flag to update the display */
	setFlag (UPDATEDISPLAY, ACTIVE);	
}

//*-------------------------------------------------------------
//*	Function name:		brakeTask
//*	Object:				Get the brake pedal ADC value when called upon 
//*						and store in a buffer for analysis. Also actuates the brake caliper
//*						(simulated here by a PWM output).
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void brakeTask(void)
{
	/*declare local variables*/
	unsigned long ADCOutputPeriod;	//Output period to modulate and apply to the PWM
	
	/* read a PWM value from the brake pedal */
	
	/*wait for ADC to be completed*/
	while(!ADCIntStatus(ADC0_BASE, 3, 0))
	{
	}
	
	/*get the ADC sequence from the ADC*/
	ADCSequenceDataGet (ADC0_BASE, 3, &ADCValue);
	
	ADCOutputPeriod = SysCtlClockGet() / (ADCValue / PERIOD_SCALAR + MIN_PERIOD);
	
	
	/* Changing audible frequency as a function brake pedal position/ pressure if no wheels are locked up*/
	if ((wheelMode != NONE) && (ADCValue > MIN_BRAKE) && (ABStick >= 5))	//i.e ABS actuation required
	{
		/*toggle the PWM output */
		if (PWMState == OFF) //Switch on
		{
			PWMGenEnable(PWM_BASE, PWM_GEN_1);
			PWMState = ON;
		}
		else	//switch off
		{
			PWMGenDisable(PWM_BASE, PWM_GEN_1);
			PWMState = OFF;
		}
		
		ABStick = 0;			
	}
	else if ((wheelMode == NONE))
	{
		if (ADCValue < MIN_BRAKE)	//special case to apply no brake pressure at lower end of pedal positions
			ADCOutputPeriod = 0;
		
		/* Enable the PWM generator*/
		PWMGenEnable(PWM_BASE, PWM_GEN_1);
		/* Set the output 'true' */
		PWMOutputState(PWM_BASE, PWM_OUT_2_BIT, true);
	}	
	/*output a tone to the speaker amplifier dependent on brake position*/
	PWMGenPeriodSet(PWM_BASE, PWM_GEN_1, ADCOutputPeriod);
	
	/* write the value to the buffer */
    setFlag (AVERAGERATE, DCB_write (ADCValue, &ADCBuffer));
	
}


//*-------------------------------------------------------------
//*	Function name:		averageRate
//*	Object:				Finding the average  of rate of change in the brake pedal position/ pressure
//*						using the ADC values gathered and stored in the buffers.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void averageRate(void)
{
	int i;
	long avBuff[BUFFLEN - 1];
	
	if (ADCBuffer.flag1 == SET) /* Buffer 1 isn't being written to */
	{
		/* Differntiating the ADC circular buffer over time (Only when brake pedal is being depressed (i.e ADC goes from low to high value)) */
		for (i = 0; i < (BUFFLEN - 1) ; i++)
		{
			/*if brake pedal is being depressed not releases */
			if (ADCBuffer.buff1[i + 1] > ADCBuffer.buff1[i])
				avBuff[i] = (long)(ADCBuffer.buff1[i + 1] - ADCBuffer.buff1[i]) * ADC_FREQUENCY;
			else
				avBuff[i] = 0;
		}
	}
	else if (ADCBuffer.flag2 == SET) /* Buffer 2 isn't being written to */
	{
		/* Differntiating the ADC circular buffer over time (Only when brake pedal is being depressed (i.e ADC goes from low to high value)) */
		for (i = 0; i < (BUFFLEN - 1) ; i++)
		{
			/*if brake pedal is being depressed not releases */
			if (ADCBuffer.buff1[i + 1] > ADCBuffer.buff1[i])
				avBuff[i] = (long)(ADCBuffer.buff1[i + 1] - ADCBuffer.buff1[i]) * ADC_FREQUENCY;
			else
				avBuff[i] = 0;
		}
	}
	
	/*total the array of differential ADC terms to find the overall average rate of change in the ADC */  
	for (i = 0; i < (BUFFLEN - 1); i++)
	{
		avgBrakeRate += avBuff[i];
	}
	avgBrakeRate /= (BUFFLEN - 1);
	
	/* activiting pulsed braking when emergency braking is detected */
	if (avgBrakeRate >= EMERGENCY_STOP)
		brakeMode = BOTHBRAKE;
		
	/* Make the next tasks required active */
	setFlag (MONITORWHEELS, ACTIVE);
	
}

//*-------------------------------------------------------------
//*	Function name:		UpdateDisplay
//*	Object:				
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void UpdateDisplay(void)
{
	char strBuf1[CHARBUFF_LENGTH], strBuf2[CHARBUFF_LENGTH], strBuf3[CHARBUFF_LENGTH];
	
    usprintf(strBuf1, "L Period: %u", avgLeftWheelPeriod);
    usprintf(strBuf2, "R Period: %u", avgRightWheelPeriod);
    usprintf(strBuf3, "Brake Mode: %u", wheelMode);
    
    
    /*INITIAL BRAKE TESTING (DEBUG PURPOSES) */
    switch(wheelMode)
    {
    	case(LEFT):
    	{
    		usprintf(strBuf2, "R Period: *: %u", avgRightWheelPeriod);
    		//usprintf(strBuf2, "R Period: INF");
    		break;
    	}
    	case (RIGHT):
    	{
    		usprintf(strBuf1, "L Period *: %u", avgLeftWheelPeriod);
    		//usprintf(strBuf1, "L Period: INF");
    		break;
    	}
    	case (BOTH):
    	{
    		usprintf(strBuf1, "L Period **: INF");
    		usprintf(strBuf2, "R Period **: INF");
    		break;
    	}
    }
    
    /*set the brakeMode to  it's default value */
	brakeMode = NOBRAKE;
    
    RIT128x96x4Clear();
    RIT128x96x4StringDraw( strBuf1, 6, 20, 15); 	
    RIT128x96x4StringDraw( strBuf2, 6, 70, 15);
    RIT128x96x4StringDraw( strBuf3, 6, 50, 15);

}

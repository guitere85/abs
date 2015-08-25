/*********************************************************************
*  File: tasks.h
*  Project: Assignment #2 for ENEL323
*  Brief: Header file for task.h
*  Authors: Paul Bowler & Aydin Arik
*  Last Modified: June 2011
**********************************************************************/

#ifndef TASKS_H_
#define TASKS_H_

/*---------------------------
   Header file inclusions
----------------------------*/
#include "utils/ustdlib.h"
#include "drivers/rit128x96x4.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "inc/hw_memmap.h"

#include "kernel.h"
#include "dcb.h"
#include "main.h"

/*---------------------------
	Public Constants
----------------------------*/
#define FACTOR (8000000 / 1000)
#define uint16_t unsigned long
#define uint32_t unsigned long
#define uint8_t unsigned char

#define CHARBUFF_LENGTH	20
#define MAX_WHEEL_PERIOD 500		//Maximum wheel period allowable before timeout process is tripped (milliseconds)

#define EMERGENCY_STOP	1000		//Threshold of rate of change in brake pedal's postion that implies an emergency stop and both wheels are pulsed.
#define DIFF_THRESHOLD 	20			//Threshold of difference between the left & right wheel encoder readings that results in braking action
#define MIN_BRAKE		10			//Minimum value of ADC at which the brake begins to be applied
#define PERIOD_SCALAR	3			//Scalar to scale the ADC reading in order to produce a variable output over the whole range of the brake's movement.
#define	MIN_PERIOD		60			//Minimum value applied to the PWM output when the brakes are first applied
#define	PULSE_FREQ		5			//Frequency of pulsed braking to apply when ABS is required (Hz)
#define DUTY_CYCLE		0.4			//Duty Cycle of PWM output for braking (both ABS and non-ABS).

enum brake_modes {NOBRAKE = 0, LEFTBRAKE, RIGHTBRAKE, BOTHBRAKE}; //Values for which the brakeMode variable can assume
enum wheel_modes {NONE = 0, LEFT, RIGHT, BOTH};	//Values for which the wheelMode variable can assume
enum PWM_states {OFF = 0, ON, ACTIVATED, INACTIVATED};


/*---------------------------
	Public type definitions
----------------------------*/

/*---------------------------
	Enumerations
----------------------------*/


/*---------------------------
	Public constants
----------------------------*/
#define ADC_FREQUENCY	50	//Number of ADC measurements / second

/*---------------------------
	Public varaibles
----------------------------*/
extern dcb_t leftWheelBuff, rightWheelBuff;
extern volatile uint32_t brakeMode;
extern volatile uint32_t wheelMode;
extern uint16_t ABStick;

/*---------------------------
  Background kernel tasks
----------------------------*/

//*-------------------------------------------------------------
//*	Function name:		monitorPedalTask
//*	Object:				<empty function presently>
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void monitorPedalTask(void);

//*-------------------------------------------------------------
//*	Function name:		checkWheelLockTask
//*	Object:				Checks to see whether there is any wheel locking
//*						occuring.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void checkWheelLockTask(void);

//*-------------------------------------------------------------
//*	Function name:		clearBufferTask
//*	Object:				clears the appropriate buffers by reinitialising them when a 
//*						new lockup event is detected.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void clearBufferTask(void);

//*-------------------------------------------------------------
//*	Function name:		monitorWheels
//*	Object:				Applies appropriate braking depending on situation
//*						encountered.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void monitorWheels(void);

//*-------------------------------------------------------------
//*	Function name:		brakeTask
//*	Object:				Get the brake pedal ADC value when called upon 
//*						and store in a buffer for analysis. Also actuates the brake caliper
//*						(simulated here by a PWM output).
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void brakeTask(void);

//*-------------------------------------------------------------
//*	Function name:		averagePeriod
//*	Object:				averages the periods from the wheel buffers.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void averagePeriod(void);

//*-------------------------------------------------------------
//*	Function name:		averageRate
//*	Object:				Finding the average  of rate of change in the brake pedal position/ pressure
//*						using the ADC values gathered and stored in the buffers.
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void averageRate(void);

//*-------------------------------------------------------------
//*	Function name:		UpdateDisplay
//*	Object:				Updates the values printed to the display
//*						(Presently wheel periods & rate of change in brake pedal position)
//*	Input Parameters:	none
//*	Output Parameters:	none
//*-------------------------------------------------------------
void UpdateDisplay(void);


#endif /*TASKS_H_*/

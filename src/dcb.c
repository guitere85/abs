/*********************************************************************
*  File: dcb.c
*  Project: Assignment #2 for ENEL323
*  Brief: Source File for Double Circular Buffer Constructs
*  Authors: Paul Bowler & Aydin Arik
*  Last Modified: June 2011
**********************************************************************/

/*---------------------------
   Header file inclusions
----------------------------*/
#include "dcb.h"

/*---------------------------
    Private constants
----------------------------*/
enum buffReadStates {DONT_READ_BUFF = 0, READ_BUFF};

/*---------------------------
  Private function prototypes
----------------------------*/
static uint8_t DCB_isBuffFull (uint16_t buffElement);



//*-------------------------------------------------------------
//*	Function name:		DCB_isBuffFull
//*	Object:				Checks to see if current buffer we're currently writing to is full
//*	Input Parameters:	buffElement - Current buffer element being written to
//*	Output Parameters:	SET - Current buffer is full
//*					    NOTSET - Current buffer is full
//*-------------------------------------------------------------
static uint8_t DCB_isBuffFull (uint16_t buffElement)
{
    if (buffElement == (BUFFLEN - 1))
        return SET;
    else 
        return NOTSET;
}


 /*---------------------------
  	Public functions
----------------------------*/

//*-------------------------------------------------------------
//*	Function name:		DCB_init
//*	Object:				Initialises buffers and various other variables used to successfully implement 
//*				        a double circular buffer write function
//*	Input Parameters:	*ptr - pointer to double circular buffer structure
//*	Output Parameters:	none
//*-------------------------------------------------------------
void DCB_init (dcb_t *ptr)
{
    int i;
    
    /* initialise all buffer elements to zero */
    for (i = 0; i < BUFFLEN; i++)
    {
        ptr->buff1[i] = 0;
        ptr->buff2[i] = 0;
    }
    
    /* initialise buffer flags and current element indicator */
    ptr->flag1 = NOTSET;
    ptr->flag2 = SET;
    ptr->currElement = 0;
}

//*-------------------------------------------------------------
//*	Function name:		DCB_write
//*	Object:				Write to a buffer element
//*	Input Parameters:	value - a value to be placed in a buffer element
//*      				*ptrToDCB - pointer to a double circular buffer structure
//*	Output Parameters:	DONTREADBUFF - No buffers should be read (no buffers have been filled again)
//*					    READBUFF - Indicates a buffer (1 or 2) should now be read as they have been filled
//*-------------------------------------------------------------
uint8_t DCB_write (uint16_t value, dcb_t *ptrToDCB)
{
	if (ptrToDCB->flag1 == NOTSET) /* Is buffer 1 not being read? */
	{
		ptrToDCB->buff1[ptrToDCB->currElement] = value; /* Write value to buffer 1 */
		ptrToDCB->flag1 = DCB_isBuffFull (ptrToDCB->currElement); /* Is buffer 1 full? */
		ptrToDCB->flag2 = ptrToDCB->flag1 ^ 1; /* Used to change buffer 2's flag when buffer 1's flag changes */
		
		if (ptrToDCB->flag2 == NOTSET) /* Buffer 1 is full, now we can read from it */
		{
			ptrToDCB->currElement = 0;
			return READ_BUFF;
		} 
		else /* Buffer 1 is still not full */
		{
			ptrToDCB->currElement++;
			return DONT_READ_BUFF;
		} 
	}
	else /* Buffer 2 is not being read */
	{
		ptrToDCB->buff2[ptrToDCB->currElement] = value; /* Write value to buffer 2 */
		ptrToDCB->flag2 = DCB_isBuffFull (ptrToDCB->currElement); /* Is buffer 2 full? */
		ptrToDCB->flag1 = ptrToDCB->flag2 ^ 1; /* Used to change buffer 1's flag when buffer 2's flag changes */
		
		if (ptrToDCB->flag1 == NOTSET) /* Buffer 2 is full, now we can read from it. */
		{ 
			ptrToDCB->currElement = 0;
			return READ_BUFF;  
		}
		else /* Buffer 2 is still not full */
		{
			ptrToDCB->currElement++; 
			return DONT_READ_BUFF;
		}
	}
}

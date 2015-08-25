/*********************************************************************
*  File: dcb.h
*  Project: Assignment #2 for ENEL323
*  Brief: Header File for dcb.c
*  Authors: Paul Bowler & Aydin Arik
*  Last Modified: June 2011
**********************************************************************/

#ifndef DCB_H
#define DCB_H

/*---------------------------
	Public constants
----------------------------*/
#define BUFFLEN 10   /* Length of a buffer */

/*---------------------------
	Public type definitions
----------------------------*/
#define uint16_t unsigned long
#define uint32_t unsigned long
#define uint8_t unsigned char

typedef struct {
	volatile uint16_t buff1[BUFFLEN];
	volatile uint16_t buff2[BUFFLEN];
	volatile uint8_t flag1;  /* Buffer 1 Flag */
	volatile uint8_t flag2;  /* Buffer 2 Flag */
	volatile uint16_t currElement;
} dcb_t;

/*---------------------------
	Public enumerations
----------------------------*/
enum flagStates {NOTSET, SET};  


/*---------------------------
  Public function prototypes
----------------------------*/

//*-------------------------------------------------------------
//*	Function name:		DCB_init
//*	Object:				Initialises buffers and various other variables used to successfully implement 
//*				        a double circular buffer write function
//*	Input Parameters:	*ptr - pointer to double circular buffer structure
//*	Output Parameters:	none
//*-------------------------------------------------------------
void DCB_init (dcb_t *ptr);

//*-------------------------------------------------------------
//*	Function name:		DCB_write
//*	Object:				Write to a buffer element
//*	Input Parameters:	value - a value to be placed in a buffer element
//*      				*ptrToDCB - pointer to a double circular buffer structure
//*	Output Parameters:	DONTREADBUFF (0) - No buffers should be read (no buffers have been filled again)
//*					    READBUFF (1) - Indicates a buffer (1 or 2) should now be read as they have been filled
//*-------------------------------------------------------------
uint8_t DCB_write (uint16_t value, dcb_t *ptrToDCB);


#endif /* DCB_H */

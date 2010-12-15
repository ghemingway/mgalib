/*** Copyright Vanderbilt ***/


#ifndef MGA_HIGHRES_TIMING_H
#define MGA_HIGHRES_TIMING_H


/*** Included Header Files ***/
#include "CoreTypes.h"


/*****************************************************************************/


/* Initialize the high resolution timer system
 *	- Arguments: None
 *	- Return: None															*/
void InitializeTime( void );


/* Zero out the high resolution timer
 * 	- Arguments: The time when the clock should have been zeroed
 *	- Return: None															*/
void ZeroTime(
		double desiredMS=0.0							// Time when the clock should have been zeroed
	);


/* Get the amount of elapsed time (in ms) since the last call to ZeroTime
 *	- Arguments: None
 *	- Return: Amount of time elapsed (in ms) since last call to ZeroTime	*/
double GetTimeMS( void );


/* Get the absolute time from when InitializeTime was called - not affected by
 * calls to ZeroTime
 *	- Arguments: None
 *	- Returns: The absolute amount of time elapsed (in MS) since InitializeTime was called */
double AbsoluteTimeMS( void );


/* Let the timing system know that an event occured at a slightly different time than expected
 *	- Arguments: Offset of event (positive means event occured early, negative means event occured late
 *	- Returns: None															*/
void SuggestOffset(
	   double offsetMS
	);


/* Sleep until the given time has elapsed
 *	- Arguments: deadline - time to sleep until relative to last called ZeroTime
 *				 tolerance - fudge factor for is NanoSleep is called a bit late
 *	- Return: None															*/
double NanoSleep(
		double deadlineMS								// Deadline (within hyperperiod) for sleeping
	);


/* Busy wait (not sleep) until the given time has elapsed
 *	- Arguments: duration - time to busywait from right now
 *	- Return: None															*/
double BusyWait(
		double durationMS								// Relative duration (in ms) to wait
	);

	
/*****************************************************************************/


#endif // MGA_HIGHRES_TIMING_H


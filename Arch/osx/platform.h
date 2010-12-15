/*** Copyright Vanderbilt ***/


#ifndef MGA_OSX_PLATFORM_H
#define MGA_OSX_PLATFORM_H

	
/*** Included Header Files ***/
#include <pthread.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#include <mach/mach_init.h>
#include <mach/mach_time.h>
#include <mach/thread_policy.h>
#include <mach/thread_act.h>
#include <sched.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <CoreServices/CoreServices.h>
#include <time.h>


/*****************************************************************************/


// Platform specific type definitions
typedef	double				pfloat_t;
//#define bool				int
//#define true				1
//#define false				0
#define INLINE				inline
typedef semaphore_t			Semaphore;


/*****************************************************************************/


#endif // MGA_OSX_PLATFORM_H


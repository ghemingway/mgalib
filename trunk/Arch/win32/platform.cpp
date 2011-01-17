/*** Included Header Files ***/
#include "platform.h"
#include <string>
#define ASSERT	assert
//#include "scheduler.h"
//#include "highres_timing.h"
//#include "logger.h"
//#include "windows.h"


/*** Externall Defined Variables and Functions ***/
extern bool							_schedExec;	
extern Semaphore					*_schedTaskComplete;


/*****************************************************************************/


#define MAX_SEMAPHORES				1024
static Semaphore					_semaphoreArray[MAX_SEMAPHORES];
static uint16_t						_numSemaphores = 0;


/*****************************************************************************/


/* Since OSX does not support unnamed semaphores and win32 pthreads does not
 * support named semaphores, we have to wrap the create and destroy functions	*/
Semaphore* _CreateSemaphore( char* name, uint32_t initValue ) {
	// Check a couple of values
	ASSERT( name != NULL );
	ASSERT( _numSemaphores < MAX_SEMAPHORES );
	// For windows we must use CreateSemaphore
	_semaphoreArray[_numSemaphores++] = CreateSemaphore( NULL, (LONG)initValue, 1000, NULL );
	// Return the good semaphore
//	return _semaphoreArray[_numSemaphores-1];
	ASSERT(false);
	return NULL;
}


/* Simple wrapper for posting to a semaphore */
int _PostSemaphore( Semaphore* semaphore ) {
	// Just call to the ReleaseSemaphore function
	ReleaseSemaphore( semaphore, 1, NULL );
	return 0;
}


/* Simple wrapper for waiting on a semaphore */
int _WaitSemaphore( Semaphore* semaphore ) {
	// Just call to the WaitForSingleObject function
	WaitForSingleObject( semaphore, INFINITE );
	return 0;
}


/* Close the named (or in Win32 case unnamed ) semaphore */
void _DestroySemaphore( char* name, Semaphore *semaphore ) {
	// Check some values
	ASSERT( name != NULL );
	ASSERT( semaphore != NULL );
	// Just close the semaphore
	CloseHandle( semaphore );
}


/*****************************************************************************/


void _SplitPath(const std::string &path, std::string &directory, std::string &filename)
{
	// Find the last / or \\ in the path
	size_t found = path.find_last_of("/\\");
	if (found != path.npos)
	{
		directory = path.substr(0, found + 1);
		// Do we need to handle paths that start with '~'
		if (directory[0] == '~') {
			// Get the user's home directory
			std::string homePath = getenv("HOME");
			// Append this to the beginning of the directory
			directory = homePath + directory.substr(1);
		}
		filename = path.substr(found + 1);
	}
	else
	{
		directory = "";
		filename = path;
	}
}


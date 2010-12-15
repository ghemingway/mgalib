/*** Included Header Files ***/
#include "platform.h"
#include <string>


/*** Externall Defined Variables and Functions ***/
//None


/*****************************************************************************/


#define MAX_SEMAPHORES					16
static semaphore_t						_semaphoreArray[MAX_SEMAPHORES];
static uint16_t							_numSemaphores = 0;


/*****************************************************************************/


/* Since OSX does not support unnamed semaphores and win32 pthreads does not
 * support named semaphores, we have to wrap the create and destroy functions	*/
Semaphore* _CreateSemaphore( char* name, uint32_t initValue ) {
	// Get the next available semaphore
	semaphore_t* semaphore = _semaphoreArray + _numSemaphores++;
	kern_return_t retVal;
	// Check a couple of values
	assert( name != NULL );
	assert( _numSemaphores < MAX_SEMAPHORES );
	// For OS X we utilize mach semaphores which are supposed to be better than posix sem_t
	retVal = semaphore_create( mach_task_self(), semaphore, SYNC_POLICY_FIFO, initValue);
	// Check for valid creation
	assert( retVal == KERN_SUCCESS );
	// Return the good semaphore
	return semaphore;
}


/* Simple wrapper for posting to a semaphore */
int _PostSemaphore( Semaphore* semaphore ) {
	// Use the mach call to post
	kern_return_t retVal = semaphore_signal( *semaphore );
	if ( retVal != KERN_SUCCESS ) assert( false );
	return 0;
}


/* Simple wrapper for waiting on a semaphore */
int _WaitSemaphore( Semaphore* semaphore ) {
	// Use the mach call to wait
	kern_return_t retVal = semaphore_wait( *semaphore );
	if ( retVal != KERN_SUCCESS ) assert( false );
	return 0;
}


/* Simple wrapper for destroying a semaphore */
void _DestroySemaphore( char* name, Semaphore *semaphore ) {
	kern_return_t retVal;
	// Check some values
	assert( name != NULL );
	assert( semaphore != NULL );
	// Just destroy the semaphore (Mach call)
	retVal = semaphore_destroy( mach_task_self(), *semaphore );
	// Check for success
	assert( retVal == KERN_SUCCESS );
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

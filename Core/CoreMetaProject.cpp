/*** Included Header Files ***/
#include "CoreMetaProject.h"
#include "CoreMetaObject.h"


// --------------------------- Public CoreMetaProject Methods ---------------------------


CoreMetaProject::~CoreMetaProject()
{
	// While there are objects in the list...
	while( !this->_objects.empty() )
	{
	// Delete the first object
		delete this->_objects.front();
		// Pop the object from the list
		this->_objects.pop_front();
	}
}


const Result_t CoreMetaProject::GetObject(const MetaID_t &metaID, CoreMetaObject* &object) throw()
{
	if ( metaID == METAID_NONE ) return E_INVALID_USAGE;
	// Get iterator to front of object list
	std::list<CoreMetaObject*>::iterator objectIter = this->_objects.begin();
	// Loop until end of list reached
	while( objectIter != this->_objects.end() )
	{
		// Make sure we are not null
		ASSERT( *objectIter != NULL );
		// Did we find the right object
		MetaID_t tmpMetaID = METAID_NONE;
		Result_t result = (*objectIter)->GetMetaID(tmpMetaID);
		ASSERT( result == S_OK );
		ASSERT( tmpMetaID != METAID_NONE );
		if( tmpMetaID == metaID )
		{
			// If so, return its pointer
			object = *objectIter;
			return S_OK;
		}
		// Move to next object
		++objectIter;
	}
	// Didn't find anything
	return E_NOTFOUND;
}


const Result_t CoreMetaProject::AddObject(const MetaID_t &metaID, const std::string &token, 
										  const std::string &name, CoreMetaObject* &object) throw()
{
	if ( metaID == METAID_NONE ) return NULL;
	// Create a new object
	CoreMetaObject *newObject = new CoreMetaObject(this, metaID, token, name);
	ASSERT( newObject != NULL );
	// Add the object to the project list
	this->_objects.push_front(newObject);
	// Return pointer to the new object
	object = newObject;
	return S_OK;
}


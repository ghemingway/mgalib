/*** Included Header Files ***/
#include "CoreProject.h"
#include "CoreObject.h"
#include "CoreAttribute.h"
#include "CoreMetaProject.h"
#include "CoreMetaObject.h"


// --------------------------- Private CoreProject Functions ---------------------------


CoreProject::CoreProject(CoreMetaProject* &coreMetaProject, ICoreStorage* &storage)
	:	_storage(storage), _coreMetaProject(coreMetaProject), _isDirty(false), _objectHash(), _transactionList()
{
	ASSERT( coreMetaProject != NULL );
	ASSERT( storage != NULL );
}


std::string CoreProject::GetFirstToken(const std::string &connection)
{
	// Get a local copy
	std::string con = connection;
	// Find the equals
	int pos = con.find('=');
	// If there is an =, get just the string to that point
	if( pos != std::string::npos )
		con.resize(pos);
	// Find the semicolon
	pos = con.find(';');
	// If there is a ;, get just the string to that point
	if( pos != std::string::npos )
		con.resize(pos);
	// Return what is left
	return con;
}


void CoreProject::RegisterObject(const Uuid &uuid, CoreObjectBase *object) throw()
{
	// Check some things
	ASSERT( this->_storage != NULL );
	ASSERT( uuid != Uuid::Null() );
	ASSERT( object != NULL );
	// Must not find object already in the hash
	ASSERT( this->_objectHash.find(uuid) == this->_objectHash.end() );
	// Insert the object into the hash
	this->_objectHash.insert( std::make_pair(uuid, object) );
}


void CoreProject::UnregisterObject(const Uuid &uuid) throw()
{
	ASSERT( this->_storage != NULL );
	ASSERT( uuid != Uuid::Null() );
	// Is the pair in the hash?
	ObjectHashIterator objIter = this->_objectHash.find(uuid);
	if( objIter != this->_objectHash.end() )
	{
		// Remove the object from the hash
		this->_objectHash.erase( objIter );
		return;
	}
	// Really should assert here.  Should never have issues
	ASSERT(false);
}


ICoreStorage* CoreProject::SetStorageObject(const Uuid &uuid) throw()
{
	// Try to open the object at the storage level
	Result_t result = this->_storage->OpenObject(uuid);
	// Really should be good here
	ASSERT( result == S_OK );
	return this->_storage;
}


void CoreProject::RegisterTransactionItem(CoreAttributeBase* attribute) throw()
{
	// Make sure we are in a transaction
	ASSERT( !this->_transactionList.empty() );
	// Add this attribute to the list of changes
	this->_transactionList.front().attributes.push_back(attribute);
}


// --------------------------- Public CoreProject Functions ---------------------------


CoreProject::~CoreProject()
{
	// Do some checks before we leave (pretty much everything has to be empty)
	if( !this->_objectHash.empty() )
	{
		std::cout << "Forgotten Objects in CoreProject ObjectHash: " << this->_objectHash.size() << std::endl;
		ObjectHash::iterator hashIter = this->_objectHash.begin();
		while (hashIter != this->_objectHash.end())
		{
			CoreObjectBase* objBase = hashIter->second;
			CoreMetaObject* metaObj;
			objBase->MetaObject(metaObj);
			
			std::string name, metaName;
			metaObj->GetName(metaName);
			Result_t result = this->BeginTransaction(true);
			ASSERT( result == S_OK );
			result = objBase->GetAttributeValue(ATTRID_NAME, name);
			if ( result != S_OK )
			{
				std::cout << metaObj;
				name = "";
			}
			result = this->CommitTransaction();
			ASSERT( result == S_OK );
			std::cout << "\tForgotten Object: (" << metaName << ") named " << name << ".\n";
			++hashIter;
		}
	}
	ASSERT( this->_transactionList.empty() );
	// Delete any storage
	if (this->_storage != NULL) delete this->_storage;
	// Delete the CoreMetaProject
	if (this->_coreMetaProject != NULL) delete this->_coreMetaProject;
	this->_coreMetaProject = NULL;
}


const Result_t CoreProject::Create(const std::string &connection, CoreMetaProject* coreMetaProject, CoreProject* &project) throw()
{
	if( connection == "" ) return E_NAMEMISSING;
	if( coreMetaProject == NULL ) return E_META_NOTOPEN;
	// What type of file are we trying to open
	std::string con = CoreProject::GetFirstToken(connection);
	ICoreStorage* coreStorage = NULL;
	if( con == "MGA" )
	{
		// Clean up the filename
		std::string cleanFileName = connection;
		cleanFileName.erase(0, 4);
		// Create the CoreStorage for the project
		Result_t retVal = ICoreStorage::Create(con, cleanFileName, coreMetaProject, coreStorage);
		if ( retVal != S_OK )
		{
			delete coreMetaProject;
			return retVal;
		}
	}
	if( coreStorage == NULL )
	{
		// Do something to signal a failure to create CoreStorage
		delete coreMetaProject;
		return E_UNKNOWN_STORAGE;
	}
	// Create the new CoreProject
	project = new CoreProject(coreMetaProject, coreStorage);
	ASSERT( project != NULL );	
	return S_OK;
}


const Result_t CoreProject::Open(const std::string &connection, CoreMetaProject* coreMetaProject, CoreProject* &project) throw()
{
	if( connection == "" ) return E_NAMEMISSING;
	if( coreMetaProject == NULL ) return E_META_NOTOPEN;
	// Now figure out what CoreStorage to use
	std::string con = CoreProject::GetFirstToken(connection);
	ICoreStorage* coreStorage = NULL;
	if( con == "MGA" )
	{
		// Clean up the filename
		std::string cleanFileName = connection;
		cleanFileName.erase(0, 4);
		// Open the CoreStorage for the project
		Result_t retVal = ICoreStorage::Open(con, cleanFileName, coreMetaProject, coreStorage);
		if ( retVal != S_OK )
		{
			delete coreMetaProject;
			return retVal;
		}
	}
	// Make sure we have a valid storage object
	if( coreStorage == NULL )
	{
		// Do something to signal a failure to create CoreStorage
		delete coreMetaProject;
		return E_UNKNOWN_STORAGE;
	}
	// Create the new CoreProject
	project = new CoreProject(coreMetaProject, coreStorage);
	ASSERT( project != NULL );
	return S_OK;
}


const Result_t CoreProject::Save(const std::string &filename, const bool &force) throw()
{
	if( filename == "" ) return E_NAMEMISSING;
	if( !this->_transactionList.empty() ) return E_TRANSACTION;
	ASSERT( this->_storage != NULL );
	// Ask the coreStorage to save itself
	return this->_storage->Save(filename, force);
}


const Result_t CoreProject::BeginTransaction(const bool &readOnly) throw()
{
	ASSERT( this->_storage != NULL );
	// Make sure a read-write transaction is not being requested nested inside of a read-only
	if( !this->_transactionList.empty() && this->_transactionList.front().readOnly && !readOnly) return E_READONLY;
	// Start the transaction at the storage level
	this->_storage->BeginTransaction();
	// Create the new transaction item and put it on top of the list
	this->_transactionList.push_front( Transaction() );
	// Set the transaction read/write type
	this->_transactionList.front().readOnly = readOnly;
	return S_OK;
}


const Result_t CoreProject::CommitTransaction(void) throw()
{
	ASSERT( this->_storage != NULL );
	if( this->_transactionList.empty() ) return S_OK;
	// Is this a nested commit transaction
	if( this->_transactionList.size() > 1 )
	{
		// Get the transaction and remove it from the list
		Transaction transaction = this->_transactionList.front();
		this->_transactionList.pop_front();
		// Get the parent transaction
		Transaction &parentTransaction = this->_transactionList.front();
		// Append nested transaction onto end of parent transaction
		parentTransaction.createdObjects.splice(parentTransaction.createdObjects.begin(), transaction.createdObjects);
		parentTransaction.attributes.splice(parentTransaction.attributes.begin(), transaction.attributes);
		parentTransaction.deletedObjects.splice(parentTransaction.deletedObjects.begin(), transaction.deletedObjects);
	}
	// This is a final commit
	else
	{
		// Get the transaction and process
		Transaction &transaction = this->_transactionList.front();
		// Don't need to worry about CreateObjects since they are already in storage)
		transaction.createdObjects.clear();
		// Go through all attribute changes and write into storage
		std::list<CoreAttributeBase*>::iterator attrIter = transaction.attributes.begin();
		while( attrIter != transaction.attributes.end() )
		{
			// Force it to write to storage and condense its values list
			(*attrIter)->CommitTransaction();
			++attrIter;
		}
		// Go through all object deletes and write into storage
		std::list<Uuid>::iterator deletedIter = transaction.deletedObjects.begin();
		while( deletedIter != transaction.deletedObjects.end() )
		{
			// Set storage to this object and delete it
			Result_t result = this->_storage->OpenObject(*deletedIter);
			ASSERT( result == S_OK );
			result = this->_storage->DeleteObject();
			ASSERT( result == S_OK );
			// Move on to the next deleted object
			++deletedIter;
		}
		// Commit the transaction in storage
		this->_storage->CommitTransaction();
		// Remove the final transaction
		this->_transactionList.pop_front();
		ASSERT( this->_transactionList.empty() );
	}
	// All is good
	return S_OK;
}


const Result_t CoreProject::AbortTransaction(void) throw()
{
	ASSERT( this->_storage != NULL );
	if( this->_transactionList.empty() ) return S_OK;

	// Get the transaction and process
	Transaction &transaction = this->_transactionList.front();
	// Go through all attribute changes and abort them
	std::list<CoreAttributeBase*>::iterator attrIter = transaction.attributes.begin();
	while( attrIter != transaction.attributes.end() )
	{
		// Revert this value change in the attribute
		(*attrIter)->AbortTransaction();
		++attrIter;
	}
	// Nothing needs to be done for deleted since they are reconnected via attribute reversions
	transaction.deletedObjects.clear();
	// Go through all created objects and delete from storage
	std::list<Uuid>::iterator createdIter = transaction.createdObjects.begin();
	while( createdIter != transaction.createdObjects.end() )
	{
		// Set storage to this object and delete it
		Result_t result = this->_storage->OpenObject(*createdIter);
		ASSERT( result == S_OK );
		result = this->_storage->DeleteObject();
		ASSERT( result == S_OK );
		// Move on to the next created object
		++createdIter;
	}
	// Remove the transaction and move on
	this->_transactionList.pop_front();
	// If this was the last transaction, call AbortTransaction on storage
	if (this->_transactionList.empty()) this->_storage->AbortTransaction();
	// All is good
	return S_OK;
}


const Result_t CoreProject::Object(const Uuid &uuid, CoreObject &object) throw()
{
	if( uuid == Uuid::Null() ) return E_INVALID_USAGE;
	if( this->_transactionList.empty() ) return E_TRANSACTION;
	// Have we already fetched this object
	ObjectHashIterator objectIter = this->_objectHash.find(uuid);
	if( objectIter != this->_objectHash.end() )
	{
		ASSERT( objectIter->second != NULL );
		// Create a CoreObject(smart pointer) for the object
		object = CoreObject(objectIter->second);
	}
	// Otherwise, we have to instantiate this object (this will register it to the project)
	else
	{
		Result_t result = CoreObjectBase::Create(this, uuid, object);
		if( result != S_OK ) return result;
	}
	ASSERT( object != NULL );
	return S_OK;
}


const Result_t CoreProject::CreateObject(const MetaID_t &metaID, CoreObject &object) throw()
{
	if( metaID == METAID_NONE ) return E_INVALID_USAGE;
	// Must be in a write transaction
	if( this->_transactionList.empty() ) return E_TRANSACTION;
	if( this->_transactionList.front().readOnly ) return E_READONLY;
	// Create an object of the specified MetaID (call to ICoreStorage)
	Uuid uuid =  Uuid::Null();
	Result_t result = this->_storage->CreateObject(metaID, uuid);
	ASSERT( result == S_OK && uuid != Uuid::Null() );
	// Now create the corresponding CoreObjectBase and CoreObject pointer
	result = CoreObjectBase::Create(this, uuid, object);
	if( result != S_OK )
	{
		// Delete that object from storage
		Result_t tmpResult = this->_storage->DeleteObject();
		ASSERT( tmpResult == S_OK );
		return result;
	}
	ASSERT( object != NULL );
	// Add the object into the created objects list of the transaction
	this->_transactionList.front().createdObjects.push_back( uuid );
	return S_OK;
}


const Result_t CoreProject::RootObject(CoreObject &coreObject) throw()
{
	// Get the root object from the storage
	Uuid uuid = Uuid::Null();
	Result_t result = this->_storage->RootUuid(uuid);
	ASSERT( result == S_OK );
	ASSERT( uuid != Uuid::Null() );
	return this->Object(uuid, coreObject);
}


const Result_t CoreProject::DeleteObject(const Uuid &uuid) throw()
{
	if( uuid == Uuid::Null() ) return E_INVALID_USAGE;
	// Must be in a write transaction
	if( this->_transactionList.empty() ) return E_TRANSACTION;
	if( this->_transactionList.front().readOnly ) return E_READONLY;
	// Is this object in the objectHash?  Don't allow delete if so
	if( this->_objectHash.find(uuid) != this->_objectHash.end() ) return E_LOCK_VIOLATION;
	// Open the object with the specified Uuid (call to ICoreStorage)
	Result_t result = this->_storage->OpenObject(uuid);
	if( result != S_OK ) return result;
	// Now delete it in storage
	result = this->_storage->DeleteObject();
	if( result != S_OK ) return result;
	// Add object to deleted objects list of the transaction
	this->_transactionList.front().deletedObjects.push_back(uuid);
	return S_OK;
}


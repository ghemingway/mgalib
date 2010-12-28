/*** Included Header Files ***/
#include "CoreProject.h"
#include "CoreObject.h"
#include "CoreAttribute.h"
#include "CoreMetaProject.h"
#include "CoreMetaObject.h"


// --------------------------- Private CoreProject Functions ---------------------------


CoreProject::CoreProject(CoreMetaProject *coreMetaProject)
	:	_storage(NULL), _coreMetaProject(coreMetaProject), _isDirty(false), _objectHash(),
		_transactionList(), _undoList(),
		_redoCount(0)
{
	ASSERT( coreMetaProject != NULL );
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
			Result_t result;
			metaObj->GetName(metaName);
			ASSERT( result = this->BeginTransaction(true) == S_OK );
			result = objBase->GetAttributeValue(ATTRID_NAME, name);
			if ( result != S_OK )
			{
				std::cout << metaObj;
				name = "";
			}
			ASSERT( result = this->CommitTransaction() == S_OK );
			std::cout << "\tForgotten Object: (" << metaName << ") named " << name << ".\n";
			++hashIter;
		}
	}
	ASSERT( this->_transactionList.empty() );
//	ASSERT( this->_undoList.empty() );
	// Delete any storage
	if (this->_storage != NULL) delete this->_storage;
	// Delete the CoreMetaProject
	if (this->_coreMetaProject != NULL) delete this->_coreMetaProject;
}


const Result_t CoreProject::CreateProject(const std::string &connection, CoreMetaProject* coreMetaProject, CoreProject* &project) throw()
{
	if( connection == "" ) return E_INVALID_USAGE;
	if( coreMetaProject == NULL ) return E_INVALID_USAGE;
	// Create the new CoreProject
	CoreProject* coreProject = new CoreProject(coreMetaProject);
	ASSERT( coreProject != NULL );
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
			delete coreProject;
			return retVal;
		}
	}
	else ASSERT( false );	
	if( coreStorage == NULL )
	{
		// Do something to signal a failure to create CoreStorage
		delete coreProject;
		return E_EXCEPTION;
	}
	// Have good storage...moving on
	coreProject->_storage = coreStorage;
	project = coreProject;
	return S_OK;
}


const Result_t CoreProject::OpenProject(const std::string &connection, CoreMetaProject* coreMetaProject, CoreProject* &project) throw()
{
	if( connection == "" ) return E_INVALID_USAGE;
	if( coreMetaProject == NULL ) return E_INVALID_USAGE;
	// Create the new CoreProject
	CoreProject* coreProject = new CoreProject(coreMetaProject);
	ASSERT( coreProject != NULL );
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
			delete coreProject;
			return retVal;
		}
	}
	else
		ASSERT( false );
	// Make sure we have a valid storage object
	if( coreStorage == NULL ) {
		delete coreProject;
		return E_EXCEPTION;
	}
	// Set the storage - will also set project's coreMetaProject pointer
	coreProject->_storage = coreStorage;
	project = coreProject;
	return S_OK;
}


const Result_t CoreProject::Save(const std::string &filename) throw()
{
	if( filename == "" ) return E_NAMEMISSING;
	if( !this->_transactionList.empty() ) return E_INVALID_USAGE;
	ASSERT( this->_storage != NULL );
	// Ask the coreStorage to save itself
	return this->_storage->Save(filename);
}


const Result_t CoreProject::BeginTransaction(const bool &readOnly) throw()
{
	ASSERT( this->_storage != NULL );
	// Is this a nested transaction
	if( !this->_transactionList.empty() )
	{
/*
		// Make sure a read-write transaction is not being requested inside of a read-only
		// ...
		TransactionItemsListIterator itemsIter = this->_transactions.front().items.begin();
		TransactionItemsListIterator itemsEnd = this->_transactions.front().items.end();
		while( itemsIter != itemsEnd )
		{
			// Make sure the transaction item is dirty
			ASSERT( (*itemsIter)->IsDirty() );
			(*itemsIter)->ChangeDirty(false);
			ASSERT( !(*itemsIter)->IsDirty() );
			// Move to the next transaction item
			++itemsIter;
		}
		//
		this->_transactions.push_front( TransactionTypes() );
		this->_transactions.front().readonly = false;
*/
	}
	// Must be a final transaction
	else
	{
		// If this is a read/write transaction then flush the redo queue
//		if( !readOnly )
//		{
//			while( this->_redoCount > 0 ) this->DiscardUndo();
//			ASSERT( this->_redoCount == 0 );
//		}
		// Start the transaction at the storage level
		this->_storage->BeginTransaction();
		// Create the new transaction item and put it on top of the list
		this->_transactionList.push_front( Transaction() );
		// Set the transaction read/write type
		this->_transactionList.front().readOnly = readOnly;
	}
	return S_OK;
}


const Result_t CoreProject::CommitTransaction(void) throw()
{
	ASSERT( this->_storage != NULL );
	if( this->_transactionList.empty() ) return E_INVALID_USAGE;
	// Is this a nested commit transaction
	if( this->_transactionList.size() > 1 )
	{
		ASSERT(false);
/*
		Transaction &currentTransaction = this->_transactionList.front();
		TransactionTypesListIterator previous = ++this->_transactions.begin();
		
		TransactionItemsListIterator i = (*previous).items.begin();
		TransactionItemsListIterator e = (*previous).items.end();
		while( i != e )
		{
			if( (*i)->IsDirty() )
			{
				(*i)->DiscardPreviousValue();
			}
			else
			{
				(*i)->ChangeDirty(true);
				current.items.push_front( (*i) );
			}
			ASSERT( (*i)->IsDirty() );
			++i;
		}
		
		// don't forget to set the read-only flag;
		current.readonly = previous->readonly;
		this->_transactions.erase(previous);
*/
	}
	// This is a final commit
	else
	{
		// Get the transaction and process (don't need to worry about CreateObjects since they are already in storage)
		Transaction &transaction = this->_transactionList.front();
		// Go through all attribute changes and write into storage
		std::list<CoreAttributeBase*>::iterator attrIter = transaction.attributes.begin();
		while( attrIter != transaction.attributes.end() )
		{
			// Force it to write to storage
			(*attrIter)->CommitTransaction();
			++attrIter;
		}
		// Go through all object deletes and write into storage
		std::list<Uuid>::iterator deletedIter = transaction.deletedObjects.begin();
		while( deletedIter != transaction.deletedObjects.end() )
		{
			// Set storage to this object and delete it
			ASSERT( this->_storage->OpenObject(*deletedIter) == S_OK );
			ASSERT( this->_storage->DeleteObject() == S_OK );
			// Move on to the next deleted object
			++deletedIter;
		}
		// Commit the transaction in storage
		this->_storage->CommitTransaction();
		// Remove the final transaction
		this->_transactionList.pop_front();
		ASSERT( this->_transactionList.empty() );
	}
	return S_OK;
}


const Result_t CoreProject::AbortTransaction(void) throw()
{
	ASSERT(false);
//	if( this->_storage == NULL || transactionType == TransactionType::Any() || !this->InTransaction() ||
//	   (limited_size(this->_transactions, 2) == 1 && transactionType == TransactionType::First() ) )
//	{
//		ASSERT(false);	//E_INVALID_USAGE
//	}
//	if( limited_size(this->_transactions, 2) >= 2 )
//	{
//		this->AbortNestedTransaction();
//	}
//	else
//	{
//		this->AbortFinalTransaction();
//	}
	return S_OK;
}


const Result_t CoreProject::Object(const Uuid &uuid, CoreObject* &object) throw()
{
	if( uuid == Uuid::Null() ) return E_INVALID_USAGE;
	if( this->_transactionList.empty() ) return E_TRANSACTION;
	// Have we already fetched this object
	ObjectHashIterator objectIter = this->_objectHash.find(uuid);
	if( objectIter != this->_objectHash.end() )
	{
		ASSERT( objectIter->second != NULL );
		// Create a CoreObject(smart pointer) for the object
		object = objectIter->second->Reference();
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


const Result_t CoreProject::CreateObject(const MetaID_t &metaID, CoreObject* &object) throw()
{
	if( metaID == METAID_NONE ) return E_INVALID_USAGE;
	// Must be in a write transaction
	if( this->_transactionList.empty() || this->_transactionList.front().readOnly ) return E_TRANSACTION;
	// Create an object of the specified MetaID (call to ICoreStorage)
	Uuid uuid =  Uuid::Null();
	Result_t result = this->_storage->CreateObject(metaID, uuid);
	ASSERT( result == S_OK && uuid != Uuid::Null() );
	// Now create the corresponding CoreObjectBase and CoreObject pointer
	result = CoreObjectBase::Create(this, uuid, object);
	if( result != S_OK )
	{
		// Delete that object from storage
		ASSERT( this->_storage->DeleteObject() == S_OK );
		object = NULL;
		return result;
	}
	ASSERT( object != NULL );
	// Add the object into the created objects list of the transaction
	this->_transactionList.front().createdObjects.push_back( object->Base());
	return S_OK;
}


const Result_t CoreProject::RootObject(CoreObject* &coreObject) throw()
{
	// Get the root object from the storage
	Uuid uuid = Uuid::Null();
	ASSERT( this->_storage->RootUuid(uuid) == S_OK );
	ASSERT( uuid != Uuid::Null() );
	return this->Object(uuid, coreObject);
}


const Result_t CoreProject::DeleteObject(const Uuid &uuid) throw()
{
	if( uuid == Uuid::Null() ) return E_INVALID_USAGE;
	// Must be in a write transaction
	if( this->_transactionList.empty() || this->_transactionList.front().readOnly ) return E_TRANSACTION;
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

/*
const Result_t CoreProject::UndoTransaction(void) throw()
{
//	ASSERT( (int) undos.size() >= redo_count && redo_count >= 0 );
//
//	if( storage == NULL || InTransaction() || (int) undos.size() <= redo_count )
//		COMRETURN(E_INVALID_USAGE);
//
//	COMTRY
//	{
//		TryUndoTransaction();
//	}
//	COMCATCH(;)
	return S_OK;
}


const Result_t CoreProject::RedoTransaction(void) throw()
{
//	ASSERT( (int) undos.size() >= redo_count && redo_count >= 0 );
//	if( storage == NULL || InTransaction() || redo_count <= 0 )
//		COMRETURN(E_INVALID_USAGE);
//	COMTRY
//	{
//		TryRedoTransaction();
//	}
//	COMCATCH(;)
	return S_OK;
}


const Result_t CoreProject::FlushUndoQueue(void) throw()
{
//	ASSERT( (int) undos.size() >= redo_count && redo_count >= 0 );
//	if( storage == NULL || InTransaction() )
//		COMRETURN(E_INVALID_USAGE);
//	COMTRY
//	{
//		while( !undos.empty() )
//			DiscardUndo();
//	}
//	COMCATCH(;)
	return S_OK;
}


const Result_t CoreProject::FlushRedoQueue(void) throw()
{
//	ASSERT( (int) undos.size() >= redo_count && redo_count >= 0 );
//	if( storage == NULL || InTransaction() )
//		COMRETURN(E_INVALID_USAGE);
//	COMTRY
//	{
//		while( redo_count > 0 )
//			DiscardUndo();
//	}
//	COMCATCH(;)
	return S_OK;
}


const Result_t CoreProject::UndoQueueSize(uint16_t &size) const throw()
{
//	ASSERT( (int)this->_undos.size() >= this->_redoCount );
//	size = undos.size() - redo_count - (InTransaction() ? 1 : 0);
//	ASSERT( *p >= 0 );
	return S_OK;
}


const Result_t CoreProject::RedoQueueSize(uint16_t &size) const throw()
{
//	ASSERT( (int) undos.size() >= redo_count );
//	size = redo_count;
	return S_OK;
}

/*
void CoreProject::RegisterFinalTransactionItem(CoreFinalTransactionItem *modified) throw()
{
	ASSERT( this->InTransaction() );
	ASSERT( modified != NULL );

#ifdef DEBUG_CONTAINERS
	ASSERT( find(this->_finalTransactionItems.begin(), this->_finalTransactionItems.end(), modified) ==
		this->_finalTransactionItems.end() );
#endif

	this->_finalTransactionItems.push_front(modified);
}


void CoreProject::RegisterUndoItem(CoreUndoItem *modified) throw()
{
	ASSERT( this->InWriteTransaction() );
	ASSERT( modified != NULL );
	ASSERT( !this->_undos.empty() );

#ifdef DEBUG_CONTAINERS
	ASSERT( find(this->_undos.front().begin(), this->_undos.front().end(), modified) ==
		this->_undos.front().end() );
#endif

	this->_undos.front().push_front(modified);
}

/*
void CCoreProject::TryUndoTransaction()
{
	ASSERT( !InTransaction() );
	ASSERT( transactions.empty() );
	ASSERT( finaltr_items.empty() );

	try
	{
		ASSERT( storage );
		COMTHROW( storage->BeginTransaction() );

		transactions.push_front(transaction_type());
		transactions.front().readonly = true;

		ASSERT( !undos.empty() );

		undo_items_iterator i = undos.front().begin();
		undo_items_iterator e = undos.front().end();
		while( i != e )
		{
			ASSERT( (*i) != NULL );
			(*i)->UndoTransaction();

			++i;
		}

		ASSERT( transactions.size() == 1 );
		ASSERT( transactions.front().items.empty() );
		ASSERT( finaltr_items.empty() );

		transactions.pop_front();

		ASSERT( storage );
		COMTHROW( storage->CommitTransaction() );

		// nothing will throw here

		i = undos.front().begin();
		ASSERT( e == undos.front().end() );
		while( i != e )
		{
			ASSERT( (*i) != NULL );
			(*i)->UndoTransactionFinish();

			++i;
		}

		++redo_count;
		undos.splice(undos.end(), undos, undos.begin());
	}
	catch(hresult_exception &)
	{
		ASSERT(false);

		if( !transactions.empty() )
		{
			ASSERT( transactions.front().items.empty() );
			transactions.pop_front();
		}
		ASSERT( transactions.empty() );

		ASSERT( storage );
		storage->AbortTransaction();
	}

	ASSERT( transactions.empty() );
	ASSERT( finaltr_items.empty() );
}


void CCoreProject::TryRedoTransaction()
{
	ASSERT( !InTransaction() );
	ASSERT( transactions.empty() );
	ASSERT( finaltr_items.empty() );
	ASSERT( !undos.empty() );

	try
	{
		ASSERT( storage );
		COMTHROW( storage->BeginTransaction() );

		transactions.push_front(transaction_type());
		transactions.front().readonly = true;

		undo_items_iterator i = undos.back().begin();
		undo_items_iterator e = undos.back().end();
		while( i != e )
		{
			ASSERT( (*i) != NULL );
			(*i)->RedoTransaction();

			++i;
		}

		ASSERT( transactions.size() == 1 );
		ASSERT( transactions.front().items.empty() );
		ASSERT( finaltr_items.empty() );

		transactions.pop_front();

		ASSERT( storage );
		COMTHROW( storage->CommitTransaction() );

		// nothing will throw here

		i = undos.back().begin();
		ASSERT( e == undos.back().end() );
		while( i != e )
		{
			ASSERT( (*i) != NULL );
			(*i)->RedoTransactionFinish();

			++i;
		}

		--redo_count;
		undos.splice(undos.begin(), undos, --undos.end());
	}
	catch(hresult_exception &)
	{
		if( !transactions.empty() )
		{
			ASSERT( transactions.front().items.empty() );
			transactions.pop_front();
		}
		ASSERT( transactions.empty() );

		ASSERT( storage );
		storage->AbortTransaction();
	}

	ASSERT( transactions.empty() );
	ASSERT( finaltr_items.empty() );
}
*/

void CoreProject::DiscardUndo(void)
{
	ASSERT( this->_transactionList.empty() );
	if (this->_undoList.empty() )return;

	UndoItemsList discarded;
/*
//	try
//	{
//		ASSERT( this->_storage );
		this->_storage->BeginTransaction();

		this->_transactions.push_front( TransactionTypes() );
		this->_transactions.front().readonly = true;

		UndoItemsListIterator undoIter = this->_undos.back().begin();
		UndoItemsListIterator undoEnd = this->_undos.back().end();
		while( undoIter != undoEnd )
		{
			ASSERT( (*undoIter) != NULL );
			(*undoIter)->DiscardLastItem();
			discarded.push_front(*undoIter);
			++undoIter;
		}

		ASSERT( this->_transactions.size() == 1 );
		ASSERT( this->_transactions.front().items.empty() );

		// now the final transaction items contain the modified locks

		FinalTransactionItemsListIterator finalIter = this->_finalTransactionItems.begin();
		FinalTransactionItemsListIterator finalEnd = this->_finalTransactionItems.end();
		while( finalIter != finalEnd )
		{
			(*finalIter)->CommitFinalTransaction();
			++finalEnd;
		}

		this->_transactions.pop_front();

		ASSERT( this->_storage );
		this->_storage->CommitTransaction();

		// nothing will throw here

		// we clear undo info at attributes, before 
		undoIter = discarded.begin();
		undoEnd = discarded.end();
		while( undoIter != undoEnd )
		{
			ASSERT( (*undoIter) != NULL );
			(*undoIter)->DiscardLastItemFinish();
			++undoIter;
		}

		discarded.clear();

		// unloading by locking attributes
		finalIter = this->_finalTransactionItems.begin();
		ASSERT( finalEnd == this->_finalTransactionItems.end() );
		while( finalIter != finalEnd )
		{
			(*finalIter)->CommitFinalTransactionFinish(false);
			++finalEnd;
		}

		this->_finalTransactionItems.clear();

		if( --this->_redoCount < 0 )
			this->_redoCount = 0;

		this->_undos.pop_back();
	}
*/
	ASSERT( this->_transactionList.empty() );
	ASSERT( discarded.empty() );
}


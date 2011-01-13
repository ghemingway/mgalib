/*** Included Header Files ***/
#include "CoreStorage.h"


// Initialize the static member
std::map<std::string,CoreStorageFactory*> ICoreStorage::_storageFactories = std::map<std::string,CoreStorageFactory*>();


const Result_t ICoreStorage::RegisterStorage(const std::string &tag, CoreStorageFactory* factory) throw()
{
	if ( tag == "" ) return E_INVALID_USAGE;
	if ( factory == NULL ) return E_INVALID_USAGE;
	// Insert the tag/factory pair into the list
	std::pair<std::map<std::string,CoreStorageFactory*>::iterator,bool> retVal;
	retVal = ICoreStorage::_storageFactories.insert( std::make_pair(tag,factory) );
	// Make sure insertion was successful - failure means key alread is registered
	if ( !retVal.second ) return E_INVALID_USAGE;
	return S_OK;
}	


const Result_t ICoreStorage::ClearStorageRegistry(void) throw()
{
	// Need to delete all factories in the registry
	std::map<std::string,CoreStorageFactory*>::iterator mapIter = ICoreStorage::_storageFactories.begin();
	while ( mapIter != ICoreStorage::_storageFactories.end() ) {
		// Delete the factory
		if (mapIter->second != NULL) delete mapIter->second;
		// Move on to the next factory
		++mapIter;
	}
	// Clear the map
	ICoreStorage::_storageFactories.clear();
	// All is good
	return S_OK;
}


const Result_t ICoreStorage::Create(const std::string &tag, const std::string &filename, CoreMetaProject* metaProject, ICoreStorage* &storage, const bool &encrypted) throw()
{
	// Find the tag in the list
	std::map<std::string,CoreStorageFactory*>::iterator mapIter = ICoreStorage::_storageFactories.find(tag);
	if (mapIter != ICoreStorage::_storageFactories.end()) {
		// Create the storage
		return mapIter->second->Create(filename, metaProject, storage);
	}
	else {
		std::cout << "ICoreStorage::Create - not able to find factory for file type: " << tag << std::endl;
		return E_UNKNOWN_STORAGE;
	}
}


const Result_t ICoreStorage::Open(const std::string &tag, const std::string &filename, CoreMetaProject* metaProject, ICoreStorage* &storage, const std::vector<char> &encryptionKey) throw()
{
	// Find the tag in the list
	std::map<std::string,CoreStorageFactory*>::iterator mapIter = ICoreStorage::_storageFactories.find(tag);
	if (mapIter != ICoreStorage::_storageFactories.end()) {
		// Create the storage
		return mapIter->second->Open(filename, metaProject, storage);
	}
	else {
		std::cout << "ICoreStorage::Open - not able to find factory for file type: " << tag << std::endl;
		return E_UNKNOWN_STORAGE;
	}
}
/*** Included Header Files ***/
#include "MetaBase.h"
#include "MetaProject.h"
#include "MetaGeneric.h"
#include "MetaConstraint.h"


// --------------------------- Public MetaBase Methods --------------------------- //


MetaBase::MetaBase(CoreObject &coreObject, MetaProject* const &metaProject) :
_coreObject(coreObject), _metaProject(metaProject)
{
	ASSERT(coreObject != NULL);
	ASSERT(metaProject != NULL);
}


MetaBase::~MetaBase()
{
	ASSERT( this->_coreObject != NULL );
	// Delete the coreObject
	this->_coreObject = NULL;
}


const Result_t MetaBase::GetName(std::string &name) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_NAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaBase::SetName(const std::string &name) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_NAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaBase::GetDisplayedName(std::string &name) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_DISPNAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaBase::SetDisplayedName(const std::string &name) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_DISPNAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaBase::GetRegistryValue(const std::string &key, std::string &value) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	std::pair<std::string,std::string> pair(key, "");
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_REGISTRY, pair);
	txResult = this->_metaProject->CommitTransaction();
	value = pair.second;
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaBase::SetRegistryValue(const std::string &key, const std::string &value) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	std::pair<std::string,std::string> pair(key, value);
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_REGISTRY, pair);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaBase::GetObjType(ObjType_t &objType) const throw()
{
	MetaID_t metaID = METAID_NONE;
	Result_t result = this->_coreObject->GetMetaID(metaID);
	ASSERT( result == S_OK );
	ASSERT( metaID != METAID_NONE );
	if( metaID == METAID_METAPROJECT ) objType = (ObjType_t)OBJTYPE_FOLDER;
	else objType = (ObjType_t)(metaID - METAID_METABASE);
	return S_OK;
}


const Result_t MetaBase::GetConstraints(std::list<MetaConstraint*> &constraintList) throw()
{
	// Get the associated coreProject
	CoreProject* coreProject = NULL;
	Result_t result = this->_coreObject->Project(coreProject);
	ASSERT( result == S_OK );
	ASSERT( coreProject != NULL );
	// Get the collection from the attributes
	std::list<Uuid> idList;
	result = this->_coreObject->GetAttributeValue(ATTRID_CONSTRAINT_PTR + ATTRID_COLLECTION, idList);
	ASSERT( result == S_OK );
	std::list<Uuid>::iterator idIter = idList.begin();
	// Iterate over the list to gather the objects
	constraintList.clear();
	while (idIter != idList.end())
	{
		// Get the coreObject from this uuid
		CoreObject object;
		result = coreProject->Object(*idIter, object);
		ASSERT( result == S_OK );
		ASSERT( object != NULL );
		// Create the MetaConstraint with the coreObject and metaProject
		MetaConstraint* metaConstraint = new MetaConstraint(object, this->_metaProject);
		ASSERT( metaConstraint != NULL );
		constraintList.push_back(metaConstraint);
		// Move on to the next uuid
		++idIter;
	}
	return S_OK;
}

const Result_t MetaBase::CreateConstraint(MetaConstraint* &constraint) throw()
{
	// Get the associated coreProject
	CoreProject* coreProject = NULL;
	Result_t result = this->_coreObject->Project(coreProject);
	ASSERT( result == S_OK );
	ASSERT( coreProject != NULL );
	// Start a transaction
	result = coreProject->BeginTransaction(false);
	ASSERT( result == S_OK );
	// Create a METAID_METACONSTRAINT core object
	CoreObject coreObject;
	result = coreProject->CreateObject(METAID_METACONSTRAINT, coreObject);
	ASSERT( result == S_OK );
	ASSERT( coreObject != NULL );
	// Link the new child to this object as parent
	Uuid uuid = Uuid::Null();
	result = this->_coreObject->GetUuid(uuid);
	ASSERT( result == S_OK );
	ASSERT( uuid != Uuid::Null() );
	result = coreObject->SetAttributeValue(ATTRID_CONSTRAINT_PTR, uuid);
	ASSERT( result == S_OK );
	// Commit transaction at the CoreProject level
	result = coreProject->CommitTransaction();
	ASSERT( result == S_OK );
	// Now use the core object to create a MetaConstraint
	constraint = new MetaConstraint(coreObject, this->_metaProject);
	ASSERT( constraint != NULL );
	return S_OK;
}

/*
const Result_t MetaBase::Delete(void) throw()
{
	// { return ComDeleteObject(GetUnknown()); }
	return S_OK;
}
*/

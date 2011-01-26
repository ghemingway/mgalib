/*** Included Header Files ***/
#include "MetaBase.h"
#include "MetaProject.h"
#include "MetaGeneric.h"
#include "MetaConstraint.h"
#include "../Core/CoreProject.h"
#include "../Core/CoreObject.h"

// Included For Access to Traverse
#include "MetaAspect.h"
#include "MetaAttribute.h"
#include "MetaConnection.h"
#include "MetaFolder.h"
#include "MetaModel.h"
#include "MetaPart.h"
#include "MetaReference.h"
#include "MetaRole.h"
#include "MetaSet.h"


// --------------------------- Protected MetaBase Methods --------------------------- //

/*
void MetaBase::TraverseObject(MetaProject* &metaProject, CoreObject* &coreObject)
{
	ASSERT( metaProject != NULL );
	ASSERT( coreObject != NULL );
	// Determine the type of object
	Uuid uuid;
	ObjType_t objType;
	ASSERT( coreObject->GetUuid(uuid) == S_OK );
	MetaID_t metaID;
	ASSERT( coreObject->GetMetaID(metaID) == S_OK );
	if( metaID == METAID_METAPROJECT ) objType = (ObjType_t)OBJTYPE_FOLDER;
	else objType = (ObjType_t)(metaID - METAID_METABASE);
	switch(objType)
	{
		case OBJTYPE_ASPECT:
			MetaAspect::Traverse(metaProject, coreObject);
			break;
		case OBJTYPE_ATOM:
			MetaAtom::Traverse(metaProject, coreObject);
			break;
		case OBJTYPE_ATTRIBUTE:
			MetaAttribute::Traverse(metaProject, coreObject);
			break;
		case OBJTYPE_CONNECTION:
			MetaConnection::Traverse(metaProject, coreObject);
			break;
		case OBJTYPE_FOLDER:
			MetaFolder::Traverse(metaProject, coreObject);
			break;
		case OBJTYPE_MODEL:
			MetaModel::Traverse(metaProject, coreObject);
			break;
		case OBJTYPE_PART:
			MetaPart::Traverse(metaProject, coreObject);
			break;
		case OBJTYPE_REFERENCE:
			MetaReference::Traverse(metaProject, coreObject);
			break;
		case OBJTYPE_ROLE:
			MetaRole::Traverse(metaProject, coreObject);
			break;
		case OBJTYPE_SET:
			MetaSet::Traverse(metaProject, coreObject);
			break;
		default:
			delete coreObject;
			break;
	}
/*
	else
	{
		CComObjPtr<IMgaMetaConnJoint> connjoint;
		CComObjPtr<IMgaMetaPointerSpec> ptrspec;
		CComObjPtr<IMgaMetaRegNode> regnode;
		
		if( SUCCEEDED(QueryInterface(me, connjoint)) )
			CMgaMetaConnJoint::Traverse(metaproject, me);
		else if( SUCCEEDED(QueryInterface(me, ptrspec)) )
			CMgaMetaPointerSpec::Traverse(metaproject, me);
		else if( SUCCEEDED(QueryInterface(me, regnode)) )
			CMgaMetaRegNode::Traverse(metaproject, me);
	}
}


void MetaBase::TraverseCollection(MetaProject* &metaProject, CoreObject* &coreObject, const AttrID_t &attrID)
{
	ASSERT( metaProject != NULL );
	ASSERT( coreObject != NULL );
	// Get the associated coreProject
	CoreProject* coreProject = NULL;
	ASSERT( coreObject->Project(coreProject) == S_OK );
	ASSERT( coreProject != NULL );
	// Get the collection from the given attribute (make sure to add ATTRID_COLLECTION)
	std::list<Uuid> idList;
	ASSERT( coreObject->GetAttributeValue(attrID+ATTRID_COLLECTION, idList) == S_OK );
	std::list<Uuid>::iterator idIter = idList.begin();
	while (idIter != idList.end())
	{
		// Create the coreObject from this pointer
		CoreObject* object = NULL;
		ASSERT( coreProject->Object(*idIter, object) == S_OK );
		ASSERT( object != NULL );
		MetaBase::TraverseObject(metaProject, object);
		// Move on to the next pair
		++idIter;
	}
}
*/

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


const Result_t MetaBase::GetUuid(Uuid &uuid) const throw()
{
	ASSERT( this->_coreObject != NULL );
	return this->_coreObject->GetUuid(uuid);
}


const Result_t MetaBase::GetMetaProject(MetaProject* &project) const throw()
{
	project = this->_metaProject; return S_OK;
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

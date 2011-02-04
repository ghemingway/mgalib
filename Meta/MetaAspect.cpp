/*** Included Header Files ***/
#include "MetaAspect.h"
#include "MetaGeneric.h"
#include "MetaRole.h"
#include "MetaPart.h"
#include "MetaModel.h"


// --------------------------- Public MetaAspect Methods --------------------------- //


const Result_t MetaAspect::GetParentModel(MetaModel* &metaModel) const throw()
{
	// Use the MetaBase helper function to get this pointer attribute
	return MetaBase::ObjectFromAttribute(this->_coreObject, this->_metaProject, ATTRID_ASPECTS_COLL, metaModel);
}


const Result_t MetaAspect::GetParts(std::list<MetaPart*> &partList)	const throw()
{
	// Use the MetaBase helper function to get this collection attribute
	return MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_PARTASPECT_PTR, partList);
}


const Result_t MetaAspect::AddAttribute(MetaAttribute* &metaAttribute) throw()
{
	// Use the MetaBase helper function to add a link between the attribute and the aspect
	return MetaBase::AddLink(this->_coreObject, this->_metaProject, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, metaAttribute);
}


const Result_t MetaAspect::CreatePart(MetaRole* &metaRole, MetaPart* &metaPart) throw()
{
	// Get the associated coreProject
	CoreProject* coreProject = NULL;
	Result_t result = this->_coreObject->Project(coreProject);
	ASSERT( result == S_OK );
	ASSERT( coreProject != NULL );
	// Start a transaction
	result = coreProject->BeginTransaction(false);
	ASSERT( result == S_OK );
	// Create a new coreObject
	CoreObject newCoreObject;
	result = coreProject->CreateObject(METAID_METAPART, newCoreObject);
	ASSERT( result == S_OK );
	ASSERT( newCoreObject != NULL );

	// Link the new Part to this aspect
	Uuid uuid = Uuid::Null();
	result = this->_coreObject->GetUuid(uuid);
	ASSERT( result == S_OK );
	ASSERT( uuid != Uuid::Null() );
	result = newCoreObject->SetAttributeValue(ATTRID_PARTASPECT_PTR, uuid);
	ASSERT( result == S_OK );

	// Link the new Part object to the metaRole provided
	result = metaRole->GetUuid(uuid);
	ASSERT( result == S_OK );
	ASSERT( uuid != Uuid::Null() );
	result = newCoreObject->SetAttributeValue(ATTRID_PARTROLE_PTR, uuid);
	ASSERT( result == S_OK );
	
	// Commit transaction at the CoreProject level
	result = coreProject->CommitTransaction();
	ASSERT( result == S_OK );

	// Now use the core object to create a new metaObject of type T
	metaPart = new MetaPart(newCoreObject, this->_metaProject);
	ASSERT( metaPart != NULL );
	return S_OK;
}
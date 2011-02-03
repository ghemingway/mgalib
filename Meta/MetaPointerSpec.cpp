/*** Included Header Files ***/
#include "MetaPointerSpec.h"
#include "MetaPointerItem.h"
#include "MetaGeneric.h"


// --------------------------- MetaPointerSpec --------------------------- //


MetaPointerSpec::MetaPointerSpec(CoreObject &coreObject, MetaProject* const &metaProject) :
	_coreObject(coreObject), _metaProject(metaProject)
{
	ASSERT(coreObject != NULL);
	ASSERT(metaProject != NULL);
}


const Result_t MetaPointerSpec::GetParent(MetaBase* &parent) throw()
{
	// Use the MetaBase helper function to get an object from this pointer attribute
	return MetaBase::ObjectFromAttribute(this->_coreObject, this->_metaProject, ATTRID_PTRSPECS_COLL, parent);
}


const Result_t MetaPointerSpec::GetName(std::string &name) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_PTRSPECNAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}

const Result_t MetaPointerSpec::SetName(const std::string &name) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_PTRSPECNAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaPointerSpec::GetItems(std::list<MetaPointerItem*> &pointerList) const throw()
{
	// Use the MetaBase helper function to get collection of objects
	return MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_PTRITEMS_COLL, pointerList);
}


const Result_t MetaPointerSpec::CreateItem(MetaPointerItem* &pointerItem) throw()
{
	// Use the helper function to create a new object
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METAPOINTERITEM, ATTRID_PTRITEMS_COLL, pointerItem);
}


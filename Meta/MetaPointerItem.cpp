/*** Included Header Files ***/
#include "MetaPointerItem.h"
#include "MetaBase.h"
#include "MetaGeneric.h"
#include "MetaPointerSpec.h"


// --------------------------- Public MetaPointerItem Methods --------------------------- //


MetaPointerItem::MetaPointerItem(CoreObject &coreObject, MetaProject* const &metaProject) :
	_coreObject(coreObject), _metaProject(metaProject)
{
	ASSERT(coreObject != NULL);
	ASSERT(metaProject != NULL);
}


const Result_t MetaPointerItem::GetDescription(std::string &description) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_PTRITEMDESC, description);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaPointerItem::SetDescription(const std::string &description) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_PTRITEMDESC, description);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaPointerItem::GetParent(MetaPointerSpec* &parent) throw()
{
	// Use the MetaBase helper function to get an object from this pointer attribute
	return MetaBase::ObjectFromAttribute(this->_coreObject, this->_metaProject, ATTRID_PTRITEMS_COLL, parent);
}


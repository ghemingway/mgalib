/*** Included Header Files ***/
#include "MetaEnumItem.h"
#include "MetaGeneric.h"
#include "MetaAttribute.h"


// --------------------------- MetaConstraint --------------------------- //


MetaEnumItem::MetaEnumItem(CoreObject &coreObject, MetaProject* &metaProject) :
_coreObject(coreObject), _metaProject(metaProject)
{
	ASSERT(coreObject != NULL);
	ASSERT(metaProject != NULL);
}


const Result_t MetaEnumItem::GetParent(MetaAttribute* &metaAttribute) throw()
{
	Result_t result = this->_metaProject->BeginTransaction();
	ASSERT( result == S_OK );
	Uuid parentUuid = Uuid::Null();
	result = this->_coreObject->GetAttributeValue(ATTRID_ENUMITEMS_COLL, parentUuid);
	ASSERT( result == S_OK );
	result = this->_metaProject->CommitTransaction();
	ASSERT( result == S_OK );

	// Is the Uuid valid
	if (parentUuid == Uuid::Null()) metaAttribute = NULL;
	// Convert this Uuid into a MetaAttribute
	else
	{
		// Get the coreProject and coreObject for the parent metaAttribute
		CoreProject *coreProject = NULL;
		result = this->_coreObject->Project(coreProject);
		ASSERT( result == S_OK );
		ASSERT( coreProject != NULL );
		CoreObject parentObject;
		result = coreProject->Object(parentUuid, parentObject);
		ASSERT( result == S_OK );
		ASSERT( parentObject != NULL );
		// Create the MetaAttribute with the coreObject and metaProject
		metaAttribute = new MetaAttribute(parentObject, this->_metaProject);
		ASSERT( metaAttribute != NULL );
	}
	return S_OK;
}


const Result_t MetaEnumItem::GetDisplayedName(std::string &name) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_NAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaEnumItem::SetDisplayedName(const std::string &name) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_NAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}

const Result_t MetaEnumItem::GetValue(std::string &value) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_VALUE, value);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaEnumItem::SetValue(const std::string &value) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_VALUE, value);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}
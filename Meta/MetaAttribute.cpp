/*** Included Header Files ***/
#include "MetaAttribute.h"
#include "MetaGeneric.h"
#include "MetaEnumItem.h"
#include "MetaFCO.h"


// --------------------------- Public MetaAttribute Methods --------------------------- //


const Result_t MetaAttribute::GetDefinedIn(MetaFCO* &fco) const throw()
{
	// Use the MetaBase helper function to get an FCO from this pointer attribute
	return MetaBase::ObjectFromAttribute(this->_coreObject, this->_metaProject, ATTRID_DEFATTR_PTR, fco);
}


const Result_t MetaAttribute::GetEnumItems(std::list<MetaEnumItem*> &enumList) const throw()
{
	// Use the MetaBase helper function to get collection of MetaEnumItems
	return MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_ENUMITEMS_COLL, enumList);
}


const Result_t MetaAttribute::GetUsedIn(std::list<MetaFCO*> &fcoList) const throw()
{
	// Use the MetaBase helper function to get collection of UsedIn FCOs
	return MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_ATTRLINK_ATTR_PTR, fcoList);
}


const Result_t MetaAttribute::GetValueType(AttVal_t &valueType) const throw() 
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longValueType;
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_ATTVALTYPE, longValueType);
	valueType = static_cast<AttVal_t>(longValueType);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaAttribute::SetValueType(const AttVal_t &valueType) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longValueType = static_cast<int32_t>(valueType);
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_ATTVALTYPE, longValueType);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaAttribute::GetDefaultValue(std::string &value) const throw() 
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_VALUE, value);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaAttribute::SetDefaultValue(const std::string &value) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_VALUE, value);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
	
}


const Result_t MetaAttribute::GetViewable(bool &flag) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longViewable;
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_VIEWABLE, longViewable);
	flag = (longViewable == false);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaAttribute::SetViewable(const bool &flag) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longViewable = static_cast<int32_t>(flag);
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_VIEWABLE, longViewable);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaAttribute::CreateEnumItem(MetaEnumItem* &enumItem) throw()
{
	// Use the MetaBase helper function to create a new attribute
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METAENUMITEM, ATTRID_ENUMITEMS_COLL, enumItem);
}


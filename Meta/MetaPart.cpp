/*** Included Header Files ***/
#include "MetaPart.h"
#include "MetaGeneric.h"
#include "MetaRole.h"


/*** Locally Defined Values ***/
#define METAPART_PRIMARY	0x0001
#define METAPART_LINKED		0x0002


// --------------------------- MetaPart --------------------------- //


const Result_t MetaPart::GetRole(MetaRole* &metaRole) const throw()
{
	// Use the MetaBase helper function to get an object from this pointer attribute
	return MetaBase::ObjectFromAttribute(this->_coreObject, this->_metaProject, ATTRID_PARTROLE_PTR, metaRole);
}


const Result_t MetaPart::GetParentAspect(MetaAspect* &MetaAspect) const throw()
{
	// Use the MetaBase helper function to get an object from this pointer attribute
	return MetaBase::ObjectFromAttribute(this->_coreObject, this->_metaProject, ATTRID_PARTASPECT_PTR, MetaAspect);
}


const Result_t MetaPart::GetKindAspect(std::string &kind) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_KINDASPECT, kind);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaPart::SetKindAspect(const std::string &kind) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_KINDASPECT, kind);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaPart::GetName(std::string &name) const throw()
{
	MetaRole* role;
	Result_t result = this->GetRole(role);
	if (result != S_OK) return result;
	result = role->GetName(name);
	delete role;
	return result;
}


const Result_t MetaPart::GetObjType(ObjType_t &objType) const throw()
{
	ASSERT(false);
//	return ComGetObjType(GetUnknown(), p);
	return S_OK;
}


const Result_t MetaPart::GetIsPrimary(bool &isPrimary) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longValue;
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_PARTDATA, longValue);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	isPrimary = (longValue & METAPART_PRIMARY) ? true : false;
	return result;
}


const Result_t MetaPart::SetIsPrimary(const bool &isPrimary) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longValue;
	// Get the current value
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_PARTDATA, longValue);
	if (result != S_OK) return result;
	// Add in the isLinked value
	if(!isPrimary) longValue &= (~METAPART_PRIMARY);
	else longValue |= METAPART_PRIMARY;
	// Now set the updated value
	result = this->_coreObject->SetAttributeValue(ATTRID_PARTDATA, longValue);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaPart::GetIsLinked(bool &isLinked) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longValue;
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_PARTDATA, longValue);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	isLinked = (longValue & METAPART_LINKED) ? true : false;
	return result;
}



const Result_t MetaPart::SetIsLinked(const bool &isLinked) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longValue;
	// Get the current value
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_PARTDATA, longValue);
	if (result != S_OK) return result;
	// Add in the isLinked value
	if(!isLinked) longValue &= (~METAPART_LINKED);
	else longValue |= METAPART_LINKED;
	// Now set the updated value
	result = this->_coreObject->SetAttributeValue(ATTRID_PARTDATA, longValue);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


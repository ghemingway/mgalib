/*** Included Header Files ***/
#include "MetaFCO.h"
#include "MetaAttribute.h"
#include "MetaGeneric.h"


// --------------------------- Public MetaFCO Methods --------------------------- //


const Result_t MetaFCO::GetDefinedIn(MetaBase* &metaBase) const throw()
{
	// Use the helper function to get an object from this pointer attribute
	return MetaBase::ObjectFromAttribute(this->_coreObject, this->_metaProject, ATTRID_DEFFCO_PTR, metaBase);
}


const Result_t MetaFCO::GetDefinedAttributes(std::list<MetaAttribute*> &attribList) const throw()
{
	// Use the helper function to get collection of objects
	return MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_DEFATTR_PTR, attribList);
}


const Result_t MetaFCO::GetDefinedAttributeByName(const std::string &name, const bool &inScope, MetaAttribute* &attrib) const throw()
{
	// Use the helper function to get collection of objects
	std::list<MetaAttribute*> attribList;
	Result_t result = MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_DEFATTR_PTR, attribList);
	ASSERT( result == S_OK );
	// Iterate through the list to find the one with name
	attrib = NULL;
	std::list<MetaAttribute*>::iterator attribListIter = attribList.begin();
	while (attribListIter != attribList.end())
	{
		// Does the name match
		std::string attribName;
		result = (*attribListIter)->GetName(attribName);
		ASSERT( result == S_OK );
		if (attribName == name) attrib = *attribListIter;
		// Otherwise, delete this item
		else delete *attribListIter;
		// Move on to the next item in the list
		++attribListIter;
	}
	// Did we find it
	if (attrib == NULL) return E_NOTFOUND;
	return S_OK;
}


const Result_t MetaFCO::GetUsedInRoles(std::list<MetaRole*> &rolesList) const throw()
{
	// Use the helper function to get collection of objects
	return MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_KIND_PTR, rolesList);
}


const Result_t MetaFCO::GetUsedInFolders(std::list<MetaFolder*> &folderList) const throw()
{
	ASSERT(false);
//	return ComGetLinkCollectionValue<IMgaMetaFolder>(GetUnknown(), ATTRID_ROOTOBJLINK_OBJECT_PTR, ATTRID_ROOTOBJLINK_FOLDER_PTR, p);
	return S_OK;
}


const Result_t MetaFCO::GetAttributes(std::list<MetaAttribute*> &attribList) const throw()
{
	ASSERT(false);
//	return ComGetSortedLinkCollValue<IMgaMetaAttribute>(GetUnknown(),	ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p);
	return S_OK;
}


const Result_t MetaFCO::GetAttributeByName(const std::string &name, MetaAttribute* &attrib) const throw()
{
	// Use the helper function to get the object from a collection by name
	return MetaBase::ObjectFromCollectionByName(this->_coreObject, this->_metaProject, ATTRID_DEFATTR_PTR, ATTRID_NAME, name, attrib);
}


const Result_t MetaFCO::GetAttributeByRef(const Uuid &uuid, MetaAttribute* &attrib) const throw()
{
	ASSERT(false);
	return S_OK;
}


const Result_t MetaFCO::CreateAttribute(MetaAttribute* &metaAttribute) throw()
{
	// Use the MetaBase helper function to create a new attribute
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METAATTRIBUTE, ATTRID_DEFATTR_PTR, metaAttribute);
}


const Result_t MetaFCO::AddAttribute(MetaAttribute* &metaAttribute) throw()
{
	// Use the MetaBase helper function to add a link between the attribute and the aspect
	return MetaBase::AddLink(this->_coreObject, this->_metaProject, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, metaAttribute);
}


const Result_t MetaFCO::GetAliasingEnabled(bool &flag) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longAlias;
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_ALIASENABLED, longAlias);
	flag = (longAlias == false);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaFCO::SetAliasingEnabled(const bool &flag) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longAlias = static_cast<int32_t>(flag);
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_ALIASENABLED, longAlias);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


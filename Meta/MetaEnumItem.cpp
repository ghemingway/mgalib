/*** Included Header Files ***/
#include "MetaEnumItem.h"


// --------------------------- MetaConstraint --------------------------- //


MetaEnumItem::MetaEnumItem(CoreObject &coreObject, MetaProject* &metaProject) :
_coreObject(coreObject), _metaProject(metaProject)
{
	ASSERT(coreObject != NULL);
	ASSERT(metaProject != NULL);
}


const Result_t MetaEnumItem::GetParent(MetaAttribute* &metaAttribute) const throw()
{
	ASSERT(false);
//	return ComGetPointerValue(GetUnknown(), ATTRID_ENUMITEMS_COLL, p);
	return S_OK;
}


const Result_t MetaEnumItem::GetDisplayedName(std::string &name) const throw()
{
	ASSERT(false);
//	return ComGetAttrValue(GetUnknown(), ATTRID_NAME, p);
	return S_OK;
}


const Result_t MetaEnumItem::SetDisplayedName(const std::string &name) throw()
{
	ASSERT(false);
//	return ComPutAttrValue(GetUnknown(), ATTRID_NAME, p);
	return S_OK;
}

const Result_t MetaEnumItem::GetValue(std::string &value) const throw()
{
	ASSERT(false);
//	return ComGetAttrValue(GetUnknown(), ATTRID_VALUE, p);
	return S_OK;
}


const Result_t MetaEnumItem::SetValue(const std::string &value) throw()
{
	ASSERT(false);
//	return ComPutAttrValue(GetUnknown(), ATTRID_VALUE, p);
	return S_OK;
}
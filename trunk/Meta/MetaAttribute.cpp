/*** Included Header Files ***/
#include "MetaAttribute.h"
#include "MetaGeneric.h"
#include "../Core/CoreObject.h"


// --------------------------- Public MetaAttribute Methods --------------------------- //


const Result_t MetaAttribute::GetDefinedIn(MetaFCO* &fco) const throw()
{
	// Use the MetaBase helper function to get an FCO from this pointer attribute
	return this->ObjectFromAttribute(ATTRID_DEFATTR_PTR, fco);
}


const Result_t MetaAttribute::GetEnumItems(std::list<MetaEnumItem*> &enumList) const throw()
{
	// Use the MetaBase helper function to get collection of MetaEnumItems
	return this->CollectionFromAttribute(ATTRID_ENUMITEMS_COLL, enumList);
}


const Result_t MetaAttribute::GetUsedIn(std::list<MetaFCO*> &fcoList) const throw()
{
	std::list<Uuid> attrList;
	this->_coreObject->GetAttributeValue(ATTRID_ATTRLINK_ATTR_PTR, attrList);
	ASSERT(false);
//	typedef CCoreCollection<MetaFCOs, std::vector<IMgaMetaFCO*>, IMgaMetaFCO, IMgaMetaFCO> COMTYPE;
//	CComObjPtr<COMTYPE> coll;
//	CreateComObject(coll);
//	coreobjects_iterator i = coreobjects.begin();
//	coreobjects_iterator e = coreobjects.end();
//	while( i != e )
//	{
//		CCoreObjectPtr obj;
//		(*i).GetPointerValue(ATTRID_ATTRLINK_USEDIN_PTR, obj);
//		CComObjPtr<IMgaMetaFCO> q;
//		if( SUCCEEDED( ::QueryInterface(obj, q)) )
//			coll->Add(q);
//		++i;
//	}
//	MoveTo(coll, p);

//	return this->CollectionFromAttribute(ATTRID_ATTRLINK_ATTR_PTR, fcoList);
	return S_OK;
}


const Result_t MetaAttribute::GetValueType(AttVal_t &valueType) const throw() 
{
	int32_t value;
	// Get the value type from the coreObject
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_ATTVALTYPE, value);
	// A bit of silly conversion magic
	if (result == S_OK) valueType = (AttVal_t)value;
	return result;
}


const Result_t MetaAttribute::SetValueType(const AttVal_t &type) throw()
{
	ASSERT(false);
	//	ASSERT( this->_metaProject->BeginTransaction() == S_OK );
	//	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_ATTVALTYPE, type);
	//	ASSERT( this->_metaProject->CommitTransaction() == S_OK );
	//	return result;
	return S_OK;
}


const Result_t MetaAttribute::GetDefaultValue(bool &value) const throw() 
{
	ASSERT(false);
	std::string strValue;
	// Get the attribute value
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_VALUE, strValue);
	if (result != S_OK ) return result;
	// Get the value type
	int32_t valueType;
	result = this->_coreObject->GetAttributeValue(ATTRID_ATTVALTYPE, valueType);
	// Change the value into the correct value type
//	ChangeAttrValueType(v, attval);
	return S_OK;
}


const Result_t MetaAttribute::SetDefaultValue(const std::string &name) throw()
{
	ASSERT( this->_metaProject->BeginTransaction() == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_NAME, name);
	ASSERT( this->_metaProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaAttribute::GetViewable(bool &flag) const throw()
{
	int32_t longValue;
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_VIEWABLE, longValue);
	// A bit of silly conversion magic
	if (result == S_OK) flag = (bool)longValue;
	return result;
}


const Result_t MetaAttribute::SetViewable(const bool &flag) throw()
{
	ASSERT(false);
//	return ComPutAttrValue(GetUnknown(), ATTRID_VIEWABLE, (p == VARIANT_FALSE)?0L:1L);
	return S_OK;
}


const Result_t MetaAttribute::CreateEnumItem(std::list<int> &list) throw()
{
	ASSERT(false);
//	return ComCreateMetaObj(GetUnknown(), METAID_METAENUMITEM, ATTRID_ENUMITEMS_COLL, p);
	return S_OK;
}


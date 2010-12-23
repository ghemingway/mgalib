/*** Included Header Files ***/
#include "MetaAttribute.h"
#include "MetaGeneric.h"
#include "../Core/CoreObject.h"


// --------------------------- Public MetaAttribute Methods --------------------------- //


void MetaAttribute::Traverse(MetaProject *metaProject, CoreObject* &coreObject)
{
	ASSERT( metaProject != NULL );
	ASSERT( coreObject != NULL );
	// Traverse the base class
	MetaBase::Traverse(metaProject, coreObject);
	// Traverse any children
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_ATTRLINK_ATTR_PTR);
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_ENUMITEMS_COLL);
}


const Result_t MetaAttribute::GetDefinedIn(MetaFCO* &fco) const throw()
{
	// Use the MetaBase helper function to get an FCO from this pointer attribute
	return this->ObjectFromAttribute(ATTRID_DEFATTR_PTR, fco);
}


const Result_t MetaAttribute::GetEnumItems(std::list<MetaEnumItem*> &enumList) const throw()
{
	// Use the MetaBase helper function to get collection of MetaEnumItems
//	return this->CollectionFromAttribute(ATTRID_ENUMITEMS_COLL, enumList);
	return S_OK;
}


const Result_t MetaAttribute::GetUsedIn(std::list<MetaFCO*> &fcoList) const throw()
{
	std::list<MetaObjIDPair> attrList;
	this->_coreObject->GetAttributeValue(ATTRID_ATTRLINK_ATTR_PTR, attrList);
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


const Result_t MetaAttribute::GetDefaultValue(bool &value) const throw() 
{
	std::string strValue;
	// Get the attribute value
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_VALUE, strValue);
	if (result != S_OK ) return result;
	int32_t valueType;
	result = this->_coreObject->GetAttributeValue(ATTRID_ATTVALTYPE, valueType);
//	ChangeAttrValueType(v, attval);
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


const Result_t MetaAttribute::GetViewable(bool &flag) const throw()
{
	int32_t longValue;
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_VIEWABLE, longValue);
	// A bit of silly conversion magic
	if (result == S_OK) flag = (bool)longValue;
	return result;
}


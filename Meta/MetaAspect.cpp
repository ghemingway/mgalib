/*** Included Header Files ***/
#include "MetaAspect.h"
#include "MetaGeneric.h"


// --------------------------- MetaAspect --------------------------- //


const Result_t MetaAspect::GetParentModel(MetaModel* &metaModel) const throw()
{
	// Use the MetaBase helper function to get this pointer attribute
	return this->ObjectFromAttribute(ATTRID_ASPECTS_COLL, metaModel);
}


const Result_t MetaAspect::GetParts(std::list<MetaPart*> &partList)	const throw()
{
	// Use the MetaBase helper function to get this collection attribute
	return this->CollectionFromAttribute(ATTRID_PARTASPECT_PTR, partList);
}


const Result_t MetaAspect::AddAttribute(MetaAttribute* &attrib) throw()
{
	ASSERT(false);
//	return ComAddLink(this, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p);
	return S_OK;
}


const Result_t MetaAspect::RemoveAttribute(MetaAttribute* &attrib) throw()
{
	ASSERT(false);
//	return ComRemoveLink(this, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p);
	return S_OK;
}


const Result_t MetaAspect::CreatePart(MetaRole* &role, MetaPart* &metaPart) throw()
{
	ASSERT(false);
	//	if( role == NULL ) return E_POINTER;
	//	ASSERT( this->_metaProject != NULL );
	//	CCoreObjectPtr other(role);
	//	ASSERT( other != NULL );
	//	CCoreObjectPtr part;
	//	metaproject->CreateMetaBase(METAID_METAPART, part);
	//	part.PutPointerValue(ATTRID_PARTASPECT_PTR, self);
	//	part.PutPointerValue(ATTRID_PARTROLE_PTR, other);
	//	COMTHROW( ::QueryInterface(part, p) );
	return S_OK;
}
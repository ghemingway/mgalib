/*** Included Header Files ***/
#include "MetaAspect.h"
#include "MetaGeneric.h"


// --------------------------- MetaAspect --------------------------- //


void MetaAspect::Traverse(MetaProject* &metaProject, CoreObject* &coreObject)
{
	ASSERT( metaProject != NULL );
	ASSERT( coreObject != NULL );
	// Traverse the base class
	MetaBase::Traverse(metaProject, coreObject);
	// Traverse any children
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_PARTASPECT_PTR);
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_ATTRLINK_USEDIN_PTR);
}


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


/*
const Result_t MetaAspect::CreatePart(MetaRole* &role, MetaPart* &metaPart) throw()
{
	if( role == NULL ) return E_POINTER;
	ASSERT( this->_metaProject != NULL );
//	CCoreObjectPtr other(role);
//	ASSERT( other != NULL );
//	CCoreObjectPtr part;
//	metaproject->CreateMetaBase(METAID_METAPART, part);
//	part.PutPointerValue(ATTRID_PARTASPECT_PTR, self);
//	part.PutPointerValue(ATTRID_PARTROLE_PTR, other);
//	COMTHROW( ::QueryInterface(part, p) );
	return S_OK;
}
*/

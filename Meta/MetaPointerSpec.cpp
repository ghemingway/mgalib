/*** Included Header Files ***/
#include "MetaPointerSpec.h"
#include "MetaGeneric.h"
//#include "MetaPointerItem.h"


// --------------------------- MetaPointerSpec --------------------------- //

/*
void MetaPointerSpec::Traverse(MetaProject* &metaProject, CoreObject* &coreObject)
{
	ASSERT( metaProject != NULL );
	ASSERT( coreObject != NULL );
	// Traverse any children
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_PTRITEMS_COLL);
}
*/
/*
bool MetaPointerSpec::CheckPath(CCoreObjectPtr &self, pathitems_type &pathitems, bool global)
{
	ASSERT( self != NULL );
	
	coreobjects_type coreobjects;
	self.GetCollectionValue(ATTRID_PTRITEMS_COLL, coreobjects);
	
	coreobjects_iterator i = coreobjects.begin();
	coreobjects_iterator e = coreobjects.end();
	while( i != e )
	{
		if( CMgaMetaPointerItem::CheckPath(*i, pathitems, global) )
			return true;
		
		++i;
	}
	
	return false;
}
*/
/*
const Result_t MetaPointerSpec::GetParent(IDispatch **p)
{
	CHECK_OUT(p);

	{
		CComObjPtr<IMgaMetaReference> self;
		if( SUCCEEDED(::QueryInterface(GetUnknown(), self)) )
			return ::QueryInterface(GetUnknown(), p);
	}

	{
		CComObjPtr<IMgaMetaSet> self;
		if( SUCCEEDED(::QueryInterface(GetUnknown(), self)) )
			return ::QueryInterface(GetUnknown(), p);
	}

	return ComGetPointerValue(GetUnknown(), ATTRID_PTRSPECS_COLL,p);
}
*/

const Result_t MetaPointerSpec::GetName(std::string &name) const throw()
{
//	return ComGetAttrValue(GetUnknown(), ATTRID_PTRSPECNAME, p);
	return S_OK;
}


const Result_t MetaPointerSpec::GetItems(std::list<MetaPointerItem*> &pointerList) const throw()
{
//	return ComGetCollectionValue<IMgaMetaPointerItem>(GetUnknown(), ATTRID_PTRITEMS_COLL, p);
	return S_OK;
}

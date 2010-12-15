/*** Included Header Files ***/
#include "MetaFolder.h"
#include "MetaProject.h"
#include "MetaGeneric.h"
#include "../Core/CoreObject.h"


// --------------------------- MetaFolder --------------------------- //


void MetaFolder::Traverse(MetaProject* &metaProject, CoreObject* &coreObject)
{
	ASSERT( metaProject != NULL );
	ASSERT( coreObject != NULL );
	// Traverse the base object
	MetaBase::Traverse(metaProject, coreObject);
	// Traverse any children
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_DEFFOLDER_PTR);
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_DEFFCO_PTR);
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_DEFATTR_PTR);
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_FOLDERLINK_CHILD_PTR);
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_ROOTOBJLINK_FOLDER_PTR);
}


const Result_t MetaFolder::GetDefinedIn(MetaFolder* &folder) const throw()
{
//	return ComGetPointerValue(GetUnknown(), ATTRID_DEFFOLDER_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetDefinedFolders(std::list<MetaFolder*> &folderList) const throw()
{
//	return ComGetCollectionValue<IMgaMetaFolder>(GetUnknown(), ATTRID_DEFFOLDER_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetDefinedFCOs(std::list<MetaFCO*> &fcoList) const throw()
{
//	return ComGetSortedCollValue<IMgaMetaFCO>(GetUnknown(), ATTRID_DEFFCO_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetDefinedAttributes(std::list<MetaAttribute*> &attribList) const throw()
{
//	return ComGetSortedCollValue<IMgaMetaAttribute>(GetUnknown(), ATTRID_DEFATTR_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetLegalChildFolders(std::list<MetaFolder*> &folderList) const throw()
{
//	return ComGetSortedLinkCollValue<IMgaMetaFolder>(GetUnknown(),ATTRID_FOLDERLINK_PARENT_PTR, ATTRID_FOLDERLINK_CHILD_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetLegalChildFolderByName(const std::string &name, MetaFolder* &metaFolder) const throw()
{
//	return ComGetLinkCollValueByName(name, GetUnknown(), ATTRID_FOLDERLINK_PARENT_PTR, ATTRID_FOLDERLINK_CHILD_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetLegalRootObjects(std::list<MetaFCO*> &fcoList) const throw()
{
//	return ComGetSortedLinkCollValue<IMgaMetaFCO>(GetUnknown(), ATTRID_ROOTOBJLINK_FOLDER_PTR, ATTRID_ROOTOBJLINK_OBJECT_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetLegalRootObjectByName(const std::string &name, MetaFCO* &metaFCO) const throw()
{
//	return ComGetLinkCollValueByName(name, GetUnknown(), ATTRID_ROOTOBJLINK_FOLDER_PTR, ATTRID_ROOTOBJLINK_OBJECT_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetUsedInFolders(std::list<MetaFolder*> &folderList) const throw()
{
//	return ComGetLinkCollectionValue<IMgaMetaFolder>(GetUnknown(), ATTRID_FOLDERLINK_CHILD_PTR, ATTRID_FOLDERLINK_PARENT_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetDefinedFolderByName(const std::string &name, const bool &inScope, MetaFolder* &metaFolder) const throw()
{
//	CCoreObjectPtr self(GetUnknown());
//	CComBstrObj_lightequal equal( GetUnknown());
//	coreobjects_type coreobjects;
//	self.GetCollectionValue(ATTRID_DEFFOLDER_PTR, coreobjects);
//	coreobjects_iterator i = coreobjects.begin();
//	coreobjects_iterator e = coreobjects.end();
//	while( i != e )
//	{
//			CComBstrObj n;
//			(*i).GetStringValue(ATTRID_NAME, PutOut(n));			
//			if( equal( n, name))//if( n == name )
//				COMRETURN( ::QueryInterface(*i, p) );
//			++i;
//	}
//	if( inscope != VARIANT_FALSE )
//	{
//		CComObjPtr<IMgaMetaFolder> parent;
//		COMTHROW( get_DefinedIn(PutOut(parent)) );
//		if( parent != NULL )
//			COMRETURN( parent->get_DefinedFolderByName(name, inscope, p) );
//	}
	return E_NOTFOUND;
}


const Result_t MetaFolder::GetDefinedFCOByName(const std::string &name, const bool &inScope, MetaFCO* &metaFCO) const throw()
{
//	CCoreObjectPtr self(GetUnknown());
//	CComBstrObj_lightequal equal( GetUnknown());
//	coreobjects_type coreobjects;
//	self.GetCollectionValue(ATTRID_DEFFCO_PTR, coreobjects);
//	coreobjects_iterator i = coreobjects.begin();
//	coreobjects_iterator e = coreobjects.end();
//	while( i != e )
//	{
//		CComBstrObj n;
//		(*i).GetStringValue(ATTRID_NAME, PutOut(n));
//		if( equal( n, name))//if( n == name )
//			COMRETURN( ::QueryInterface(*i, p) );
//		++i;
//	}
//	if( inscope != VARIANT_FALSE )
//	{
//		CComObjPtr<IMgaMetaFolder> parent;
//		COMTHROW( get_DefinedIn(PutOut(parent)) );
//		if( parent != NULL )
//			COMRETURN( parent->get_DefinedFCOByName(name, inscope, p) );
//	}
	return E_NOTFOUND;
}


const Result_t MetaFolder::GetDefinedAttributeByName(const std::string &name, const bool &inScope, MetaAttribute* &metaAttribute) const throw()
{
//	CCoreObjectPtr self(GetUnknown());
//	coreobjects_type coreobjects;
//	self.GetCollectionValue(ATTRID_DEFATTR_PTR, coreobjects);
//	coreobjects_iterator i = coreobjects.begin();
//	coreobjects_iterator e = coreobjects.end();
//	while( i != e )
//	{
//		CComBstrObj n;
//		(*i).GetStringValue(ATTRID_NAME, PutOut(n));		
//		if( n == name )
//			COMRETURN( ::QueryInterface(*i, p) );
//		++i;
//	}
//	if( inscope != VARIANT_FALSE )
//	{
//		CComObjPtr<IMgaMetaFolder> parent;
//		COMTHROW( get_DefinedIn(PutOut(parent)) );		
//		if( parent != NULL )
//			COMRETURN( parent->get_DefinedAttributeByName(name, inscope, p) );
//	}
	return E_NOTFOUND;
}


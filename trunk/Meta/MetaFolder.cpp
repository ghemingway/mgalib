/*** Included Header Files ***/
#include "MetaFolder.h"
#include "MetaProject.h"
#include "MetaGeneric.h"
#include "MetaFCO.h"
#include "MetaAttribute.h"
#include "../Core/CoreObject.h"


// --------------------------- MetaFolder --------------------------- //


const Result_t MetaFolder::GetDefinedIn(MetaFolder* &folder) const throw()
{
	// Use the MetaBase helper function to get a Folder from this pointer attribute
	return this->ObjectFromAttribute(ATTRID_DEFFOLDER_PTR, folder);
}


const Result_t MetaFolder::GetDefinedFolders(std::list<MetaFolder*> &folderList) const throw()
{
	ASSERT(false);
//	return ComGetCollectionValue<IMgaMetaFolder>(GetUnknown(), ATTRID_DEFFOLDER_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetDefinedFCOs(std::list<MetaFCO*> &fcoList) const throw()
{
	ASSERT(false);
//	return ComGetSortedCollValue<IMgaMetaFCO>(GetUnknown(), ATTRID_DEFFCO_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetDefinedAttributes(std::list<MetaAttribute*> &attribList) const throw()
{
	ASSERT(false);
//	return ComGetSortedCollValue<IMgaMetaAttribute>(GetUnknown(), ATTRID_DEFATTR_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetLegalChildFolders(std::list<MetaFolder*> &folderList) const throw()
{
	ASSERT(false);
//	return ComGetSortedLinkCollValue<IMgaMetaFolder>(GetUnknown(),ATTRID_FOLDERLINK_PARENT_PTR, ATTRID_FOLDERLINK_CHILD_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetLegalChildFolderByName(const std::string &name, MetaFolder* &metaFolder) const throw()
{
	ASSERT(false);
//	return ComGetLinkCollValueByName(name, GetUnknown(), ATTRID_FOLDERLINK_PARENT_PTR, ATTRID_FOLDERLINK_CHILD_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetLegalRootObjects(std::list<MetaFCO*> &fcoList) const throw()
{
	ASSERT(false);
//	return ComGetSortedLinkCollValue<IMgaMetaFCO>(GetUnknown(), ATTRID_ROOTOBJLINK_FOLDER_PTR, ATTRID_ROOTOBJLINK_OBJECT_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetLegalRootObjectByName(const std::string &name, MetaFCO* &metaFCO) const throw()
{
	ASSERT(false);
//	return ComGetLinkCollValueByName(name, GetUnknown(), ATTRID_ROOTOBJLINK_FOLDER_PTR, ATTRID_ROOTOBJLINK_OBJECT_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetUsedInFolders(std::list<MetaFolder*> &folderList) const throw()
{
	ASSERT(false);
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
	ASSERT(false);
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
	ASSERT(false);
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


const Result_t MetaFolder::CreateAtom(MetaAtom* &metaAtom) throw()
{
	// Get the associated coreProject
	CoreProject* coreProject = NULL;
	ASSERT( this->_coreObject->Project(coreProject) == S_OK );
	ASSERT( coreProject != NULL );
	// Start a transaction
	ASSERT( coreProject->BeginTransaction(false) == S_OK );
	// Create a METAID_METAATOM object
	CoreObject coreObject;
	ASSERT( coreProject->CreateObject(METAID_METAATOM, coreObject) == S_OK );
	// Link the new child to this object as parent
	Uuid uuid;
	ASSERT( this->_coreObject->GetUuid(uuid) == S_OK );
	ASSERT( coreObject->SetAttributeValue(ATTRID_DEFFCO_PTR, uuid) == S_OK );
	// Commit transaction at the CoreProject level
	coreProject->CommitTransaction();
	// Now use the core object to create a MetaAtom
	metaAtom = new MetaAtom(coreObject, this->_metaProject);
	ASSERT( metaAtom != NULL );
	return S_OK;
}


const Result_t MetaFolder::CreateAttribute(MetaAttribute* &metaAttribute) throw()
{
	// Get the associated coreProject
	CoreProject* coreProject = NULL;
	ASSERT( this->_coreObject->Project(coreProject) == S_OK );
	ASSERT( coreProject != NULL );
	// Start a transaction
	ASSERT( coreProject->BeginTransaction(false) == S_OK );
	// Create a METAID_METAATTRIBUTE object
	CoreObject coreObject;
	ASSERT( coreProject->CreateObject(METAID_METAATTRIBUTE, coreObject) == S_OK );
	// Link the new child to this object as parent
	Uuid uuid;
	ASSERT( this->_coreObject->GetUuid(uuid) == S_OK );
	ASSERT( coreObject->SetAttributeValue(ATTRID_DEFATTR_PTR, uuid) == S_OK );
	// Commit transaction at the CoreProject level
	coreProject->CommitTransaction();
	// Now use the core object to create a MetaAttribute
	metaAttribute = new MetaAttribute(coreObject, this->_metaProject);
	ASSERT( metaAttribute != NULL );
	return S_OK;
}


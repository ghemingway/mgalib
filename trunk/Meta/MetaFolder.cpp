/*** Included Header Files ***/
#include "MetaFolder.h"
#include "MetaProject.h"
#include "MetaReference.h"
#include "MetaModel.h"
#include "MetaSet.h"
#include "MetaConnection.h"
#include "MetaFCO.h"
#include "MetaAttribute.h"
#include "MetaGeneric.h"


// --------------------------- Public MetaFolder Methods --------------------------- //


const Result_t MetaFolder::GetDefinedIn(MetaFolder* &folder) const throw()
{
	// Use the MetaBase helper function to get a Folder from this pointer attribute
	return MetaBase::ObjectFromAttribute(this->_coreObject, this->_metaProject, ATTRID_DEFFOLDER_PTR, folder);
}


const Result_t MetaFolder::GetChildFolders(std::list<MetaFolder*> &folderList) const throw()
{
	// Use the MetaBase helper function to get collection of child objects
	return MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_DEFFOLDER_PTR, folderList);
}


const Result_t MetaFolder::GetDefinedFCOs(std::list<MetaFCO*> &fcoList) const throw()
{
	// Use the MetaBase helper function to get collection of child objects
	return MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_DEFFCO_PTR, fcoList);
}


const Result_t MetaFolder::GetDefinedAttributes(std::list<MetaAttribute*> &attribList) const throw()
{
	// Use the MetaBase helper function to get collection of child objects
	return MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_DEFATTR_PTR, attribList);
}


const Result_t MetaFolder::GetLegalChildFolders(std::list<MetaFolder*> &folderList) const throw()
{
	ASSERT(false);
//	return ComGetSortedLinkCollValue<IMgaMetaFolder>(GetUnknown(),ATTRID_FOLDERLINK_PARENT_PTR, ATTRID_FOLDERLINK_CHILD_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetLegalChildFolderByName(const std::string &name, MetaFolder* &metaFolder) const throw()
{
	// Use the MetaBase helper function to get this pointer object by name
	return MetaBase::ObjectFromCollectionByName(this->_coreObject, this->_metaProject, ATTRID_FOLDERLINK_PARENT_PTR, ATTRID_FOLDERLINK_CHILD_PTR, name, metaFolder);
}


const Result_t MetaFolder::GetLegalRootObjects(std::list<MetaFCO*> &fcoList) const throw()
{
	ASSERT(false);
//	return ComGetSortedLinkCollValue<IMgaMetaFCO>(GetUnknown(), ATTRID_ROOTOBJLINK_FOLDER_PTR, ATTRID_ROOTOBJLINK_OBJECT_PTR, p);
	return S_OK;
}


const Result_t MetaFolder::GetLegalRootObjectByName(const std::string &name, MetaFCO* &metaFCO) const throw()
{
	// Use the MetaBase helper function to get this pointer object by name
	return MetaBase::ObjectFromCollectionByName(this->_coreObject, this->_metaProject, ATTRID_ROOTOBJLINK_FOLDER_PTR, ATTRID_ROOTOBJLINK_OBJECT_PTR, name, metaFCO);
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


const Result_t MetaFolder::CreateFolder(MetaFolder* &metaFolder) throw()
{
	// Use the MetaBase helper function to create a new object
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METAFOLDER, ATTRID_DEFFOLDER_PTR, metaFolder);
}


const Result_t MetaFolder::CreateModel(MetaModel* &metaModel) throw()
{
	// Use the MetaBase helper function to create a new object
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METAMODEL, ATTRID_DEFFCO_PTR, metaModel);
}


const Result_t MetaFolder::CreateAtom(MetaAtom* &metaAtom) throw()
{
	// Use the MetaBase helper function to create a new atom
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METAATOM, ATTRID_DEFFCO_PTR, metaAtom);
}


const Result_t MetaFolder::CreateReference(MetaReference* &metaReference) throw()
{
	// Use the MetaBase helper function to create a new atom
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METAATOM, ATTRID_DEFFCO_PTR, metaReference);
}


const Result_t MetaFolder::CreateSet(MetaSet* &metaSet) throw()
{
	// Use the MetaBase helper function to create a new atom
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METASET, ATTRID_DEFFCO_PTR, metaSet);
}


const Result_t MetaFolder::CreateConnection(MetaConnection* &metaConnection) throw()
{
	// Use the MetaBase helper function to create a new atom
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METACONNECTION, ATTRID_DEFFCO_PTR, metaConnection);
}


const Result_t MetaFolder::CreateAttribute(MetaAttribute* &metaAttribute) throw()
{
	// Use the MetaBase helper function to create a new attribute
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METAATTRIBUTE, ATTRID_DEFATTR_PTR, metaAttribute);
}


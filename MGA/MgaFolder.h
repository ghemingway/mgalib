#ifndef __MGA_FOLDER_H__
#define __MGA_FOLDER_H__


/*** Included Header Files ***/
#include "MgaFCO.h"


/*** Class Predefinitions ***/
class MetaFCO;
class MgaAttribute;
class MetaAttribute;


// --------------------------- MgaContainerImpl --------------------------- //


class MgaContainer : public MgaFCO {
private:
	MgaContainer();												//!< Deny access
	MgaContainer(const MgaContainer &);							//!< Deny access
	MgaContainer& operator=(const MgaContainer &);				//!< Deny access

public:
	MgaContainer(MgaTerritory* &territory, MgaProject* &project, CoreObject* &coreObject) : ::MgaFCO(territory, project, coreObject) { }

	const Result_t GetChildrenOfKind(const std::string &kind, std::list<MgaFCO*> &fcoList) throw()
		{ return this->_fcoBase->GetChildrenOfKind(kind, fcoList); }
	inline const Result_t GetDescendantFCOs(MgaFilter* &filter, std::list<MgaFCO*> &fcoList) const throw()
		{ return this->_fcoBase->GetDescendantFCOs(filter, fcoList); }
	inline const Result_t GetChildFCOs(std::list<MgaFCO*> &fcoList) { return this->_fcoBase->GetChildFCOs(fcoList); }
	inline const Result_t GetChildFCO(const std::string &name, MgaFCO* &mgaFCO) { return this->_fcoBase->GetChildFCO(name, mgaFCO); }
};


// --------------------------- MgaFolder --------------------------- //


class MgaFolder	: public MgaContainer
{
private:
	MgaFolder();												//!< Deny access
	MgaFolder(const MgaFolder &);								//!< Deny access
	MgaFolder& operator=(const MgaFolder &);					//!< Deny access

public:
	MgaFolder(MgaTerritory* &territory, MgaProject* project, CoreObject* &coreObject) : ::MgaContainer(territory, project, coreObject) { }
/*
	const Result_t DeriveRootObject(MgaFCO* base, const bool &instance, MgaFCO* &newObj) { return this->_fcoBase->DeriveFCO(base, NULL, instance, nobj); 	}
	const Result_t CreateRootObject(MetaFCO* meta, MgaFCO* &newObj)					{ return this->_fcoBase->CreateRootObject(meta,newObj); }
	const Result_t CreateFolder(MetaFolder *meta, MgaFolder **nf)					{ return this->_fcoBase->CreateFolder(meta, nf); }
*/
	const Result_t GetChildFolders(std::list<MgaFolder*> &folderList) throw()		{ return this->_fcoBase->GetChildFolders(folderList); }
/*
	const Result_t GetParentFolder(MgaFolder **pVal)								{ return this->_fcoBase->GetParent(reinterpret_cast<IMgaContainer **>(pVal),NULL); }
	const Result_t GetMetaFolder(MetaFolder **pVal)									{ return this->_fcoBase->get_MetaFolder(pVal); } 
	const Result_t CopyFCOs(std::list<MgaFCO*> &fcoList, std::list<MgaFCO*> &newList){ return this->_fcoBase->CopyFCOs(fcoList, NULL, newList); } 
	const Result_t MoveFCOs(std::list<MgaFCO*> &fcoList, std::list<MgaFCO*> &newList){ return this->_fcoBase->MoveFCOs(fcoList, NULL, newList); } 
	const Result_t CopyFolders(MgaFolders *list, MgaFolders **objs)					{ return this->_fcoBase->CopyFolders(list, objs); } 
	const Result_t MoveFolders(MgaFolders *list, MgaFolders **objs)					{ return this->_fcoBase->MoveFolders(list, objs); } 
//	const Result_t MoveFCOsAndFolders(MgaFolders *list, MgaFCOs *list2, MgaFolders **objs, MgaFCOs **objs2) { return this->_fcoBase->MoveFCOsAndFolders(list, list2, objs, objs2); } 
	const Result_t RefreshParent(MgaFolder *parent)									{ return this->_fcoBase->RefreshParent( parent); }
	const Result_t AttachLibraryV3(BSTR libname, VARIANT_BOOL ungroup, IMgaFolder **nf)		{ return this->_fcoBase->AttachLibraryV3(libname, ungroup, nf); }
	const Result_t RefreshLibraryV3(BSTR libname, VARIANT_BOOL ungroup, long *numOfErrors)	{ return this->_fcoBase->RefreshLibraryV3(libname, ungroup, numOfErrors); }
	const Result_t AttachLibrary(BSTR libname, IMgaFolder **nf)						{ return this->_fcoBase->AttachLibrary(libname, nf); }
	const Result_t RefreshLibrary(BSTR libname)										{ return this->_fcoBase->RefreshLibrary(libname); }
	const Result_t GetLibraryName(BSTR *libname)									{ return this->_fcoBase->get_LibraryName(libname); }
	const Result_t SetLibraryName(BSTR libname)										{ return this->_fcoBase->put_LibraryName(libname); }
	const Result_t GetVirtuallyIncludedBy( MgaFolders** pDep)						{ return this->_fcoBase->GetVirtuallyIncludedBy( pDep); }
	const Result_t SetVirtuallyIncludedBy( MgaFolders*  pDep)						{ return this->_fcoBase->SetVirtuallyIncludedBy( pDep); }
	const Result_t GetVirtuallyIncludes( MgaFolders** pDep)							{ return this->_fcoBase->GetVirtuallyIncludes  ( pDep); }
	const Result_t SetVirtuallyIncludes( MgaFolders*  pDep)							{ return this->_fcoBase->SetVirtuallyIncludes  ( pDep); }
	const Result_t GetSourceControlInfo( long* p_scInfo)							{ return this->_fcoBase->GetSourceControlInfo( p_scInfo); }
	const Result_t GetChildRelIDCounter(long &pVal)									{ return this->_fcoBase->get_ChildRelIDCounter(pVal); }
	const Result_t SetChildRelIDCounter(const long &pVal)							{ return this->_fcoBase->put_ChildRelIDCounter(pVal); }
*/
	const Result_t GetAttributes(std::list<MgaAttribute*> &attributes) throw()				{ return E_MGA_NOT_SUPPORTED; };
	const Result_t GetAttribute(MetaAttribute *meta, bool *pVal) throw()					{ return E_MGA_NOT_SUPPORTED; };
	const Result_t GetAttributeByName(const std::string &name, bool *pVal) throw()			{ return E_MGA_NOT_SUPPORTED; };
	const Result_t SetAttributeByName(const std::string &name, bool newVal) throw()			{ return E_MGA_NOT_SUPPORTED; };
	const Result_t GetStrAttrByName(const std::string &name, std::string &value) throw()	{ return E_MGA_NOT_SUPPORTED; }
	const Result_t SetStrAttrByName(const std::string &name, const std::string &val) throw(){ return E_MGA_NOT_SUPPORTED; }
	const Result_t GetIntAttrByName(const std::string &name, long &pVal) throw()			{ return E_MGA_NOT_SUPPORTED; }
	const Result_t SetIntAttrByName(const std::string &name, const long &newval) throw()	{ return E_MGA_NOT_SUPPORTED; }
	const Result_t GetFloatAttrByName(const std::string &name, double &value) throw()		{ return E_MGA_NOT_SUPPORTED; }
	const Result_t SetFloatAttrByName(const std::string &name, const double &value) throw()	{ return E_MGA_NOT_SUPPORTED; }
	const Result_t GetBoolAttrByName(const std::string &name, bool &value) throw()			{ return E_MGA_NOT_SUPPORTED; }
	const Result_t SetBoolAttrByName(const std::string &name, const bool &value) throw()	{ return E_MGA_NOT_SUPPORTED; }
	const Result_t GetRefAttrByName(const std::string &name, MgaFCOBase* &attr) throw()		{ return E_MGA_NOT_SUPPORTED; }
	const Result_t SetRefAttrByName(const std::string &name, MgaFCOBase* &attr) throw()		{ return E_MGA_NOT_SUPPORTED; }
	const Result_t RemoveAttrByName(const std::string &name) throw()						{ return E_MGA_NOT_SUPPORTED; }
	const Result_t GetMeta(MetaFCO* &metaFCO) throw()										{ return E_MGA_NOT_SUPPORTED; }
};


#endif //__MGA_FOLDER_H__


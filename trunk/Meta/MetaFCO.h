#ifndef __META_METAFCO_H__
#define __META_METAFCO_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Class Predefinitions ***/
class MetaAttribute;
class MetaRole;
class MetaFolder;


// --------------------------- MetaFCO --------------------------- //


class MetaFCO : public MetaBase
{
public:
	virtual ~MetaFCO() { }

	static const Result_t GetAttributeByName(MetaFCO* &metaFCO, const std::string &name, MetaAttribute* &metaAttribute);
	static const Result_t GetAttributeByRef(MetaFCO *metaFCO, const MetaRef_t &metaRef, MetaAttribute* &metaAttribute);
	static const Result_t GetDefinedAttributeByName(MetaFCO *metaFCO, const std::string &name, const bool &inscope, MetaAttribute* &metaAttribute);
	static const Result_t GetAliasingEnabled(MetaFCO *metaFCO, bool &value);
	static const Result_t SetAliasingEnabled(MetaFCO *metaFCO, const bool &value);
	static void Traverse(MetaProject* &metaProject, CoreObject* &coreObject);

	virtual const Result_t GetDefinedIn(MetaBase* &metaBase) const throw();								//{ return ComGetPointerValue(GetUnknown(), ATTRID_DEFFCO_PTR, p); }
	virtual const Result_t GetDefinedAttributes(std::list<MetaAttribute*> &attribList) const throw();	//{ return ComGetSortedCollValue<IMgaMetaAttribute>(GetUnknown(), ATTRID_DEFATTR_PTR, p); }
	virtual const Result_t GetDefinedAttributeByName(const std::string &name, const bool &inScope, MetaAttribute* &attrib) const throw();
	virtual const Result_t GetUsedInRoles(std::list<MetaRole*> &rolesList) const throw();				//{ return ComGetCollectionValue<IMgaMetaRole>(GetUnknown(), ATTRID_KIND_PTR, p); }
	virtual const Result_t GetUsedInFolders(std::list<MetaFolder*> &folderList) const throw();			//{ return ComGetLinkCollectionValue<IMgaMetaFolder>(GetUnknown(), ATTRID_ROOTOBJLINK_OBJECT_PTR, ATTRID_ROOTOBJLINK_FOLDER_PTR, p); }
	virtual const Result_t GetAttributes(std::list<MetaAttribute*> &attribList) const throw();			//{ return ComGetSortedLinkCollValue<IMgaMetaAttribute>(GetUnknown(),	ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p); }
	virtual const Result_t GetAttributeByName(const std::string &name, MetaAttribute* &attrib) const throw();
	virtual const Result_t GetAttributeByRef(const MetaRef_t &metaRef, MetaAttribute* &attrib) const throw();
//	virtual const Result_t DefineAttribute(MetaAttribute* &attrib) throw();								//{ return ComDefineBase(this, METAID_METAATTRIBUTE, ATTRID_DEFATTR_PTR, p); }
//	virtual const Result_t AddAttribute(MetaAttribute* &attrib) throw();								//{ return ComAddLink(this, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p); }
//	virtual const Result_t RemoveAttribute(MetaAttribute* &attrib) throw();								//{ return ComRemoveLink(this, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p); }
	virtual const Result_t GetAliasingEnabled(bool &value) const throw();
//	virtual const Result_t SetAliasingEnabled(const bool &value) throw();
};


// --------------------------- MetaAtom --------------------------- //


class MetaAtom : public MetaFCO
{
public:
};


#endif // __META_METAFCO_H__


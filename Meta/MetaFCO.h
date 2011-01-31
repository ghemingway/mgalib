#ifndef __META_METAFCO_H__
#define __META_METAFCO_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaAttribute;
class MetaRole;
class MetaFolder;


// --------------------------- MetaFCO --------------------------- //


class MetaFCO : public MetaBase
{
protected:
	MetaFCO(CoreObject &coreObject, MetaProject* const &metaProject) : ::MetaBase(coreObject, metaProject) { }

public:
	virtual ~MetaFCO() { }

	static const Result_t GetAttributeByName(MetaFCO* &metaFCO, const std::string &name, MetaAttribute* &metaAttribute) throw();
	static const Result_t GetAttributeByRef(MetaFCO *metaFCO, const Uuid &uuid, MetaAttribute* &metaAttribute) throw();
	static const Result_t GetDefinedAttributeByName(MetaFCO *metaFCO, const std::string &name, const bool &inscope, MetaAttribute* &metaAttribute) throw();
	static const Result_t GetAliasingEnabled(MetaFCO *metaFCO, bool &value) throw();
	static const Result_t SetAliasingEnabled(MetaFCO *metaFCO, const bool &value) throw();

	virtual const Result_t GetDefinedIn(MetaBase* &metaBase) const throw();										//!<
	virtual const Result_t GetDefinedAttributes(std::list<MetaAttribute*> &attribList) const throw();			//!<
	virtual const Result_t GetDefinedAttributeByName(const std::string &name, const bool &inScope, MetaAttribute* &attrib) const throw();
	virtual const Result_t GetUsedInRoles(std::list<MetaRole*> &rolesList) const throw();						//!<
	virtual const Result_t GetUsedInFolders(std::list<MetaFolder*> &folderList) const throw();					//!<
	virtual const Result_t GetAttributes(std::list<MetaAttribute*> &attribList) const throw();					//!<
	virtual const Result_t GetAttributeByName(const std::string &name, MetaAttribute* &attrib) const throw();	//!<
	virtual const Result_t GetAttributeByRef(const Uuid &uuid, MetaAttribute* &attrib) const throw();			//!<
	virtual const Result_t CreateAttribute(MetaAttribute* &metaAttribute) throw();								//!<
	virtual const Result_t AddAttribute(MetaAttribute* &attrib) throw();										//!<
	virtual const Result_t RemoveAttribute(MetaAttribute* &attrib) throw();										//!<
	virtual const Result_t GetAliasingEnabled(bool &value) const throw();										//!<
	virtual const Result_t SetAliasingEnabled(const bool &value) throw();										//!<
};


// --------------------------- MetaAtom --------------------------- //


class MetaAtom : public MetaFCO
{
private:
	friend class MetaBase;
	MetaAtom(CoreObject &coreObject, MetaProject* const &metaProject) : ::MetaFCO(coreObject, metaProject) { }

public:
	virtual ~MetaAtom() { }
};


/*** End of MGA Namespace ***/
}


#endif // __META_METAFCO_H__


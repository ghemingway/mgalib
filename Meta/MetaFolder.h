#ifndef __META_FOLDER_H__
#define __META_FOLDER_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaFCO;
class MetaAttribute;
class MetaModel;
class MetaAtom;
class MetaReference;
class MetaSet;
class MetaConnection;


// --------------------------- MetaFolder --------------------------- //


class MetaFolder : public MetaBase
{
private:
	MetaFolder();											//!< Deny access to default constructor
	MetaFolder(const MetaFolder &);							//!< Deny access to copy constructor
	MetaFolder& operator=(const MetaFolder &);				//!< Deny access to equals operator

	friend class MetaProject;
	friend class MetaBase;
	MetaFolder(CoreObject &coreObject, MetaProject* const &metaProject) : ::MetaBase(coreObject, metaProject) { }

public:
	virtual ~MetaFolder() { }

	const Result_t GetDefinedIn(MetaFolder* &folder) const throw();
	const Result_t GetChildFolders(std::list<MetaFolder*> &folderList) const throw();
	const Result_t GetDefinedFCOs(std::list<MetaFCO*> &fcoList) const throw();
	const Result_t GetDefinedAttributes(std::list<MetaAttribute*> &attribList) const throw();
	const Result_t GetLegalChildFolders(std::list<MetaFolder*> &folderList) const throw();
	const Result_t GetLegalChildFolderByName(const std::string &name, MetaFolder* &metaFolder) const throw();
	const Result_t GetLegalRootObjects(std::list<MetaFCO*> &fcoList) const throw();
	const Result_t GetLegalRootObjectByName(const std::string &name, MetaFCO* &metaFCO) const throw();
	const Result_t GetUsedInFolders(std::list<MetaFolder*> &folderList) const throw();
	const Result_t GetDefinedFolderByName(const std::string &name, const bool &inScope, MetaFolder* &metaFolder) const throw();
	const Result_t GetDefinedFCOByName(const std::string &name, const bool &inScope, MetaFCO* &metaFCO) const throw();
	const Result_t GetDefinedAttributeByName(const std::string &name, const bool &inScope, MetaAttribute* &metaAttribute) const throw();
	const Result_t CreateFolder(MetaFolder* &metaFolder) throw();				//!<
	const Result_t CreateModel(MetaModel* &metaModel) throw();					//!<
	const Result_t CreateAtom(MetaAtom* &metaAtom) throw();						//!<
	const Result_t CreateReference(MetaReference* &metaReference) throw();		//!<
	const Result_t CreateSet(MetaSet* &metaSet) throw();						//!<
	const Result_t CreateConnection(MetaConnection* &metaConnection) throw();	//!<
	const Result_t CreateAttribute(MetaAttribute* &metaAttribute) throw();		//!<
//	const Result_t AddLegalChildFolder(MetaFolder* &metaFolder) throw()			{ return ComAddLink(this, METAID_METAFOLDERLINK, ATTRID_FOLDERLINK_PARENT_PTR, ATTRID_FOLDERLINK_CHILD_PTR, p); }
//	const Result_t AddLegalRootObject(MetaFCO* &metaFCO) throw()				{ return ComAddLink(this, METAID_METAROOTOBJLINK, ATTRID_ROOTOBJLINK_FOLDER_PTR, ATTRID_ROOTOBJLINK_OBJECT_PTR, p); }
};


/*** End of MGA Namespace ***/
}


#endif // __META_FOLDER_H__


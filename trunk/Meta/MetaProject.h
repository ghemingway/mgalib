#ifndef __META_PROJECT_H__
#define __META_PROJECT_H__


/*** Included Header Files ***/
#include "../MGA/MgaTypes.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaBase;
class CoreObject;
class CoreProject;
class MetaFolder;


/*** Type Definitions ***/
typedef STDEXT::hash_map<Uuid,MetaBase*,Uuid_HashFunc> MetaObjectHash;
typedef MetaObjectHash::iterator MetaObjectHashIterator;


// --------------------------- MetaProject --------------------------- //


class MetaProject
{
private:
	MetaObjectHash				_metaObjectHash;
	CoreProject					*_coreProject;
	CoreObject					*_rootObject;
	std::string					_namespace;

	friend class MetaBase;
	const Result_t RegisterMetaBase(const Uuid &uuid, MetaBase* obj);
	const Result_t UnregisterMetaBase(const Uuid &uuid, MetaBase* obj);

//	void CreateMetaBase(MetaID_t metaid, CoreObject* &obj);
//	void CreateMetaObj(MetaID_t metaid, CoreObject* &obj);
//	void CreatePathItems(bstr_const_iterator i, bstr_const_iterator e, pathitems_type &pathitems);
//	void CreateJointPaths(std::string paths, jointpaths_type &jointpaths);

	MetaProject();													//!< Deny access to default constructor
	MetaProject(const MetaProject &);								//!< Deny access to copy constructor
	MetaProject& operator=(const MetaProject&);						//!< Deny access to equals operator
	MetaProject(CoreProject* &coreProject);							//!< Hidden primary constructor

public:
	~MetaProject();

	static const Result_t Open(const std::string &connection, MetaProject* &project) throw();
	static const Result_t Create(const std::string &connection, MetaProject* &project) throw();

	const Result_t Save(const std::string &filename) throw();
	const Result_t BeginTransaction(void) throw();
	const Result_t CommitTransaction(void) throw();
	const Result_t AbortTransaction(void) throw();
	const Result_t GetUuid(Uuid &uuid) const throw();
	const Result_t SetUuid(const Uuid &uuid) throw();
	const Result_t GetName(std::string &name) const throw();
	const Result_t SetName(const std::string &name) throw();
	const Result_t GetDisplayedName(std::string &name) const throw();
	const Result_t SetDisplayedName(const std::string &name) throw();
	const Result_t GetVersion(std::string &version) const throw();
	const Result_t SetVersion(const std::string &version) throw();
	const Result_t GetAuthor(std::string &author) const throw();
	const Result_t SetAuthor(const std::string &author) throw();
	const Result_t GetComment(std::string &comment) const throw();
	const Result_t SetComment(const std::string &comment) throw();
	const Result_t GetCreatedAt(std::string &dateTime) const throw();
	const Result_t SetCreatedAt(const std::string &dateTime) throw();
	const Result_t GetModifiedAt(std::string &dateTime) const throw();
	const Result_t SetModifiedAt(const std::string &dateTime) throw();
	const Result_t RootFolder(MetaFolder* &folder) throw();
	const Result_t FindObject(const Uuid &uuid, MetaBase* &metaBase) throw();
	inline const Result_t GetNamespace(std::string &name) const throw()		{ name = this->_namespace; return S_OK; }
	inline const Result_t SetNamespace(const std::string &name) throw()		{ this->_namespace = name; return S_OK; }
};


/*** End of MGA Namespace ***/
}


#endif // __META_PROJECT_H__


#ifndef __META_PROJECT_H__
#define __META_PROJECT_H__


/*** Included Header Files ***/
#include "../MGA/MgaTypes.h"
#include "../Core/CoreObject.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaBase;
class CoreProject;
class MetaFolder;


/*** Type Definitions ***/
//None


// --------------------------- MetaProject --------------------------- //


class MetaProject
{
private:
	CoreProject					*_coreProject;
	CoreObject					_rootObject;

	MetaProject();													//!< Deny access to default constructor
	MetaProject(const MetaProject &);								//!< Deny access to copy constructor
	MetaProject& operator=(const MetaProject&);						//!< Deny access to equals operator
	MetaProject(CoreProject* &coreProject);							//!< Hidden primary constructor

public:
	~MetaProject();

	static const Result_t Open(const std::string &connection, MetaProject* &project) throw();
	static const Result_t Create(const std::string &connection, MetaProject* &project) throw();

	const Result_t Save(const std::string &filename="", const bool &forceOverwrite=false) throw();
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
};


/*** End of MGA Namespace ***/
}


#endif // __META_PROJECT_H__


#ifndef __CORE_PROJECT_H__
#define __CORE_PROJECT_H__


/*** Included Header Files ***/
#include "CoreTypes.h"
#include "CoreStorage.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class CoreProject;
class CoreObjectBase;
class CoreAttributeBase;
class CoreMetaProject;


// --------------------------- Local Type Definitions ---------------------------


typedef struct Transaction
{
	bool							readOnly;
	std::list<Uuid>					createdObjects;
	std::list<CoreAttributeBase*>	attributes;
	std::list<Uuid>					deletedObjects;
} Transaction;

typedef STDEXT::hash_map<Uuid,CoreObjectBase*,Uuid_HashFunc> ObjectHash;
typedef ObjectHash::iterator ObjectHashIterator;


// --------------------------- CoreProject ---------------------------


class CoreProject 
{
private:
	CoreProject();															//!< Deny access to default constructor
	CoreProject(CoreMetaProject* &coreMetaProject, ICoreStorage* &storage);	//!< Primary constructor - but use static methods to create

protected:
	ICoreStorage						*_storage;					//!<
	CoreMetaProject						*_coreMetaProject;			//!<
	bool								_isDirty;					//!<
	ObjectHash							_objectHash;				//!<
	std::list<Transaction>				_transactionList;			//!<

	static std::string GetFirstToken(const std::string &connection);

	friend class CoreObjectBase;
	void RegisterObject(const Uuid &uuid, CoreObjectBase *object) throw();
	void UnregisterObject(const Uuid &uuid) throw();
	ICoreStorage* SetStorageObject(const Uuid &uuid) throw();

	friend class CoreAttributeBase;
	void RegisterTransactionItem(CoreAttributeBase* attribute) throw();	

public:
	~CoreProject();

	// ------- ICoreProject Interface
	static const Result_t Create(const std::string &connection, CoreMetaProject* coreMetaProject, CoreProject* &project) throw();
	static const Result_t Open(const std::string &connection, CoreMetaProject* coreMetaProject, CoreProject* &project) throw();

	inline const Result_t Storage(ICoreStorage* &storage) const throw()				{ storage = this->_storage; return S_OK; }
	inline const Result_t MetaProject(CoreMetaProject* &metaProject) const throw()	{ metaProject = this->_coreMetaProject; return S_OK; }
	inline const Result_t IsDirty(bool &flag) const throw()							{ flag = this->_isDirty; return S_OK; }
	inline const Result_t MarkDirty(void) throw()									{ this->_isDirty = true; return S_OK; }
	inline const Result_t InTransaction(bool &flag) const throw()					{ flag = !this->_transactionList.empty(); return S_OK; }
	inline const Result_t InWriteTransaction(bool &flag) const throw()
	{
		// Must be active transaction and it must not be readOnly
		flag = (!this->_transactionList.empty() && !this->_transactionList.front().readOnly);
		return S_OK;
	}
	inline const Result_t NestedTransactionCount(short &count) const throw()		{ count = this->_transactionList.size(); return S_OK; }

	const Result_t Save(const std::string &filename) throw();
	const Result_t BeginTransaction(const bool &readOnly) throw();
	const Result_t CommitTransaction(void) throw();
	const Result_t AbortTransaction(void) throw();

	const Result_t Object(const Uuid &uuid, CoreObject &object) throw();
	const Result_t CreateObject(const MetaID_t &metaID, CoreObject &object) throw();
	const Result_t RootObject(CoreObject &rootObject) throw();
	const Result_t DeleteObject(const Uuid &uuid) throw();
};


/*** End of MGA Namespace ***/
}


#endif	//__CORE_PROJECT_H__


#ifndef __CORE_PROJECT_H__
#define __CORE_PROJECT_H__


/*** Included Header Files ***/
#include "CoreTypes.h"
#include "CoreStorage.h"
#include "CoreTransactionItem.h"


/*** Class Predefinitions ***/
class CoreProject;
class CoreObject;
class CoreObjectBase;
class CoreAttributeBase;
class CoreMetaProject;


// --------------------------- Local Type Definitions ---------------------------


typedef struct Transaction
{
	bool							readOnly;
	std::list<CoreObjectBase*>		createdObjects;
	std::list<CoreAttributeBase*>	attributes;
	std::list<MetaObjIDPair>		deletedObjects;
} Transaction;

typedef std::list<CoreAttributeBase*>UndoItemsList;
typedef UndoItemsList::iterator		UndoItemsListIterator;
typedef std::list<UndoItemsList>	UndoItemsListList;


typedef STDEXT::hash_map<MetaObjIDPair,CoreObjectBase*,MetaObjID2Pair_HashFunc> ObjectHash;
typedef ObjectHash::iterator ObjectHashIterator;


// --------------------------- CoreProject ---------------------------


class CoreProject 
{
private:
	CoreProject();													//!< Deny access to default constructor
	CoreProject(CoreMetaProject *coreMetaProject);					//!< Primary constructor - but use static methods to create

protected:
	ICoreStorage						*_storage;					//!<
	CoreMetaProject						*_coreMetaProject;			//!<
	bool								_isDirty;					//!<
	ObjectHash							_objectHash;				//!<
	std::list<Transaction>				_transactionList;			//!<
	UndoItemsListList					_undoList;					//!<
	uint16_t							_redoCount;					//!<

	static std::string GetFirstToken(const std::string &connection);
//	void RegisterFinalTransactionItem(CoreFinalTransactionItem *modified) throw();
//	void RegisterUndoItem(CoreUndoItem *modified) throw();
//	void BeginFinalTransaction(void);
//	void CommitFinalTransaction(const bool &undo);
//	void AbortFinalTransaction(void);
//	void BeginNestedTransaction(void) throw();
//	void CommitNestedTransaction(void) throw();
//	void AbortNestedTransaction(void) throw();
//	void TryUndoTransaction(void);
//	void TryRedoTransaction(void);
	void DiscardUndo(void);

	friend class CoreObjectBase;
	void RegisterObject(const MetaObjIDPair &idPair, CoreObjectBase *object) throw();
	void UnregisterObject(const MetaObjIDPair &idPair) throw();
	ICoreStorage* SetStorageObject(const MetaObjIDPair &idPair) throw();

	friend class CoreAttributeBase;
	void RegisterTransactionItem(CoreAttributeBase* attribute) throw();	

public:
	~CoreProject();

	// ------- ICoreProject Interface
	static const Result_t CreateProject(const std::string &connection, CoreMetaProject* coreMetaProject, CoreProject* &project) throw();
	static const Result_t OpenProject(const std::string &connection, CoreMetaProject* coreMetaProject, CoreProject* &project) throw();

	inline const Result_t Storage(ICoreStorage* &storage) const throw()				{ storage = this->_storage; return S_OK; }
	inline const Result_t MetaProject(CoreMetaProject* &metaProject) const throw()	{ metaProject = this->_coreMetaProject; return S_OK; }
	inline const Result_t IsDirty(bool &flag) const throw()							{ flag = this->_isDirty; return S_OK; }
	inline const Result_t MarkDirty(void) throw()									{ this->_isDirty = true; return S_OK; }
	inline const Result_t InTransaction(bool &flag) const throw()					{ flag = !this->_transactionList.empty(); return S_OK; }
	inline const Result_t NestedTransactionCount(short &count) const throw()		{ count = this->_transactionList.size(); return S_OK; }

	const Result_t Save(const std::string &filename) throw();
	const Result_t BeginTransaction(const bool &readOnly) throw();
	const Result_t CommitTransaction(void) throw();
	const Result_t AbortTransaction(void) throw();

	const Result_t Object(const MetaObjIDPair &idPair, CoreObject* &object) throw();
	const Result_t CreateObject(const MetaID_t &metaID, CoreObject* &object) throw();
	const Result_t RootObject(CoreObject* &rootObject) throw();
	const Result_t DeleteObject(const MetaObjIDPair &idPair) throw();

	const Result_t UndoTransaction(void) throw();
	const Result_t RedoTransaction(void) throw();
	const Result_t FlushUndoQueue(void) throw();
	const Result_t FlushRedoQueue(void) throw();
	const Result_t UndoQueueSize(uint16_t &size) const throw();
	const Result_t RedoQueueSize(uint16_t &size) const throw();
};


#endif	//__CORE_PROJECT_H__


#ifndef __CORE_STORAGE_H__
#define __CORE_STORAGE_H__


/*** Included Header Files ***/
#include "CoreTypes.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class CoreMetaProject;
class CoreMetaObject;
class ICoreStorage;


// --------------------------- CoreStorageFactory ---------------------------


class CoreStorageFactory {
public:
	virtual const Result_t Create(const std::string &filename, CoreMetaProject *metaProject, ICoreStorage* &coreStorage, const bool &encrypted=false)=0;
	virtual const Result_t Open(const std::string &filename, CoreMetaProject *metaProject, ICoreStorage* &coreStorage, const std::vector<char> &encryptionKey=std::vector<char>())=0;
};


// --------------------------- CoreStorage ---------------------------


class ICoreStorage
{
private:
	ICoreStorage();																			//!< Hide the default constructor
	ICoreStorage(const ICoreStorage &);														//!< Hide copy constructor
	ICoreStorage& operator=(const ICoreStorage &);											//!< Hide equals operator
	static std::map<std::string,CoreStorageFactory*>		_storageFactories;				//!< Static map of factories

protected:
	CoreMetaProject											*_metaProject;					//!< Related metaProject
	bool													_isDirty;						//!< Dirty flag for the storage
	bool													_inTransaction;					//!< Transaction flag for the storage
	ICoreStorage(CoreMetaProject* metaProject) : _metaProject(metaProject), _isDirty(false), _inTransaction(false)
		{ ASSERT( this->_metaProject != NULL); }

	inline virtual CoreMetaProject* MetaProject(void) const	{ return this->_metaProject; }	//!< Get associated CoreMetaProject
	inline virtual bool InTransaction(void) const			{ return this->_inTransaction; }//!< Is transaction open
	inline virtual const bool IsDirty(void) const			{ return this->_isDirty; }		//!< Is project dirty
	inline virtual void MarkDirty(void)						{ this->_isDirty = true; }		//!< Mark project as dirty

public:
	virtual ~ICoreStorage()									{ }								//!< Default destructor

	// --------------------------- Public Interface Methods --------------------------- //

	// --- Static Methods
	
	static const Result_t RegisterStorage(const std::string &tag, CoreStorageFactory* factory) throw();	//!< Register a factory
	static const Result_t ClearStorageRegistry(void) throw();								//!< Clear factory registry
	static const Result_t Create(const std::string &tag, const std::string &filename,		//!< Create new storage using factory
								 CoreMetaProject* metaProject, ICoreStorage* &storage, const bool &encrypted=false) throw();
	static const Result_t Open(const std::string &tag, const std::string &filename,			//!< Open storage from file using factory
							   CoreMetaProject* metaProject, ICoreStorage* &storage, const std::vector<char> &encryptionKey=std::vector<char>()) throw();

	// --- CoreMetaObject

	virtual const Result_t MetaObject(CoreMetaObject* &coreMetaObject) const throw()=0;		//!< Get open object's metaObject
	virtual const Result_t MetaID(MetaID_t &metaID) const throw()=0;						//!< Get open object's metaID

	// --- Project

	virtual const Result_t ObjectVector(std::vector<Uuid> &objectVector) const throw()=0;	//!< Get idPair vector for all objects in project
	virtual const Result_t RootUuid(Uuid &uuid) const throw()=0;							//!< Get the root Uuid of the project
	virtual const Result_t Save(const std::string &filename, const bool &forceOverwrite=false) throw()=0;	//!< Save project to file
	virtual const Result_t BeginTransaction(void) throw()=0;								//!< Begin transaction on project
	virtual const Result_t CommitTransaction(const Uuid tag=Uuid::Null()) throw()=0;		//!< Commit transaction on project (with optional transaction tag)
	virtual const Result_t AbortTransaction(void) throw()=0;								//!< Abort transaction on project

	// --- Object

	virtual const Result_t OpenObject(const Uuid &uuid) throw()=0;							//!< Open an object in project
	virtual const Result_t CloseObject(void) throw()=0;										//!< Close current object
	virtual const Result_t CreateObject(const MetaID_t &metaID, Uuid &newUuid) throw()=0;	//!< Create a new object
	virtual const Result_t DeleteObject(void) throw()=0;									//!< Delete current object

	// --- Attribute

	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, int32_t &value) throw()=0;			//!< Get long
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, double &value) throw()=0;			//!< Get real
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, std::string &value) throw()=0;		//!< Get string
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, std::list<Uuid> &value) throw()=0;	//!< Get collection
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, Uuid &value) throw()=0;			//!< Get pointer (long or regular)
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, DictionaryMap &value) throw()=0;	//!< Get key value

	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const int32_t &value) throw()=0;	//!< Set long
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const double &value) throw()=0;	//!< Set real
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const std::string &value) throw()=0;		//!< Set string
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const std::list<Uuid> &value) throw()=0;	//!< Set collection
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const Uuid &value) throw()=0;		//!< Set pointer (long or regular)
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const DictionaryMap &value) throw()=0;		//!< Set key value

	// --- Object Caching

	virtual const Result_t EnableCaching(void) throw()=0;									//!<
	virtual const Result_t DisableCaching(void) throw()=0;									//!<
	virtual const Result_t IsCaching(bool &flag) const throw()=0;							//!<
	virtual const Result_t GetCacheSize(uint64_t &size) const throw()=0;					//!<
	virtual const Result_t SetCacheSize(const uint64_t &size) throw()=0;					//!<

	// --- Undo/Redo

	virtual const Result_t EnableJournaling(void) throw()=0;								//!<
	virtual const Result_t DisableJournaling(void) throw()=0;								//!<
	virtual const Result_t IsJournaled(bool &flag) const throw()=0;							//!<
	virtual const Result_t Undo(Uuid &tag) throw()=0;										//!<
	virtual const Result_t Redo(Uuid &tag) throw()=0;										//!<
	virtual const Result_t UndoCount(uint64_t &count) const throw()=0;						//!<
	virtual const Result_t RedoCount(uint64_t &count) const throw()=0;						//!<
	virtual const Result_t JournalInfo(const uint64_t &undoMaxSize, const uint64_t &redoMaxSize,	//!<
									   std::list<Uuid> &undoJournal, std::list<Uuid> &redoJournal) const throw()=0;

	// --- Compression

	virtual const Result_t EnableCompression(void) throw()=0;								//!<
	virtual const Result_t DisableCompression(void) throw()=0;								//!<
	virtual const Result_t IsCompressed(bool &flag) const throw()=0;						//!<

	// --- Encryption

	virtual const Result_t EnableEncryption(const std::vector<char> &key) throw()=0;		//!<
	virtual const Result_t DisableEncryption(void) throw()=0;								//!<
	virtual const Result_t IsEncrypted(bool &flag) const throw()=0;							//!<
	virtual const Result_t EncryptionKey(std::vector<char> &key) const throw()=0;			//!<
};
	
	
	/*** End of MGA Namespace ***/
}


#endif	//__CORE_STORAGE_H__




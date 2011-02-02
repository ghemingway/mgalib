#ifndef __BINFILE_H__
#define __BINFILE_H__


/*** Included Header Files ***/
#include "CoreStorage.h"
#include "CoreMetaProject.h"
#include "CoreMetaObject.h"
#include "CoreMetaAttribute.h"


namespace CryptoPP {
	class Filter;
}


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class BinObject;
class BinFileFactory;


// --------------------------- EntryLocation Class  --------------------------- //


// EntryLocation Helper Class
class EntryLocation {
private:
	// Hidden Type Enum
	typedef enum _EntryLocationEnum {
		ENTRYLOCATION_INPUT			= 0,		//!< Located in the input file
		ENTRYLOCATION_CACHE			= 1,		//!< Located in the memory cache
		ENTRYLOCATION_SCRATCH		= 2,		//!< Located in the scratch file
		ENTRYLOCATION_OUTPUT		= 3,		//!< Located in the output file
	} _EntryLocationEnum;
	_EntryLocationEnum				_location;	//!< Internal location value
	EntryLocation();							//!< Deny access to default constructor
	EntryLocation(const _EntryLocationEnum &loc) : _location(loc) { }			//!< Hidden primary constructor
public:
	EntryLocation(const EntryLocation &loc) : _location(loc._location)	{ }		//!< Copy constructor
	//Static Creation Methods
	inline static EntryLocation Input(void)		{ return EntryLocation(ENTRYLOCATION_INPUT); }	//!< Create a EntryLocation of type input
	inline static EntryLocation Cache(void)		{ return EntryLocation(ENTRYLOCATION_CACHE); }	//!< Create a EntryLocation of type cache
	inline static EntryLocation Scratch(void)	{ return EntryLocation(ENTRYLOCATION_SCRATCH); }//!< Create a EntryLocation of type scratch
	inline static EntryLocation Output(void)	{ return EntryLocation(ENTRYLOCATION_OUTPUT); }	//!< Create a EntryLocation of type output
	//Overridden Operators
	inline EntryLocation& operator=(const EntryLocation &loc)	{ this->_location = loc._location; return *this; }	//!< Equals operator
	inline bool operator==(const EntryLocation &loc) const	{ return this->_location == loc._location; }			//!< Equality operator
	inline bool operator!=(const EntryLocation &loc) const	{ return this->_location != loc._location; }			//!< Inequality operator
};


/*** Type Definitions ***/

typedef struct IndexEntry
{
	BinObject								*object;
	EntryLocation							location;
	std::streampos							position;
	uint32_t								sizeB;
	bool									isCompressed;
	bool									isEncrypted;
} IndexEntry;
typedef std::tr1::unordered_map<Uuid,IndexEntry,Uuid_HashFunc> IndexHash;
typedef IndexHash::iterator IndexHashIterator;

typedef struct AttributeChangeBase
{
	ValueType								type;
} AttributeChangeBase;
template <class T>
struct AttributeChange : public AttributeChangeBase
{
	T										oldValue;
	T										newValue;
};
typedef std::tr1::unordered_map<AttributeID,AttributeChangeBase*,AttributeID_HashFunc> ChangedAttributesHash;
typedef ChangedAttributesHash::iterator ChangedAttributesHashIterator;

typedef struct JournalEntry
{
	char									*buffer;
	EntryLocation							location;
	std::streampos							position;
	uint32_t								sizeB;
	Uuid									tag;
	uint32_t								numCreated, numChanged, numDeleted;
	bool									isCompressed;
	bool									isEncrypted;
	bool									preDirtyState;
} JournalEntry;
typedef std::list<JournalEntry> JournalList;


// --------------------------- BinAttribute ---------------------------


class BinAttribute
{
private:
	BinAttribute() : _parent(NULL), _attrID(ATTRID_NONE) { }		//!< Private default constructor

protected:
	BinObject										*_parent;		//!< Parent BinObject (needed for marking parent dirty)
	AttrID_t										_attrID;		//!< Attribute's ID (from the meta)
	BinAttribute(BinObject* parent, const AttrID_t &attrID) : _parent(parent), _attrID(attrID) { }	//!< Protected primary constructor

public:
	virtual ~BinAttribute()							{ }				//!< Default destructor
	// Static methods to read an attribute in from disk or create a blank new one
	static BinAttribute* Read(BinObject *parent, char* &stream);	//!< Read an attribute in from buffer
	static BinAttribute* Create(BinObject *parent, const ValueType &valueType, const AttrID_t &attrID);

	inline const AttrID_t GetAttributeID(void) const				{ return this->_attrID; }
	virtual const ValueType GetValueType(void) const throw()=0;		//!< Abstract method to get attribute type
	virtual void StreamWrite(char* &stream) const=0;				//!< Abstract buffer write
	virtual void StreamRead(char* &stream)=0;						//!< Abstract buffer read
	virtual uint32_t Size(void) const=0;							//!< Abstract size of the attribute
};


// --------------------------- BinObject ---------------------------


class BinObject
{
private:
	Uuid								_uuid;						//!< Uuid for identification
	CoreMetaObject						*_metaObject;				//!< MetaObject for this object
	std::list<BinAttribute*>			_attributes;				//!< List of attributes
	bool								_isDirty;					//!< Flag for if object attributes have been changed

	BinObject();													//!< Deny access to default constructor
	BinObject(CoreMetaObject* &metaObject, const Uuid &uuid) : _uuid(uuid), _metaObject(metaObject), _attributes(), _isDirty(false) { }
	void CreateAttributes(CoreMetaObject *metaobject);				//!< Uses metaObject to create empty attributes
	bool IsConnected(void) const;									//!< Are any forward pointers connected?

public:
	~BinObject();
	// Static methods to read or create an object
	static BinObject* Read(CoreMetaProject* &metaProject, char* &buffer, const Uuid &uuid, uint32_t &sizeB);
	static BinObject* Create(CoreMetaObject *metaObject, const Uuid &uuid);

	inline const Uuid GetUuid(void) const							{ return this->_uuid; }
	inline const MetaID_t GetMetaID(void) const						{ MetaID_t metaID; this->_metaObject->GetMetaID(metaID); return metaID; }
	inline CoreMetaObject* GetMetaObject(void) const				{ return this->_metaObject; }
	inline const bool IsDirty(void) const							{ return this->_isDirty; }
	inline void MarkDirty(void)										{ this->_isDirty = true; }
	uint32_t Size(void) const;										//!< Size of this object
	uint32_t Write(char* &buffer) const;							//!< Write the object to the vector
	BinAttribute* GetAttribute(const AttrID_t &attrID);				//!< Get a particular attribute
};


// --------------------------- BinFile ---------------------------


class BinFile : public ICoreStorage
{
private:
	// Base State & Option Variables
	std::string							_filename;					//!< Primary file name (opened as read-only)
	Uuid								_metaProjectUuid;			//!< MetaProject Uuid
	Uuid								_rootUuid;					//!< Uuid of the root object for the graph
	std::fstream						_inputFile;					//!< Handle to input file (see _filename)
	std::fstream						_scratchFile;				//!< Handle to scratch file
	// Index and Cache Queue Variables
	IndexHash							_indexHash;					//!< Hash of objects residing in memory cache
	bool								_isCaching;					//!< Is object caching being used
	std::list<Uuid>						_cacheQueue;				//!< Most-recently-used queue of memory cache
	uint64_t							_maxCacheSize;				//!< Maximum size (in bytes) of memory cache
	// Cursor Variable
	IndexHashIterator					_openedObject;				//!< Iterator to currently opened object
	// Transaction Variables
	std::list<std::pair<Uuid,MetaID_t> >_createdObjects;			//!< List of all objects created in current transaction
	ChangedAttributesHash				_changedAttributes;			//!< Hash of all attributes changed during current transaction
	std::list<std::pair<Uuid,IndexEntry> >_deletedObjects;			//!< List of all objects deleted in current transaction
	// Undo/Redo Variables
	bool								_isJournaling;				//!< Is journaling turned on
	uint64_t							_maxUndoSize;				//!< Maximum number of journal entries
	JournalList							_undoList;					//!< List of undo journal entries
	JournalList							_redoList;					//!< List of redo journal entries
	// Encryption & Compression Variables
	bool								_isCompressed;				//!< Is compression turned on
	CryptoPP::Filter					*_compressor, *_decompressor;	//!< Compression filters
	bool								_isEncrypted;				//!< Is encryption turned on
	CryptoPP::Filter					*_encryptor, *_decryptor;	//!< Encryption filters
	char								*_encryptionKey;			//!< What is the encryption key
	char								*_encryptionIV;				//!< The encryption IV

	BinFile();														//!< Deny access to default constructor
	BinFile(const BinFile &);										//!< Deny access to copy constructor
	BinFile& operator=(const BinFile &);							//!< Deny access to equals operator
	BinFile(const std::string &filename, CoreMetaProject *coreMetaProject);	//!< Hidden primary constructor (use Factory)

	friend class BinFileFactory;
	static const Result_t Create(const std::string &filename, CoreMetaProject *coreMetaProject,	//!<
								 ICoreStorage* &storage, const bool &encrypted) throw();
	static const Result_t Open(const std::string &filename, CoreMetaProject *coreMetaProject,	//!<
							   ICoreStorage* &storage, const std::vector<char> &encryptionKey) throw();

	// Private Methods
	const Result_t Load(void);										//!< Load an MGA in from file (really just gets index ready)
	const Result_t ReadIndex(std::fstream &stream, const uint64_t &indexSizeB);		//!< Read an index from an MGA file
	const Result_t WriteIndex(std::fstream &stream, const IndexHash &index, uint64_t &indexSizeB) const;//!< Write an index into an MGA file
	const Result_t ReadJournal(std::fstream &stream, const uint64_t &journalSizeB);	//!< Read a journal index from an MGA file
	const Result_t WriteJournal(std::fstream &stream, uint64_t &journalSizeB) const;//!< Write a journal index into an MGA file
	const Result_t ReadOptions(std::fstream &stream, const uint32_t &sizeB, std::streampos &startOfIndex, uint64_t &indexSize,	//!< Read the options
							   std::streampos &startOfJournal, uint64_t &journalSizeB);
	const uint32_t WriteOptions(std::fstream &stream, const std::streampos &startOfIndex, const uint64_t &indexSizeB,			//!< Write the options
								const std::streampos &startOfJournal, const uint64_t &journalSizeB) const;
	IndexHashIterator FetchObject(const Uuid &uuid);				//!< Bring an object into the cache
	void ObjectFromFile(std::fstream &stream, IndexEntry &indexEntry, const Uuid &uuid);//!< Move object from file to cache
	void ObjectToFile(std::fstream &stream, IndexEntry &entry);		//!< Move object to scratch file
	void CheckCacheSize(void);										//!< Make sure the cache is not getting too big
	void FlushCache(void);											//!< Clear the cache of all objects (no writing to any file)
	void FlushUndoRedo(const uint32_t &undoCount);					//!<
	const Result_t PickleTransaction(const Uuid &tag);				//!<
	const Result_t UnpickleTransaction(JournalEntry &entry);		//!<
	const Result_t PointerUpdate(const AttrID_t &attrID, const Uuid &uuid, const Uuid &oldVal, const Uuid &newVal);	//!< Update a pointer & backpointers

	template<class T, class P, ValueTypeEnum VT>
	const Result_t GetAttributeValue(const AttrID_t &attrID, T &value) throw();
	template<class T, class P, ValueTypeEnum VT>
	const Result_t SetAttributeValue(const AttrID_t &attrID, const T &value) throw();

	void TryAbortTransaction(void);
	IndexHashIterator TryCreateObject(const MetaID_t &metaID, const Uuid &uuid);

public:
	virtual ~BinFile();

	// ------- ICoreStorage Interface

	virtual const Result_t MetaObject(CoreMetaObject* &coreMetaObject) const throw();	//!< Get metaObject of open object
	virtual const Result_t MetaID(MetaID_t &metaID) const throw();						//!< Get metaID of open object

	virtual const Result_t ObjectVector(std::vector<Uuid> &objectVector) const throw();	//!< Get a vector of all objects
	virtual inline const Result_t RootUuid(Uuid &uuid) const throw()					{ uuid = this->_rootUuid; return S_OK; }
	virtual const Result_t Save(const std::string &filename, const bool &forceOverwrite=false) throw();//!< Save the project to filename (overwrites if same as _filename)
	virtual const Result_t BeginTransaction(void) throw();								//!< Begin a transaction (no nesting allowed)
	virtual const Result_t CommitTransaction(const Uuid tag=Uuid::Null()) throw();		//!< Commit a transaction (with optional transaction tag)
	virtual const Result_t AbortTransaction(void) throw();								//!< Abort a transaction and rollback all changes

	virtual const Result_t OpenObject(const Uuid &uuid) throw();						//!< Open an object
	virtual const Result_t CloseObject(void) throw();									//!< Close an object
	virtual const Result_t CreateObject(const MetaID_t &metaID, Uuid &newUuid) throw();	//!< Create a new object via metaObject
	virtual const Result_t DeleteObject(void) throw();									//!< Delete currently opened object

	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, int32_t &value) throw();			//!<
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, double &value) throw();			//!<
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, std::string &value) throw();		//!<
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, std::list<Uuid> &value) throw();	//!<
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, Uuid &value) throw();				//!<
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, DictionaryMap &value) throw();		//!< Get key value
	
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const int32_t &value) throw();		//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const double &value) throw();		//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const std::string &value) throw();	//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const std::list<Uuid> &value) throw();	//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const Uuid &value) throw();		//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const DictionaryMap &value) throw();	//!< Set key value

	// --- Object Caching

	virtual const Result_t EnableCaching(void) throw();									//!< Turn on object caching scheme
	virtual const Result_t DisableCaching(void) throw();								//!< Turn off object caching scheme
	virtual inline const Result_t IsCaching(bool &flag) const throw()					{ flag = this->_isCaching; return S_OK; }
	virtual inline const Result_t GetCacheSize(uint64_t &size) const throw()			{ size = this->_cacheQueue.size(); return S_OK; }
	virtual const Result_t SetCacheSize(const uint64_t &size) throw();					//!< Set limit for number of obj in the cache

	// --- Transaction Journaling

	virtual inline const Result_t EnableJournaling(void) throw()						{ this->_isJournaling = true; return S_OK; }
	virtual const Result_t DisableJournaling(void) throw();								//!<
	virtual const Result_t Undo(Uuid &tag) throw();										//!<
	virtual const Result_t Redo(Uuid &tag) throw();										//!<
	virtual inline const Result_t UndoCount(uint64_t &count) const throw()				{ count = (uint64_t)this->_undoList.size(); return S_OK; }
	virtual inline const Result_t RedoCount(uint64_t &count) const throw()				{ count = (uint64_t)this->_redoList.size(); return S_OK; }
	virtual inline const Result_t IsJournaled(bool &flag) const throw()					{ flag = this->_isJournaling; return S_OK; }
	virtual const Result_t JournalInfo(const uint64_t &undoMaxSize, const uint64_t &redoMaxSize,	//!<
									   std::list<Uuid> &undoJournal, std::list<Uuid> &redoJournal) const throw();

	// --- Compression

	virtual inline const Result_t EnableCompression(void) throw()						{ if(this->_inTransaction) return E_TRANSACTION; this->_isCompressed = true; return S_OK; }
	virtual inline const Result_t DisableCompression(void) throw()						{ if(this->_inTransaction) return E_TRANSACTION; this->_isCompressed = false; return S_OK; }
	virtual inline const Result_t IsCompressed(bool &flag) const throw()				{ flag = this->_isCompressed; return S_OK; }

	// --- Encryption

	virtual const Result_t EnableEncryption(const std::vector<char> &key) throw();		//!<
	virtual const Result_t DisableEncryption(void) throw();								//!<	
	virtual const Result_t IsEncrypted(bool &flag) const throw()						{ flag = this->_isEncrypted; return S_OK; }
	virtual const Result_t EncryptionKey(std::vector<char> &key) const throw();			//!<
};


// --------------------------- BinFileFactory --------------------------- //


// Factory class for creating BinFile objects
class BinFileFactory :
	public CoreStorageFactory
{
public:
	inline virtual const Result_t Create(const std::string &filename, CoreMetaProject *metaProject, ICoreStorage* &storage, const bool &encrypted=false)	{
		return BinFile::Create(filename, metaProject, storage, encrypted); }
	inline virtual const Result_t Open(const std::string &filename, CoreMetaProject *metaProject, ICoreStorage* &storage, const std::vector<char> &encryptionKey=std::vector<char>()) {
		return BinFile::Open(filename, metaProject, storage, encryptionKey); }
};


/*** End of MGA Namespace ***/
}

#endif	//__BINFILE_H__

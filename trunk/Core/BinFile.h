#ifndef __BINFILE_H__
#define __BINFILE_H__


/*** Included Header Files ***/
#include "CoreStorage.h"
#include "CoreMetaProject.h"
#include "CoreMetaObject.h"
#include "CoreMetaAttribute.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class BinFile;
class BinObject;
class BinAttribute;
class BinFileFactory;


/*** Locally Defined Values ***/
#define DTID_INDEX							10



// --------------------------- IndexLocation Class  --------------------------- //


// IndexLocation Helper Class
class IndexLocation {
private:
	// Hidden Type Enum
	typedef enum _IndexLocationEnum {
		INDEXLOCATION_INPUT			= 0,		//!< Located in the input file
		INDEXLOCATION_CACHE			= 1,		//!< Located in the memory cache
		INDEXLOCATION_SCRATCH		= 2,		//!< Located in the scratch file
	} _IndexLocationEnum;
	_IndexLocationEnum				_location;	//!< Internal location value
	IndexLocation();							//!< Deny access to default constructor
	IndexLocation(const _IndexLocationEnum &loc) : _location(loc) { }			//!< Hidden primary constructor
public:
	IndexLocation(const IndexLocation &loc) : _location(loc._location)	{ }		//!< Copy constructor
	//Static Creation Methods
	inline static IndexLocation Input(void)		{ return IndexLocation(INDEXLOCATION_INPUT); }	//!< Create a IndexLocation of type input
	inline static IndexLocation Cache(void)		{ return IndexLocation(INDEXLOCATION_CACHE); }	//!< Create a IndexLocation of type cache
	inline static IndexLocation Scratch(void)	{ return IndexLocation(INDEXLOCATION_SCRATCH); }//!< Create a IndexLocation of type scratch
	//Overridden Operators
	inline IndexLocation& operator=(const IndexLocation &loc)	{ this->_location = loc._location; return *this; }	//!< Equals operator
	inline bool operator==(const IndexLocation &loc) const	{ return this->_location == loc._location; }			//!< Equality operator
	inline bool operator!=(const IndexLocation &loc) const	{ return this->_location != loc._location; }			//!< Inequality operator
};


/*** Type Definitions ***/

typedef struct IndexEntry
{
	BinObject								*object;
	IndexLocation							location;
	std::streampos							position;
	uint32_t								sizeB;
} IndexEntry;
typedef STDEXT::hash_map<Uuid,IndexEntry,Uuid_HashFunc> IndexHash;
typedef IndexHash::iterator IndexHashIterator;

typedef struct AttributeChangeBase
{
	Uuid									uuid;
	AttrID_t								attrID;
} AttributeChangeBase;

template <class T>
struct AttributeChange : public AttributeChangeBase
{
	T										oldValue;
	T										newValue;
};
typedef std::list<AttributeChangeBase*> ChangedObjectsList;
typedef ChangedObjectsList::iterator ChangedObjectsListIterator;

typedef struct JournalEntry
{
	std::streampos							position;
	uint32_t								sizeB;
	Uuid									tag;
	bool									inScratch;
};
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
	static BinAttribute* Read(BinObject *parent, char* stream);
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
	std::map<std::string,std::string>	_dictionary;				//!< String::String dictionary
private:
	BinObject();													//!< Deny access to default constructor
	BinObject(CoreMetaObject* &metaObject, const Uuid &uuid) :
		_uuid(uuid), _metaObject(metaObject), _attributes(), _isDirty(false), _dictionary() { }
	void CreateAttributes(CoreMetaObject *metaobject);				//!< Uses metaObject to create empty attributes
	bool IsConnected(void) const;									//!< Are any forward pointers connected?

public:
	~BinObject();
	// Static methods to read or create an object
	static BinObject* Read(CoreMetaProject* &metaProject, const std::vector<char> &buffer, const Uuid &uuid);
	static BinObject* Create(CoreMetaObject *metaObject, const Uuid &uuid);

	inline const Uuid GetUuid(void) const							{ return this->_uuid; }
	inline const MetaID_t GetMetaID(void) const						{ MetaID_t metaID; this->_metaObject->GetMetaID(metaID); return metaID; }
	inline CoreMetaObject* GetMetaObject(void) const				{ return this->_metaObject; }
	inline const bool IsDirty(void) const							{ return this->_isDirty; }
	inline void MarkDirty(void)										{ this->_isDirty = true; }
	uint32_t Size(void) const;										//!<
	uint32_t Write(std::vector<char> &buffer) const;				//!<
	BinAttribute* GetAttribute(const AttrID_t &attrID);				//!<
};


// --------------------------- BinFile ---------------------------


class BinFile : 
	public ICoreStorage
{
private:
	// Base State & Option Variables
	std::string							_filename;					//!< Primary file name (opened as read-only)
	Uuid								_metaProjectUuid;			//!< MetaProject Uuid
	Uuid								_rootUuid;					//!< Uuid of the root object for the graph
	std::fstream						_inputFile;					//!< Handle to input file (see _filename)
	std::fstream						_scratchFile;				//!< Handle to scratch file
	bool								_isCompressed;				//!<
	// Index and Cache Queue Variables
	IndexHash							_indexHash;					//!< Hash of objects residing in memory cache
	std::list<Uuid>						_cacheQueue;				//!< Most-recently-used queue of memory cache
	uint64_t							_maxCacheSize;				//!< Maximum size (in bytes) of memory cache
	// Cursor Variable
	IndexHashIterator					_openedObject;				//!< Iterator to currently opened object
	// Transaction Variables
	std::list<std::pair<Uuid,MetaID_t> >_createdObjects;			//!< List of all objects created in current transaction
	ChangedObjectsList					_changedObjects;			//!< List of all object changes from current transaction
	std::list<std::pair<Uuid,IndexEntry> >_deletedObjects;			//!< List of all objects deleted in current transaction
	// Undo/Redo Variables
	bool								_isJournaling;				//!<
	uint32_t							_maxUndoSize;				//!<
	JournalList							_undoList;					//!<
	JournalList							_redoList;					//!<
	// Encryption Variables
	bool								_isEncrypted;				//!<
	Uuid								_encryptionKey;				//!<

private:
	BinFile();														//!< Deny access to default constructor
	BinFile(const BinFile &);										//!< Deny access to copy constructor
	BinFile& operator=(const BinFile &);							//!< Deny access to equals operator
	BinFile(const std::string &filename, CoreMetaProject *coreMetaProject);	//!< Hidden primary constructor (use Factory)

	friend class BinFileFactory;
	static const Result_t Create(const std::string &filename, CoreMetaProject *coreMetaProject, ICoreStorage* &storage);
	static const Result_t Open(const std::string &filename, CoreMetaProject *coreMetaProject, ICoreStorage* &storage);

	// Private Methods
	const Result_t Load(void);														//!< Load an MGA in from file (really just gets index ready)
	const Result_t ReadIndex(std::fstream &stream, const uint32_t &objCount);		//!< Read an index from an MGA file
	const Result_t WriteIndex(std::fstream &stream, const uint32_t &objCount) const;//!< Write an index into an MGA file
	IndexHashIterator FetchObject(const Uuid &uuid);								//!< Bring an object into the cache
	void ObjectFromFile(std::fstream &stream, IndexEntry &indexEntry, const Uuid &uuid);//!< Move object from file to cache
	void ObjectToFile(std::fstream &stream, IndexEntry &indexEntry, const Uuid &uuid);	//!< Move object to scratch file
	void CheckCacheSize(void);														//!< Make sure the cache is not getting too big
	void FlushCache(void);															//!< Clear the cache of all objects (no writing to any file)
	const Result_t PickleTransaction(uint32_t &sizeB) throw();						//!<
	const Result_t UnpickleTransaction(const JournalEntry &entry) throw();			//!<

public:
	virtual ~BinFile();
	const Result_t MaxMemoryFootprint(const uint64_t &size) throw();				//!< Set the maximum amount of memory the cache can use

	// ------- ICoreStorage Interface

	virtual const Result_t MetaObject(CoreMetaObject* &coreMetaObject) const throw();	//!< Get metaObject of open object
	virtual const Result_t MetaID(MetaID_t &metaID) const throw();						//!< Get metaID of open object

	virtual const Result_t ObjectVector(std::vector<Uuid> &objectVector) const throw();	//!< Get a vector of all objects
	virtual inline const Result_t RootUuid(Uuid &uuid) const throw()					{ uuid = this->_rootUuid; return S_OK; }
	virtual const Result_t Save(const std::string &filename) throw();					//!< Save the project to filename (overwrites if same as _filename)
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
	
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const int32_t &value) throw();		//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const double &value) throw();		//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const std::string &value) throw();	//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const std::list<Uuid> &value) throw();//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const Uuid &value) throw();		//!<

	virtual const Result_t Undo(Uuid &tag) throw();											//!<
	virtual const Result_t Redo(Uuid &tag) throw();											//!<
	virtual inline const Result_t UndoCount(uint32_t &count) const throw()					{ count = this->_undoList.size(); return S_OK; }
	virtual inline const Result_t RedoCount(uint32_t &count) const throw()					{ count = this->_redoList.size(); return S_OK; }
	virtual const Result_t JournalInfo(const uint32_t &undoMaxSize, const uint32_t redoMaxSize,	//!<
									   std::list<Uuid> &undoJournal, std::list<Uuid> &redoJournal) const throw();
	virtual const Result_t BeginJournal(void) throw();										//!<
	virtual const Result_t EndJournal(void) throw();										//!<
};


// --------------------------- BinFileFactory --------------------------- //


// Factory class for creating BinFile objects
class BinFileFactory :
	public CoreStorageFactory
{
public:
	inline virtual const Result_t Create(const std::string &filename, CoreMetaProject *metaProject, ICoreStorage* &storage)	{
		return BinFile::Create(filename, metaProject, storage); }
	inline virtual const Result_t Open(const std::string &filename, CoreMetaProject *metaProject, ICoreStorage* &storage) {
		return BinFile::Open(filename, metaProject, storage); }
};


/*** End of MGA Namespace ***/
}

#endif	//__BINFILE_H__
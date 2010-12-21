#ifndef __BINFILE_H__
#define __BINFILE_H__


/*** Included Header Files ***/
#include "CoreStorage.h"
#include "CoreMetaProject.h"
#include "CoreMetaObject.h"
#include "CoreMetaAttribute.h"


/*** Class Predefinitions ***/
class BinFile;
class BinObject;
class BinAttribute;
class BinFileFactory;


/*** Locally Defined Values ***/
#define DTID_INDEX							10


/*** Type Definitions ***/
typedef STDEXT::hash_map<MetaID_t, ObjID_t> MetaObjIDHash;
typedef MetaObjIDHash::iterator MetaObjIDHashIterator;

typedef STDEXT::hash_map<MetaObjIDPair,std::streampos,MetaObjIDPair_HashFunc,MetaObjIDPair_EqualKey> IndexHash;
typedef IndexHash::iterator IndexHashIterator;

typedef struct CacheEntry
{
	BinObject								*object;
	std::streampos							pos;
} CacheEntry;
typedef STDEXT::hash_map<MetaObjIDPair,CacheEntry,MetaObjIDPair_HashFunc,MetaObjIDPair_EqualKey> CacheHash;
typedef CacheHash::iterator CacheHashIterator;

typedef struct AttributeChangeBase
{
	MetaObjIDPair							idPair;
	AttrID_t								attrID;
} AttributeChangeBase;

template <class T>
struct AttributeChange : public AttributeChangeBase
{
	T										value;
};
typedef std::list<AttributeChangeBase*> ChangedObjectsList;
typedef ChangedObjectsList::iterator ChangedObjectsListIterator;


// --------------------------- BinAttribute ---------------------------


class BinAttribute
{
protected:
	BinObject										*_parent;		//!< Parent BinObject (needed for marking parent dirty)
	AttrID_t										_attrID;		//!< Attribute's ID (from the meta)
	BinAttribute(BinObject* parent, const AttrID_t &attrID) : _parent(parent), _attrID(attrID) { }	//!< Protected primary constructor
private:
	BinAttribute() : _parent(NULL), _attrID(ATTRID_NONE) { }		//!< Private default constructor

public:
	virtual ~BinAttribute()							{ }				//!< Default destructor
	// Static methods to read an attribute in from disk or create a blank new one
	static BinAttribute* Read(BinObject *parent, std::fstream &stream, std::vector<std::pair<AttrID_t,MetaObjIDPair> > &pointers, const bool &v3);
	static BinAttribute* Create(BinObject *parent, const ValueType &valueType, const AttrID_t &attrID);

	inline const AttrID_t AttributeID(void) const	{ return this->_attrID; }
	virtual const ValueType GetValueType(void) const throw()=0;					//!< Abstract method to get attribute type
	virtual void FileWrite(std::fstream &stream, const bool &v3) const=0;		//!< Abstract file write
	virtual void FileRead(std::fstream &stream, const bool &v3)=0;				//!< Abstract file read
};


// --------------------------- BinObject ---------------------------


class BinObject
{
private:
	MetaObjIDPair						_idPair;					//!< MetaID and ObjID pair for identification
	std::list<BinAttribute*>			_attributes;				//!< List of attributes
	bool								_isDirty;					//!< Flag for if object attributes have been changed
private:
	BinObject();													//!< Deny access to default constructor
	BinObject(const MetaObjIDPair &idPair) : _idPair(idPair), _attributes(), _isDirty(false) { }
	void CreateAttributes(CoreMetaObject *metaobject);				//!< Uses metaObject to create empty attributes
	bool IsConnected(void) const;									//!< Are any forward pointers connected?

public:
	~BinObject();
	// Static methods to read or create an object
	static BinObject* Read(std::fstream &stream, std::vector<std::pair<AttrID_t,MetaObjIDPair> > &pointers, const bool &v3);
	static BinObject* Create(CoreMetaObject *metaObject, const ObjID_t &objID);

	inline const MetaObjIDPair IDPair(void) const	{ return this->_idPair; }
	inline const MetaID_t MetaID(void) const		{ return this->_idPair.metaID; }
	inline const ObjID_t ObjID(void) const			{ return this->_idPair.objID; }
	inline const bool IsDirty(void) const			{ return this->_isDirty; }
	inline void MarkDirty(void)						{ this->_isDirty = true; }
	void Write(std::fstream &stream, IndexHash &hash, const bool &v3);
	BinAttribute* Attribute(const AttrID_t &attrID);
};


// --------------------------- BinFile ---------------------------


class BinFile : 
	public ICoreStorage
{
private:
	// Base State Variables
	std::string							_filename;					//!< Primary file name (opened as read-only)
	bool								_isV3;						//!< Is primary input file (see above) v3 format?
	UUID								_metaProjectUUID;			//!< MetaProject UUID
	std::fstream						_inputFile;					//!< Handle to input file (see _filename)
	std::fstream						_scratchFile;				//!< Handle to scratch file
	MetaObjIDHash						_metaIDHash;				//!< Hash of allowed metaIDs (from metaProject)
	// Input, Scratch and Cache Hash/Queue Variables
	IndexHash							_inputHash;					//!< Hash of objects residing in input file
	IndexHash							_scratchHash;				//!< Hash of objects residing in scratch file
	CacheHash							_cacheHash;					//!< Hash of objects residing in memory cache
	std::list<CacheHashIterator>		_cacheQueue;				//!< Most-recently-used queue of memory cache
	uint64_t							_maxCacheSize;				//!< Maximum size (in bytes) of memory cache
	// Cursor Variables
	CacheHashIterator					_openedObject;				//!< Iterator to currently opened object
	// Transaction Variables
	std::list<MetaObjIDPair>			_createdObjects;			//!< List of all objects created in current transaction
	ChangedObjectsList					_changedObjects;			//!< List of all object changes from current transaction
	std::list<std::pair<MetaObjIDPair,CacheEntry> >	_deletedObjects;//!< List of all objects deleted in current transaction

private:
	BinFile();														//!< Deny access to default constructor
	BinFile(const BinFile &);										//!< Deny access to copy constructor
	BinFile& operator=(const BinFile &);							//!< Deny access to equals operator
	BinFile(const std::string &filename, CoreMetaProject *coreMetaProject);	//!< Hidden primary constructor (use Factory)

	friend class BinFileFactory;
	static const Result_t Create(const std::string &filename, CoreMetaProject *coreMetaProject, ICoreStorage* &storage);
	static const Result_t Open(const std::string &filename, CoreMetaProject *coreMetaProject, ICoreStorage* &storage);

	// Private Methods
	const Result_t Load(void);										//!< Load an MGA in from file (really just gets index ready)
	void OpenMetaProject(void);										//!< MetaProject UUID -> _metaProjectUUID and hash all METAIDs
	const Result_t ReadIndex(std::fstream &stream);					//!< Read an index from an MGA file
	const Result_t WriteIndex(std::fstream &stream, const IndexHash &hash);	//!< Write an index into an MGA file
	const Result_t BuildIndex(void);								//!< Build an index by reading through an entire MGA file
	CacheHashIterator FetchObject(const MetaObjIDPair &idPair);		//!< Bring an object into the cache
	CacheHashIterator CacheObjectFromFile(std::fstream &stream, IndexHash &hash, IndexHashIterator &iter, const bool &v3);	//!< Move object from file to cache
	void CheckCacheSize(void);										//!< Make sure the cache is not getting too big
	void FlushCache(void);											//!< Clear the cache of all objects (no writing to any file)

public:
	virtual ~BinFile();
	const Result_t MaxMemoryFootprint(const uint64_t &size) throw();//!< Set the maximum amount of memory the cache can use

	// ------- ICoreStorage Interface

	virtual const Result_t MetaObject(CoreMetaObject* &coreMetaObject) const throw();		//!< Get metaObject of open object
	virtual const Result_t MetaID(MetaID_t &metaID) const throw();							//!< Get metaID of open object

	virtual const Result_t ObjectVector(std::vector<MetaObjIDPair> &objectVector) throw();	//!< Get a vector of all objects
	virtual const Result_t Save(const std::string &filename, const bool &v3=true) throw();	//!< Save the project to filename (overwrites if same as _filename)
	virtual const Result_t BeginTransaction(void) throw();									//!< Begin a transaction (no nesting allowed)
	virtual const Result_t CommitTransaction(void) throw();									//!< Commit a transaction
	virtual const Result_t AbortTransaction(void) throw();									//!< Abort a transaction and rollback all changes

	virtual const Result_t OpenObject(const MetaObjIDPair &idPair) throw();					//!< Open an object (must be in transaction)
	virtual const Result_t CloseObject(void) throw();										//!< Close an object
	virtual const Result_t CreateObject(const MetaID_t &metaID, ObjID_t &newObjID) throw();	//!< Create a new object via metaObject
	virtual const Result_t DeleteObject(void) throw();										//!< Delete currently opened object

	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, int32_t &value) throw();							//!<
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, double &value) throw();							//!<
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, std::string &value) throw();						//!<
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, std::vector<unsigned char> &value) throw();		//!<
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, std::list<MetaObjIDPair> &value) throw();			//!<
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, MetaObjIDPair &value) throw();						//!<
	
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const int32_t &value) throw();						//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const double &value) throw();						//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const std::string &value) throw();					//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const std::vector<unsigned char> &value) throw();	//!<
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const MetaObjIDPair &value) throw();				//!<
};


// --------------------------- BinAttributeBase --------------------------- //


template<class T>
class BinAttributeBase : public BinAttribute
{
protected:
	T						_value;
	BinAttributeBase<T>();
public:
	BinAttributeBase<T>(BinObject* parent, const AttrID_t &attrID, const T &value) : ::BinAttribute(parent, attrID), _value(value) { }
	inline const Result_t Set(const T &value)			{ this->_parent->MarkDirty(); this->_value = value; return S_OK; }
	inline T Get(void) const							{ return this->_value; }
	virtual inline void FileRead(std::fstream &stream, const bool &v3)	{ stream.read((char*)&this->_value, sizeof(T)); }
};


// --------------------------- BinAttribute::ValueType::Long() --------------------------- //



class BinAttributeLong : public BinAttributeBase<int32_t> {
public:
	BinAttributeLong(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<int32_t>(parent, attrID, 0) { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::Long(); }
	virtual void FileWrite(std::fstream &stream, const bool &v3) const;
};


// --------------------------- BinAttribute::ValueType::Real() --------------------------- //


class BinAttributeReal : public BinAttributeBase<double> {
public:
	BinAttributeReal(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<double>(parent, attrID, 0.0) { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::Real(); }
	virtual void FileWrite(std::fstream &stream, const bool &v3) const;
};


// --------------------------- BinAttribute::ValueType::String() --------------------------- //


class BinAttributeString : public BinAttributeBase<std::string> {
public:
	BinAttributeString(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<std::string>(parent, attrID, "") { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::String(); }
	virtual void FileWrite(std::fstream &stream, const bool &v3) const;
	virtual void FileRead(std::fstream &stream, const bool &v3);
};


// --------------------------- BinAttribute::ValueType::Binary() --------------------------- //


class BinAttributeBinary : public BinAttributeBase<std::vector<unsigned char> > {
public:
	BinAttributeBinary(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<std::vector<unsigned char> >(parent, attrID, std::vector<unsigned char>()) { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::Binary(); }
	virtual void FileWrite(std::fstream &stream, const bool &v3) const;
	virtual void FileRead(std::fstream &stream, const bool &v3);
};


// --------------------------- BinAttribute::ValueType::Lock() --------------------------- //


class BinAttributeLock : public BinAttributeBase<LockType> {
public:
	BinAttributeLock(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<LockType>(parent, attrID, LockType::None()) { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::Lock(); }
	virtual inline void FileWrite(std::fstream &stream, const bool &v3) const	{ }
	virtual inline void FileRead(std::fstream &stream, const bool &v3)			{ this->_value = LockType::None(); }
};


// --------------------------- BinAttribute::ValueType::Collection() --------------------------- //


class BinAttributeCollection : public BinAttributeBase<std::list<MetaObjIDPair> > {
public:
	BinAttributeCollection(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<std::list<MetaObjIDPair> >(parent, attrID, std::list<MetaObjIDPair>()) { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::Collection(); }
	inline void Add(const MetaObjIDPair &value)					{ this->_parent->MarkDirty(); this->_value.push_back(value); }
	inline void Remove(const MetaObjIDPair &value)				{ this->_parent->MarkDirty(); this->_value.remove(value); }
	virtual void FileWrite(std::fstream &stream, const bool &v3) const;
	virtual void FileRead(std::fstream &stream, const bool &v3);
};


// --------------------------- BinAttribute::ValueType::Pointer() --------------------------- //


class BinAttributePointer : public BinAttributeBase<MetaObjIDPair> {
public:
	BinAttributePointer(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<MetaObjIDPair>(parent, attrID, MetaObjIDPair(METAID_NONE,OBJID_NONE)) { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::Pointer(); }
	inline const MetaID_t MetaID(void) const					{ return this->_value.metaID; }
	inline const ObjID_t ObjID(void) const						{ return this->_value.objID; }
	inline const bool IsConnected(void) const					{ return (this->_value.metaID != METAID_NONE && this->_value.objID != OBJID_NONE); }
	virtual void FileWrite(std::fstream &stream, const bool &v3) const;
	virtual void FileRead(std::fstream &stream, const bool &v3);
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


#endif	//__BINFILE_H__

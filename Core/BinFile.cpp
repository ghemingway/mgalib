/*** Included Header Files ***/
#include "BinFile.h"
#include "CoreMetaObject.h"
#include "CoreMetaProject.h"
#include <utf8.h>
#include <cryptopp/zlib.h>
#include <cryptopp/gcm.h>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>


/*** Internally Defined Constants ***/
#define BINFILE_DEFAULTCACHING				true
#define BINFILE_DEFAULTCACHESIZE			SIZE_MAX
#define BINFILE_DEFAULTMAXUNDO				SIZE_MAX
#define BINFILE_DEFAULTCOMPRESSION			true
#define BINFILE_DEFAULTJOURNALING			true
#define BINFILE_JOURNALCOMPRESSIONLIMIT		0
#define BINFILE_ENCRYPTIONKEYSIZE			CryptoPP::AES::DEFAULT_KEYLENGTH
#define BINFILE_ENCRYPTIONIVSIZE			CryptoPP::AES::BLOCKSIZE * 16


// --------------------------- IOStream Access  --------------------------- //


// IFStream Reads and Writes
template <class T> void _Read(char* &stream, T &value)						{ memcpy(&value, stream, sizeof(T)); stream += sizeof(T); }
template <> void _Read<ValueType>(char* &stream, ValueType &valueType)		{ valueType = ValueType::Read(stream); }
template <> void _Read<std::streampos>(char* &stream, std::streampos &pos)	{ uint64_t value; memcpy(&value, stream, sizeof(uint64_t));  pos = (std::streampos)value; stream += sizeof(uint64_t); }
template <> void _Read<std::string>(char* &stream, std::string &str)
{
	// Read the length of the string
	uint32_t len;
	_Read(stream, len);
	if (len > 0)
	{
		// Create the string
		str.resize(len);
		memcpy(&str[0], stream, len);
		stream += len;
	}
}
template <> void _Read<std::list<Uuid> >(char* &stream, std::list<Uuid> &collection)
{
	// Read the number of Uuids in the list
	uint32_t len;
	_Read(stream, len);
	// Read data into the list
	Uuid uuid;
	for (uint32_t i=0; i<len; ++i)
	{
		// One read from the stream per Uuid
		_Read(stream, uuid);
		collection.push_back(uuid);
	}
}
template <> void _Read<DictionaryMap>(char* &stream, DictionaryMap &dictionary)
{
	// Read the number of dictionary entries
	uint32_t entryCount;
	_Read(stream, entryCount);
	// Read entryCount number of entries
	for (uint32_t i=0; i<entryCount; ++i)
	{
		std::string key="", value="";
		// Read the key
		_Read<std::string>(stream, key);
		// Read the value
		_Read<std::string>(stream, value);
		// Entry the key-value pair into the dictionary
		dictionary.insert( std::make_pair(key, value) );
	}
}

template <class T> void _Write(char* &stream, const T &value)					{ memcpy(stream, &value, sizeof(T)); stream += sizeof(T); }
template <> void _Write<ValueType>(char* &stream, const ValueType &valueType)	{ valueType.Write(stream); }
template <> void _Write<std::streampos>(char* &stream, const std::streampos &pos){ uint64_t value = (uint64_t)pos; memcpy(stream, &value, sizeof(uint64_t)); stream += sizeof(uint64_t); }
template <> void _Write<std::string>(char* &stream, const std::string &str)
{
	// Write the length of the string
	uint32_t len = str.length();
	_Write(stream, len);
	if (len > 0)
	{
		// Write the actual string
		memcpy(stream, &str[0], len);
		stream += len;
	}
}
template <> inline void _Write<std::list<Uuid> >(char* &stream, const std::list<Uuid> &collection)
{
	// Write the length of the list
	uint32_t len = collection.size();
	_Write(stream, len);
	// Write the actual list
	std::list<Uuid>::const_iterator collectionIter = collection.begin();
	while (collectionIter != collection.end())
	{
		// One write to the stream per Uuid
		_Write(stream, *collectionIter);
		++collectionIter;
	}
}
template <> void _Write<DictionaryMap>(char* &stream, const DictionaryMap &dictionary)
{
	// Write the number of dictionary entries
	uint32_t entryCount = dictionary.size();
	_Write(stream, entryCount);
	// Iterate through the whole map and write each key and each value
	DictionaryMap::const_iterator dictIter = dictionary.begin();
	while (dictIter != dictionary.end())
	{
		// Write the key
		_Write<std::string>(stream, dictIter->first);
		// Write the value
		_Write<std::string>(stream, dictIter->second);
		// Move to the next key-value
		++dictIter;
	}
}


// --------------------------- Attribute Sizing Functions --------------------------- //


template <class T> static inline uint32_t _Size(const T &value)	{ return sizeof(T); }
template <> inline uint32_t _Size<std::string>(const std::string &value)
	{ return (sizeof(uint32_t) + value.size()); }
template <> inline uint32_t _Size<std::list<Uuid> >(const std::list<Uuid> &value)
	{ return (sizeof(uint32_t) + (value.size() * sizeof(Uuid))); }
template <> inline uint32_t _Size<DictionaryMap>(const DictionaryMap &value)
	{
		// Size it at least the number of elements in the dictionary
		uint32_t size = sizeof(uint32_t);
		// Iterate through the whole map and get the size of each key and value
		DictionaryMap::const_iterator dictIter = value.begin();
		while (dictIter != value.end())
		{
			// Add the size of the key (plus 4 for recording its size)
			size += sizeof(uint32_t) + dictIter->first.size();
			// Add the size of the value (plus 4 for recording its size)
			size += sizeof(uint32_t) + dictIter->second.size();
			// Move to the next key-value
			++dictIter;
		}
		return size;
	}


// --------------------------- UTF-8 Encoding Check --------------------------- //


static inline const Result_t _CheckUTF8(const std::string &value)
{
	std::string localValue = value;
	Result_t result = S_OK;
	// Make sure this is a valid UTF-8 encoding
	std::string::iterator endIter = utf8::find_invalid(localValue.begin(), localValue.end());
	if (endIter != localValue.end())
	{
		std::cout << "Invalid UTF-8 encoding detected!\n";
		std::cout << "This part is fine: " << std::string(localValue.begin(), endIter) << "\n";
		result = E_BADUTF8STRING;
	}
	return result;
}


// --------------------------- Compression and Encryption Support --------------------------- //


static inline uint64_t _Compress(CryptoPP::Filter *compressor, char* &buffer, const uint64_t &inputSizeB)
{
	CryptoPP::ZlibCompressor tmpCompressor;
	// Clear and load up the compressor
	tmpCompressor.Put((const byte*)buffer, (size_t)inputSizeB);
	tmpCompressor.MessageEnd();
	// Get the new size
	uint64_t outputSizeB = (uint64_t)tmpCompressor.MaxRetrievable();
	ASSERT( outputSizeB != 0 );
	delete buffer;
	buffer = new char[(size_t)outputSizeB];
	// Get the data and clean up
	tmpCompressor.Get((byte*)buffer, (size_t)outputSizeB);
	return outputSizeB;
}


static inline uint64_t _Decompress(CryptoPP::Filter *decompressor, char* &buffer, const uint64_t &inputSizeB)
{
	// Clear the decompressor and load it up
	CryptoPP::ZlibDecompressor tmpDecompressor;
	tmpDecompressor.Put((const byte*)buffer, (size_t)inputSizeB);
	tmpDecompressor.MessageEnd();
	// Get the decompressed size
	uint64_t outputSizeB = (uint64_t)tmpDecompressor.MaxRetrievable();
	ASSERT( outputSizeB != 0 );
	// Resize the buffer
	delete buffer;
	buffer = new char[(size_t)outputSizeB];
	// Get the data and clean up
	tmpDecompressor.Get((byte*)buffer, (size_t)outputSizeB);
	return outputSizeB;
}


static inline void _Encrypt(CryptoPP::Filter *filter, const char* key, const char* iv, char* &buffer, const uint64_t &sizeB)
{
	// Create the encryptor and filter
	CryptoPP::GCM<CryptoPP::AES>::Encryption encryptor;
	encryptor.SetKeyWithIV((const byte*)key, BINFILE_ENCRYPTIONKEYSIZE,
						   (const byte*)iv, BINFILE_ENCRYPTIONIVSIZE);
	CryptoPP::AuthenticatedEncryptionFilter tmpFilter( encryptor );
	// Load up our data
	tmpFilter.Put( (const byte*)buffer, (size_t)sizeB );
	tmpFilter.MessageEnd();
	// Make sure we get out the same number of bytes we put in
	ASSERT( sizeB == (uint64_t)tmpFilter.MaxRetrievable() );
	// Encrypt and get data
	tmpFilter.Get( (byte*)buffer, (size_t)sizeB );
}


static inline void _Decrypt(CryptoPP::Filter *filter, const char* key, const char* iv, char* &buffer, const uint64_t &sizeB)
{
	// Create the decryptor and filter
	CryptoPP::GCM<CryptoPP::AES>::Decryption decryptor;
	decryptor.SetKeyWithIV((const byte*)key, BINFILE_ENCRYPTIONKEYSIZE,
						   (const byte*)iv, BINFILE_ENCRYPTIONIVSIZE);
	CryptoPP::AuthenticatedDecryptionFilter tmpFilter( decryptor );
	// Load up our data
	tmpFilter.Put( (const byte*)buffer, (size_t)sizeB );
	tmpFilter.MessageEnd();
	// Make sure we get out the same number of bytes we put in
	ASSERT( sizeB == (uint64_t)tmpFilter.MaxRetrievable() );
	// Decrypt and get data
	tmpFilter.Get( (byte*)buffer, (size_t)sizeB );
}


// --------------------------- BinAttributeBase --------------------------- //


template<class T>
class BinAttributeBase : public BinAttribute
{
protected:
	T						_value;
	BinAttributeBase<T>();
public:
	BinAttributeBase<T>(BinObject* parent, const AttrID_t &attrID, const T &value) : ::BinAttribute(parent, attrID), _value(value) { }
	inline virtual const Result_t Set(const T &value)	{ this->_parent->MarkDirty(); this->_value = value; return S_OK; }
	inline virtual T Get(void) const					{ return this->_value; }
	inline virtual uint32_t Size(void) const			{ return _Size(this->_value); }
	inline virtual void StreamRead(char* &stream)		{ _Read(stream, this->_value); }
	inline virtual void StreamWrite(char* &stream) const{ _Write(stream, this->_attrID); _Write(stream, this->_value); }
};


// --------------------------- BinAttribute::ValueType::Long() --------------------------- //


class BinAttributeLong : public BinAttributeBase<int32_t>
{
public:
	BinAttributeLong(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<int32_t>(parent, attrID, 0) { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::Long(); }
	virtual inline void StreamWrite(char* &stream) const{ _Write(stream, ValueType::Long()); BinAttributeBase<int32_t>::StreamWrite(stream); }
};


// --------------------------- BinAttribute::ValueType::Real() --------------------------- //


class BinAttributeReal : public BinAttributeBase<double>
{
public:
	BinAttributeReal(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<double>(parent, attrID, 0.0) { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::Real(); }
	virtual inline void StreamWrite(char* &stream) const{ _Write(stream, ValueType::Real()); BinAttributeBase<double>::StreamWrite(stream); }
};


// --------------------------- BinAttribute::ValueType::String() --------------------------- //


class BinAttributeString : public BinAttributeBase<std::string>
{
public:
	BinAttributeString(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<std::string>(parent, attrID, "") { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::String(); }
	virtual inline const Result_t Set(const std::string &value)
	{
		// Make sure string is valid UTF-8
		if (_CheckUTF8(value) != S_OK) return E_BADUTF8STRING;
		// Get the line length (at least for the valid part)
		this->_value = value;
		this->_parent->MarkDirty();
		return S_OK;
	}
	virtual inline void StreamWrite(char* &stream) const{ _Write(stream, ValueType::String()); BinAttributeBase<std::string>::StreamWrite(stream); }
};


// --------------------------- BinAttribute::ValueType::LongPointer() --------------------------- //


class BinAttributeLongPointer : public BinAttributeBase<Uuid>
{
public:
	BinAttributeLongPointer(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<Uuid>(parent, attrID, Uuid(Uuid::Null())) { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::LongPointer(); }
	virtual inline void StreamWrite(char* &stream) const{ _Write(stream, ValueType::LongPointer()); BinAttributeBase<Uuid>::StreamWrite(stream); }
};


// --------------------------- BinAttribute::ValueType::Collection() --------------------------- //


class BinAttributeCollection : public BinAttributeBase<std::list<Uuid> >
{
public:
	BinAttributeCollection(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<std::list<Uuid> >(parent, attrID, std::list<Uuid>()) { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::Collection(); }
	inline void Add(const Uuid &value)					{ this->_parent->MarkDirty(); this->_value.push_back(value); }
	inline void Remove(const Uuid &value)				{ this->_parent->MarkDirty(); this->_value.remove(value); }
	virtual inline void StreamWrite(char* &stream) const{ _Write(stream, ValueType::Collection()); BinAttributeBase<std::list<Uuid> >::StreamWrite(stream); }
};


// --------------------------- BinAttribute::ValueType::Pointer() --------------------------- //


class BinAttributePointer : public BinAttributeBase<Uuid>
{
public:
	BinAttributePointer(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<Uuid>(parent, attrID, Uuid(Uuid::Null())) { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::Pointer(); }
	inline const bool IsConnected(void) const			{ return (this->_value != Uuid::Null()); }
	virtual inline void StreamWrite(char* &stream) const{ _Write(stream, ValueType::Pointer()); BinAttributeBase<Uuid>::StreamWrite(stream); }
};


// --------------------------- BinAttribute::ValueType::Dictionary() --------------------------- //


class BinAttributeDictionary : public BinAttributeBase<DictionaryMap>
{
public:
	BinAttributeDictionary(BinObject* parent, const AttrID_t &attrID) : ::BinAttributeBase<DictionaryMap>(parent, attrID, DictionaryMap()) { }
	virtual inline const ValueType GetValueType(void) const throw()	{ return ValueType::Dictionary(); }
	virtual inline void StreamWrite(char* &stream) const{ _Write(stream, ValueType::Dictionary()); BinAttributeBase<DictionaryMap>::StreamWrite(stream); }
};


// --------------------------- BinAttr ---------------------------


BinAttribute *BinAttribute::Read(BinObject *parent, char* &stream)
{
	ASSERT( parent != NULL );
	ASSERT( stream != NULL );
	// Get the value type
	ValueType valueType;
	_Read(stream, valueType);
	if( valueType == ValueType::None() ) return NULL;
	// Get the attribute ID
	AttrID_t attrID;
	_Read(stream, attrID);
	ASSERT( attrID != ATTRID_NONE );	
	BinAttribute *binAttribute = NULL;

	if(valueType == ValueType::Long())
		binAttribute = new BinAttributeLong(parent, attrID);
	else if (valueType == ValueType::Real())
		binAttribute = new BinAttributeReal(parent, attrID);
	else if (valueType == ValueType::String())
		binAttribute = new BinAttributeString(parent, attrID);
	else if (valueType == ValueType::LongPointer())
		binAttribute = new BinAttributeLongPointer(parent, attrID);
	else if (valueType == ValueType::Collection())
		binAttribute = new BinAttributeCollection(parent, attrID);
	else if (valueType == ValueType::Pointer())
		binAttribute = new BinAttributePointer(parent, attrID);
	else if (valueType == ValueType::Dictionary())
		binAttribute = new BinAttributeDictionary(parent, attrID);
	// Return what we have
	ASSERT( binAttribute != NULL );
	// Command the attribute to finish reading itself
	binAttribute->StreamRead(stream);
	// Return the new attribute
	return binAttribute;
};


BinAttribute* BinAttribute::Create(BinObject *parent, const ValueType &valueType, const AttrID_t &attrID)
{
	ASSERT( parent != NULL );
	ASSERT( valueType != ValueType::None() );
	ASSERT( attrID != ATTRID_NONE );
	BinAttribute *binAttribute = NULL;
	// Create the new attribute based on the type
	if(valueType == ValueType::Long())
		binAttribute = new BinAttributeLong(parent, attrID);
	else if (valueType == ValueType::Real())
		binAttribute = new BinAttributeReal(parent, attrID);
	else if (valueType == ValueType::String())
		binAttribute = new BinAttributeString(parent, attrID);
	else if (valueType == ValueType::LongPointer())
		binAttribute = new BinAttributeLongPointer(parent, attrID);
	else if (valueType == ValueType::Collection())
		binAttribute = new BinAttributeCollection(parent, attrID);
	else if (valueType == ValueType::Pointer())
		binAttribute = new BinAttributePointer(parent, attrID);
	else if (valueType == ValueType::Dictionary())
		binAttribute = new BinAttributeDictionary(parent, attrID);
	// Return what we have
	ASSERT( binAttribute != NULL );	
	// Return the new attribute
	return binAttribute;
}


// --------------------------- Private BinObject Methods ---------------------------

 
void BinObject::CreateAttributes(CoreMetaObject *metaObject)
{
	ASSERT( metaObject != NULL );
	ASSERT( this->_attributes.empty() );
	// Get the meta attributes for the meta object
	std::list<CoreMetaAttribute*> metaAttributeList;
	Result_t result = metaObject->GetAttributes(metaAttributeList);
	ASSERT( result == S_OK );
	std::list<CoreMetaAttribute*>::const_iterator attribIter = metaAttributeList.begin();
	while( attribIter != metaAttributeList.end() )
	{
		ValueType valueType;
		result = (*attribIter)->GetValueType(valueType);
		ASSERT( result == S_OK );
		AttrID_t attrID = ATTRID_NONE;
		result = (*attribIter)->GetAttributeID(attrID);
		ASSERT( result == S_OK );
		BinAttribute *binAttribute = BinAttribute::Create(this, valueType, attrID);
		ASSERT( binAttribute != NULL );
		this->_attributes.push_front(binAttribute);
		++attribIter;
	}
	// Mark as dirty
	this->_isDirty = true;
}


bool BinObject::IsConnected(void) const
{
	// Special case - Root object (METAID_ROOT) is always connected
	MetaID_t metaID;
	Result_t result = this->_metaObject->GetMetaID(metaID);
	ASSERT( result == S_OK );
	if( metaID == METAID_ROOT ) return true;
	// Go through all attributes
	std::list<BinAttribute*>::const_iterator attrIter = this->_attributes.begin();
	while( attrIter != this->_attributes.end() )
	{
		// If the attribute is a pointer
		if( (*attrIter)->GetValueType() == ValueType::Pointer() )
		{
			BinAttributePointer* pointer = (BinAttributePointer*)(*attrIter);
			// Is the pointer connected?
			if( pointer->IsConnected() ) return true;
		}
		// Move on to the next attribute
		++attrIter;
	}
	// Never found a connected pointer = not connected
	ASSERT(false); // Do we actually want to support deferred deletion???
	return false;
}


// --------------------------- Public BinObject Methods ---------------------------


BinObject::~BinObject()
{
	// Delete every attribute in the list
	while( !this->_attributes.empty() )
	{
		delete this->_attributes.front();
		this->_attributes.pop_front();
	}
}


BinObject* BinObject::Read(CoreMetaProject* &metaProject, char* &buffer, const Uuid &uuid, uint32_t &sizeB)
{
	char* stream = buffer;
	// First, read in the metaID
	MetaID_t metaID;
	_Read(stream, metaID);
	// We are done if is METAID_NONE
	if( metaID == METAID_NONE ) return NULL;
	// Get the metaObject using this metaID
	CoreMetaObject* metaObject = NULL;
	Result_t result = metaProject->GetObject(metaID, metaObject);
	ASSERT( result == S_OK );
	
	// Create the binObject
	BinObject* binObject = new BinObject(metaObject, uuid);
	ASSERT( binObject != NULL );
	BinAttribute *binAttribute = NULL;
	// Continue to read in attributes until a NULL is returned
	do
	{
		// Read in an attribute
		binAttribute = BinAttribute::Read(binObject, stream);
		// Add the attribute to the object
		if (binAttribute != NULL) binObject->_attributes.push_front(binAttribute);
	} while (binAttribute != NULL);
	// How much did we read
	sizeB = (stream - buffer);
	// Return the new BinObject
	return binObject;
};


BinObject* BinObject::Create(CoreMetaObject *metaObject, const Uuid &uuid)
{
	ASSERT( metaObject != NULL );
	ASSERT( uuid != Uuid::Null() );
	// Create a new binObject (uuid is determined by BinFile, which should be calling this method)
	BinObject* binObject = new BinObject(metaObject, uuid);
	ASSERT( binObject != NULL);
	// Add the object's attributes (also marks the object as dirty)
	binObject->CreateAttributes(metaObject);
	return binObject;
}


uint32_t BinObject::Size(void) const
{
	// Remember, objects have a MetaID in addition to attributes
	uint32_t size = sizeof(MetaID_t);
	// Go through all attributes
	std::list<BinAttribute*>::const_iterator attrIter = this->_attributes.begin();
	while( attrIter != this->_attributes.end() )
	{
		// Sum the size (don't forget the ValueType and AttrID for each attribute)
		size += (*attrIter)->Size() + sizeof(uint8_t) + sizeof(AttrID_t);
		// Move on to the next attribute
		++attrIter;
	}
	// Return the total size, remember the trailing ValueType::None()
	size += sizeof(uint8_t);
	return size;
}


uint32_t BinObject::Write(char* &buffer) const
{
	// Is this object deleted (via implicit deferred deletion - i.e. no connected forward pointers)
//	if ( !this->IsConnected() ) return 0;
	char* stream = buffer;
	// Write the metaID
	MetaID_t metaID;
	Result_t result = this->_metaObject->GetMetaID(metaID);
	ASSERT( result == S_OK );
	_Write(stream, metaID);
	// Write all of the attributes
	std::list<BinAttribute*>::const_iterator attrIter = this->_attributes.begin();
	while( attrIter != this->_attributes.end() )
	{
		BinAttribute* binAttribute = *attrIter;
		binAttribute->StreamWrite(stream);
		// Move to the next attribute
		++attrIter;
	}
	// Write the closing ValueType::None() to signal end of object
	_Write(stream, ValueType::None());
	uint32_t writeSize = (uint32_t)(stream - buffer);
	ASSERT( writeSize == this->Size() );
	return writeSize;
}


// Find the desired attribute in this object
BinAttribute* BinObject::GetAttribute(const AttrID_t &attrID)
{
	std::list<BinAttribute*>::iterator binAttrIter = this->_attributes.begin();
	while( binAttrIter != this->_attributes.end() && (*binAttrIter)->GetAttributeID() != attrID )
		++binAttrIter;
	// Did we not find it return NULL
	if( binAttrIter == this->_attributes.end() ) return NULL;
	// Otherise, return the binAttribute poiter
	return *binAttrIter;
}

 
// --------------------------- Private BinFile Methods ---------------------------


BinFile::BinFile(const std::string &filename, CoreMetaProject *coreMetaProject) : ::ICoreStorage(coreMetaProject),
	_filename(filename), _metaProjectUuid(), _rootUuid(), _inputFile(), _scratchFile(),
	_indexHash(), _isCaching(BINFILE_DEFAULTCACHING), _cacheQueue(), _maxCacheSize(BINFILE_DEFAULTCACHESIZE),
	_openedObject(), _createdObjects(), _changedAttributes(), _deletedObjects(),
	_isJournaling(BINFILE_DEFAULTJOURNALING), _maxUndoSize(BINFILE_DEFAULTMAXUNDO), _undoList(), _redoList(),
	_isCompressed(BINFILE_DEFAULTCOMPRESSION), _compressor(NULL), _decompressor(NULL),
	_isEncrypted(false), _encryptor(NULL), _decryptor(NULL), _encryptionKey(NULL), _encryptionIV(NULL)
{
	ASSERT(filename != "" );
	ASSERT( coreMetaProject != NULL );
	this->_openedObject = this->_indexHash.end();
	// Setup CryptoPP filters
	this->_compressor = new CryptoPP::ZlibCompressor();
	ASSERT( this->_compressor != NULL );
	this->_decompressor = new CryptoPP::ZlibDecompressor();
	ASSERT( this->_decompressor != NULL );
	// Set up the encryption support
	CryptoPP::GCM<CryptoPP::AES>::Encryption encryptor;
	this->_encryptor = new CryptoPP::AuthenticatedEncryptionFilter(encryptor);
	ASSERT( this->_encryptor != NULL );
	CryptoPP::GCM<CryptoPP::AES>::Decryption decryptor;
	this->_decryptor = new CryptoPP::AuthenticatedDecryptionFilter(decryptor);
	ASSERT( this->_decryptor != NULL );
	this->_encryptionKey = new char[BINFILE_ENCRYPTIONKEYSIZE];
	ASSERT( this->_encryptionKey != NULL );
	this->_encryptionIV = new char[BINFILE_ENCRYPTIONIVSIZE];
	ASSERT( this->_encryptionIV != NULL );
}


const Result_t BinFile::Create(const std::string &filename, CoreMetaProject *coreMetaProject, ICoreStorage* &storage, const bool &encrypted) throw()
{
	if ( filename == "" ) return E_NAMEMISSING;
	if ( coreMetaProject == NULL ) return E_META_NOTOPEN;
	// Make sure filename is valid UTF-8
	if (_CheckUTF8(filename) != S_OK) return E_BADUTF8STRING;
	// Create a BinFile object with the given name and metaProject
	BinFile *binFile = new BinFile(filename, coreMetaProject);
	ASSERT( binFile != NULL );
	// Open the metaProject and get the Uuid
	Result_t result = binFile->_metaProject->GetUuid(binFile->_metaProjectUuid);
	ASSERT( result == S_OK );

	// Try to open the file -- previously ios::nocreate had been used but no file is created if opened for read only
	binFile->_inputFile.open(binFile->_filename.c_str(), std::ios::in | std::ios::out | std::ios::binary | std::ios_base::trunc );
	if( binFile->_inputFile.fail() || !binFile->_inputFile.is_open() ) {
		delete binFile;
		return E_FILEOPEN;
	}
	// Get the scratch file ready for writing and reading
	binFile->_scratchFile.clear();
	std::string tmpFileName, directory;
	_SplitPath(binFile->_filename, directory, tmpFileName);
	std::string scratchFileName = directory + std::string("~") + filename;
	binFile->_scratchFile.open(scratchFileName.c_str(), std::ios_base::in | std::ios_base::out | std::ios::binary | std::ios_base::trunc);
	if( binFile->_scratchFile.fail() || !binFile->_scratchFile.is_open() ) {
		// Close the input file
		binFile->_inputFile.close();
		delete binFile;
		return E_FILEOPEN;
	}

	// Ready on encryption for the binFile (may not be turned on though)
	binFile->_isEncrypted = encrypted;
	// Generate a key
//	CryptoPP::AutoSeededRandomPool randomPool;
//	randomPool.GenerateBlock( (byte*)binFile->_encryptionKey, BINFILE_ENCRYPTIONKEYSIZE );
//	// Generate the iv
//	randomPool.GenerateBlock( (byte*)binFile->_encryptionIV, BINFILE_ENCRYPTIONIVSIZE );  

	// Now just create the actual METAID_ROOT object (using a new Uuid and a nice transaction of course)
	Uuid rootUuid;
	binFile->_isJournaling = false;
	result = binFile->BeginTransaction();
	ASSERT( result == S_OK );
	result = binFile->CreateObject(METAID_ROOT, rootUuid);
	ASSERT( result == S_OK );
	result = binFile->CommitTransaction();
	ASSERT( result == S_OK );
	binFile->_isJournaling = BINFILE_DEFAULTJOURNALING;
	// Make sure to capture the rootUuid
	binFile->_rootUuid = rootUuid;
	// Return the new BinFile
	binFile->_openedObject = binFile->_indexHash.end();
	storage = binFile;
	return S_OK;
}


const Result_t BinFile::Open(const std::string &filename, CoreMetaProject *coreMetaProject, ICoreStorage* &storage, const std::vector<char> &encryptionKey) throw()
{
	if ( filename == "" ) return E_NAMEMISSING;
	if ( coreMetaProject == NULL ) return E_META_NOTOPEN;
	// Make sure filename is valid UTF-8
	if (_CheckUTF8(filename) != S_OK) return E_BADUTF8STRING;
	// Clean up the filename a bit (handle ~ and such)
	std::string tmpName, directory;
	_SplitPath(filename, directory, tmpName);
	tmpName = directory + tmpName;
	// Create a BinFile object with the given name
	BinFile *binFile = new BinFile(tmpName, coreMetaProject);
	ASSERT( binFile != NULL );
	// Open the metaProject and get the Uuid
	Result_t result = binFile->_metaProject->GetUuid(binFile->_metaProjectUuid);
	ASSERT( result == S_OK );

	// Is encryption enabled
	if (encryptionKey.size() != 0)
	{
		// Turn on encryption for the binFile
		binFile->_isEncrypted = true;
		// Copy in the key
		memcpy(binFile->_encryptionKey, &encryptionKey[0], BINFILE_ENCRYPTIONKEYSIZE);
	}

	// Load the project (check for errors)
	result = binFile->Load();
	if (result != S_OK)
	{
		// Failure to open file properly
		delete binFile;
		binFile = NULL;
	}
	// Return the new BinFile
	storage = binFile;
	return result;
}


const Result_t BinFile::Load(void)
{
	ASSERT( !this->_inTransaction );
	ASSERT( this->_filename != "" );
	ASSERT( this->_metaProject != NULL );
	ASSERT( this->_indexHash.empty() );
	ASSERT( this->_cacheQueue.empty() );
	ASSERT( this->_undoList.empty() );
	ASSERT( this->_redoList.empty() );
	// Try to open the file -- previously ios::nocreate had been used but no file is created if opened for read only
	this->_inputFile.open(this->_filename.c_str(), std::ios::in | std::ios::binary);
	if( this->_inputFile.fail() || !this->_inputFile.is_open() || this->_inputFile.bad() ) return E_FILEOPEN;
	// Get the scratch file ready for writing and reading
	this->_scratchFile.clear();
	std::string filename, directory, scratchFileName;
	_SplitPath(this->_filename, directory, filename);
	scratchFileName = directory + std::string("~") + filename;
	this->_scratchFile.open(scratchFileName.c_str(), std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc );
	if( this->_scratchFile.fail() || !this->_scratchFile.is_open() || this->_scratchFile.bad() )
	{
		// Close the input file
		this->_inputFile.close();
		return E_FILEOPEN;
	}

	// Read the preamble (metaProject Uuid, options position and options size)
	Uuid uuid;
	uint32_t preambleSizeB = sizeof(Uuid) + sizeof(uint64_t) + sizeof(uint32_t);
	char* buffer = new char[preambleSizeB];
	char* bufferPointer = buffer;
	this->_inputFile.read(buffer, preambleSizeB);
	_Read(bufferPointer, uuid);
	// Make sure the Uuid matches that of the metaProject
	if( !(uuid == this->_metaProjectUuid) )
	{
		// Close the files
		this->_inputFile.close();
		this->_scratchFile.close();
		return E_PROJECT_MISMATCH;
	}

	// Read the options information
	std::streampos startOfOptions, startOfIndex, startOfJournal;
	uint64_t indexSizeB, journalSizeB;
	uint32_t optionsSizeB;
	_Read(bufferPointer, startOfOptions);
	_Read(bufferPointer, optionsSizeB);
	// Make sure to clean up the buffer
	delete buffer;

	// Get the options
	this->_inputFile.seekg(startOfOptions);
	this->ReadOptions(this->_inputFile, optionsSizeB, startOfIndex, indexSizeB, startOfJournal, journalSizeB);

	// Read the object index
	this->_inputFile.seekg(startOfIndex);
	this->ReadIndex(this->_inputFile, indexSizeB);

	// Read the journal index
	this->_inputFile.seekg(startOfJournal);
	this->ReadJournal(this->_inputFile, journalSizeB);

	// Open went well (make sure to clear the openedObject)
	this->_openedObject = this->_indexHash.end();
	return S_OK;
}


const Result_t BinFile::ReadIndex(std::fstream &stream, const uint64_t &originalIndexSizeB)
{
	ASSERT( stream.is_open() );
	// Size the buffer
	uint64_t indexSizeB = originalIndexSizeB;
	char* buffer = new char[(size_t)indexSizeB];
	// Read the index from the file itself
	stream.read(buffer, indexSizeB);
	// Is there encryption
	if (this->_isEncrypted) _Decrypt(this->_decryptor, buffer, this->_encryptionKey, this->_encryptionIV, indexSizeB);
	// Is there compression
	if (this->_isCompressed) indexSizeB = _Decompress(this->_compressor, buffer, indexSizeB );
	// How many objects are there
	uint64_t objCount = indexSizeB / (sizeof(Uuid) + sizeof(uint64_t) + sizeof(uint32_t));
	// Read in each item for the index
	char *bufferPointer = buffer;
	Uuid uuid;
	IndexEntry entry = { NULL, EntryLocation::Input(), 0, 0, this->_isCompressed, this->_isEncrypted };
	for (uint64_t i=0; i < objCount; ++i)
	{
		// Copy the Uuid into the variable
		_Read(bufferPointer, uuid);
		// Copy the position into a temp value and then into the entry
		_Read(bufferPointer, entry.position);
		// Copy the size into the entry
		_Read(bufferPointer, entry.sizeB);
		// Place these into the indexHash
		this->_indexHash.insert( std::make_pair(uuid, entry) );
	}
	// All is good
	delete buffer;
	return S_OK;
}


const Result_t BinFile::WriteIndex(std::fstream &stream, const IndexHash &index, uint64_t &indexSizeB) const
{
	ASSERT( stream.is_open() );
	// Create a correctly sized output buffer
	indexSizeB = index.size() * (sizeof(Uuid) + sizeof(uint64_t) + sizeof(uint32_t));
	char* buffer = new char[(size_t)indexSizeB];
	// Write each item from the index into the buffer
	char *bufferPointer = buffer;
	IndexHash::const_iterator hashIter = index.begin();
	while( hashIter != this->_indexHash.end() )
	{
		// Copy in the Uuid
		_Write(bufferPointer, hashIter->first);
		// Copy in the File Position
		_Write(bufferPointer, hashIter->second.position);
		// Copy in the Object Size
		_Write(bufferPointer, hashIter->second.sizeB);
		// Move on to the next hash element
		++hashIter;
	}
	// Is there compression
	if (this->_isCompressed) indexSizeB = _Compress(this->_compressor, buffer, indexSizeB );
	// Is there encryption
	if (this->_isEncrypted) _Encrypt(this->_encryptor, this->_encryptionKey, this->_encryptionIV, buffer, indexSizeB);
	// Now write out the data to the file
	stream.write(buffer, indexSizeB);
	ASSERT( !stream.bad() );
	delete buffer;
	return S_OK;
}


const Result_t BinFile::ReadJournal(std::fstream &stream, const uint64_t &originalJournalSizeB)
{
	ASSERT( stream.is_open() );
	// Check to see if we have an empty journal
	if (originalJournalSizeB == 0) return S_OK;
	// Size the buffer
	uint64_t journalSizeB = originalJournalSizeB;
	char* buffer = new char[(size_t)journalSizeB];
	// Read the index from the file itself
	stream.read(buffer, journalSizeB);
	// Is there encryption
	if (this->_isEncrypted) _Decrypt(this->_decryptor, buffer, this->_encryptionKey, this->_encryptionIV, journalSizeB);
	// Is there compression
	if (this->_isCompressed) journalSizeB = _Decompress(this->_compressor, buffer, journalSizeB );
	// How many journal entries are there
	uint64_t entryCount = journalSizeB / (sizeof(uint64_t) + sizeof(uint32_t) + sizeof(Uuid) + 3 * sizeof(uint32_t) + 2 * sizeof(uint8_t));
	// Read in each item for the index
	char *bufferPointer = buffer;
	JournalEntry entry = { NULL, EntryLocation::Input(), 0, 0, Uuid::Null(), 0, 0, 0, false, false, false };
	for (uint64_t i=0; i < entryCount; ++i)
	{
		// Copy the position into a temp value and then into the entry
		_Read(bufferPointer, entry.position);
		// Copy the size into the entry
		_Read(bufferPointer, entry.sizeB);
		// Copy the Uuid of the tag into the variable
		_Read(bufferPointer, entry.tag);
		// Copy in the numCreated, numChanged, numDeleted
		_Read(bufferPointer, entry.numCreated);
		_Read(bufferPointer, entry.numChanged);
		_Read(bufferPointer, entry.numDeleted);
		// Copy in the compression and encryption status
		uint8_t flag;
		_Read(bufferPointer, flag);
		entry.isCompressed = (flag != false);
		_Read(bufferPointer, flag);
		entry.isEncrypted = (flag != false);
		// Place these into the undoList
		this->_undoList.push_back(entry);
	}
	// All is good
	delete buffer;
	return S_OK;
}


const Result_t BinFile::WriteJournal(std::fstream &stream, uint64_t &journalSizeB) const
{
	ASSERT( stream.is_open() );
	// Check to see if we have an empty journal
	if (this->_undoList.size() == 0)
	{
		journalSizeB = 0;
		return S_OK;
	}
	// Create a correctly sized output buffer (position, size, tag, numCreated, numChanged, numDeleted, isCompressed, isEncrypted)
	journalSizeB = this->_undoList.size() * (sizeof(uint64_t) + sizeof(uint32_t) + sizeof(Uuid) + 3 * sizeof(uint32_t) + 2 * sizeof(uint8_t));
	char* buffer = new char[(size_t)journalSizeB];
	// Write each item from the index into the buffer
	char *bufferPointer = buffer;
	JournalList::const_iterator journalIter = this->_undoList.begin();
	while( journalIter != this->_undoList.end() )
	{
		// Copy in the File Position
		_Write(bufferPointer, journalIter->position);
		// Copy in the Journal Entry Size
		_Write(bufferPointer, journalIter->sizeB);
		// Copy in the tag Uuid
		_Write(bufferPointer, journalIter->tag);
		// Copy in the numCreated, numChanged, numDeleted
		_Write(bufferPointer, journalIter->numCreated);
		_Write(bufferPointer, journalIter->numChanged);
		_Write(bufferPointer, journalIter->numDeleted);
		// Copy in the compression and encryption status
		_Write<uint8_t>(bufferPointer, journalIter->isCompressed);
		_Write<uint8_t>(bufferPointer, journalIter->isEncrypted);
		// Move on to the next journal entry
		++journalIter;
	}
	// Is there compression
	if (this->_isCompressed) journalSizeB = _Compress(this->_compressor, buffer, journalSizeB );
	// Is there encryption
	if (this->_isEncrypted) _Encrypt(this->_encryptor, this->_encryptionKey, this->_encryptionIV, buffer, journalSizeB);
	// Now write out the data to the file
	stream.write(buffer, journalSizeB);
	ASSERT( !stream.bad() );
	delete buffer;
	return S_OK;
}

const Result_t BinFile::ReadOptions(std::fstream &stream, const uint32_t &sizeB, std::streampos &startOfIndex, uint64_t &indexSizeB,
									std::streampos &startOfJournal, uint64_t &journalSizeB)
{
	ASSERT( stream.is_open() );
	// Create a correctly sized input buffer
	char* buffer = new char[sizeB];
	char* bufferPointer = buffer;
	// Read data from the stream
	stream.read(buffer, sizeB);

	// Read the index info
	uint8_t boolVal;
	_Read<uint8_t>(bufferPointer, boolVal);
	this->_isCaching = (boolVal != false);
	_Read<uint64_t>(bufferPointer, this->_maxCacheSize);
	_Read<Uuid>(bufferPointer, this->_rootUuid);
	_Read<std::streampos>(bufferPointer, startOfIndex);
	_Read<uint64_t>(bufferPointer, indexSizeB);

	// Read the journal info
	_Read<uint8_t>(bufferPointer, boolVal);
	this->_isJournaling = (boolVal != false);
	_Read<uint64_t>(bufferPointer, this->_maxUndoSize);
	_Read<std::streampos>(bufferPointer, startOfJournal);
	_Read<uint64_t>(bufferPointer, journalSizeB);

	// Read the compression info
	_Read<uint8_t>(bufferPointer, boolVal);
	this->_isCompressed = (boolVal != false);

	// Parse the options
	_Read<uint8_t>(bufferPointer, boolVal);
	this->_isEncrypted = (boolVal != false);
	memcpy(this->_encryptionIV, bufferPointer, BINFILE_ENCRYPTIONIVSIZE);
	bufferPointer += BINFILE_ENCRYPTIONIVSIZE;

	// Clean up and be done
	delete buffer;
	return S_OK;
}


const uint32_t BinFile::WriteOptions(std::fstream &stream, const std::streampos &startOfIndex, const uint64_t &indexSizeB,
									 const std::streampos &startOfJournal, const uint64_t &journalSizeB) const
{
	ASSERT( stream.is_open() );
	// Create a correctly sized output buffer
	uint32_t sizeB = 0;
	sizeB += sizeof(uint8_t) + 3 * sizeof(uint64_t) + sizeof(Uuid);			//!< Index info
	sizeB += sizeof(uint8_t) + 3 *sizeof(uint64_t);							//!< Journal info
	sizeB += sizeof(uint8_t);												//!< Compression info
	sizeB += sizeof(uint8_t) + BINFILE_ENCRYPTIONIVSIZE;					//!< Encryption info
	// Allocate a buffer to write into
	char* buffer = new char[sizeB];
	char* bufferPointer = buffer;

	// Write index info
	_Write<uint8_t>(bufferPointer, this->_isCaching);
	_Write<uint64_t>(bufferPointer, this->_maxCacheSize);
	_Write<Uuid>(bufferPointer, this->_rootUuid);
	_Write<std::streampos>(bufferPointer, startOfIndex);
	_Write<uint64_t>(bufferPointer, indexSizeB);

	// Write journal info
	_Write<uint8_t>(bufferPointer, this->_isJournaling);
	_Write<uint64_t>(bufferPointer, this->_maxUndoSize);
	_Write<std::streampos>(bufferPointer, startOfJournal);
	_Write<uint64_t>(bufferPointer, journalSizeB);

	// Write compression info
	_Write<uint8_t>(bufferPointer, this->_isCompressed);

	// Write encryption info
	_Write<uint8_t>(bufferPointer, this->_isEncrypted);
	memcpy(bufferPointer, this->_encryptionIV, BINFILE_ENCRYPTIONIVSIZE);
	bufferPointer += BINFILE_ENCRYPTIONIVSIZE;

	// Write it out to the stream
	stream.write(buffer, sizeB);
	ASSERT( !stream.bad() );
	// Clean up and be done
	delete buffer;
	return sizeB;
}


IndexHashIterator BinFile::FetchObject(const Uuid &uuid)
{
	// Is the object in index
	IndexHashIterator indexIter = this->_indexHash.find(uuid);
	// Did we not find it in the index?
	if( indexIter == this->_indexHash.end() ) return indexIter;
	//Is the object in the cache
	if (indexIter->second.location == EntryLocation::Cache())
	{
		// If caching
		if (this->_isCaching)
		{
			// Remove the object from the cacheQueue
			this->_cacheQueue.remove(uuid);
			// Move the object to the top of the cacheQueue
			this->_cacheQueue.push_front(uuid);
		}
	}
	// Next, try the inputFile
	else if (indexIter->second.location == EntryLocation::Input())
	{
		// Move the object from the scratch file to the cache
		this->ObjectFromFile(this->_inputFile, indexIter->second, indexIter->first);
	}
	// Object must be in the scratch file
	else if (indexIter->second.location == EntryLocation::Scratch())
	{
		// Move the object from the scratch file to the cache
		this->ObjectFromFile(this->_scratchFile, indexIter->second, indexIter->first);
		// Since the scratchFile is only for dirty objects, must mark object as dirty
		indexIter->second.object->MarkDirty();
	}
	else ASSERT(false);
	// Return the indexIter
	return indexIter;
}


void BinFile::ObjectFromFile(std::fstream &stream, IndexEntry &indexEntry, const Uuid &uuid)
{
	ASSERT( stream.is_open() );
	// Move the read pointer to the appropraite place
	stream.seekg(indexEntry.position);
	// Try to read the object
	char* buffer = new char[indexEntry.sizeB];
	stream.read(buffer, indexEntry.sizeB);
	ASSERT( !stream.bad() );
	// Is there encryption
	if (indexEntry.isEncrypted) _Decrypt(this->_decryptor, this->_encryptionKey, this->_encryptionIV, buffer, indexEntry.sizeB);
	// Is there compression
	if (indexEntry.isCompressed) _Decompress(this->_decompressor, buffer, indexEntry.sizeB);
	uint32_t tmpSize;
	indexEntry.object = BinObject::Read(this->_metaProject, buffer, uuid, tmpSize);
	ASSERT( indexEntry.object != NULL );
	delete buffer;
	// Set the object and mark it as in cache
	indexEntry.location = EntryLocation::Cache();
	// Are we caching (using the scratch file?)
	if (this->_isCaching)
	{
		// Move the object to the cache and to the front of the cacheQueue
		this->_cacheQueue.push_front(uuid);
		// Make sure there is space in the cache
		this->CheckCacheSize();
	}
}


void BinFile::ObjectToFile(std::fstream &stream, IndexEntry &indexEntry)
{
	ASSERT( stream.is_open() );
	// Get the position of where the write it going to happen
	indexEntry.position = stream.tellp();
	// Get the size of the binObject
	indexEntry.sizeB = indexEntry.object->Size();
	// Now, write to the end of scratchFile
	char* buffer = new char[indexEntry.sizeB];
	indexEntry.object->Write(buffer);
	// Is there compression
 	if (indexEntry.isCompressed) indexEntry.sizeB = (uint32_t)_Compress(this->_compressor, buffer, indexEntry.sizeB);
	// Is there encryption
	if (indexEntry.isEncrypted) _Encrypt(this->_encryptor, this->_encryptionKey, this->_encryptionIV, buffer, indexEntry.sizeB);
	// Write the final data into the stream (make sure to seek first - windows issue)
	stream.seekp(indexEntry.position);
	stream.write(buffer, indexEntry.sizeB);
	ASSERT( !stream.bad() );
	delete buffer;
}


void BinFile::CheckCacheSize(void)
{
	// If cache size is too large...
	while (this->_cacheQueue.size() > this->_maxCacheSize)
	{
		// Get the object info from the back of the cache and index
		Uuid uuid  = this->_cacheQueue.back();
		ASSERT( uuid != Uuid::Null() );
		IndexHashIterator hashIter = this->_indexHash.find(uuid);
		ASSERT( hashIter != this->_indexHash.end() );
		// If the object is not dirty
		if (!hashIter->second.object->IsDirty())
		{
			// Place the object back into inputFile (no need to write anything - this is why ObjectFrom file does alter sizeB)
			hashIter->second.location = EntryLocation::Input();
		}
		// If the object is dirty and is located in the cache, write it to the scratch file
		else if (hashIter->second.location == EntryLocation::Cache())
		{
			// Set the IndexEntry flags correctly
			hashIter->second.isCompressed = this->_isCompressed;
			hashIter->second.isEncrypted = this->_isEncrypted;
			hashIter->second.location = EntryLocation::Scratch();
			// Write to file
			this->ObjectToFile(this->_scratchFile, hashIter->second);
		}
		// Delete the object now (not needed any longer)
		ASSERT( hashIter->second.object != NULL );
		delete hashIter->second.object;
		hashIter->second.object = NULL;
		// Finally, remove that object from the cacheQueue
		this->_cacheQueue.pop_back();
	}
}


void BinFile::FlushCache(void)
{
	// Loop through each index item
	std::list<Uuid>::iterator cacheIter = this->_cacheQueue.begin();
	while (cacheIter != this->_cacheQueue.end())
	{
		// Find the index for this object
		IndexHashIterator indexIter = this->_indexHash.find(*cacheIter);
		// Delete the object if it exists
		ASSERT( indexIter->second.location == EntryLocation::Cache() || indexIter->second.location == EntryLocation::Output() );
		ASSERT( indexIter->second.object != NULL );
		delete indexIter->second.object;
		indexIter->second.object = NULL;
		// Move on to the next cacheEntry
		++cacheIter;
	}
	// Make sure to clear the cache
	this->_cacheQueue.clear();
}


void BinFile::FlushUndoRedo(const uint32_t &undoCount)
{
	// Delete any allocated memory
	uint32_t i = 0;
	JournalList::iterator undoIter;
	while (i < undoCount && !this->_undoList.empty())
	{
		// Get the front of the undo list
		undoIter = this->_undoList.begin();
		// Delete any allocated memory
		if (undoIter->buffer != NULL) delete undoIter->buffer;
		// Remove the entry from the list
		this->_undoList.pop_front();
		// Move on
		++i;
	}

	// Always just clear the redo list
	JournalList::iterator redoIter = this->_redoList.begin();
	while (redoIter != this->_redoList.end())
	{
		// Delete any allocated memory
		if (redoIter->buffer != NULL) delete redoIter->buffer;
		// Move on to next redo entry
		++redoIter;
	}
	this->_redoList.clear();
}


const Result_t BinFile::PickleTransaction(const Uuid &tag)
{
	// Create the journal entry
	JournalEntry entry = { NULL, EntryLocation::Cache(), 0, 0, tag,
		this->_createdObjects.size(), this->_changedAttributes.size(), this->_deletedObjects.size(),
		this->_isCompressed, this->_isEncrypted, this->_isDirty };
	// Get the size of the total transaction
	std::list<std::pair<Uuid,MetaID_t> >::iterator createdIter = this->_createdObjects.begin();
	while( createdIter != this->_createdObjects.end() )
	{
		// Created are just a MetaID and the resulting Uuid
		entry.sizeB += sizeof(MetaID_t) + sizeof(Uuid);
		++createdIter;
	}
	ChangedAttributesHashIterator changeIter = this->_changedAttributes.begin();
	while (changeIter != this->_changedAttributes.end())
	{
		// Add the sizes of the Uuid, AttrID_t and ValueType
		uint32_t sizeB = sizeof(Uuid) + sizeof(AttrID_t) + sizeof(uint8_t);
		// Add the size of the attribute old and new values (size depends on type unfortunately)
		ASSERT( changeIter->second != NULL );
		if (changeIter->second->type == ValueType::Long())				sizeB += (2 * sizeof(int32_t));
		else if (changeIter->second->type == ValueType::Real())			sizeB += (2 * sizeof(double));
		else if (changeIter->second->type == ValueType::LongPointer())	sizeB += (2 * sizeof(Uuid));
		else if (changeIter->second->type == ValueType::Pointer())		sizeB += (2 * sizeof(Uuid));
		else if (changeIter->second->type == ValueType::String())
		{
			AttributeChange<std::string>* changeRecord = (AttributeChange<std::string>*)changeIter->second;
			sizeB += _Size(changeRecord->oldValue);
			sizeB += _Size(changeRecord->newValue);
		}
		else if (changeIter->second->type == ValueType::Dictionary())
		{
			AttributeChange<DictionaryMap>* changeRecord = (AttributeChange<DictionaryMap>*)changeIter->second;
			sizeB += _Size(changeRecord->oldValue);
			sizeB += _Size(changeRecord->newValue);			
		}
		else ASSERT(false);
		// Add the size to the size list
		entry.sizeB += sizeB;
		// Move on to the next changed attribute
		++changeIter;
	}
	std::list< std::pair<Uuid,IndexEntry> >::iterator deletedIter = this->_deletedObjects.begin();
	while (deletedIter != this->_deletedObjects.end())
	{
		// Add the size to the pickle total
		entry.sizeB += deletedIter->second.object->Size() + sizeof(Uuid);
		// Move on to the next deleted object
		++deletedIter;
	}
	// Setup the entry buffer
	entry.buffer = new char[entry.sizeB];
	char* bufferPointer = entry.buffer;

	// Serialize the three transaction lists, start with created objects
	createdIter = this->_createdObjects.begin();
	while( createdIter != this->_createdObjects.end() )
	{
		// Write the MetaID and Uuid
		_Write(bufferPointer, createdIter->second);
		_Write(bufferPointer, createdIter->first);
		++createdIter;
	}
	// Then on to changed attributes
	changeIter = this->_changedAttributes.begin();
	while (changeIter != this->_changedAttributes.end())
	{
		// Write out the change record (Uuid, attrID, attribute type)
		_Write(bufferPointer, changeIter->first.uuid);
		_Write(bufferPointer, changeIter->first.attrID);
		_Write(bufferPointer, changeIter->second->type);
		// Write out the changed attribute record info
		if (changeIter->second->type == ValueType::Long())
		{
			AttributeChange<int32_t>* changeRecord = (AttributeChange<int32_t>*)changeIter->second;
			_Write(bufferPointer, changeRecord->oldValue);
			_Write(bufferPointer, changeRecord->newValue);
		}
		else if (changeIter->second->type == ValueType::Real())
		{
			AttributeChange<double>* changeRecord = (AttributeChange<double>*)changeIter->second;
			_Write(bufferPointer, changeRecord->oldValue);
			_Write(bufferPointer, changeRecord->newValue);
		}
		else if (changeIter->second->type == ValueType::String())
		{
			AttributeChange<std::string>* changeRecord = (AttributeChange<std::string>*)changeIter->second;
			_Write(bufferPointer, changeRecord->oldValue);
			_Write(bufferPointer, changeRecord->newValue);
		}
		else if (changeIter->second->type == ValueType::Pointer() || changeIter->second->type == ValueType::LongPointer())
		{
			AttributeChange<Uuid>* changeRecord = (AttributeChange<Uuid>*)changeIter->second;
			_Write(bufferPointer, changeRecord->oldValue);
			_Write(bufferPointer, changeRecord->newValue);
		}
		else if (changeIter->second->type == ValueType::Dictionary())
		{
			AttributeChange<DictionaryMap>* changeRecord = (AttributeChange<DictionaryMap>*)changeIter->second;
			_Write(bufferPointer, changeRecord->oldValue);
			_Write(bufferPointer, changeRecord->newValue);
		}
		else ASSERT(false);
		// Move to the next changed attribute
		++changeIter;
	}
	// Finally, write out the deleted objects
	deletedIter = this->_deletedObjects.begin();
	while (deletedIter != this->_deletedObjects.end())
	{
		// Write out the deleted object's Uuid
		_Write(bufferPointer, deletedIter->first);
		// Write out the deleted object itself
		bufferPointer += deletedIter->second.object->Write(bufferPointer);
		// Move to the next delete object
		++deletedIter;
	}
	// Make sure we have written as much as we are supposed to
	ASSERT( bufferPointer-entry.buffer == entry.sizeB );
	// Is there compression (size must be above a certain bounds)
	if (entry.isCompressed && entry.sizeB > BINFILE_JOURNALCOMPRESSIONLIMIT)
		entry.sizeB = (uint32_t)_Compress(this->_compressor, entry.buffer, entry.sizeB);
	else entry.isCompressed = false;
	// Is there encryption
	if (entry.isEncrypted) _Encrypt(this->_encryptor, entry.buffer, this->_encryptionKey, this->_encryptionIV, entry.sizeB);
	// All is good, add the entry to the back of the undo list
	this->_undoList.push_back(entry);
	return S_OK;
}


const Result_t BinFile::UnpickleTransaction(JournalEntry &entry)
{
	ASSERT( this->_createdObjects.empty() );
	ASSERT( this->_changedAttributes.empty() );
	ASSERT( this->_deletedObjects.empty() );
	// Make sure entry is in the cache
	if (entry.location != EntryLocation::Cache())
	{
		// If in input or scratch file, copy the journal entry buffer into memory
		entry.buffer = new char[entry.sizeB];
		if (entry.location == EntryLocation::Input())
		{
			this->_inputFile.seekg(entry.position);
			this->_inputFile.read(entry.buffer, entry.sizeB);
		}
		else if (entry.location == EntryLocation::Scratch())
		{
			this->_scratchFile.seekg(entry.position);
			this->_scratchFile.read(entry.buffer, entry.sizeB);
		}
		else ASSERT(false);
		entry.location = EntryLocation::Cache();
	}
	// Is there encryption
	if (entry.isEncrypted)
	{
		_Decrypt(this->_decryptor, entry.buffer, this->_encryptionKey, this->_encryptionIV, entry.sizeB);
		entry.isEncrypted = false;
	}
	// Is there compression
	if (entry.isCompressed)
	{
		entry.sizeB = (uint32_t)_Decompress(this->_decompressor, entry.buffer, entry.sizeB);
		entry.isCompressed = false;
	}
	// Deserialize the three transaction lists (created, changed, deleted) to memory
	char* bufferPointer = entry.buffer;
	ASSERT( bufferPointer != NULL );
	for (uint32_t numCreated=0; numCreated < entry.numCreated; numCreated++)
	{
		MetaID_t metaID;
		Uuid uuid;
		// Read a MetaID and the resulting Uuid
		_Read(bufferPointer, metaID);
		_Read(bufferPointer, uuid);
		// Put them into the createdObjects list
		this->_createdObjects.push_back(std::make_pair(uuid, metaID));
	}
	// Then on to changed attributes
	for (uint32_t numChanged=0; numChanged < entry.numChanged; numChanged++)
	{
		AttributeID attributeID;
		ValueType valueType;
		AttributeChangeBase* changeBase = NULL;
		// Read in the change record (Uuid, attrID, attribute type)
		_Read(bufferPointer, attributeID.uuid);
		_Read(bufferPointer, attributeID.attrID);
		_Read(bufferPointer, valueType);
		// Read in the changed attribute record info
		if (valueType == ValueType::Long())
		{
			AttributeChange<int32_t>* changeRecord = new AttributeChange<int32_t>();
			_Read(bufferPointer, changeRecord->oldValue);
			_Read(bufferPointer, changeRecord->newValue);
			changeBase = changeRecord;
		}
		else if (valueType == ValueType::Real())
		{
			AttributeChange<double>* changeRecord = new AttributeChange<double>();
			_Read(bufferPointer, changeRecord->oldValue);
			_Read(bufferPointer, changeRecord->newValue);
			changeBase = changeRecord;
		}
		else if (valueType == ValueType::String())
		{
			AttributeChange<std::string>* changeRecord = new AttributeChange<std::string>();
			_Read(bufferPointer, changeRecord->oldValue);
			_Read(bufferPointer, changeRecord->newValue);
			changeBase = changeRecord;
		}
		else if (valueType == ValueType::Pointer() || valueType == ValueType::LongPointer())
		{
			AttributeChange<Uuid>* changeRecord = new AttributeChange<Uuid>();
			_Read(bufferPointer, changeRecord->oldValue);
			_Read(bufferPointer, changeRecord->newValue);
			changeBase = changeRecord;
		}
		else if (valueType == ValueType::Dictionary())
		{
			AttributeChange<DictionaryMap>* changeRecord = new AttributeChange<DictionaryMap>();
			_Read(bufferPointer, changeRecord->oldValue);
			_Read(bufferPointer, changeRecord->newValue);
			changeBase = changeRecord;
		}
		else ASSERT(false);
		// Put the changeRecord into the changedObjects list
		changeBase->type = valueType;
		this->_changedAttributes.insert(std::make_pair(attributeID,changeBase));
	}
	// Finally, the deleted objects
	Uuid uuid;
	uint32_t sizeB;
	for (uint32_t numDeleted=0; numDeleted < entry.numDeleted; numDeleted++)
	{
		// Read in the deleted object's Uuid
		_Read(bufferPointer, uuid);
		// Read in the deleted object itself (and mark as dirty)
		BinObject* object = BinObject::Read(this->_metaProject, bufferPointer, uuid, sizeB);
		ASSERT( object != NULL );
		object->MarkDirty();
		// Advance the read pointer
		bufferPointer += sizeB;
		// Create the entry and insert into deletedObject
		IndexEntry deletedEntry = { object, EntryLocation::Cache(), 0, sizeB, this->_isCompressed, this->_isEncrypted };
		this->_deletedObjects.push_back( std::make_pair(uuid, deletedEntry) );
	}
	return S_OK;
}


const Result_t BinFile::PointerUpdate(const AttrID_t &attrID, const Uuid &uuid, const Uuid &oldValue, const Uuid &newValue)
{
	// Make sure value is valid (either points to a valid object or NULL - and any backpointer is valid)
	BinAttributeCollection *newCollection = NULL;
	if (newValue != Uuid::Null())
	{
		// Can we fetch this object - if not it is an error
		IndexHashIterator newObjecterIterator = this->FetchObject(newValue);
		if (newObjecterIterator == this->_indexHash.end()) return E_NOTFOUND;
		// Does the object have a correct backpointer collection
		BinObject* binObject = newObjecterIterator->second.object;
		ASSERT( binObject != NULL );
		newCollection = (BinAttributeCollection*)binObject->GetAttribute(attrID + ATTRID_COLLECTION);
		// Make sure the backpointer collection exists
		if (newCollection == NULL) return E_VALTYPE;
	}
	// Update the current pointed-to object's backpointer collection (if idPair is valid)
	if (oldValue != Uuid::Null())
	{
		BinObject *binObject = this->FetchObject(oldValue)->second.object;
		ASSERT( binObject != NULL );
		BinAttributeCollection *collection = (BinAttributeCollection*)binObject->GetAttribute(attrID + ATTRID_COLLECTION);
		ASSERT( collection != NULL );	//	<-- Means pointed-to-object doesn't have backpointer collection for this type of pointer
		collection->Remove(uuid);
	}
	// Update the pointed-to object's backpointer collection (if it points to something valid)
	if (newCollection != NULL) newCollection->Add(uuid);
	return S_OK;
}


template <class BAT, class VT>
static inline void _AttributeChange(BinAttribute *binAttribute, AttributeChangeBase* changeBase, const bool &useOld)
{
	BAT *attribute = (BAT*)binAttribute;
	ASSERT( attribute != NULL );
	AttributeChange<VT>* changeRecord = (AttributeChange<VT>*)(changeBase);
	ASSERT( changeRecord != NULL );
	if(useOld)	attribute->Set(changeRecord->oldValue);
	else		attribute->Set(changeRecord->newValue);
	delete changeRecord;
}


void BinFile::TryAbortTransaction(void)
{
	// Is this transaction actually going to do anything - if not, we are done
	if ( !this->_createdObjects.empty() || !this->_deletedObjects.empty() || !this->_changedAttributes.empty() )
	{
		// Must move all deletedObjects back to cache
		std::list< std::pair<Uuid,IndexEntry> >::iterator deletedIter = this->_deletedObjects.begin();
		while (deletedIter != this->_deletedObjects.end())
		{
			// Insert into the cache and queue
			std::pair<IndexHashIterator,bool> insertReturn = this->_indexHash.insert( *deletedIter );
			ASSERT( insertReturn.second );
			if (this->_isCaching) this->_cacheQueue.push_front(deletedIter->first);
			// Move to the next deleted object in the list
			++deletedIter;
		}
		// Clear the deleted objects list
		this->_deletedObjects.clear();
		
		// Changed attributes - rollback changes
		ChangedAttributesHashIterator changeIter = this->_changedAttributes.begin();
		while (changeIter != this->_changedAttributes.end())
		{
			// Get the attribute change record
			IndexHashIterator binIter = this->FetchObject(changeIter->first.uuid);
			ASSERT( binIter != this->_indexHash.end() );
			BinObject* binObject = binIter->second.object;
			ASSERT( binObject != NULL );
			BinAttribute* binAttribute = binObject->GetAttribute(changeIter->first.attrID);
			ASSERT( binAttribute != NULL );
			// Is the changed attribute a LONG
			if (binAttribute->GetValueType() == ValueType::Long())
			{
				// Change attribute value back to old value
				_AttributeChange<BinAttributeLong,int32_t>(binAttribute, changeIter->second, true);
			}
			// Is the changed attribute a REAL
			else if (binAttribute->GetValueType() == ValueType::Real())
			{
				// Change attribute value back to old value
				_AttributeChange<BinAttributeReal,double>(binAttribute, changeIter->second, true);
			}
			// Is the changed attribute a STRING
			else if (binAttribute->GetValueType() == ValueType::String())
			{
				// Change attribute value back to old value
				_AttributeChange<BinAttributeString,std::string>(binAttribute, changeIter->second, true);
			}
			// Is the changed attribute a LONGPOINTER
			else if (binAttribute->GetValueType() == ValueType::LongPointer())
			{
				// Change attribute value back to old value
				_AttributeChange<BinAttributeLongPointer,Uuid>(binAttribute, changeIter->second, true);
			}
			// Is the changed attribute a POINTER
			else if (binAttribute->GetValueType() == ValueType::Pointer())
			{
				BinAttributePointer *attribute = (BinAttributePointer*)binAttribute;
				ASSERT( attribute != NULL );
				AttributeChange<Uuid>* changeRecord = (AttributeChange<Uuid>*)(changeIter->second);
				ASSERT( changeRecord != NULL );
				this->PointerUpdate(binAttribute->GetAttributeID(), changeIter->first.uuid, attribute->Get(), changeRecord->oldValue);
				attribute->Set(changeRecord->oldValue);
				delete changeRecord;
			}
			// Is the changed attribute a DICTIONARY
			else if (binAttribute->GetValueType() == ValueType::Dictionary())
			{
				// Change attribute value back to old value
				_AttributeChange<BinAttributeDictionary,DictionaryMap>(binAttribute, changeIter->second, true);
			}
			// Never should be here!
			else ASSERT(false);
			// Move on to the next change
			++changeIter;
		}
		// Clear the changes list
		this->_changedAttributes.clear();
		
		// Delete all Created objects
		std::list<std::pair<Uuid,MetaID_t> >::iterator createdIter = this->_createdObjects.begin();
		while( createdIter != this->_createdObjects.end() )
		{
			// Fetch the created object
			IndexHashIterator indexIter = this->FetchObject(createdIter->first);
			ASSERT( indexIter != this->_indexHash.end() );
			// Delete the object itself
			BinObject* binObject = indexIter->second.object;
			ASSERT( binObject != NULL );
			delete binObject;
			// Remove object info from cacheHash and cacheQueue
			this->_indexHash.erase(indexIter);
			if (this->_isCaching) this->_cacheQueue.remove(createdIter->first);
			// Move on to the next created object
			++createdIter;
		}
		// Clear the created objects list
		this->_createdObjects.clear();
	}
}


IndexHashIterator BinFile::TryCreateObject(const MetaID_t &metaID, const Uuid &uuid)
{
	// Lookup the metaID in the metaProject to make sure it is valid
	CoreMetaObject* metaObject = NULL;
	Result_t result = this->_metaProject->GetObject(metaID, metaObject);
	ASSERT ( result == S_OK && metaObject != NULL );
	// Create the binObject and use the passed in Uuid
	BinObject* binObject = BinObject::Create(metaObject, uuid);
	ASSERT( binObject != NULL );
	// Put the object into the index and queue
	IndexEntry indexEntry = { binObject, EntryLocation::Cache(), 0, 0, this->_isCompressed, this->_isEncrypted };
	std::pair<IndexHashIterator,bool> insertReturn = this->_indexHash.insert( std::make_pair(uuid, indexEntry) );
	ASSERT( insertReturn.second );
	if (this->_isCaching) this->_cacheQueue.push_front(uuid);
	// Return the hashIterator
	return insertReturn.first;
}


// --------------------------- Public BinFile Methods ---------------------------


BinFile::~BinFile()
{
	// If in a transaction, abort
	if( this->_inTransaction ) this->AbortTransaction();
	// Clean up any open files
	if( this->_inputFile.is_open() ) this->_inputFile.close();
	if( this->_scratchFile.is_open() )
	{
		// First, close the scratch file
		this->_scratchFile.close();
		// Make sure we get the correct scratchfile name
		std::string filename, directory, scratchFileName;
		_SplitPath(this->_filename, directory, filename);
		scratchFileName = directory + std::string("~") + filename;
		// Remove the scratchfile
		int retVal = remove(scratchFileName.c_str());
		ASSERT( retVal == 0 );
	}
	// Clean up compression and encryption
	delete this->_compressor;
	delete this->_decompressor;
	delete this->_encryptor;
	delete this->_decryptor;
	delete this->_encryptionKey;
	delete this->_encryptionIV;
	// Clear the cache (and its objects)
	this->FlushCache();
	// Clear the journal (and its objects)
	this->FlushUndoRedo(this->_undoList.size());
}


const Result_t BinFile::MetaObject(CoreMetaObject* &coreMetaObject) const throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	// If no currently opened object, return NULL
	if (this->_indexHash.end() == this->_openedObject) return E_INVALID_USAGE;
	// Get the metaObject from the associated metaProject
	BinObject* binObject = this->_openedObject->second.object;
	ASSERT( binObject != NULL );
	coreMetaObject = binObject->GetMetaObject();
	ASSERT( coreMetaObject != NULL );
	return S_OK;
}


const Result_t BinFile::MetaID(MetaID_t &metaID) const throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	// If no currently opened object, return METAID_NONE
	if (this->_indexHash.end() == this->_openedObject) return METAID_NONE;
	metaID = this->_openedObject->second.object->GetMetaID();
	ASSERT( metaID != METAID_NONE );
	return S_OK;
}


const Result_t BinFile::ObjectVector(std::vector<Uuid> &objectVector) const throw()
{
	// Clear the incoming vector and size it
	objectVector.clear();
	objectVector.resize(this->_indexHash.size());
	// Load it with the indexHash
	size_t objCount = 0;
	IndexHash::const_iterator indexIter = this->_indexHash.begin();
	while(indexIter != this->_indexHash.end())
	{
		objectVector[objCount++] = indexIter->first;
		++indexIter;
	}
	// All is good
	return S_OK;
}


const Result_t BinFile::Save(const std::string &filename, const bool &forceOverwrite) throw()
{
	// Make sure we have a valid project file open
	ASSERT( this->_metaProject != NULL );
	ASSERT( this->_inputFile.is_open() );
	ASSERT( this->_scratchFile.is_open() );
	// Make sure filename is valid UTF-8
	if (_CheckUTF8(filename) != S_OK) return E_BADUTF8STRING;
	// Passing an empty filename implies saving the file with the current filename
	std::string saveAs, directory;
	if (filename == "") saveAs = this->_filename;
	else saveAs = filename;
	// Are we overwriting the origial file?
	bool overwrite = (this->_filename == saveAs);
	// Does this file already exist
	if (!overwrite)
	{
		// Try opening the file
		std::ifstream testFile(saveAs.c_str());
		if (testFile)
		{
			// Close the test file
			testFile.close();
			// File exists, but we are not allowed to overwrite
			if (!forceOverwrite) return E_FILEOPEN;
			// Otherwise, remove the offending file
			int retVal = remove(saveAs.c_str());
			ASSERT( retVal == 0 );
		}
	}
	// Open the file for writing
	_SplitPath(saveAs, directory, saveAs);
	std::string tmpFilename = directory + "~_" + saveAs;
	std::fstream outputFile;
	outputFile.open(tmpFilename.c_str(), std::ios::out | std::ios::binary | std::ios::trunc );
	if( outputFile.fail() || !outputFile.is_open() ) return E_FILEOPEN;
	// Save implies commiting any open transaction
	if (this->_inTransaction) this->CommitTransaction();

	// Move the write pointer to just after the preamble
	size_t preambleSize = sizeof(Uuid) + sizeof(uint64_t) + sizeof(uint32_t);
	outputFile.seekp(preambleSize);
	// Write out all of the objects
	IndexHashIterator hashIter = this->_indexHash.begin();
	while (hashIter != this->_indexHash.end())
	{
		ASSERT( hashIter->second.location != EntryLocation::Output() );
		// See if object is in input and correctly compressed and encrypted
		if (hashIter->second.location != EntryLocation::Cache() &&
			hashIter->second.isCompressed == this->_isCompressed &&
			hashIter->second.isEncrypted == this->_isEncrypted)
		{
			// Create appropriate buffer for read
			char* buffer = new char[hashIter->second.sizeB];
			// Where are we reading from?
			if (hashIter->second.location == EntryLocation::Input())
			{
				// Read data into buffer from the file
				this->_inputFile.seekg(hashIter->second.position);
				this->_inputFile.read(buffer, hashIter->second.sizeB);
			}
			else
			{
				// Read data into buffer from the file
				this->_scratchFile.seekg(hashIter->second.position);
				this->_scratchFile.read(buffer, hashIter->second.sizeB);
			}
			// Write data out to output file
			hashIter->second.position = outputFile.tellp();
			hashIter->second.location = EntryLocation::Output();
			outputFile.write(buffer, hashIter->second.sizeB);
			// Clean up a bit
			delete buffer;
		}
		// Otherwise, fetch the object into memory and write it out to the input file
		else
		{
			this->FetchObject(hashIter->first);
			// Set the indexEntry flags appropriately
			hashIter->second.isCompressed = this->_isCompressed;
			hashIter->second.isEncrypted = this->_isEncrypted;
			hashIter->second.location = EntryLocation::Output();
			// Write the object out to file
			this->ObjectToFile(outputFile, hashIter->second);
		}
		// Move on to the next item in the hash
		++hashIter;
	}

	// Write out all journaled transaction buffers
	JournalList::iterator journalIter = this->_undoList.begin();
	while (journalIter != this->_undoList.end())
	{
		ASSERT( journalIter->location != EntryLocation::Output() );
		// Find the journal entry
		if (journalIter->location == EntryLocation::Input())
		{
			// If in input or scratch file, copy the journal entry buffer into memory
			journalIter->buffer = new char[journalIter->sizeB];
			this->_inputFile.seekg(journalIter->position);
			this->_inputFile.read(journalIter->buffer, journalIter->sizeB);
		}
		else if (journalIter->location == EntryLocation::Scratch())
		{
			// Copy the journal entry buffer into memory
			journalIter->buffer = new char[journalIter->sizeB];
			this->_scratchFile.seekg(journalIter->position);
			this->_scratchFile.read(journalIter->buffer, journalIter->sizeB);
		}
		// Buffer must be present now
		ASSERT( journalIter->buffer != NULL );
		// Get some info about the new journal buffer location
		journalIter->location = EntryLocation::Output();
		journalIter->position = outputFile.tellp();
		// Write out each journal buffer
		outputFile.write(journalIter->buffer, journalIter->sizeB);
		// Move to next journal entry
		++journalIter;
	}

	// Write out the object index
	std::streampos startOfIndex = outputFile.tellp();
	uint64_t indexSizeB;
	this->WriteIndex(outputFile, this->_indexHash, indexSizeB);
	// Make sure the cache and indexHash are clear
	this->FlushCache();
	this->_indexHash.clear();	

	// Write out all journal index
	std::streampos startOfJournal = outputFile.tellp();
	uint64_t journalSizeB;
	this->WriteJournal(outputFile, journalSizeB);
	// Clear the undo and redo lists
	this->FlushUndoRedo(this->_undoList.size());

	// Write out the options
	std::streampos startOfOptions = outputFile.tellp();
	uint32_t optionsSize = this->WriteOptions(outputFile, startOfIndex, indexSizeB, startOfJournal, journalSizeB);

	// Write out the preamble (metaProject Uuid + options location and options size)
	char* buffer = new char[preambleSize];
	char* bufferPointer = buffer;
	_Write(bufferPointer, this->_metaProjectUuid);
	_Write(bufferPointer, startOfOptions);
	_Write(bufferPointer, optionsSize);
	// Make sure to start writing at the beginning of the file d'uh
	outputFile.seekp(0);
	outputFile.write(buffer, preambleSize);
	ASSERT( !outputFile.bad() );
	delete buffer;

	// Clean up the original input file
	this->_inputFile.close();
	// Are we overwriting the original inputfile, then delete it
	ASSERT( !this->_inputFile.fail() );
	if (overwrite)
	{
		int retVal = remove(this->_filename.c_str());
		ASSERT( retVal == 0 );
	}

	// Close the file and we are done with it
	outputFile.close();
	ASSERT( !outputFile.fail() );
	// Rename tmp file to desired name (make sure to grab the filename before it is changed)
	std::string scratchFileName = this->_filename;
	this->_filename = directory + saveAs;
	int retVal = rename(tmpFilename.c_str(), this->_filename.c_str());
	ASSERT( retVal == 0 );
	
	// Close and delete scratch file
	this->_scratchFile.close();
	ASSERT( !this->_scratchFile.fail() );
	_SplitPath(scratchFileName, directory, scratchFileName);
	scratchFileName = directory + std::string("~") + scratchFileName;
	retVal = remove(scratchFileName.c_str());
	ASSERT( retVal == 0 );
	// Now load the newly saved file and keep on truckin'
	return this->Load();
}


const Result_t BinFile::BeginTransaction(void) throw()
{
	if( this->_inTransaction ) return E_TRANSACTION;
	// Make sure we are clean
	ASSERT( this->_deletedObjects.empty() );
	ASSERT( this->_createdObjects.empty() );
	ASSERT( this->_changedAttributes.empty() );
	// Get ready for new transaction
	this->_openedObject = this->_indexHash.end();
	this->_inTransaction = true;
	return S_OK;
}


const Result_t BinFile::CommitTransaction(const Uuid tag) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	// Close any opened object - and mark as no longer in transaction
	this->CloseObject();
	// Is this transaction actually going to do anything - if not, we are done
	if ( !this->_createdObjects.empty() || !this->_deletedObjects.empty() || !this->_changedAttributes.empty() )
	{
		// Ok, so we are doing something, mark the binFile as dirty
		this->MarkDirty();
		// Are we journaling this transaction
		if (this->_isJournaling)
		{
			// Do we need to trim the undo list
			if (this->_undoList.size() == this->_maxUndoSize) this->FlushUndoRedo(1);
			// Or just the redo list
			else this->FlushUndoRedo(0);
			// Pickle the transaction and add it to the undo list
			Result_t result = this->PickleTransaction(tag);
			ASSERT( result == S_OK );
		}
		// Don't need to do anything for created objects...
		this->_createdObjects.clear();
		// Changed objects - discard pre/post values
		ChangedAttributesHashIterator changeIter = this->_changedAttributes.begin();
		while (changeIter != this->_changedAttributes.end())
		{
			// Simply delete the AttributeChange
			ASSERT( changeIter->second != NULL );
			delete changeIter->second;
			// Move on to the next attributeChange
			++changeIter;
		}
		this->_changedAttributes.clear();
		// Must actually delete all objects in deletedObjects list
		std::list< std::pair<Uuid,IndexEntry> >::iterator deletedIter = this->_deletedObjects.begin();
		while ( deletedIter != this->_deletedObjects.end() )
		{
			// Delete the object itself
			BinObject* binObject = deletedIter->second.object;
			ASSERT( binObject != NULL );
			delete binObject;
			// Move to the next deleted object in the list
			++deletedIter;
		}
		this->_deletedObjects.clear();
	}
	// We are good - wrap it up
	this->_openedObject = this->_indexHash.end();
	this->_inTransaction = false;
	return S_OK;
}


const Result_t BinFile::AbortTransaction(void) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	// Close any opened object - and mark as no longer in transaction
	this->CloseObject();

	// Is this transaction actually going to do anything - if not, we are done
	if (!this->_createdObjects.empty() ||
		!this->_deletedObjects.empty() ||
		!this->_changedAttributes.empty() ) this->TryAbortTransaction();
	// We were successful
	this->_openedObject = this->_indexHash.end();
	this->_inTransaction = false;
	return S_OK;
}


const Result_t BinFile::OpenObject(const Uuid &uuid) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( uuid == Uuid::Null() ) return E_INVALID_USAGE;
	// If there is already an open object...
	if( this->_openedObject != this->_indexHash.end() )
	{
		// Make sure the desired object is not already open
		if (this->_openedObject->first == uuid) return S_OK;
		// Otherwise, close the object
		this->CloseObject();
	}
	// Fetch the object - make sure we got it
	this->_openedObject = this->FetchObject(uuid);
	if( this->_openedObject == this->_indexHash.end() ) return E_NOTFOUND;
	return S_OK;
}
 
 
const Result_t BinFile::CreateObject(const MetaID_t &metaID, Uuid &newUuid) throw()
{
	if( metaID == METAID_NONE ) return E_INVALID_USAGE;
	if( !this->_inTransaction ) return E_TRANSACTION;
	// Close any open object
	this->CloseObject();
	// Lookup the metaID in the metaProject to make sure it is valid
	CoreMetaObject* metaObject = NULL;
	Result_t result = this->_metaProject->GetObject(metaID, metaObject);
	if ( result != S_OK || metaObject == NULL ) return E_METAID;
	// Use a new Uuid to make sure it is unique
	Uuid uuid;
	IndexHashIterator hashIter = this->TryCreateObject(metaID, uuid);
	// Put the object into the createdObjects list
	this->_createdObjects.push_front(std::make_pair(uuid, metaID));
	this->_openedObject = hashIter;
	// Return the new UUID
	newUuid = uuid;
	return S_OK;
}
 
 
const Result_t BinFile::CloseObject(void) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	// No longer need the object
	this->_openedObject = this->_indexHash.end();
	return S_OK;
}

 
const Result_t BinFile::DeleteObject(void) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_indexHash.end() ) return E_INVALID_USAGE;
	if( this->_openedObject->first == this->_rootUuid ) return E_INVALID_USAGE;
	// Clean up dangling pointers to/from binObject
	BinObject *object = this->_openedObject->second.object;
	ASSERT( object != NULL );
	CoreMetaObject *metaObject = NULL;
	Result_t result = this->_metaProject->GetObject(object->GetMetaID(), metaObject);
	ASSERT( result == S_OK );
	ASSERT( metaObject != NULL );
	std::list<CoreMetaAttribute*> metaAttributes;
	result = metaObject->GetAttributes(metaAttributes);
	ASSERT( result == S_OK );
	std::list<CoreMetaAttribute*>::const_iterator attributeIter = metaAttributes.begin();
	while (attributeIter != metaAttributes.end())
	{
		// Is this attribute a pointer?
		ValueType valueType;
		result = (*attributeIter)->GetValueType(valueType);
		ASSERT( result == S_OK );
		if (valueType == ValueType::Pointer())
		{
			// Just set this attribute to Uuid::Null()
			Uuid newUuid = Uuid::Null();
			AttrID_t attrID;
			result = (*attributeIter)->GetAttributeID(attrID);
			ASSERT( result == S_OK );
			result = this->SetAttributeValue(attrID, newUuid);
			ASSERT( result == S_OK );
		}
		// Or a backpointer collection
		else if (valueType == ValueType::Collection())
		{
			// TODO: Also clean up all back pointers
//			ASSERT(false);
		}
		// Move on to the next attribute
		++attributeIter;
	}
	// Capture the object's Uuid, pos, and pointer
	this->_deletedObjects.push_front( std::make_pair(this->_openedObject->first, this->_openedObject->second) );
	// Remove object from cacheQueue and indexHash
	if (this->_isCaching) this->_cacheQueue.remove(this->_openedObject->first);
	this->_indexHash.erase(this->_openedObject->first);
	// "Close" the object
	return this->CloseObject();
}


template<class T, class P, ValueTypeEnum VT>
inline const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, T &value) throw()
{
	// Make sure we are in a transaction
	if( !this->_inTransaction ) return E_TRANSACTION;
	// Make sure there is a valid open object
	if( this->_openedObject == this->_indexHash.end() ) return E_INVALID_USAGE;
	// Make sure the requested attrID is valid
	BinAttribute* binAttribute = this->_openedObject->second.object->GetAttribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	// Make sure the type of the attribute matches the type of Value
	if( binAttribute->GetValueType() != VT ) return E_ATTVALTYPE;
	// Now return the actual value of the object
	value = ((P*)binAttribute)->Get();
	return S_OK;
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, int32_t &value) throw()
{
	// Just call to the templated private method
	return this->GetAttributeValue<int32_t,BinAttributeLong,VALUETYPE_LONG>(attrID,value);
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, double &value) throw()
{
	// Just call to the templated private method
	return this->GetAttributeValue<double,BinAttributeReal,VALUETYPE_REAL>(attrID,value);
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, std::string &value) throw()
{
	// Just call to the templated private method
	return this->GetAttributeValue<std::string,BinAttributeString,VALUETYPE_STRING>(attrID,value);
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, std::list<Uuid> &value) throw()
{
	// Just call to the templated private method
	return this->GetAttributeValue<std::list<Uuid>,BinAttributeCollection,VALUETYPE_COLLECTION>(attrID,value);
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, Uuid &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_indexHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->GetAttribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	// Make sure we are looking at either a long pointer or a regular pointer
	if( binAttribute->GetValueType() == ValueType::Pointer() )
		value = ((BinAttributePointer*)binAttribute)->Get();
	else if ( binAttribute->GetValueType() == ValueType::LongPointer())
		value = ((BinAttributeLongPointer*)binAttribute)->Get();
	else return E_ATTVALTYPE;
	// Must have found the right stuff
	return S_OK;
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, DictionaryMap &value) throw()
{
	// Just call to the templated private method
	return this->GetAttributeValue<DictionaryMap,BinAttributeDictionary,VALUETYPE_DICTIONARY>(attrID,value);
}


template<class T, class P, ValueTypeEnum VT>
inline const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const T &value) throw()
{
	// Make sure we are in a transaction
	if( !this->_inTransaction ) return E_TRANSACTION;
	// Make sure we have a valid open object
	if( this->_openedObject == this->_indexHash.end() ) return E_INVALID_USAGE;
	// Make sure the attribute exists
	BinAttribute* binAttribute = this->_openedObject->second.object->GetAttribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	// Make sure the attribute is of the correct type for the given Value
	if( binAttribute->GetValueType() != VT ) return E_ATTVALTYPE;
	P *attribute = (P*)binAttribute;
	// Quick check to see if there is a no-change requested
	T currentValue = attribute->Get();
	if (currentValue == value) return S_OK;
	// Is there already a changeRecord for this attribute
	AttributeID attributeID = { this->_openedObject->first, attrID };
	AttributeChange<T> *changeRecord = NULL;
	ChangedAttributesHashIterator changeIter = this->_changedAttributes.find(attributeID);
	// A changed record already exists
	if (changeIter == this->_changedAttributes.end())
	{
		// Must create a new change record and save old value
		changeRecord = new AttributeChange<T>();
		ASSERT( changeRecord != NULL );
		changeRecord->oldValue = currentValue;
		changeRecord->type = VT;
		// Add the change record into the changedAttributes hash
		this->_changedAttributes.insert( std::make_pair(attributeID, changeRecord));
	}
	// Just grab the old changeRecord
	else changeRecord = (AttributeChange<T>*)changeIter->second;
	// Update the value in the record and the attribute
	changeRecord->newValue = value;
	attribute->Set(value);
	// Mark the binFile as dirty and move on
	this->MarkDirty();
	return S_OK;
}


const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const int32_t &value) throw()
{
	// Just call to the templated private method
	return this->SetAttributeValue<int32_t,BinAttributeLong,VALUETYPE_LONG>(attrID,value);
}


const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const double &value) throw()
{
	// Just call to the templated private method
	return this->SetAttributeValue<double,BinAttributeReal,VALUETYPE_REAL>(attrID,value);
}


const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const std::string &value) throw()
{
	// Just call to the templated private method
	return this->SetAttributeValue<std::string,BinAttributeString,VALUETYPE_STRING>(attrID,value);
}


const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const std::list<Uuid> &value) throw()
{
	// This is a dummy function to help out some template issues.  Really should find a better way
	ASSERT(false);
	return S_OK;
}


const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const Uuid &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_indexHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->GetAttribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	// Get the value type (we kind of use it a lot here)
	ValueType valueType = binAttribute->GetValueType();
	if( valueType != ValueType::Pointer() && valueType != ValueType::LongPointer() ) return E_ATTVALTYPE;
	BinAttributePointer *attribute = (BinAttributePointer*)binAttribute;
	// Quick check to see if there is a no-change requested
	Uuid currentValue = attribute->Get();
	if (currentValue == value) return S_OK;

	// Is this a pointer?
	if ( valueType == ValueType::Pointer() )
	{
		// Try to update the attribute value for a pointer type
		Result_t result = binAttribute->GetAttributeID();
		ASSERT( result == attrID );
		result = this->PointerUpdate(binAttribute->GetAttributeID(), this->_openedObject->first, currentValue, value);
		if (result != S_OK) return result;
	}

	// Is there already a changeRecord for this attribute
	AttributeID attributeID = { this->_openedObject->first, attrID };
	AttributeChange<Uuid> *changeRecord = NULL;
	ChangedAttributesHashIterator changeIter = this->_changedAttributes.find(attributeID);
	// A changed record already exists
	if (changeIter == this->_changedAttributes.end())
	{
		// Must create a new change record and save old value
		changeRecord = new AttributeChange<Uuid>();
		ASSERT( changeRecord != NULL );
		changeRecord->oldValue = currentValue;
		changeRecord->type = valueType;
		// Add the change record into the changedAttributes hash
		this->_changedAttributes.insert( std::make_pair(attributeID, changeRecord));
	}
	// Just grab the old changeRecord
	else changeRecord = (AttributeChange<Uuid>*)changeIter->second;
	// Update the value in the record and the attribute
	changeRecord->newValue = value;
	attribute->Set(value);
	// Mark the binFile as dirty and move on
	this->MarkDirty();
	return S_OK;
}


const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const DictionaryMap &value) throw()
{
	// Just call to the templated private method
	return this->SetAttributeValue<DictionaryMap,BinAttributeDictionary,VALUETYPE_DICTIONARY>(attrID,value);
}


// --------------------------- Configuration Options  --------------------------- //


const Result_t BinFile::EnableCaching(void) throw()
{
	// Make sure we are not in a transaction
	if (this->_inTransaction) return E_TRANSACTION;
	// Set the flag and go
	this->_isCaching = true;
	return S_OK;
}


const Result_t BinFile::DisableCaching(void) throw()
{
	// Can not happen during a transaction (for simplicity)
	if ( this->_inTransaction ) return E_TRANSACTION;
	// Nothing to be done if we already are not caching
	if (!this->_isCaching) return S_OK;
	// Otherwise, clear the cache
	this->FlushCache();
	// And set the caching flag to false
	this->_isCaching = false;
	return S_OK;
}


const Result_t BinFile::SetCacheSize(const uint64_t &size) throw()
{
	// Can not happen during a transaction (for simplicity)
	if ( this->_inTransaction ) return E_TRANSACTION;
	if ( !this->_isCaching ) return E_INVALID_USAGE;
	// Set the size and make any needed adjustments to the actual cache (may push some objs to file)
	this->_maxCacheSize = size;
	this->CheckCacheSize();
	return S_OK;
}


const Result_t BinFile::Undo(Uuid &tag) throw()
{
	// Must not be in a transaction
	if( this->_inTransaction ) return E_TRANSACTION;
	ASSERT( this->_createdObjects.empty() );
	ASSERT( this->_changedAttributes.empty() );
	ASSERT( this->_deletedObjects.empty() );
	if( this->_undoList.empty() ) return E_INVALID_USAGE;
	// Unpickle the back undo journal transaction
	JournalEntry entry = this->_undoList.back();
	Result_t result = this->UnpickleTransaction(entry);
	ASSERT( result == S_OK );

	// Ok, so we are doing something, mark the binFile as dirty
	this->MarkDirty();
	// Undo all of the changes (save as calling an AbortTransaction really)
	this->TryAbortTransaction();
	tag = entry.tag;
	// Move journal entry to redo list
	this->_redoList.push_back(entry);
	this->_undoList.pop_back();
	return S_OK;
}


const Result_t BinFile::Redo(Uuid &tag) throw()
{
	// Must not be in a transaction
	if( this->_inTransaction ) return E_TRANSACTION;
	ASSERT( this->_createdObjects.empty() );
	ASSERT( this->_changedAttributes.empty() );
	ASSERT( this->_deletedObjects.empty() );
	// Make sure there is something to redo
	if( this->_redoList.empty() ) return E_INVALID_USAGE;
	// Unpickle the back redo journal entry
	JournalEntry entry = this->_redoList.back();
	Result_t result = this->UnpickleTransaction(entry);
	ASSERT( result == S_OK );

	// Ok, so we are doing something, mark the binFile as dirty
	this->MarkDirty();
	// Create the created objects
	std::list<std::pair<Uuid,MetaID_t> >::iterator createdIter = this->_createdObjects.begin();
	while (createdIter != this->_createdObjects.end())
	{
		// Just call to TryCreateObject (don't care about the return IndexHashIterator)
		this->TryCreateObject(createdIter->second, createdIter->first);
		// Move on to the next created object
		++createdIter;
	}
	// Changed objects
	ChangedAttributesHashIterator changeIter = this->_changedAttributes.begin();
	while (changeIter != this->_changedAttributes.end())
	{
		// Get the attribute change record
		IndexHashIterator binIter = this->FetchObject(changeIter->first.uuid);
		ASSERT( binIter != this->_indexHash.end() );
		BinObject* binObject = binIter->second.object;
		ASSERT( binObject != NULL );
		BinAttribute* binAttribute = binObject->GetAttribute(changeIter->first.attrID);
		ASSERT( binAttribute != NULL );
		// Is the changed attribute a LONG
		if (binAttribute->GetValueType() == ValueType::Long())
		{
			// Change attribute value back to old value
			_AttributeChange<BinAttributeLong,int32_t>(binAttribute, changeIter->second, false);
		}
		// Is the changed attribute a REAL
		else if (binAttribute->GetValueType() == ValueType::Real())
		{
			// Change attribute value back to old value
			_AttributeChange<BinAttributeReal,double>(binAttribute, changeIter->second, false);
		}
		// Is the changed attribute a STRING
		else if (binAttribute->GetValueType() == ValueType::String())
		{
			// Change attribute value back to old value
			_AttributeChange<BinAttributeString,std::string>(binAttribute, changeIter->second, false);
		}
		// Is the changed attribute a LONGPOINTER
		else if (binAttribute->GetValueType() == ValueType::LongPointer())
		{
			// Change attribute value back to old value
			_AttributeChange<BinAttributeLongPointer,Uuid>(binAttribute, changeIter->second, false);
		}
		// Is the changed attribute a POINTER
		else if (binAttribute->GetValueType() == ValueType::Pointer())
		{
			BinAttributePointer *attribute = (BinAttributePointer*)binAttribute;
			ASSERT( attribute != NULL );
			AttributeChange<Uuid>* changeRecord = (AttributeChange<Uuid>*)(changeIter->second);
			ASSERT( changeRecord != NULL );
			this->PointerUpdate(binAttribute->GetAttributeID(), changeIter->first.uuid, attribute->Get(), changeRecord->oldValue);
			attribute->Set(changeRecord->newValue);
			delete changeRecord;
		}
		// Is the changed attribute a DICTIONARY
		else if (binAttribute->GetValueType() == ValueType::Dictionary())
		{
			// Change attribute value back to old value
			_AttributeChange<BinAttributeDictionary,DictionaryMap>(binAttribute, changeIter->second, false);
		}
		// Never should be here!
		else ASSERT(false);
		// Move on to the next attributeChange
		++changeIter;
	}
	// Must delete all objects in deletedObjects list
	std::list< std::pair<Uuid,IndexEntry> >::iterator deletedIter = this->_deletedObjects.begin();
	while ( deletedIter != this->_deletedObjects.end() )
	{
		// First, fetch the object to the cache
		this->FetchObject(deletedIter->first);
		// Now, remove object from cacheQueue and indexHash
		if (this->_isCaching) this->_cacheQueue.remove(deletedIter->first);
		this->_indexHash.erase(deletedIter->first);
		// Delete the object itself
		BinObject* binObject = deletedIter->second.object;
		ASSERT( binObject != NULL );
		delete binObject;
		// Move to the next deleted object in the list
		++deletedIter;
	}
	// Clear the transaction lists
	this->_createdObjects.clear();
	this->_changedAttributes.clear();
	this->_deletedObjects.clear();

	// Move entry to undoList from redoList
	this->_undoList.push_back(entry);
	this->_redoList.pop_back();
	// Return a tag (if any)
	tag = entry.tag;
	return S_OK;
}


const Result_t BinFile::JournalInfo(const uint64_t &undoSize, const uint64_t &redoSize,
									std::list<Uuid> &undoJournal, std::list<Uuid> &redoJournal) const throw()
{
	// Nothing to do if not journaling
	undoJournal.clear();
	redoJournal.clear();
	if (!this->_isJournaling) return E_INVALID_USAGE;
	// Get journal info
	// TODO: Get journal info
	return S_OK;
}


const Result_t BinFile::DisableJournaling(void) throw()
{
	// Nothing to be done if we already are not journaling
	if (!this->_isJournaling) return S_OK;
	// Otherwise, clear the journal (both undo and redo)
	this->FlushUndoRedo(this->_undoList.size());
	// And set the journaling flag to false
	this->_isJournaling = false;
	return S_OK;
}


const Result_t BinFile::EncryptionKey(std::vector<char> &key) const throw()
{
	// Is encryption not enabled?
	key.clear();
	if (!this->_isEncrypted) return S_OK;
	// Resize lists and copy in bytes
	key.resize(BINFILE_ENCRYPTIONKEYSIZE);
	memcpy(&key[0], this->_encryptionKey, BINFILE_ENCRYPTIONKEYSIZE);
	// All is good
	return S_OK;
}


const Result_t BinFile::EnableEncryption(const std::vector<char> &key) throw()
{
	// Must not be in a transaction
	if( this->_inTransaction ) return E_TRANSACTION;
	// Nothing to be done if we already are encrypting
	if (this->_isEncrypted) return S_OK;
	ASSERT(false);
	// TODO: Start encryption
	return S_OK;
}


const Result_t BinFile::DisableEncryption(void) throw()
{
	// Must not be in a transaction
	if( this->_inTransaction ) return E_TRANSACTION;
	// Nothing to be done if we already are not encryption
	if (!this->_isEncrypted) return S_OK;
	ASSERT(false);
	// TODO: Stop encryption
	return S_OK;
}

/*** Main Todo List
 *	1) Finish Enable/Disable encryption
 *	2) Work on generic search API
 *	3) Clean up and optimize the dirty flag
 *	4) DeleteObject backpointer clean up
 *	5) Finish journal caching scheme
 *	6) Consider removal of IsConnected
 *	7) Scratch file free space manager
***/



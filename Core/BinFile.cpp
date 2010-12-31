/*** Included Header Files ***/
#include "BinFile.h"
#include "CoreMetaObject.h"
#include "CoreMetaProject.h"
#include <utf8.h>


/*** Internally Defined Symbols ***/
#define COREBIN_DEFAULTCACHESIZE			10000000
#define COREBIN_FILEBUFFERSIZE_BYTES		8096


// --------------------------- IOStream Access  --------------------------- //


// IFStream Reads
inline static void _Read(char* &stream, int16_t &value)			{ memcpy(&value, stream, sizeof(int16_t)); stream += sizeof(int16_t); }
inline static void _Read(char* &stream, uint32_t &value)		{ memcpy(&value, stream, sizeof(uint32_t)); stream += sizeof(uint32_t); }
inline static void _Read(char* &stream, int32_t &value)			{ memcpy(&value, stream, sizeof(int32_t)); stream += sizeof(int32_t); }
inline static void _Read(char* &stream, double &value)			{ memcpy(&value, stream, sizeof(double)); stream += sizeof(double); }
inline static void _Read(char* &stream, ValueType &valueType)	{ valueType = ValueType::Read(stream); }
inline static void _Read(char* &stream, std::streampos &pos)	{ uint64_t value; memcpy(&value, stream, sizeof(uint64_t));  pos = (std::streampos)value; stream += sizeof(uint64_t); }
inline static void _Read(char* &stream, Uuid &value)			{ memcpy(&value, stream, sizeof(Uuid));  stream += sizeof(Uuid); }

inline static void _Read(char* &stream, std::string &str)
{
	// Read the length of the string
	uint32_t len;
	_Read(stream, len);
	ASSERT( len >= 0 );
	// Create the string
	str.resize(len);
	memcpy(&str[0], stream, len);
	stream += len;
}

inline static void _Read(char* &stream, std::list<Uuid> &collection)
{
	// Read the number of Uuids in the list
	uint32_t len;
	_Read(stream, len);
	ASSERT( len >= 0 );
	// Read data into the list
	Uuid uuid;
	for (uint32_t i=0; i<len; ++i)
	{
		// One read from the stream per Uuid
		_Read(stream, uuid);
		collection.push_back(uuid);
	}
}

// OFStream Writes
inline static void _Write(char* &stream, const int16_t &value)		{ memcpy(stream, &value, sizeof(int16_t)); stream += sizeof(int16_t); }
inline static void _Write(char* &stream, const uint32_t &value)		{ memcpy(stream, &value, sizeof(uint32_t)); stream += sizeof(uint32_t); }
inline static void _Write(char* &stream, const int32_t &value)		{ memcpy(stream, &value, sizeof(int32_t)); stream += sizeof(int32_t); }
inline static void _Write(char* &stream, const double &value)		{ memcpy(stream, &value, sizeof(double)); stream += sizeof(double); }
inline static void _Write(char* &stream, const ValueType &valueType){ valueType.Write(stream); }
inline static void _Write(char* &stream, const std::streampos &pos)	{ uint64_t value = (uint64_t)pos; memcpy(stream, &value, sizeof(uint64_t)); stream += sizeof(uint64_t); }
inline static void _Write(char* &stream, const Uuid &value)			{ memcpy(stream, &value, sizeof(Uuid)); stream += sizeof(Uuid); }

inline static void _Write(char* &stream, const std::string &str) {
	// Write the length of the string
	uint32_t len = str.length();
	ASSERT( len >= 0 );
	_Write(stream, len);
	// Write the actual string
	memcpy(stream, &str[0], len);
	stream += len;
}

inline static void _Write(char* &stream, const std::list<Uuid> &collection) {
	// Write the length of the list
	uint32_t len = collection.size();
	ASSERT( len >= 0 );
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


// --------------------------- BinAttr ---------------------------


BinAttribute *BinAttribute::Read(BinObject *parent, char *stream)
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
//	std::cout << "\tAttribute (" << valueType << ", " << attrID << ")\n";	
	ASSERT( valueType != ValueType::None() );
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
	// Return what we have
	ASSERT( binAttribute != NULL );	
	// Return the new attribute
	return binAttribute;
}


// BinAttribute Read/Write Methods (need access to _Write and _Read functions)
inline void BinAttributeLong::StreamRead(char* &stream)
{
	_Read(stream, this->_value);
}

inline void BinAttributeLong::StreamWrite(char* &stream) const
{
	_Write(stream, ValueType::Long());
	_Write(stream, this->_attrID);
	_Write(stream, this->_value);
}

inline void BinAttributeReal::StreamRead(char* &stream)
{
	_Read(stream, this->_value);
}

inline void BinAttributeReal::StreamWrite(char* &stream) const
{
	_Write(stream, ValueType::Real());
	_Write(stream, this->_attrID);
	_Write(stream, this->_value);
}

inline void BinAttributeString::StreamRead(char* &stream)
{
	_Read(stream, this->_value);
}

inline void BinAttributeString::StreamWrite(char* &stream) const
{
	_Write(stream, ValueType::String());
	_Write(stream, this->_attrID);
	_Write(stream, this->_value);
}


inline const Result_t BinAttributeString::Set(const std::string &value)
{
	std::string localValue = value;
	// Make sure this is a valid UTF-8 encoding
	std::string::iterator endIter = utf8::find_invalid(localValue.begin(), localValue.end());
	if (endIter != localValue.end())
	{
		std::cout << "Invalid UTF-8 encoding detected!\n";
		std::cout << "This part is fine: " << std::string(localValue.begin(), endIter) << "\n";
		return E_BADUTF8STRING;
	}
	
	// Get the line length (at least for the valid part)
	this->_value = value;
	this->_parent->MarkDirty();
	return S_OK;
}

inline void BinAttributeLongPointer::StreamRead(char* &stream)
{
	_Read(stream, this->_value);
}

inline void BinAttributeLongPointer::StreamWrite(char* &stream) const
{
	_Write(stream, ValueType::LongPointer());
	_Write(stream, this->_attrID);
	_Write(stream, this->_value);
}

inline void BinAttributeCollection::StreamRead(char* &stream)
{
	_Read(stream, this->_value);
}

inline void BinAttributeCollection::StreamWrite(char* &stream) const
{
	_Write(stream, ValueType::Collection());
	_Write(stream, this->_attrID);
	_Write(stream, this->_value);
}

void BinAttributePointer::StreamRead(char* &stream)
{
	_Read(stream, this->_value);
}

void BinAttributePointer::StreamWrite(char* &stream) const
{
	_Write(stream, ValueType::Pointer());
	_Write(stream, this->_attrID);
	_Write(stream, this->_value);
}


// --------------------------- Private BinObject Methods ---------------------------

 
void BinObject::CreateAttributes(CoreMetaObject *metaObject)
{
	ASSERT( metaObject != NULL );
	ASSERT( this->_attributes.empty() );
	// Get the meta attributes for the meta object
	std::list<CoreMetaAttribute*> metaAttributeList;
	ASSERT( metaObject->GetAttributes(metaAttributeList) == S_OK );
	std::list<CoreMetaAttribute*>::const_iterator attribIter = metaAttributeList.begin();
	while( attribIter != metaAttributeList.end() )
	{
		ValueType valueType;
		ASSERT( (*attribIter)->GetValueType(valueType) == S_OK );
		AttrID_t attrID = ATTRID_NONE;
		ASSERT( (*attribIter)->GetAttributeID(attrID) == S_OK );
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
	ASSERT( this->_metaObject->GetMetaID(metaID) == S_OK );
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
	return false;
}


// --------------------------- Public BinObject Methods ---------------------------


BinObject::~BinObject()
{
	std::list<BinAttribute*>::iterator attrIter = this->_attributes.begin();
	// Delete every attribute in the list
	while( attrIter != this->_attributes.end() )
	{
		if (*attrIter != NULL ) delete *attrIter;
		++attrIter;
	}
	// Now clear the list
	this->_attributes.clear();
}


BinObject* BinObject::Read(CoreMetaProject* &metaProject, char* &stream, const Uuid &uuid)
{
	ASSERT( stream != NULL );
	// First, read in the metaID
	MetaID_t metaID;
	_Read(stream, metaID);
	// We are done if is METAID_NONE
	if( metaID == METAID_NONE ) return NULL;
	// Get the metaObject using this metaID
	CoreMetaObject* metaObject = NULL;
	ASSERT( metaProject->GetObject(metaID, metaObject) == S_OK );
	
	// Create the binObject
	BinObject* binObject = new BinObject(metaObject, uuid);
	ASSERT( binObject != NULL );
	BinAttribute *binAttribute = NULL;
	ValueType valueType;
	// Continue to read in attributes until a NULL is returned
	do
	{
		// Read in an attribute
		binAttribute = BinAttribute::Read(binObject, stream);
		// Add the attribute to the object
		if (binAttribute != NULL) binObject->_attributes.push_front(binAttribute);
	} while (binAttribute != NULL);
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
		size += (*attrIter)->Size();
		size += sizeof(uint8_t);
		size += sizeof(AttrID_t);
		// Move on to the next attribute
		++attrIter;
	}
	// Return the total size, remember the trailing ValueType::None()
	size += sizeof(uint8_t);
	return size;
}


uint32_t BinObject::Write(char* &stream) const
{
	ASSERT( stream != NULL );
	// Is this object deleted (via implicit deferred deletion - i.e. no connected forward pointers)
	if ( !this->IsConnected() ) return 0;
	char* streamMark = stream;
	// Write the metaID
	MetaID_t metaID;
	ASSERT( this->_metaObject->GetMetaID(metaID) == S_OK );
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
	uint32_t writeSize = (uint32_t)(stream - streamMark);
	uint32_t sizeSize = this->Size();
	ASSERT( writeSize == sizeSize );
//	std::cout << "Write Object (" << pos << " @ " << stream.tellp() << ")-(" << this->_idPair.metaID << ", " << this->_idPair.objID << ").\n";
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
	_indexHash(),_cacheQueue(), _maxCacheSize(COREBIN_DEFAULTCACHESIZE),
	_openedObject(), _createdObjects(), _changedObjects(), _deletedObjects()
{
	ASSERT(filename != "" );
	ASSERT( coreMetaProject != NULL );
	this->_openedObject = this->_indexHash.end();
}


const Result_t BinFile::Create(const std::string &filename, CoreMetaProject *coreMetaProject, ICoreStorage* &storage)
{
	if ( filename == "" ) return E_INVALID_USAGE;
	if ( coreMetaProject == NULL ) return E_META_NOTOPEN;
	// Create a BinFile object with the given name and metaProject
	BinFile *binFile = new BinFile(filename, coreMetaProject);
	ASSERT( binFile != NULL );
	// Open the metaProject and get the Uuid
	ASSERT( binFile->_metaProject->GetUuid(binFile->_metaProjectUuid) == S_OK );

	// Try to open the file -- previously ios::nocreate had been used but no file is created if opened for read only
	binFile->_inputFile.clear();
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

	// Now just create the actual METAID_ROOT object (using a nice transaction of course)
	Uuid rootUuid;
	ASSERT( binFile->BeginTransaction() == S_OK );
	ASSERT( binFile->CreateObject(METAID_ROOT, rootUuid) == S_OK );
	ASSERT( binFile->CommitTransaction() == S_OK );
	// Make sure to capture the rootUuid
	binFile->_rootUuid = rootUuid;
	// Return the new BinFile
	binFile->_openedObject = binFile->_indexHash.end();
	storage = binFile;
	return S_OK;
}


const Result_t BinFile::Open(const std::string &filename, CoreMetaProject *coreMetaProject, ICoreStorage* &storage)
{
	if ( filename == "" ) return E_INVALID_USAGE;
	if ( coreMetaProject == NULL ) return E_INVALID_USAGE;
	// Clean up the filename a bit (handle ~ and such)
	std::string tmpName, directory;
	_SplitPath(filename, directory, tmpName);
	tmpName = directory + tmpName;
	// Create a BinFile object with the given name
	BinFile *binFile = new BinFile(tmpName, coreMetaProject);
	ASSERT( binFile != NULL );
	// Open the metaProject and get the Uuid
	ASSERT( binFile->_metaProject->GetUuid(binFile->_metaProjectUuid) == S_OK );

	// Load the project
	Result_t result = binFile->Load();
	if (result != S_OK)
	{
		// Failure to open file properly
		delete binFile;
		storage = NULL;
		return result;
	}
	// Return the new BinFile
	storage = binFile;
	return S_OK;
}


const Result_t BinFile::Load(void)
{
	ASSERT( !this->_inTransaction );
	ASSERT( this->_filename != "" );
	ASSERT( this->_metaProject != NULL );
	// Make sure the cache and indexHash are clear
	this->FlushCache();
	this->_indexHash.clear();

	// Try to open the file -- previously ios::nocreate had been used but no file is created if opened for read only
	this->_inputFile.open(this->_filename.c_str(), std::ios::in | std::ios::binary);
	if( this->_inputFile.fail() || !this->_inputFile.is_open() ) return E_FILEOPEN;
	// Get the scratch file ready for writing and reading
	this->_scratchFile.clear();
	std::string filename, directory, scratchFileName;
	_SplitPath(this->_filename, directory, filename);
	scratchFileName = directory + std::string("~") + filename;
	this->_scratchFile.open(scratchFileName.c_str(), std::ios::binary | std::ios_base::in | std::ios_base::out | std::ios::trunc);
	if( this->_scratchFile.fail() || !this->_scratchFile.is_open() )
	{
		// Close the input file
		this->_inputFile.close();
		return E_FILEOPEN;
	}

	// Read the project Uuid and index object count from the file
	Uuid uuid;
	uint32_t objCount;
	char buffer[sizeof(Uuid) + sizeof(uint32_t) + sizeof(Uuid)], *stream = buffer;
	this->_inputFile.read(buffer, sizeof(Uuid) + sizeof(uint32_t));
	_Read(stream, uuid);
	// Make sure the Uuid matches 
	if( !(uuid == this->_metaProjectUuid) )
	{
		// Close the files
		this->_inputFile.close();
		this->_scratchFile.close();
		return E_PROJECT_MISMATCH;
	}
	// Read the object index
	_Read(stream, objCount);
	Result_t result = this->ReadIndex(this->_inputFile, objCount);
	// Do we have a good index
	if (result != S_OK)
	{
		// Close the files
		this->_inputFile.close();
		this->_scratchFile.close();
		return result;
	}
	// Finally, read the root object Uuid
	_Read(stream, this->_rootUuid);
	// Open went well (make sure to clear the openedObject)
	this->_openedObject = this->_indexHash.end();
	return S_OK;
}


const Result_t BinFile::ReadIndex(std::fstream &stream, const uint32_t &objCount)
{
	ASSERT( stream.is_open() );
	// Therefore, the index is how large (Uuid, Position, Size)
	uint32_t indexSizeB = objCount * (sizeof(Uuid) + sizeof(uint64_t) + sizeof(uint32_t));
	std::vector<char> indexBuffer;
	indexBuffer.reserve(indexSizeB);
	char *bufferPointer = &indexBuffer[0];
	// Read the index from the file itself
	stream.read(bufferPointer, indexSizeB);
	// Read in each item for the index
	Uuid uuid;
	IndexEntry entry = { NULL, IndexLocation::Input(), 0, 0};
	for (uint32_t i=0; i < objCount; ++i)
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
//	std::cout << objCount << " objects read into index.\n";
	return S_OK;
}


const Result_t BinFile::WriteIndex(std::fstream &stream, const uint32_t &objCount) const
{
	ASSERT( stream.is_open() );
	// Create a correctly sized output buffer
	uint32_t indexSizeB = objCount * (sizeof(Uuid) + sizeof(uint64_t) + sizeof(uint32_t));
	std::vector<char> indexBuffer;
	indexBuffer.reserve(indexSizeB);
	char *bufferPointer = &indexBuffer[0];
	// Write each item from the index into the buffer
	IndexHash::const_iterator hashIter = this->_indexHash.begin();
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
	// Now write out the data to the file
	stream.write(&indexBuffer[0], indexSizeB);
	return S_OK;
}


IndexHashIterator BinFile::FetchObject(const Uuid &uuid) {
//	std::cout << "Fetching (" << uuid << ").\n";
	// Is the object in index
	IndexHashIterator indexIter = this->_indexHash.find(uuid);
	if (indexIter == this->_indexHash.end()) return indexIter;

	//Is the object in the cache
	if (indexIter->second.location == IndexLocation::Cache())
	{
		// Remove the object from the cacheQueue
		this->_cacheQueue.remove(uuid);
		// Move the object to the top of the cacheQueue
		this->_cacheQueue.push_front(uuid);
//		std::cout << "Found Object (" << indexIter->first << ") in cache.\n";
	}
	// Next, try the inputFile
	else if (indexIter->second.location == IndexLocation::Input())
	{
		// Move the object from the scratch file to the cache
		this->CacheObjectFromFile(this->_inputFile, indexIter);
//		std::cout << "Found Object (" << indexIter->first << ") in file.\n";
	}
	// Lastly, is the object in the scratch file
	else // if (indexIter->second.location == IndexLocation::Scratch())
	{
		// Move the object from the scratch file to the cache
		this->CacheObjectFromFile(this->_scratchFile, indexIter);
//		std::cout << "Found Object (" << indexIter->first << ") in scratch.\n";
		// Since the scratchFile is only for dirty objects, must mark object as dirty
		indexIter->second.object->MarkDirty();
	}
	// Return the indexIter
	return indexIter;
}


void BinFile::CacheObjectFromFile(std::fstream &stream, IndexHashIterator &hashIter)
{
	ASSERT( stream.is_open() );
	// Move the read pointer to the appropraite place
	stream.seekg(hashIter->second.position);
	// Try to read the object
	std::vector<char> buffer;
	buffer.reserve(hashIter->second.sizeB);
	char *bufferPointer = &buffer[0];
	stream.read(bufferPointer, hashIter->second.sizeB);
	BinObject* binObject = BinObject::Read(this->_metaProject, bufferPointer, hashIter->first);
	ASSERT( binObject != NULL );
	// Make sure there is space in the cache
	this->CheckCacheSize();
	// Move the object to the cache and to the front of the cacheQueue
	this->_cacheQueue.push_front(hashIter->first);
}


void BinFile::CheckCacheSize(void)
{
	// If cache size (+1 object) will be too large, get last used object
	while (this->_cacheQueue.size() >= this->_maxCacheSize)
	{
		// Get the object info from the cache and index
		Uuid uuid  = this->_cacheQueue.back();
		ASSERT( uuid != Uuid::Null() );
		IndexHashIterator hashIter = this->_indexHash.find(uuid);
		ASSERT( hashIter != this->_indexHash.end() );
		// If the object is not dirty
		if (!hashIter->second.object->IsDirty())
		{
			// Place the object back into _inputFile (no need to write anything)
			hashIter->second.location = IndexLocation::Input();
		}
		// If the object is dirty, see where the end of scratchFile is
		else
		{
			// Set the location to _scratchFile (need to write it out)
			hashIter->second.location = IndexLocation::Scratch();
			// Get the size of the binObject
			hashIter->second.sizeB = hashIter->second.object->Size();
			// Get the position of where the write it going to happen
			hashIter->second.position = (uint64_t)this->_scratchFile.tellp();
			// Now, write to the end of scratchFile
			std::vector<char> buffer;
			buffer.reserve(hashIter->second.sizeB);
			char *bufferPointer = &buffer[0];
			hashIter->second.object->Write(bufferPointer);
			this->_scratchFile.write(bufferPointer, hashIter->second.sizeB);
		}
		// Delete the object now (not needed any longer)
		ASSERT( hashIter->second.object != NULL );
		delete hashIter->second.object;		
		// Finally, remove that object from the cacheQueue
		this->_cacheQueue.pop_back();
	}
}


void BinFile::FlushCache(void)
{
	// Loop through each index item
	IndexHashIterator indexIter = this->_indexHash.begin();
	while (indexIter != this->_indexHash.end())
	{
		// Delete the object if it is in Cache
		if (indexIter->second.location == IndexLocation::Cache())
			delete indexIter->second.object;
		// Move on to the next indexEntry
		++indexIter;
	}
	// Make sure to clear the cache
	this->_cacheQueue.clear();
	this->_openedObject = this->_indexHash.end();
}


// --------------------------- Public BinFile Methods ---------------------------


BinFile::~BinFile()
{
	// If in a transaction, abort
	if( this->_inTransaction ) this->AbortTransaction();
	// Clean up any open files
	if( this->_inputFile.is_open() )
		this->_inputFile.close();
	if( this->_scratchFile.is_open() ) {
		this->_scratchFile.close();
		// Delete the scratch file
		std::string scratchFilename = "~" + this->_filename;
		remove(scratchFilename.c_str());
	}
	// Clear the cache (and its objects)
	this->FlushCache();
}


const Result_t BinFile::MaxMemoryFootprint(const uint64_t &size) throw()
{
	// Set the max cache size (in number of objects in cache)
	this->_maxCacheSize = size;
	// See if we need to reduce the number of objects in the cache
	this->CheckCacheSize();
	// We are done now
	return S_OK;
}


// ------- CoreStorage Interface -------


const Result_t BinFile::MetaObject(CoreMetaObject* &coreMetaObject) const throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	// Stupid messing with CONST workaround
	IndexHash::const_iterator iter = this->_indexHash.end();
	IndexHash::const_iterator iter2 = this->_openedObject;
	// If no currently opened object, return NULL
	if (iter == iter2) return E_INVALID_USAGE;
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
	// Stupid messing with CONST workaround
	IndexHash::const_iterator iter = this->_indexHash.end();
	IndexHash::const_iterator iter2 = this->_openedObject;
	// If no currently opened object, return METAID_NONE
	if (iter == iter2) return E_INVALID_USAGE;
	metaID = this->_openedObject->second.object->GetMetaID();
	ASSERT( metaID != METAID_NONE );
	return S_OK;
}


const Result_t BinFile::ObjectVector(std::vector<Uuid> &objectVector) const throw()
{
	// Clear the incoming vector
	objectVector.clear();
	// Load it with the indexHash
	IndexHash::const_iterator indexIter = this->_indexHash.begin();
	while(indexIter != this->_indexHash.end())
	{
		objectVector.push_back( indexIter->first );
		++indexIter;
	}
	// All is good
	return S_OK;
}


const Result_t BinFile::RootUuid(Uuid &uuid) const throw()
{
	// Simply copy the root Uuid through
	uuid = this->_rootUuid;
	return S_OK;
}


const Result_t BinFile::Save(const std::string &filename, const bool &v3) throw()
{
	// Make sure we have a valid project file open
	ASSERT( this->_metaProject != NULL );
	ASSERT( this->_inputFile.is_open() );
	ASSERT( this->_scratchFile.is_open() );
	// Passing an empty filename implies saving the file with the current filename
	std::string saveAs, directory;
	if (filename == "") saveAs = this->_filename;
	else saveAs = filename;
	// Are we overwriting the origial file?
	bool overwrite = (this->_filename == saveAs);
	// Open the file for writing
	_SplitPath(saveAs, directory, saveAs);
	std::string tmpFilename = directory + "~_" + saveAs;
	std::fstream outputFile;
	outputFile.open(tmpFilename.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
	ASSERT( !outputFile.fail() );
	ASSERT( outputFile.is_open() );
	// Save implies commiting any open transaction
	if (this->_inTransaction) this->CommitTransaction();

	// Write out the metaProjectID + number of objects in the index
	std::vector<char> buffer;
	buffer.reserve(sizeof(Uuid) + sizeof(uint32_t) + sizeof(Uuid));
	char *bufferPointer = &buffer[0];
	uint32_t objectCount = this->_indexHash.size();
	_Write(bufferPointer, this->_metaProjectUuid);
	_Write(bufferPointer, objectCount);
	_Write(bufferPointer, this->_rootUuid);
	outputFile.write(&buffer[0], sizeof(Uuid) + sizeof(uint32_t) + sizeof(Uuid));
	// Move streampos to end of what will be the index
	int indexSizeB = objectCount * (sizeof(Uuid) + sizeof(uint64_t) + sizeof(uint32_t));
	std::streampos startOfIndex = outputFile.tellp();
	// Move the write pointer to the end of the index
	outputFile.seekp(startOfIndex + (std::streampos)indexSizeB);

	// Write out all of the objects
	IndexHashIterator hashIter = this->_indexHash.begin();
	while (hashIter != this->_indexHash.end())
	{
		ASSERT( hashIter->second.object != NULL );
		// Fetch the object into memory
		this->FetchObject(hashIter->first);
		// Get the current outputFile position and object size (needed for correct index)
		hashIter->second.position = outputFile.tellp();
		hashIter->second.sizeB = hashIter->second.object->Size();
		// Size the buffer and set the bufferPointer
		buffer.reserve(hashIter->second.sizeB);
		bufferPointer = &buffer[0];
		// Write the object to the buffer
		hashIter->second.object->Write(bufferPointer);
		// Write the buffer to the stream
		outputFile.write(&buffer[0], hashIter->second.sizeB);
		// Move on to the next item in the hash
		++hashIter;
	}

	// Now that we know where everything will be, write out the index
	outputFile.seekp(startOfIndex);
	this->WriteIndex(outputFile, objectCount);
	// Close the file and we are done with it
	outputFile.close();
	ASSERT( !outputFile.fail() );	//E_FILEOPEN;

	// Close and delete scratch file
	this->_scratchFile.close();
	ASSERT( !this->_scratchFile.fail() );	//E_FILEOPEN
	std::string scratchFileName = "~" + this->_filename;
	remove(scratchFileName.c_str());

	// Are we overwriting the original inputfile
	this->_inputFile.close();
	ASSERT( !this->_inputFile.fail() );		//E_FILEOPEN
	if (overwrite) {
		// Delete the original input file
		remove(this->_filename.c_str());
	}

	// Rename tmp file to desired name
	rename(tmpFilename.c_str(), saveAs.c_str());
	// Set the new filename and load (what was outputFile)
	this->_filename = directory + saveAs;
	// Now load the newly saved file and keep on truckin'
	this->Load();
	return S_OK;
}


const Result_t BinFile::BeginTransaction(void) throw()
{
	if( this->_inTransaction ) return E_TRANSACTION;
	// Make sure we are clean
	ASSERT( this->_deletedObjects.empty() );
	ASSERT( this->_createdObjects.empty() );
	ASSERT( this->_changedObjects.empty() );
	// Get ready for new transaction
	this->_openedObject = this->_indexHash.end();
	this->_inTransaction = true;
	return S_OK;
}


const Result_t BinFile::CommitTransaction(void) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	// Close any opened object - and mark as no longer in transaction
	this->CloseObject();
	// Is this transaction actually going to do anything - if not, we are done
	if ( !this->_createdObjects.empty() || !this->_deletedObjects.empty() || !this->_changedObjects.empty() )
	{
		// Ok, so we are doing something, mark the binFile as dirty
		this->MarkDirty();
		// Created objects - nothing to be done here
		this->_createdObjects.clear();

		// Changed objects - discard pre/post - otherwise we are good
		ChangedObjectsList::iterator changeIter = this->_changedObjects.begin();
		while (changeIter != this->_changedObjects.end())
		{
			// Simply delete the AttributeChange
			ASSERT( *changeIter != NULL );
			delete *changeIter;
			++changeIter;
		}
		this->_changedObjects.clear();

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
	// We are good
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
	if ( !this->_createdObjects.empty() || !this->_deletedObjects.empty() || !this->_changedObjects.empty() )
	{
		// Must move all deletedObjects back to cache
		std::list< std::pair<Uuid,IndexEntry> >::iterator deletedIter = this->_deletedObjects.begin();
		while (deletedIter != this->_deletedObjects.end())
		{
			// Insert into the cache and queue
			std::pair<IndexHashIterator,bool> insertReturn = this->_indexHash.insert( *deletedIter );
			ASSERT( insertReturn.second );
			this->_cacheQueue.push_front(deletedIter->first);
			// Move to the next deleted object in the list
			++deletedIter;
		}
		// Clear the deleted objects list
		this->_deletedObjects.clear();
		
		// Changed objects - rollback changes in reverse order
		ChangedObjectsList::reverse_iterator changeIter = this->_changedObjects.rbegin();
		while (changeIter != this->_changedObjects.rend())
		{
			// Get the attribute change record
			IndexHashIterator binIter = this->FetchObject((*changeIter)->uuid);
			ASSERT( binIter != this->_indexHash.end() );
			BinObject* binObject = binIter->second.object;
			ASSERT( binObject != NULL );
			BinAttribute* binAttribute = binObject->GetAttribute((*changeIter)->attrID);
			ASSERT( binAttribute != NULL );
			// Is the changed attribute a LONG
			if (binAttribute->GetValueType() == ValueType::Long()) {
				BinAttributeLong *attribute = (BinAttributeLong*)binAttribute;
				ASSERT( attribute != NULL );
				AttributeChange<int32_t>* changeRecord = (AttributeChange<int32_t>*)*changeIter;
				ASSERT( changeRecord != NULL );
				attribute->Set(changeRecord->value);
			}
			// Is the changed attribute a REAL
			else if (binAttribute->GetValueType() == ValueType::Real()) {
				BinAttributeReal *attribute = (BinAttributeReal*)binAttribute;
				ASSERT( attribute != NULL );
				AttributeChange<double>* changeRecord = (AttributeChange<double>*)*changeIter;
				ASSERT( changeRecord != NULL );
				attribute->Set(changeRecord->value);
			}
			// Is the changed attribute a STRING
			else if (binAttribute->GetValueType() == ValueType::String()) {
				BinAttributeString *attribute = (BinAttributeString*)binAttribute;
				ASSERT( attribute != NULL );
				AttributeChange<std::string>* changeRecord = (AttributeChange<std::string>*)*changeIter;
				ASSERT( changeRecord != NULL );
				attribute->Set(changeRecord->value);
			}
			// Is the changed attribute a LONGPOINTER
			else if (binAttribute->GetValueType() == ValueType::LongPointer()) {
				BinAttributeLongPointer *attribute = (BinAttributeLongPointer*)binAttribute;
				ASSERT( attribute != NULL );
				AttributeChange<Uuid>* changeRecord = (AttributeChange<Uuid>*)*changeIter;
				ASSERT( changeRecord != NULL );
				attribute->Set(changeRecord->value);
			}
			// Is the changed attribute a COLLECTION
			else if (binAttribute->GetValueType() == ValueType::Collection()) {
				BinAttributeCollection *attribute = (BinAttributeCollection*)binAttribute;
				ASSERT( attribute != NULL );
				AttributeChange< std::list<Uuid> >* changeRecord = (AttributeChange< std::list<Uuid> >*)*changeIter;
				ASSERT( changeRecord != NULL );
				attribute->Set(changeRecord->value);
			}
			// Is the changed attribute a POINTER
			else if (binAttribute->GetValueType() == ValueType::Pointer()) {
				BinAttributePointer *attribute = (BinAttributePointer*)binAttribute;
				ASSERT( attribute != NULL );
				AttributeChange<Uuid>* changeRecord = (AttributeChange<Uuid>*)*changeIter;
				ASSERT( changeRecord != NULL );
				attribute->Set(changeRecord->value);
			}
			// Finally, delete the AttributeChange
			delete *changeIter;
			++changeIter;
		}
		// Clear the changes list
		this->_changedObjects.clear();
		
		// Delete all Created objects
		std::list<Uuid>::iterator createdIter = this->_createdObjects.begin();
		while( createdIter != this->_createdObjects.end() )
		{
			// Fetch the created object
			IndexHashIterator indexIter = this->FetchObject(*createdIter);
			ASSERT( indexIter != this->_indexHash.end() );
			// Delete the object itself
			BinObject* binObject = indexIter->second.object;
			ASSERT( binObject != NULL );
			delete binObject;
			// Remove object info from cacheHash and cacheQueue
			this->_indexHash.erase(indexIter);
			this->_cacheQueue.remove(*createdIter);
			// Move on to the next created object
			++createdIter;
		}
		// Clear the created objects list
		this->_createdObjects.clear();
	}
	// We were successful
	this->_openedObject = this->_indexHash.end();
	this->_inTransaction = false;
	return S_OK;
}


const Result_t BinFile::OpenObject(const Uuid &uuid) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( uuid == Uuid::Null() ) return E_INVALID_USAGE;
	// If there is already an open object, close it
	if( this->_openedObject != this->_indexHash.end() )
		this->CloseObject();
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
	ASSERT( this->CloseObject() == S_OK );
	// Lookup the metaID in the metaProject to make sure it is valid
	CoreMetaObject* metaObject = NULL;
	Result_t result = this->_metaProject->GetObject(metaID, metaObject);
	if ( result != S_OK || metaObject == NULL ) return E_METAID;
	// Make sure to use a new Uuid to make sure it is unique
	Uuid uuid;
	BinObject* binObject = BinObject::Create(metaObject, uuid);
	ASSERT( binObject != NULL );

	// Put the object into the index and queue
	IndexEntry indexEntry = { binObject, IndexLocation::Cache(), 0, 0 };
	std::pair<IndexHashIterator,bool> insertReturn = this->_indexHash.insert( std::make_pair(uuid, indexEntry) );
	ASSERT( insertReturn.second );
	this->_cacheQueue.push_front(uuid);
	// Put the object into the createdObjects list
	this->_createdObjects.push_front(uuid);
	this->_openedObject = insertReturn.first;
	// Set the new UUID
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

	// Clean up dangling pointers to/from binObject
	BinObject *object = this->_openedObject->second.object;
	ASSERT( object != NULL );
	CoreMetaObject *metaObject = NULL;
	ASSERT( this->_metaProject->GetObject(object->GetMetaID(), metaObject) == S_OK );
	ASSERT( metaObject != NULL );
	std::list<CoreMetaAttribute*> metaAttributes;
	ASSERT( metaObject->GetAttributes(metaAttributes) == S_OK );
	std::list<CoreMetaAttribute*>::const_iterator attributeIter = metaAttributes.begin();
	while (attributeIter != metaAttributes.end())
	{
		// Is this attribute a pointer?
		ValueType valueType;
		ASSERT( (*attributeIter)->GetValueType(valueType) == S_OK );
		if (valueType == ValueType::Pointer())
		{
			// Just set this attribute to Uuid::Null()
			Uuid newUuid = Uuid::Null();
			AttrID_t attrID;
			ASSERT( (*attributeIter)->GetAttributeID(attrID) == S_OK );
			ASSERT( this->SetAttributeValue(attrID, newUuid) == S_OK );
		}
		// Move on to the next attribute
		++attributeIter;
	}

	// Capture the object's Uuid, pos, and pointer
	this->_deletedObjects.push_front( std::make_pair(this->_openedObject->first, this->_openedObject->second) );
	// Remove object from cacheQueue and indexHash
	this->_cacheQueue.remove(this->_openedObject->first);
	this->_indexHash.erase(this->_openedObject->first);
	// "Close" the object
	return this->CloseObject();
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, int32_t &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_indexHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->GetAttribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Long() ) return E_ATTVALTYPE;
	// Now return the actual value of the object
	value = ((BinAttributeLong*)binAttribute)->Get();
	return S_OK;
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, double &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_indexHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->GetAttribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Real() ) return E_ATTVALTYPE;
	// Now return the actual value of the object
	value = ((BinAttributeReal*)binAttribute)->Get();
	return S_OK;
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, std::string &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_indexHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->GetAttribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::String() ) return E_ATTVALTYPE;
	// Now return the actual value of the object
	value = ((BinAttributeString*)binAttribute)->Get();
	return S_OK;
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, std::list<Uuid> &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_indexHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->GetAttribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Collection() ) return E_ATTVALTYPE;
	// Now return the actual value of the object
	value = ((BinAttributeCollection*)binAttribute)->Get();
	return S_OK;
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


const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const int32_t &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_indexHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->GetAttribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Long() ) return E_ATTVALTYPE;
	BinAttributeLong *attribute = (BinAttributeLong*)binAttribute;
	// Quick check to see if there is a no-change requested
	if (attribute->Get() == value) return S_OK;
	// Must save old value
	AttributeChange<int32_t>* changeRecord = new AttributeChange<int32_t>();
	ASSERT( changeRecord != NULL );
	changeRecord->uuid = this->_openedObject->first;
	changeRecord->attrID = attrID;
	changeRecord->value = attribute->Get();
	// Add the change record into the changedObjects list
	this->_changedObjects.push_back(changeRecord);
	// Update the attribute value
	attribute->Set(value);
	return S_OK;
}


const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const double &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_indexHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->GetAttribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Real() ) return E_ATTVALTYPE;
	BinAttributeReal *attribute = (BinAttributeReal*)binAttribute;
	// Quick check to see if there is a no-change requested
	if (attribute->Get() == value) return S_OK;
	// Must save old value
	AttributeChange<double>* changeRecord = new AttributeChange<double>();
	ASSERT( changeRecord != NULL );
	changeRecord->uuid = this->_openedObject->first;
	changeRecord->attrID = attrID;
	changeRecord->value = attribute->Get();
	// Add the change record into the changedObjects list
	this->_changedObjects.push_back(changeRecord);
	// Update the attribute value
	attribute->Set(value);
	return S_OK;
}


const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const std::string &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_indexHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->GetAttribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::String() ) return E_ATTVALTYPE;
	BinAttributeString *attribute = (BinAttributeString*)binAttribute;
	// Quick check to see if there is a no-change requested
	if (attribute->Get() == value) return S_OK;
	// Must save old value
	AttributeChange<std::string>* changeRecord = new AttributeChange<std::string>();
	ASSERT( changeRecord != NULL );
	changeRecord->uuid = this->_openedObject->first;
	changeRecord->attrID = attrID;
	changeRecord->value = attribute->Get();
	// Add the change record into the changedObjects list
	this->_changedObjects.push_back(changeRecord);
	// Update the attribute value
	attribute->Set(value);
	return S_OK;
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
	if( binAttribute->GetValueType() != ValueType::Pointer() &&
	    binAttribute->GetValueType() != ValueType::LongPointer() ) return E_ATTVALTYPE;
	BinAttributePointer *attribute = (BinAttributePointer*)binAttribute;
	// Quick check to see if there is a no-change requested
	if (attribute->Get() == value) return S_OK;
	// Is this a LongPointer
	if (binAttribute->GetValueType() == ValueType::LongPointer())
	{
		// Must save old value
		AttributeChange<Uuid>* changeRecord = new AttributeChange<Uuid>();
		ASSERT( changeRecord != NULL );
		changeRecord->uuid = this->_openedObject->first;
		changeRecord->attrID = attrID;
		changeRecord->value = attribute->Get();
		// Add the change record into the changedObjects list
		this->_changedObjects.push_back(changeRecord);
		// Update the attribute value
		attribute->Set(value);
	}
	else if (binAttribute->GetValueType() == ValueType::Pointer())
	{
		// Make sure value is valid (either a valid object or NULL - and pointed to has good backpointer collection)
		BinAttributeCollection *newCollection = NULL;
		if (value != Uuid::Null())
		{
			// Can we fetch this object - if not it is an error
			IndexHashIterator newObjecterIterator = this->FetchObject(value);
			if (newObjecterIterator == this->_indexHash.end()) return E_NOTFOUND;
			// Does the object have a correct backpointer collection
			BinObject* binObject = newObjecterIterator->second.object;
			ASSERT( binObject != NULL );
			newCollection = (BinAttributeCollection*)binObject->GetAttribute(attrID + ATTRID_COLLECTION);
			// Make sure the backpointer collection exists
			if (newCollection == NULL) return E_VALTYPE;
		}
		// Must save old value
		AttributeChange<Uuid>* changeRecord = new AttributeChange<Uuid>();
		ASSERT( changeRecord != NULL );
		changeRecord->uuid = this->_openedObject->first;
		changeRecord->attrID = attrID;
		changeRecord->value = attribute->Get();
		// Add the change record into the changedObjects list
		this->_changedObjects.push_back(changeRecord);
		// Update the current pointed-to object's backpointer collection (if idPair is valid)
		if (changeRecord->value != Uuid::Null())
		{
			BinObject *binObject = this->FetchObject(changeRecord->value)->second.object;
			ASSERT( binObject != NULL );
			BinAttributeCollection *collection = (BinAttributeCollection*)binObject->GetAttribute(attrID + ATTRID_COLLECTION);
			ASSERT( collection != NULL );	//	<-- Means pointed-to-object doesn't have backpointer collection for this type of pointer
			collection->Remove(this->_openedObject->first);
		}
		// Update the actual pointer value
		attribute->Set(value);
		// Update the pointed-to object's backpointer collection (if it points to something valid)
		if (newCollection != NULL) newCollection->Add(this->_openedObject->first);
	}
	// All is good...
	return S_OK;
}


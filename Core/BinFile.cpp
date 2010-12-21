/*** Included Header Files ***/
#include "BinFile.h"
#include "CoreMetaObject.h"
#include "CoreMetaProject.h"


/*** Internally Defined Symbols ***/
#define COREBIN_DEFAULTCACHESIZE	10000000


// --------------------------- IOStream Access  --------------------------- //


// IFStream Reads
inline static void _Read(std::fstream &ifs, int16_t &value)			{ ifs.read((char*)&value, sizeof(int16_t)); }
inline static void _Read(std::fstream &ifs, uint32_t &value)		{ ifs.read((char*)&value, sizeof(uint32_t)); }
inline static void _Read(std::fstream &ifs, int32_t &value)			{ ifs.read((char*)&value, sizeof(int32_t)); }
inline static void _Read(std::fstream &ifs, double &value)			{ ifs.read((char*)&value, sizeof(double)); }
inline static void _Read(std::fstream &ifs, ValueType &valueType)	{ valueType = ValueType::Read(ifs); }
inline static void _Read(std::fstream &ifs, MetaObjIDPair &value)
	{ ifs.read((char*)&(value.metaID), sizeof(MetaID_t)); ifs.read((char*)&(value.objID), sizeof(ObjID_t)); }
inline static void _Read(std::fstream &ifs, std::streampos &pos)
	{ uint64_t value; ifs.read((char*)&value, sizeof(uint64_t));  pos = (std::streampos)value; }

static void _Read(std::fstream &ifs, std::vector<unsigned char> &value)
{
	// Make sure the file is open
	ASSERT( ifs.is_open() );
	// Get the length of the vector
	uint32_t len;
	_Read(ifs, len);
	ASSERT( len >= 0 );
	// Read data into the vector
	value.resize(len);
	if( len > 0 )
		ifs.read( (char *) &value[0], len);
}

static void _Read(std::fstream &ifs, UUID &value)
{
	// Make sure the file is open
	ASSERT( ifs.is_open() );
	// Get the length of the UUID (must be 16)
	uint32_t len;
	_Read(ifs, len);
	ASSERT( len == 16 );
	// Read data into the UUID
	ifs.read( (char *) &value, len);
}

static void _Read(std::fstream &ifs, std::string &str)
{
	// Make sure the file is open
	ASSERT( ifs.is_open() );
	// Read the length of the string
	uint32_t len;
	_Read(ifs, len);
	ASSERT( len >= 0 );
	// Create the string
	str.resize(len);
	if( len > 0 )
		ifs.read( (char *) &str[0], len);
}

static void _Read(std::fstream &ifs, std::list<MetaObjIDPair> &collection)
{
	// Make sure the file is open
	ASSERT( ifs.is_open() );
	// Read the length of the string
	uint32_t len;
	_Read(ifs, len);
	ASSERT( len >= 0 );
	// Read data into the list
	MetaObjIDPair idPair;
	for (int32_t i=0; i<len; ++i)
	{
		_Read(ifs, idPair);
		collection.push_back(idPair);
	}
}

// OFStream Writes
template<class T>
inline static void _Write(std::fstream &ofs, const T &value)			{ ofs.write((const char*)&value, sizeof(T)); }
inline static void _Write(std::fstream &ofs, const int16_t &value)		{ ofs.write((const char*)&value, sizeof(int16_t)); }
inline static void _Write(std::fstream &ofs, const uint32_t &value)		{ ofs.write((const char*)&value, sizeof(uint32_t)); }
inline static void _Write(std::fstream &ofs, const int32_t &value)		{ ofs.write((const char*)&value, sizeof(int32_t)); }
inline static void _Write(std::fstream &ofs, const double &value)		{ ofs.write((const char*)&value, sizeof(double)); }
inline static void _Write(std::fstream &ofs, const ValueType &valueType){ valueType.Write(ofs); }
inline static void _Write(std::fstream &ofs, const MetaObjIDPair &value)
	{ ofs.write((const char*)&(value.metaID), sizeof(MetaID_t)); ofs.write((const char*)&(value.objID), sizeof(ObjID_t)); }
inline static void _Write(std::fstream &ofs, const std::streampos &pos)
	{ uint64_t value = (uint64_t)pos; ofs.write((const char*)&value, sizeof(uint64_t)); }

static void _Write(std::fstream &ofs, const std::vector<unsigned char> &value) {
	// Make sure the file is open
	ASSERT( ofs.is_open() );
	// Write the length of the vector
	uint32_t len = value.size();
	ASSERT( len >= 0 );
	_Write(ofs, len);
	// Write the actual the vector
	if( len > 0 )
		ofs.write( (char*)&value[0], len);
}

static void _Write(std::fstream &ofs, UUID &value) {
	// Make sure the file is open
	ASSERT( ofs.is_open() );
	// Write the length of the UUID (always 16)
	uint32_t len = sizeof(UUID);
	ASSERT( len == 16 );
	_Write(ofs, len);
	// Write the actual the UUID
	ofs.write( (char*)&value, len);
}

static void _Write(std::fstream &ofs, const std::string &str) {
	// Make sure the file is open
	ASSERT( ofs.is_open() );
	// Write the length of the string
	uint32_t len = str.length();
	ASSERT( len >= 0 );
	_Write(ofs, len);
	// Write the actual string
	if( len > 0 )
		ofs.write( (char*)&str[0], len);
}

static void _Write(std::fstream &ofs, const std::list<MetaObjIDPair> &collection) {
	// Make sure the file is open
	ASSERT( ofs.is_open() );
	// Write the length of the vector
	uint32_t len = collection.size();
	ASSERT( len >= 0 );
	_Write(ofs, len);
	// Write the actual vector
	std::list<MetaObjIDPair>::const_iterator collectionIter = collection.begin();
	while (collectionIter != collection.end())
	{
		_Write(ofs, *collectionIter);
		++collectionIter;
	}
}


// --------------------------- BinAttr ---------------------------


BinAttribute *BinAttribute::Read(BinObject *parent, std::fstream &stream, std::vector<std::pair<AttrID_t,MetaObjIDPair> > &pointers, const bool &v3)
{
	ASSERT( parent != NULL );
	ASSERT( parent->MetaID() != METAID_NONE );
	ASSERT( stream.is_open() );
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
	else if (valueType == ValueType::Binary())
		binAttribute = new BinAttributeBinary(parent, attrID);
	else if (valueType == ValueType::Lock())
		binAttribute = new BinAttributeLock(parent, attrID);
	else if (valueType == ValueType::Pointer())
		binAttribute = new BinAttributePointer(parent, attrID);
	else if (valueType == ValueType::Collection())
		binAttribute = new BinAttributeCollection(parent, attrID);
	// Return what we have
	ASSERT( binAttribute != NULL );
	// Command the attribute to finish reading itself
	binAttribute->FileRead(stream, v3);
	// Special case for ValueType::Pointer
	if (valueType == ValueType::Pointer()) {
		BinAttributePointer *pointer = (BinAttributePointer*)binAttribute;
		// Add the pointer to the vector if non-NONE
		if ( pointer->MetaID() != METAID_NONE ) {
//			std::cout << "\t\tPointer: (" << pointer->MetaID() << ", " << pointer->ObjID() << ").\n";
			pointers.push_back( std::make_pair(attrID, pointer->Get()) );
		}
	}
	// Return the new attribute
	return binAttribute;
};


BinAttribute* BinAttribute::Create(BinObject *parent, const ValueType &valueType, const AttrID_t &attrID)
{
	ASSERT( parent != NULL );
	ASSERT( parent->MetaID() != METAID_NONE );
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
	else if (valueType == ValueType::Binary())
		binAttribute = new BinAttributeBinary(parent, attrID);
	else if (valueType == ValueType::Lock())
		binAttribute = new BinAttributeLock(parent, attrID);
	else if (valueType == ValueType::Pointer())
		binAttribute = new BinAttributePointer(parent, attrID);
	else if (valueType == ValueType::Collection())
		binAttribute = new BinAttributeCollection(parent, attrID);
	// Return what we have
	ASSERT( binAttribute != NULL );	
	// Return the new attribute
	return binAttribute;
}


// BinAttribute Read/Write Methods (need access to _Write and _Read functions)

inline void BinAttributeLong::FileWrite(std::fstream &stream, const bool &v3) const
{
	_Write(stream, ValueType::Long());
	_Write(stream, this->_attrID);
	_Write(stream, this->_value);
}

inline void BinAttributeReal::FileWrite(std::fstream &stream, const bool &v3) const
{
	_Write(stream, ValueType::Real());
	_Write(stream, this->_attrID);
	_Write(stream, this->_value);
}

inline void BinAttributeString::FileWrite(std::fstream &stream, const bool &v3) const
{
	_Write(stream, ValueType::String());
	_Write(stream, this->_attrID);
	_Write(stream, this->_value);
}
inline void BinAttributeString::FileRead(std::fstream &stream, const bool &v3)			{ _Read(stream, this->_value); }

inline void BinAttributeBinary::FileWrite(std::fstream &stream, const bool &v3) const
{
	_Write(stream, ValueType::Binary());
	_Write(stream, this->_attrID);
	_Write(stream, this->_value);
}
inline void BinAttributeBinary::FileRead(std::fstream &stream, const bool &v3)			{ _Read(stream, this->_value); }

inline void BinAttributeCollection::FileWrite(std::fstream &stream, const bool &v3) const
{
	_Write(stream, ValueType::Collection());
	_Write(stream, this->_attrID);
	// Do nothing for v2
	if (!v3) return;
	// Otherwise, write out the collection size
	_Write(stream, this->_value);
}
inline void BinAttributeCollection::FileRead(std::fstream &stream, const bool &v3)
{
	// Do nothing for v2
	if (!v3) return;
	// Otherwise, read in the collection
	_Read(stream, this->_value);
}

void BinAttributePointer::FileWrite(std::fstream &stream, const bool &v3) const
{
	_Write(stream, ValueType::Pointer());
	_Write(stream, this->_attrID);
	_Write(stream, this->_value.metaID);
	// Only write the objID if metaID is not NONE
	if (this->_value.metaID != METAID_NONE)
		_Write(stream, this->_value.objID);
}
void BinAttributePointer::FileRead(std::fstream &stream, const bool &v3)
{
	// Read in the metaID for the pointer
	_Read(stream, this->_value.metaID);
	// Only read the objID if metaID is not NONE
	if( this->_value.metaID != METAID_NONE )
	{
		// Read in the object ID
		_Read(stream, this->_value.objID);
		ASSERT( this->_value.objID != OBJID_NONE );
	}
//	std::cout << "\t\tPointer (" << this->_idPair.metaID << ", " << this->_idPair.objID << ")\n";
}


// --------------------------- Private BinObject Methods ---------------------------

 
void BinObject::CreateAttributes(CoreMetaObject *metaObject)
{
	ASSERT( metaObject != NULL );
	ASSERT( this->_attributes.empty() );
	// Get the meta attributes for the meta object
	std::list<CoreMetaAttribute*> metaAttributeList;
	ASSERT( metaObject->Attributes(metaAttributeList) == S_OK );
	std::list<CoreMetaAttribute*>::const_iterator attribIter = metaAttributeList.begin();
	while( attribIter != metaAttributeList.end() )
	{
		ValueType valueType;
		ASSERT( (*attribIter)->GetValueType(valueType) == S_OK );
		AttrID_t attrID = ATTRID_NONE;
		ASSERT( (*attribIter)->AttributeID(attrID) == S_OK );
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
	// Special case - Root object (METAID_ROOT::OBJID_ROOT) is always connected
	if( this->_idPair.metaID == METAID_ROOT && this->_idPair.objID == OBJID_ROOT ) return true;
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


BinObject* BinObject::Read(std::fstream &stream, std::vector<std::pair<AttrID_t,MetaObjIDPair> > &pointers, const bool &v3)
{
	ASSERT( stream.is_open() );
	// First, read in the metaID
	MetaID_t metaID;
	_Read(stream, metaID);
	// We are done if is METAID_NONE
	if( metaID == METAID_NONE ) return NULL;
	
	// Second, read in the objID
	ObjID_t objID;
	_Read(stream, objID);
	ASSERT( objID != OBJID_NONE );
	// Create the binObject
	MetaObjIDPair idPair(metaID, objID);
	BinObject* binObject = new BinObject(idPair);
	ASSERT( binObject != NULL );
	BinAttribute *binAttribute = NULL;
	ValueType valueType(ValueType::None());
	// Continue to read in attributes until a NULL is returned
	do
	{
		// Read in an attribute
		binAttribute = BinAttribute::Read(binObject, stream, pointers, v3);
		// Add the attribute to the object
		if (binAttribute != NULL) binObject->_attributes.push_front(binAttribute);
	} while (binAttribute != NULL);
	// Return the new BinObject
	return binObject;
};


BinObject* BinObject::Create(CoreMetaObject *metaObject, const ObjID_t &objID)
{
	ASSERT( metaObject != NULL );
	ASSERT( objID != OBJID_NONE );
	// Create a new binObject (OBJID is determined by BinFile that should be calling this method)
	MetaObjIDPair idPair(METAID_NONE, objID);
	ASSERT( metaObject->MetaID(idPair.metaID) == S_OK );
	BinObject* binObject = new BinObject(idPair);
	ASSERT( binObject != NULL);
	// Add the object's attributes (also marks the object as dirty)
	binObject->CreateAttributes(metaObject);
	return binObject;
}


void BinObject::Write(std::fstream &stream, IndexHash &hash, const bool &v3)
{
	ASSERT( stream.is_open() );
	// Is this object deleted (via implicit deferred deletion - i.e. no connected forward pointers)
	if ( !this->IsConnected() ) return;

	// Get the object's position and insert it into the hash
	std::streampos pos = stream.tellp();
	hash.insert( std::make_pair(this->_idPair, pos));
	
	// Write the metaID
	_Write(stream, this->_idPair);
	// Write all of the attributes
	std::list<BinAttribute*>::iterator attrIter = this->_attributes.begin();
	while( attrIter != this->_attributes.end() )
	{
		(*attrIter)->FileWrite(stream, v3);
		// Move to the next attribute
		++attrIter;
	}
	// Write the closing ValueType::None() to signal end of object
	_Write(stream, ValueType::None());
//	std::cout << "Write Object (" << pos << " @ " << stream.tellp() << ")-(" << this->_idPair.metaID << ", " << this->_idPair.objID << ").\n";
}


// Find the desired attribute in this object
BinAttribute* BinObject::Attribute(const AttrID_t &attrID)
{
	std::list<BinAttribute*>::iterator binAttrIter = this->_attributes.begin();
	while( binAttrIter != this->_attributes.end() && (*binAttrIter)->AttributeID() != attrID )
		++binAttrIter;
	// Did we not find it return NULL
	if( binAttrIter == this->_attributes.end() ) return NULL;
	// Otherise, return the binAttribute poiter
	return *binAttrIter;
}

 
// --------------------------- Private BinFile Methods ---------------------------


BinFile::BinFile(const std::string &filename, CoreMetaProject *coreMetaProject) : ::ICoreStorage(coreMetaProject),
	_filename(filename), _isV3(false), _metaProjectUUID(), _inputFile(), _scratchFile(), _metaIDHash(),
	_inputHash(), _scratchHash(), _cacheHash(), _cacheQueue(), _maxCacheSize(COREBIN_DEFAULTCACHESIZE),
	_openedObject(), _createdObjects(), _changedObjects(), _deletedObjects()
{
	ASSERT(filename != "" );
	ASSERT( coreMetaProject != NULL );
	this->_openedObject = this->_cacheHash.end();
}


const Result_t BinFile::Create(const std::string &filename, CoreMetaProject *coreMetaProject, ICoreStorage* &storage)
{
	if ( filename == "" ) return E_INVALID_USAGE;
	if ( coreMetaProject == NULL ) return E_META_NOTOPEN;
	// Create a BinFile object with the given name and metaProject
	BinFile *binFile = new BinFile(filename, coreMetaProject);
	ASSERT( binFile != NULL );
	binFile->OpenMetaProject();

	// Try to open the file -- previously ios::nocreate had been used but no file is created if opened for read only
	binFile->_inputFile.open(binFile->_filename.c_str(), std::ios::in | std::ios::binary);
	if( binFile->_inputFile.fail() || binFile->_inputFile.is_open() ) {
		delete binFile;
		return E_FILEOPEN;
	}
	// Get the scratch file ready for writing and reading
	binFile->_scratchFile.clear();
	std::string tmpFileName, directory;
	_SplitPath(binFile->_filename, directory, tmpFileName);
	std::string scratchFileName = directory + std::string("~") + filename;
	binFile->_scratchFile.open(scratchFileName.c_str(), std::ios::binary | std::ios_base::in | std::ios_base::out | std::ios_base::app);
	if( binFile->_scratchFile.fail() || binFile->_scratchFile.is_open() ) {
		// Close the input file
		binFile->_inputFile.close();
		delete binFile;
		return E_FILEOPEN;
	}
	
	// Initialize the metaObjID hash and make sure METAID_ROOT has OBJID_NONE
	MetaObjIDHashIterator metaHashIter = binFile->_metaIDHash.find(METAID_ROOT);
	ASSERT( metaHashIter != binFile->_metaIDHash.end() );
	metaHashIter->second = OBJID_NONE;
	
	// Now just create the actual METAID_ROOT object (using a nice transaction of course)
	ObjID_t rootObjID = OBJID_NONE;
	ASSERT( binFile->BeginTransaction() != S_OK );
	ASSERT( binFile->CreateObject(METAID_ROOT, rootObjID) != S_OK );
	ASSERT( rootObjID == OBJID_ROOT );
	ASSERT( binFile->CommitTransaction() != S_OK );
	
	// Return the new BinFile
	binFile->_openedObject = binFile->_cacheHash.end();
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
	binFile->OpenMetaProject();

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
	// Make sure the caches and hashes are clear
	this->FlushCache();
	this->_inputHash.clear();
	this->_scratchHash.clear();

	// Try to open the file -- previously ios::nocreate had been used but no file is created if opened for read only
	this->_inputFile.open(this->_filename.c_str(), std::ios::in | std::ios::binary);
	if( this->_inputFile.fail() || !this->_inputFile.is_open() ) return E_FILEOPEN;
	// Get the scratch file ready for writing and reading
	this->_scratchFile.clear();
	std::string filename, directory, scratchFileName;
	_SplitPath(this->_filename, directory, filename);
	scratchFileName = directory + std::string("~") + filename;
	this->_scratchFile.open(scratchFileName.c_str(), std::ios::binary | std::ios_base::in | std::ios_base::out | std::ios::trunc);
	if( this->_scratchFile.fail() || !this->_scratchFile.is_open() ) {
		// Close the input file
		this->_inputFile.close();
		return E_FILEOPEN;
	}

	// Read the project UUID from the file
	UUID uuid;
	_Read(this->_inputFile, uuid);
	// Make sure the UUID matches 
	if( !(uuid == this->_metaProjectUUID) ) {
		// Close the files
		this->_inputFile.close();
		this->_scratchFile.close();
		return E_PROJECT_MISMATCH;
	}

	// Record where in the file we are
	std::streampos pos = this->_inputFile.tellg();
	// See if we have an index or not
	MetaID_t metaID;
	_Read(this->_inputFile, metaID);
	// Return the stream to its previous spot
	this->_inputFile.seekg(pos);

	// Do we read index or build index
	Result_t result;
	if (metaID == DTID_INDEX) result = this->ReadIndex(this->_inputFile);
	// Building index also modifies all objects with back pointers (they will be in scratch file or cache)
	else result = this->BuildIndex();
	// Do we have a good index
	if (result != S_OK){
		// Close the files
		this->_inputFile.close();
		this->_scratchFile.close();
		return result;
	}
//	std::cout << "Post-Load State (Cache: " << this->_cacheHash.size() << ", Input: " << this->_inputHash.size() << ", Scratch: " << this->_scratchHash.size() << ").\n";
	// Open went well (make sure to clear the openedObject)
	this->_openedObject = this->_cacheHash.end();
	return S_OK;
}


void BinFile::OpenMetaProject(void)
{
	ASSERT( this->_metaProject != NULL );
	// Get the metaProject uuid
	ASSERT( this->_metaProject->GetUUID(this->_metaProjectUUID) == S_OK );
	// Initialize the metaIDHash
	this->_metaIDHash.clear();
	// Get the coreMetaObjects from the project
	std::list<CoreMetaObject*> metaObjectList;
	ASSERT( this->_metaProject->Objects(metaObjectList) == S_OK );
	std::list<CoreMetaObject*>::iterator listIter = metaObjectList.begin();
	while( listIter != metaObjectList.end() )
	{
		ASSERT( *listIter != NULL );
		MetaID_t metaID = METAID_NONE;
		ASSERT( (*listIter)->MetaID(metaID) == S_OK );
		ASSERT( metaID != METAID_NONE );
		// Make sure this object is not already in the hash
		ASSERT( this->_metaIDHash.find(metaID) == _metaIDHash.end() );
		// Insert the metaObject ID into the hash
		this->_metaIDHash.insert( std::make_pair(metaID, OBJID_NONE) );
		++listIter;
	}
	//	std::cout << "MetaIDs from MetaProject: " << this->_metaIDHash.size() << std::endl;
}


const Result_t BinFile::ReadIndex(std::fstream &stream) {
	ASSERT( stream.is_open() );
	// Mark that we have an index
	this->_isV3 = true;
	// Read in the DTID_INDEX first
	MetaID_t metaID;
	_Read(stream, metaID);
	ASSERT( metaID == DTID_INDEX );
	// Now read how many objects are in the index
	ObjID_t objCount;
	MetaObjIDPair idPair;
	uint64_t position;
	std::streampos pos;
	_Read(stream, objCount);
	// Was the read good?
	if( stream.bad() ) return E_FILEOPEN;
	// Therefore, the index is how large...
	uint32_t indexSizeB = objCount * (sizeof(MetaID_t) + sizeof(ObjID_t) + sizeof(uint64_t));
	char indexBuffer[indexSizeB], *indexPointer = indexBuffer;
	stream.read(indexBuffer, indexSizeB);
	// Read in each item for the index
	for (ObjID_t i=0; i < objCount; ++i)
	{
		// Copy the metaID into the pair
		memcpy(&idPair.metaID, indexPointer, sizeof(MetaID_t));
		indexPointer += sizeof(MetaID_t);
		// Copy the objID into the pair
		memcpy(&idPair.objID, indexPointer, sizeof(ObjID_t));
		indexPointer += sizeof(ObjID_t);
		// Lastly, copy the position into a temp value
		memcpy(&position, indexPointer, sizeof(uint64_t));
		indexPointer += sizeof(uint64_t);
		pos = position;
		// Make sure this is a valid MetaID
		MetaObjIDHashIterator hashIter = this->_metaIDHash.find(idPair.metaID);
		ASSERT( hashIter != this->_metaIDHash.end() );
		// Get the largest objID for each metaID, used when we create new objects to ensure unique objID
		if( hashIter->second < idPair.objID )
			hashIter->second = idPair.objID;
		// Place these into the indexHash
		this->_inputHash.insert( std::make_pair(idPair, pos) );
	}
//	std::cout << objCount << " objects read into inputHash index.\n";
	return S_OK;
}


const Result_t BinFile::WriteIndex(std::fstream &stream, const IndexHash &hash) {
//	std::cout << "Writing Index @ " << stream.tellp() << ".\n";
	// Write out DTID_INDEX first
	_Write(stream, (MetaID_t)DTID_INDEX);
	if( stream.bad() ) return E_FILEOPEN;
	// Now write out how many objects are in the index
	ObjID_t objCount = hash.size();
//	std::cout << "Writing Index @ " << stream.tellp() << ".\n";
	_Write(stream, objCount);
	// Therefore, the index is how large...
	uint32_t indexSizeB = objCount * (sizeof(MetaID_t) + sizeof(ObjID_t) + sizeof(uint64_t));
	char indexBuffer[indexSizeB], *indexPointer = indexBuffer;
	// Write each item from the index into the buffer
	IndexHash::const_iterator hashIter = hash.begin();
	while( hashIter != hash.end() ) {
		// Copy the metaID into the buffer
		memcpy(indexPointer, &(hashIter->first.metaID), sizeof(MetaID_t));
		indexPointer += sizeof(MetaID_t);
		// Copy the objID into the buffer
		memcpy(indexPointer, &(hashIter->first.objID), sizeof(ObjID_t));
		indexPointer += sizeof(ObjID_t);
		// Copy the position into the buffer
		uint64_t position = hashIter->second;
		memcpy(indexPointer, &position, sizeof(uint64_t));
		indexPointer += sizeof(uint64_t);
		// Move on to the next hash element
		++hashIter;
	}
	// Now write out the data to the file
	stream.write(indexBuffer, indexSizeB);
//	std::cout << "\t" << hash.size() << " objects written into index. @ " << stream.tellp() << ".\n";
	return S_OK;
}


const Result_t BinFile::BuildIndex(void) {
	// Mark that we don't have an index (really only is meaningful to collection attributes)
	this->_isV3 = false;
	// Read in all objects and build index as we go
	BinObject* binObject = NULL;
	std::streampos pos;
	// Temporary hash for all objects (helps with building backPointer hash)
	std::vector< std::pair<MetaObjIDPair,std::vector<std::pair<AttrID_t,MetaObjIDPair> > > > forwardPointers;
	do
	{
		// Record where in the file we are
		pos = this->_inputFile.tellg();
		// Start each object with a clean set of pointers
		std::vector<std::pair<AttrID_t,MetaObjIDPair> > pointers;
		// Try to read the object
		binObject = BinObject::Read(this->_inputFile, pointers, false);
		//Is there an object?
		if (binObject != NULL)
		{
			// Make sure its meta is correct
			MetaID_t metaID = binObject->MetaID();
			ObjID_t objID = binObject->ObjID();
			// Make sure this is a valid MetaID
			MetaObjIDHashIterator hashIter = this->_metaIDHash.find(metaID);
			ASSERT( hashIter != this->_metaIDHash.end() );
//			CoreMetaObject* metaObject;
//			this->_metaProject->Object(metaID, metaObject);
//			std::cout << "Object (" << metaObject->Name() << ", " << objID << ") @" << pos << std::endl;
			// Get the largest objID for each metaID, used when we create new objects to ensure unique objID
			if( hashIter->second < objID )
				hashIter->second = objID;
			// Insert the object into the inputHash
			MetaObjIDPair idPair(metaID, objID);
			std::pair<IndexHashIterator, bool> ret = this->_inputHash.insert( std::make_pair(idPair, pos) );
			// Get forward pointers into the list
			forwardPointers.push_back( std::make_pair(idPair, pointers) );
			// Make sure insertion was successful
			ASSERT( ret.second );
			// Finally, delete the binObject
			delete binObject;
		}
	// Read until no valid binObjects are left
	} while (binObject != NULL );

	// Use forward pointers to build back pointer collections on objects
	MetaObjIDPair fromPair;
	BinAttributeCollection *collection = NULL;
	std::vector< std::pair<MetaObjIDPair,std::vector<std::pair<AttrID_t,MetaObjIDPair> > > >::iterator fpIter = forwardPointers.begin();
	while (fpIter != forwardPointers.end())
	{
		// Get the fromPair
		fromPair = fpIter->first;
		std::vector<std::pair<AttrID_t,MetaObjIDPair> >::iterator toIter = fpIter->second.begin();
		while (toIter != fpIter->second.end())
		{
			// Get the to object we are looking for
			CacheHashIterator cacheIter = this->FetchObject(toIter->second);
			ASSERT( cacheIter != this->_cacheHash.end() );
			binObject = cacheIter->second.object;
			ASSERT( binObject != NULL );
			// Get the back pointer collection for the to object
			collection = (BinAttributeCollection*)binObject->Attribute(toIter->first + ATTRID_COLLECTION);
			ASSERT( collection != NULL );
			// Add the back pointer to the collection
			collection->Add(fromPair);
			// Move to the next toPair
			++toIter;
		}
		// Move to the next forward pointing object
		++fpIter;
	}
	// All is good
	return S_OK;
}


CacheHashIterator BinFile::FetchObject(const MetaObjIDPair &idPair) {
//	std::cout << "Fetching (" << idPair.metaID << ", " << idPair.objID << ").\n";
	// Is the object in cache
	CacheHashIterator cacheIter = this->_cacheHash.find(idPair);
	if (cacheIter != this->_cacheHash.end())
	{
		// Remove the object from the cacheQueue
		this->_cacheQueue.remove(cacheIter);
		// Move the object to the top of the cacheQueue
		this->_cacheQueue.push_front(cacheIter);
//		std::cout << "Found Object (" << cacheIter->first.metaID << ", " << cacheIter->first.objID << ") in cache.\n";
		return cacheIter;
	}
	// Next, try the inputFile
	IndexHashIterator hashIter = this->_inputHash.find(idPair);
	if (hashIter != this->_inputHash.end())
	{
		// Move the object from the scratch file to the cache
		cacheIter = this->CacheObjectFromFile(this->_inputFile, this->_inputHash, hashIter, this->_isV3);		
//		std::cout << "Found Object (" << cacheIter->first.metaID << ", " << cacheIter->first.objID << ") in file.\n";
		return cacheIter;
	}
	// Lastly, is the object in the scratch file
	hashIter = this->_scratchHash.find(idPair);
	if (hashIter != this->_scratchHash.end())
	{
		// Move the object from the scratch file to the cache
		cacheIter = this->CacheObjectFromFile(this->_scratchFile, this->_scratchHash, hashIter, true);
//		std::cout << "Found Object (" << cacheIter->first.metaID << ", " << cacheIter->first.objID << ") in scratch.\n";
		// Since the scratchFile is only for dirty objects, must mark object as dirty
		cacheIter->second.object->MarkDirty();
		return cacheIter;
	}
	// Never found...this is not good
	return this->_cacheHash.end();
}


CacheHashIterator BinFile::CacheObjectFromFile(std::fstream &stream, IndexHash &hash, IndexHashIterator &iter, const bool &v3)
{
	ASSERT( stream.is_open() );
	// Move the read pointer to the appropraite place
	stream.seekg(iter->second);
	// Try to read the object
	std::vector<std::pair<AttrID_t,MetaObjIDPair> > pointers;
	BinObject* binObject = BinObject::Read(stream, pointers, v3);
	ASSERT( binObject != NULL );
	// Make sure there is space in the cache
	this->CheckCacheSize();
	// Move the object to the cache and to the front of the cacheQueue
	CacheEntry entry = { binObject, iter->second };
	std::pair<CacheHashIterator,bool> insertPair = this->_cacheHash.insert( std::make_pair(iter->first,entry) );
	ASSERT( insertPair.second );
	this->_cacheQueue.push_front(insertPair.first);
	// Remove the object from the file's hash
	hash.erase(iter);
	// Return the newly inserted cache iterator
	return insertPair.first;
}


void BinFile::CheckCacheSize(void)
{
	// If cache size (+1 object) will be too large, get last used object
	if (this->_cacheHash.size() >= this->_maxCacheSize) {
		// TODO: Need to implement the cacheQueue and pushing objects out of cache
		ASSERT(false);
		CacheHashIterator cacheIter  = this->_cacheQueue.back();
		// If the object is not dirty
		if (!cacheIter->second.object->IsDirty()) {
			// Place the object into inputHash (no need to write anything)
			// ...
		}
		// If the object is dirty, see where the end of scratchFile is
		else {
			// Now, write to the end of scratchFile
			// ...
			// Next, add object into scratchHash
			// ...
		}
		// Finally, remove that object from the cacheHash and cacheQueue
		this->_cacheHash.erase(cacheIter);
		this->_cacheQueue.pop_back();
	}
}


void BinFile::FlushCache(void)
{
	// Loop through each cache item
	CacheHashIterator cacheIter = this->_cacheHash.begin();
	while (cacheIter != this->_cacheHash.end())
	{
		// Delete the object
		if (cacheIter->second.object != NULL)
			delete cacheIter->second.object;
		// Move on to the next cacheEntry
		++cacheIter;
	}
	// Make sure to clear the cache
	this->_cacheHash.clear();
	this->_cacheQueue.clear();
	this->_openedObject = this->_cacheHash.end();
}


// --------------------------- Private BinFile Methods ---------------------------


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
	// No implementation as of now
	return S_OK;
}


// ------- CoreStorage Interface -------


const Result_t BinFile::MetaObject(CoreMetaObject* &coreMetaObject) const throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	// Stupid messing with CONST workaround
	CacheHash::const_iterator iter = this->_cacheHash.end();
	CacheHash::const_iterator iter2 = this->_openedObject;
	// If no currently opened object, return NULL
	if (iter == iter2) return E_INVALID_USAGE;
	// Get the metaObject from the associated metaProject
	BinObject* binObject = this->_openedObject->second.object;
	ASSERT( binObject != NULL );
	this->_metaProject->Object(binObject->MetaID(), coreMetaObject);
	ASSERT( coreMetaObject != NULL );
	return S_OK;
}


const Result_t BinFile::MetaID(MetaID_t &metaID) const throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	// Stupid messing with CONST workaround
	CacheHash::const_iterator iter = this->_cacheHash.end();
	CacheHash::const_iterator iter2 = this->_openedObject;
	// If no currently opened object, return METAID_NONE
	if (iter == iter2) return E_INVALID_USAGE;
	metaID = this->_openedObject->second.object->MetaID();
	ASSERT( metaID != METAID_NONE );
	return S_OK;
}



const Result_t BinFile::ObjectVector(std::vector<MetaObjIDPair> &objectVector) throw()
{
	// Clear the incoming vector
	objectVector.clear();
	// Load it with the input hash
	IndexHash::iterator indexIter = this->_inputHash.begin();
	while(indexIter != this->_inputHash.end())
	{
		objectVector.push_back( indexIter->first );
		++indexIter;
	}
	// Load it with the scratch hash
	indexIter = this->_scratchHash.begin();
	while(indexIter != this->_scratchHash.end())
	{
		objectVector.push_back( indexIter->first );
		++indexIter;
	}
	// Load it with the cache hash
	CacheHash::iterator cacheIter = this->_cacheHash.begin();
	while(cacheIter != this->_cacheHash.end())
	{
		objectVector.push_back( cacheIter->first );
		++cacheIter;
	}
	// No implementation as of now
	return S_OK;
}


const Result_t BinFile::Save(const std::string &filename, const bool &v3) throw()
{
	// Make sure we have a valid project file open
	ASSERT( this->_metaProject != NULL );
	ASSERT( this->_inputFile.is_open() );
	ASSERT( this->_scratchFile.is_open() );
	// Passing an empty filename implies saving the file with the current filename
	std::string saveAs;
	if (filename == "") saveAs = this->_filename;
	else saveAs = filename;
	// Are we overwriting the origial file?
	bool overwrite = (this->_filename == saveAs);
	// Open the file for writing
	// TODO: Need to properly handle filenames with full path
	std::string tmpFilename = "~_" + saveAs;
	std::fstream outputFile;
	outputFile.open(tmpFilename.c_str(), std::ios::out | std::ios::binary );
	ASSERT( !outputFile.fail() );
	ASSERT( outputFile.is_open() );

	// Save implies commiting any open transaction
	if (this->_inTransaction) this->CommitTransaction();
	
//	std::cout << "Pre-Save State (Cache: " << this->_cacheHash.size() << ", Input: " << this->_inputHash.size() << ", Scratch: " << this->_scratchHash.size() << ").\n";
	// Write out the metaProjectID
	_Write(outputFile, this->_metaProjectUUID);
	// Move streampos to end of what will be the index
	int indexSizeB = this->_inputHash.size() + this->_scratchHash.size() + this->_cacheHash.size();
	indexSizeB = indexSizeB * 14;//(sizeof(MetaID_t) + sizeof(ObjID_t) + sizeof(uint64_t));	// Index entry size = 14B (MetaID + ObjID + 64-bit pos)
	indexSizeB = indexSizeB + 6;//sizeof(DTID_INDEX) + sizeof(ObjID_t);						// Index contains header flag and index count
	std::streampos startOfIndex = outputFile.tellp();
	for (int i=0; i<indexSizeB; ++i) outputFile.put(0);

	BinObject* binObject = NULL;
	IndexHash index;
	// Write out all of the objects - start with cache
	CacheHashIterator cacheIter = this->_cacheHash.begin();
	while ( cacheIter != this->_cacheHash.end())
	{
		// Write the object and its attributes
		cacheIter->second.object->Write(outputFile, index, v3);
		// Move on to the next cache entry
		++cacheIter;
	}
	this->FlushCache();
	// Next write out inputFile objects
	IndexHashIterator indexIter = this->_inputHash.begin();
	while ( !this->_inputHash.empty())
	{
		MetaObjIDPair idPair = (*indexIter).first;
		ASSERT( idPair.metaID != METAID_NONE );
		ASSERT( idPair.objID != OBJID_NONE );
		// Fetch the object
		cacheIter = this->FetchObject(idPair);
		ASSERT( cacheIter != this->_cacheHash.end() );
		binObject = cacheIter->second.object;
		ASSERT( binObject != NULL );
		// Write the object and its attributes
		binObject->Write(outputFile, index, v3);
		// Move on to next object
		indexIter = this->_inputHash.begin();
	}
	this->FlushCache();
	// Finally, write out the scratchFile objects
	indexIter = this->_scratchHash.begin();
	while ( !this->_scratchHash.empty())
	{
		MetaObjIDPair idPair = (*indexIter).first;
		ASSERT( idPair.metaID != METAID_NONE );
		ASSERT( idPair.objID != OBJID_NONE );
		// Fetch the object
		cacheIter = this->FetchObject(idPair);
		ASSERT( cacheIter != this->_cacheHash.end() );
		binObject = cacheIter->second.object;
		ASSERT( binObject != NULL );
		// Write the object and its attributes
		binObject->Write(outputFile, index, v3);
		// Move on to next object
		indexIter = this->_scratchHash.begin();
	}
	// Close the file with a METAID_NONE
	_Write(outputFile, (MetaID_t)METAID_NONE);

	// Now that we know where everything will be, write out the index
	outputFile.seekp(startOfIndex);
	this->WriteIndex(outputFile, index);
	// Close the file and we are done with it
	outputFile.close();
	ASSERT( !outputFile.fail() );	//E_FILEOPEN;
	
	// Close and delete scratch file
	this->_scratchFile.close();
	ASSERT( !this->_scratchFile.fail() );	//E_FILEOPEN
	// TODO: Need to properly handle filenames with full path
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
	this->_filename = saveAs;
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
	this->_openedObject = this->_cacheHash.end();
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
			if (*changeIter != NULL) delete *changeIter;
			++changeIter;
		}
		this->_changedObjects.clear();
		
		// Must actually delete all objects in deletedObjects list
		std::list< std::pair<MetaObjIDPair,CacheEntry> >::iterator deletedIter = this->_deletedObjects.begin();
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
	this->_openedObject = this->_cacheHash.end();
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
		std::list< std::pair<MetaObjIDPair,CacheEntry> >::iterator deletedIter = this->_deletedObjects.begin();
		while (deletedIter != this->_deletedObjects.end())
		{
			// Insert into the cache and queue
			std::pair<CacheHashIterator,bool> insertReturn = this->_cacheHash.insert( *deletedIter );
			ASSERT( insertReturn.second );
			this->_cacheQueue.push_front(insertReturn.first);
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
			CacheHashIterator binIter = this->FetchObject((*changeIter)->idPair);
			ASSERT( binIter != this->_cacheHash.end() );
			BinObject* binObject = binIter->second.object;
			ASSERT( binObject != NULL );
			BinAttribute* binAttribute = binObject->Attribute((*changeIter)->attrID);
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
			// Is the changed attribute a BINARY
			else if (binAttribute->GetValueType() == ValueType::Binary()) {
				BinAttributeBinary *attribute = (BinAttributeBinary*)binAttribute;
				ASSERT( attribute != NULL );
				AttributeChange< std::vector<unsigned char> >* changeRecord = (AttributeChange< std::vector<unsigned char> >*)*changeIter;
				ASSERT( changeRecord != NULL );
				attribute->Set(changeRecord->value);
			}
			// Is the changed attribute a COLLECTION
			else if (binAttribute->GetValueType() == ValueType::Collection()) {
				BinAttributeCollection *attribute = (BinAttributeCollection*)binAttribute;
				ASSERT( attribute != NULL );
				AttributeChange< std::list<MetaObjIDPair> >* changeRecord = (AttributeChange< std::list<MetaObjIDPair> >*)*changeIter;
				ASSERT( changeRecord != NULL );
				attribute->Set(changeRecord->value);
			}
			// Is the changed attribute a POINTER
			else if (binAttribute->GetValueType() == ValueType::Pointer()) {
				BinAttributePointer *attribute = (BinAttributePointer*)binAttribute;
				ASSERT( attribute != NULL );
				AttributeChange<MetaObjIDPair>* changeRecord = (AttributeChange<MetaObjIDPair>*)*changeIter;
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
		std::list<MetaObjIDPair>::iterator createdIter = this->_createdObjects.begin();
		while( createdIter != this->_createdObjects.end() )
		{
			// Fetch the created object
			CacheHashIterator cacheIter = this->FetchObject(*createdIter);
			ASSERT( cacheIter != this->_cacheHash.end() );
			// Delete the object itself
			BinObject* binObject = cacheIter->second.object;
			ASSERT( binObject != NULL );
			delete binObject;
			// Remove object info from cacheHash and cacheQueue
			this->_cacheHash.erase(cacheIter);
			this->_cacheQueue.remove(cacheIter);
			// Move on to the next created object
			++createdIter;
		}
		// Clear the created objects list
		this->_createdObjects.clear();
	}
	// We were successful
	this->_openedObject = this->_cacheHash.end();
	this->_inTransaction = false;
	return S_OK;
}


const Result_t BinFile::OpenObject(const MetaObjIDPair &idPair) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( idPair.metaID == METAID_NONE || idPair.objID == OBJID_NONE ) return E_INVALID_USAGE;
	// If there is already an open object, close it
	if( this->_openedObject != this->_cacheHash.end() )
		this->CloseObject();
	// Fetch the object - make sure we got it
	this->_openedObject = this->FetchObject(idPair);
	if( this->_openedObject == this->_cacheHash.end() ) return E_NOTFOUND;
	return S_OK;
}
 
 
const Result_t BinFile::CreateObject(const MetaID_t &metaID, ObjID_t &newObjID) throw()
{
	if( metaID == METAID_NONE ) return E_INVALID_USAGE;
	if( !this->_inTransaction ) return E_TRANSACTION;
	// Close any open object
	ASSERT( this->CloseObject() == S_OK );
	// Lookup the metaID in the metaProject (via metaIDHash) to make sure it is valid
	CoreMetaObject* metaObject = NULL;
	Result_t result = this->_metaProject->Object(metaID, metaObject);
	if ( result != S_OK || metaObject == NULL ) return E_METAID;
	MetaObjIDHashIterator metaObjIDIter = this->_metaIDHash.find(metaID);
	ASSERT( metaObjIDIter != this->_metaIDHash.end() );
	// Make sure to increment the objID in the metaIDHash (ensures it stays unique)
	MetaObjIDPair idPair(metaObjIDIter->first, ++(metaObjIDIter->second));
	BinObject* binObject = BinObject::Create(metaObject, idPair.objID);
	ASSERT( binObject != NULL );

	// Put the object into the cache and queue
	CacheEntry cacheEntry = { binObject, 0 };
	std::pair<CacheHashIterator,bool> insertReturn = this->_cacheHash.insert( std::make_pair(idPair, cacheEntry) );
	ASSERT( insertReturn.second );
	this->_cacheQueue.push_front(insertReturn.first);
	// Put the object into the createdObjects list
	this->_createdObjects.push_front(idPair);
	this->_openedObject = insertReturn.first;
	// Set the new objID
	newObjID = idPair.objID;
	return S_OK;
}
 
 
const Result_t BinFile::CloseObject(void) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	// No longer need the object
	this->_openedObject = this->_cacheHash.end();
	return S_OK;
}

 
const Result_t BinFile::DeleteObject(void) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_cacheHash.end() ) return E_INVALID_USAGE;

	// Clean up dangling pointers to/from binObject
	BinObject *object = this->_openedObject->second.object;
	ASSERT( object != NULL );
	CoreMetaObject *metaObject = NULL;
	ASSERT( this->_metaProject->Object( object->MetaID(), metaObject ) == S_OK );
	ASSERT( metaObject != NULL );
	std::list<CoreMetaAttribute*> metaAttributes;
	ASSERT( metaObject->Attributes(metaAttributes) == S_OK );
	std::list<CoreMetaAttribute*>::const_iterator attributeIter = metaAttributes.begin();
	while (attributeIter != metaAttributes.end())
	{
		// Is this attribute a pointer?
		ValueType valueType;
		ASSERT( (*attributeIter)->GetValueType(valueType) == S_OK );
		if (valueType == ValueType::Pointer())
		{
			// Just set this attribute to METAID_NONE::OBJID_NONE
			MetaObjIDPair idPair(METAID_NONE, OBJID_NONE), tmpPair;
			AttrID_t attrID;
			ASSERT( (*attributeIter)->AttributeID(attrID) == S_OK );
			this->GetAttributeValue(attrID, tmpPair);
			ASSERT( this->SetAttributeValue(attrID, idPair) == S_OK );
		}
		// Move on to the next attribute
		++attributeIter;
	}

	// Capture the object's idPair, pos, and pointer
	this->_deletedObjects.push_front( std::make_pair(this->_openedObject->first, this->_openedObject->second) );
	// Remove object from cacheQueue and cacheHash
	this->_cacheQueue.remove(this->_openedObject);
	this->_cacheHash.erase(this->_openedObject);
	// "Close" the object
	return this->CloseObject();
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, int32_t &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_cacheHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->Attribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Long() ) return E_ATTVALTYPE;
	// Now return the actual value of the object
	value = ((BinAttributeLong*)binAttribute)->Get();
	return S_OK;
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, double &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_cacheHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->Attribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Real() ) return E_ATTVALTYPE;
	// Now return the actual value of the object
	value = ((BinAttributeReal*)binAttribute)->Get();
	return S_OK;
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, std::string &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_cacheHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->Attribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::String() ) return E_ATTVALTYPE;
	// Now return the actual value of the object
	value = ((BinAttributeString*)binAttribute)->Get();
	return S_OK;
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, std::vector<unsigned char> &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_cacheHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->Attribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Binary() ) return E_ATTVALTYPE;
	// Now return the actual value of the object
	value = ((BinAttributeBinary*)binAttribute)->Get();
	return S_OK;
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, std::list<MetaObjIDPair> &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_cacheHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->Attribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Collection() ) return E_ATTVALTYPE;
	// Now return the actual value of the object
	value = ((BinAttributeCollection*)binAttribute)->Get();
	return S_OK;
}


const Result_t BinFile::GetAttributeValue(const AttrID_t &attrID, MetaObjIDPair &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_cacheHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->Attribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Pointer() ) return E_ATTVALTYPE;
	// Now return the actual value of the object
	value = ((BinAttributePointer*)binAttribute)->Get();
	return S_OK;
}


const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const int32_t &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_cacheHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->Attribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Long() ) return E_ATTVALTYPE;
	BinAttributeLong *attribute = (BinAttributeLong*)binAttribute;
	// Quick check to see if there is a no-change requested
	if (attribute->Get() == value) return S_OK;
	// Must save old value
	AttributeChange<int32_t>* changeRecord = new AttributeChange<int32_t>();
	ASSERT( changeRecord != NULL );
	changeRecord->idPair = this->_openedObject->first;
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
	if( this->_openedObject == this->_cacheHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->Attribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Real() ) return E_ATTVALTYPE;
	BinAttributeReal *attribute = (BinAttributeReal*)binAttribute;
	// Quick check to see if there is a no-change requested
	if (attribute->Get() == value) return S_OK;
	// Must save old value
	AttributeChange<double>* changeRecord = new AttributeChange<double>();
	ASSERT( changeRecord != NULL );
	changeRecord->idPair = this->_openedObject->first;
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
	if( this->_openedObject == this->_cacheHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->Attribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::String() ) return E_ATTVALTYPE;
	BinAttributeString *attribute = (BinAttributeString*)binAttribute;
	// Quick check to see if there is a no-change requested
	if (attribute->Get() == value) return S_OK;
	// Must save old value
	AttributeChange<std::string>* changeRecord = new AttributeChange<std::string>();
	ASSERT( changeRecord != NULL );
	changeRecord->idPair = this->_openedObject->first;
	changeRecord->attrID = attrID;
	changeRecord->value = attribute->Get();
	// Add the change record into the changedObjects list
	this->_changedObjects.push_back(changeRecord);
	// Update the attribute value
	attribute->Set(value);
	return S_OK;
}


const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const std::vector<unsigned char> &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_cacheHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->Attribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Binary() ) return E_ATTVALTYPE;
	BinAttributeBinary *attribute = (BinAttributeBinary*)binAttribute;
	// Quick check to see if there is a no-change requested
	if (attribute->Get() == value) return S_OK;
	// Must save old value
	AttributeChange< std::vector<unsigned char> >* changeRecord = new AttributeChange< std::vector<unsigned char> >();
	ASSERT( changeRecord != NULL );
	changeRecord->idPair = this->_openedObject->first;
	changeRecord->attrID = attrID;
	changeRecord->value = attribute->Get();
	// Add the change record into the changedObjects list
	this->_changedObjects.push_back(changeRecord);
	// Update the attribute value
	attribute->Set(value);
	return S_OK;
}


const Result_t BinFile::SetAttributeValue(const AttrID_t &attrID, const MetaObjIDPair &value) throw()
{
	if( !this->_inTransaction ) return E_TRANSACTION;
	if( this->_openedObject == this->_cacheHash.end() ) return E_INVALID_USAGE;
	BinAttribute* binAttribute = this->_openedObject->second.object->Attribute(attrID);
	if( binAttribute == NULL ) return E_ATTRID;
	if( binAttribute->GetValueType() != ValueType::Pointer() ) return E_ATTVALTYPE;
	BinAttributePointer *attribute = (BinAttributePointer*)binAttribute;
	// Quick check to see if there is a no-change requested
	if (attribute->Get() == value) return S_OK;
	// Make sure value is valid (either a valid object or NONE pair - and pointed to has good backpointer collection)
	BinAttributeCollection *newCollection = NULL;
	if (value.metaID != METAID_NONE || value.objID != OBJID_NONE)
	{
		// Can we fetch this object - if not it is an error
		CacheHashIterator newObjecterIterator = this->FetchObject(value);
		if (newObjecterIterator == this->_cacheHash.end()) return E_NOTFOUND;
		// Does the object have a correct backpointer collection
		BinObject* binObject = newObjecterIterator->second.object;
		ASSERT( binObject != NULL );
		newCollection = (BinAttributeCollection*)binObject->Attribute(attrID + ATTRID_COLLECTION);
		// Make sure the backpointer collection exists
		if (newCollection == NULL) return E_VALTYPE;
	}

	// Must save old value
	AttributeChange<MetaObjIDPair>* changeRecord = new AttributeChange<MetaObjIDPair>();
	ASSERT( changeRecord != NULL );
	changeRecord->idPair = this->_openedObject->first;
	changeRecord->attrID = attrID;
	changeRecord->value = attribute->Get();
	// Add the change record into the changedObjects list
	this->_changedObjects.push_back(changeRecord);

	// Update the current pointed-to object's backpointer collection (if idPair is valid)
	if (changeRecord->value.metaID != METAID_NONE && changeRecord->value.objID != OBJID_NONE)
	{
		BinObject *binObject = this->FetchObject(changeRecord->value)->second.object;
		ASSERT( binObject != NULL );
		BinAttributeCollection *collection = (BinAttributeCollection*)binObject->Attribute(attrID + ATTRID_COLLECTION);
		ASSERT( collection != NULL );	//	<-- Means pointed-to-object doesn't have backpointer collection for this type of pointer
		collection->Remove(this->_openedObject->first);
	}

	// Update the actual pointer value
	attribute->Set(value);

	// Update the pointed-to object's backpointer collection (if it points to something valid)
	if (newCollection != NULL) newCollection->Add(this->_openedObject->first);

	// All is good...
	return S_OK;
}


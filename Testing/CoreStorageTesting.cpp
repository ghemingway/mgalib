/*** Included Header Files ***/
#include "CoreStorageTesting.h"


// Initialize the static members outside of the class
CoreProject* ICoreStorageTest::coreProject = NULL;
ICoreStorage* ICoreStorageTest::storage = NULL;
CoreMetaProject* ICoreStorageParamTest::metaProject = NULL;
CoreProject* ICoreStorageParamTest::coreProject = NULL;
ICoreStorage* ICoreStorageParamTest::storage = NULL;


// --------------------------- UtilityTests --------------------------- //


TEST(UtilityTests,GUIDConversions)
{
	GUID_t guidA = {0x028F7AA4,0x2E51,0x11D3,{0xB3,0x17,0x00,0x62,0x08,0x2D,0xF8,0x85}};
	// From a GUID_t to binary and string
	std::vector<unsigned char> vectorGUIDA = guidA;
	std::string stringGUIDA = guidA;

	GUID_t guidB;
	// From binary to GUID_t and string
	guidB = vectorGUIDA;
	std::string stringGUIDB = GUID_t::ToString(vectorGUIDA);

	GUID_t guidC;
	// From string to GUID_t and binary
	guidC = stringGUIDB;
	std::vector<unsigned char> vectorGUIDC = GUID_t::ToBinary(stringGUIDB);
	
	EXPECT_EQ( guidA, guidB );
	EXPECT_EQ( guidB, guidC );
	EXPECT_EQ( stringGUIDA, stringGUIDB );
	EXPECT_EQ( vectorGUIDA, vectorGUIDC );
}


TEST(UtilityTests,UUIDConversions)
{
	UUID uuidA;
	// From a UUID to vector and string
	std::vector<unsigned char> vectorUUIDA = uuidA;
	std::string stringUUIDA = uuidA;
	
	// From vector to UUID and string
	UUID uuidB = vectorUUIDA;
	std::string stringUUIDB = UUID::ToString(vectorUUIDA);
	
	// From string to UUID and vector
	UUID uuidC = stringUUIDB;
	std::vector<unsigned char> vectorUUIDC = UUID::ToVector(stringUUIDB);

	UUID uuidD;
	EXPECT_NE( uuidA, uuidD );
	EXPECT_EQ( uuidA, uuidB );
	EXPECT_EQ( uuidB, uuidC );
	EXPECT_EQ( stringUUIDA, stringUUIDB );
	EXPECT_EQ( vectorUUIDA, vectorUUIDC );
}



// --------------------------- StaticICoreStorage --------------------------- //


TEST(StaticICoreStorageTest,RegisterStorageFactory)
{
	// Try registering a factory with no tag (Expect failure)
	EXPECT_NE( ICoreStorage::RegisterStorage("", NULL), S_OK )
		<< "Failure on ICoreStorage::RegisterStorage with empty tag.";
	// Try registering a factory with a NULL factory (Expect failure)
	EXPECT_NE( ICoreStorage::RegisterStorage("MGA", NULL), S_OK )
		<< "Failure on ICoreStorage::RegisterStorage with NULL factory.";
	// Register this storage class (Expect S_OK)
	ASSERT_EQ( ICoreStorage::RegisterStorage("MGA", new BinFileFactory()), S_OK )
		<< "Failure on ICoreStorage::RegisterStorage - MUST succeed to continue.";
	// Try registering the same storage TAG again (Expect failure)
	EXPECT_NE( ICoreStorage::RegisterStorage("MGA", new BinFileFactory()), S_OK)
		<< "Failure on ICoreStorage::RegisterStorage with duplicate tag.";

	// Try clearing the factory registry (Expect S_OK)
	EXPECT_EQ( ICoreStorage::ClearStorageRegistry(), S_OK );

	// Try registering the previously registered MGA factory (Expect S_OK)
	ASSERT_EQ( ICoreStorage::RegisterStorage("MGA", new BinFileFactory()), S_OK )
		<< "Failure on ICoreStorage::RegisterStorage - MUST succeed to continue.";
}


TEST(StaticICoreStorageTest,Create)
{
//	Result_t result = Create(const std::string &tag, const std::string &filename,
//							 CoreMetaProject* metaProject, ICoreStorage* &storage);

	// TODO: Create with simple filename (no directory path)

	// TODO: Create with full path (directory + filename)
}


TEST(StaticICoreStorageTest,Open)
{
//	Result_t result = Open(const std::string &tag, const std::string &filename,
//						   CoreMetaProject* metaProject, ICoreStorage* &storage);

	// TODO: Open with simple filename (no directory path)
	
	// TODO: Open with full path (directory + filename)
	
}


// --------------------------- ICoreStorage Standard --------------------------- //


TEST_F(ICoreStorageTest,OpenObject)
{
	MetaObjIDPair idPair(METAID_NONE, OBJID_NONE);
	// OpenObject outside of transaction (Expect E_TRANSACTION)
	EXPECT_EQ( storage->OpenObject(idPair), E_TRANSACTION);

	// OpenObject with transaction but with METAID_NONE (Expect E_INVALID_USAGE)
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->OpenObject(idPair), E_INVALID_USAGE);

	// OpenObject with transaction but with OBJID_NONE (Expect E_INVALID_USAGE)
	idPair.metaID = METAID_ROOT;
	EXPECT_EQ( storage->OpenObject(idPair), E_INVALID_USAGE);

	// OpenObject with transaction but with invalid idPair (Expect E_NOTFOUND)
	idPair.metaID = 150;
	idPair.objID = 272734;
	EXPECT_EQ( storage->OpenObject(idPair), E_NOTFOUND);

	// OpenObject with transaction and valid idPair (Expect S_OK)
	idPair.metaID = 102;
	idPair.objID = 27;
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->CommitTransaction(), S_OK);
}


TEST_F(ICoreStorageTest,CloseObject)
{
	MetaObjIDPair idPair(METAID_NONE, OBJID_NONE);
	// CloseObject outside of transaction (Expect E_TRANSACTION)
	EXPECT_EQ( storage->CloseObject(), E_TRANSACTION );
	
	// CloseObject with transaction but no opened object (Expect S_OK)
	EXPECT_EQ( storage->BeginTransaction(), S_OK );
	EXPECT_EQ( storage->CloseObject(), S_OK );

	// CloseObject with transaction and correctly opened object (Expect S_OK)
	idPair.metaID = 102;
	idPair.objID = 27;
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->CloseObject(), S_OK);
	EXPECT_EQ( storage->CommitTransaction(), S_OK);
}


TEST_F(ICoreStorageTest,CreateObject)
{
	MetaObjIDPair idPair(DTID_ATOM, OBJID_NONE);
	// CreateObject with METAID_NONE (Expect E_INVALID_USAGE)
	EXPECT_EQ( storage->CreateObject(METAID_NONE, idPair.objID), E_INVALID_USAGE );

	// CreateObject outside of transaction (Expect E_TRANSACTION)
	EXPECT_EQ( storage->CreateObject(DTID_ATOM, idPair.objID), E_TRANSACTION );

	EXPECT_EQ( storage->BeginTransaction(), S_OK );
	// CreateObject inside transaction with invalid MetaID (Expect E_METAID)
	EXPECT_EQ( storage->CreateObject(123456, idPair.objID), E_METAID );

	// CreateObject inside transaction with valid MetaID (Expect S_OK)
	EXPECT_EQ( storage->CreateObject(DTID_ATOM, idPair.objID), S_OK );
	// Make sure objID is not OBJID_NONE
	EXPECT_NE( idPair.objID, OBJID_NONE );
	// Close the object
	EXPECT_EQ( storage->CloseObject(), S_OK );
	// Try opening the object again
	EXPECT_EQ( storage->OpenObject(idPair), S_OK );
	// Clean up and exit
	EXPECT_EQ( storage->CloseObject(), S_OK );
	EXPECT_EQ( storage->AbortTransaction(), S_OK );

	// Start another transaction and see if we can open the object (Expect E_NOTFOUND)
	EXPECT_EQ( storage->BeginTransaction(), S_OK );
	EXPECT_EQ( storage->OpenObject(idPair), E_NOTFOUND );
	EXPECT_EQ( storage->CommitTransaction(), S_OK );

	// Start another transaction and see if creation persists across commit transaction (Expect S_OK)
	EXPECT_EQ( storage->BeginTransaction(), S_OK );
	EXPECT_EQ( storage->CreateObject(DTID_ATOM, idPair.objID), S_OK );
	EXPECT_EQ( storage->CommitTransaction(), S_OK );
	EXPECT_EQ( storage->BeginTransaction(), S_OK );
	EXPECT_EQ( storage->OpenObject(idPair), S_OK );
	EXPECT_EQ( storage->CommitTransaction(), S_OK );
}


TEST_F(ICoreStorageTest,DeleteObject)
{
	// DeleteObject outside of transaction (Expect E_TRANSACTION)
	EXPECT_EQ( storage->DeleteObject(), E_TRANSACTION );

	EXPECT_EQ( storage->BeginTransaction(), S_OK );
	// DeleteObject without valid open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( storage->DeleteObject(), E_INVALID_USAGE );

	MetaObjIDPair idPair(102,28);
	EXPECT_EQ( storage->OpenObject(idPair), S_OK );
	// DeleteObject with transaction and valid open object (Expect S_OK)
	EXPECT_EQ( storage->DeleteObject(), S_OK );
	// Make sure there is no open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( storage->DeleteObject(), E_INVALID_USAGE );
	// Make sure we can not open the deleted object
	EXPECT_EQ( storage->OpenObject(idPair), E_NOTFOUND );
	// Make sure objects that were being pointed to no longer have backpointers
	MetaObjIDPair bpPair(101,2);
	EXPECT_EQ( storage->OpenObject(bpPair), S_OK );
	std::list<MetaObjIDPair> bpList;
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_PARENT + ATTRID_COLLECTION, bpList), S_OK );
	std::list<MetaObjIDPair>::iterator bpListIter = bpList.begin();
	while (bpListIter != bpList.end()) {
		// Make sure this pair does not refer to the idPair
		EXPECT_NE( *bpListIter, idPair );
		++bpListIter;
	}
	// Abort the transaction to restore the deleted object
	EXPECT_EQ( storage->AbortTransaction(), S_OK );
	// Make sure we can open the object
	EXPECT_EQ( storage->BeginTransaction(), S_OK );
	EXPECT_EQ( storage->OpenObject(idPair), S_OK );
	// Make sure objects that are being pointed to now have backpointers
	// ...
	EXPECT_EQ( storage->CommitTransaction(), S_OK );

	// Delete the object through commit and test for it being gone
	EXPECT_EQ( storage->BeginTransaction(), S_OK );
	EXPECT_EQ( storage->OpenObject(idPair), S_OK );
	EXPECT_EQ( storage->DeleteObject(), S_OK );
	EXPECT_EQ( storage->CommitTransaction(), S_OK );
	EXPECT_EQ( storage->BeginTransaction(), S_OK );
	EXPECT_EQ( storage->OpenObject(idPair), E_NOTFOUND );
	EXPECT_EQ( storage->CommitTransaction(), S_OK );
}


TEST_F(ICoreStorageTest,BeginTransaction)
{
	// BeginTransaction with no existing transaction (Expect S_OK)
	EXPECT_EQ( storage->BeginTransaction(), S_OK );

	// BeginTransaction with an outstanding transaction (Expect E_TRANSACTION)
	EXPECT_EQ( storage->BeginTransaction(), E_TRANSACTION );
	EXPECT_EQ( storage->CommitTransaction(), S_OK );

	// BeginTransaction after a commit transaction (Expect S_OK)
	EXPECT_EQ( storage->BeginTransaction(), S_OK );
	EXPECT_EQ( storage->AbortTransaction(), S_OK );

	// BeginTransaction after an abort transaction (Expect S_OK)
	EXPECT_EQ( storage->BeginTransaction(), S_OK );
	EXPECT_EQ( storage->AbortTransaction(), S_OK );
}


TEST_F(ICoreStorageTest,CommitTransaction)
{
	// CommitTransaction with no existing transaction (Expect E_TRANSACTION)
	EXPECT_EQ( storage->CommitTransaction(), E_TRANSACTION );

	// TODO: Event (Create, Alter, Delete) ordering permutations
	//...
}


TEST_F(ICoreStorageTest,AbortTransaction)
{
	// AbortTransaction with no existing transaction (Expect E_TRANSACTION)
	EXPECT_EQ( storage->AbortTransaction(), E_TRANSACTION );

	// TODO: Event (Create, Alter, Delete) ordering permutations
	//...
}


TEST_F(ICoreStorageTest,LongAttribute)
{
	// GetAttributeValue outside of transaction (Expect E_TRANSACTION)
	int32_t value;
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_FCOPARENT, value), E_TRANSACTION );
	
	// GetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_FCOPARENT, value), E_INVALID_USAGE );

	// GetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	MetaObjIDPair idPair(102,30);
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->GetAttributeValue(12345, value), E_ATTRID );

	// GetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match Long (Expect E_ATTVALTYPE)
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_NAME, value), E_ATTVALTYPE );

	// GetAttributeValue with everything corret (Expect S_OK)
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_ROLEMETA, value), S_OK );
	EXPECT_EQ( storage->AbortTransaction(), S_OK);

	// SetAttributeValue outside of transaction (Expect E_TRANSACTION)
	int32_t newValue = 34879;
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_FCOPARENT, newValue), E_TRANSACTION );

	// SetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_FCOPARENT, newValue), E_INVALID_USAGE );

	// SetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->SetAttributeValue(12345, newValue), E_ATTRID );

	// SetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match Long (Expect E_ATTVALTYPE)
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_NAME, newValue), E_ATTVALTYPE );

	// SetAttributeValue with everything corret (Expect S_OK)
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_ROLEMETA, newValue), S_OK );
	// Test to make sure value was accepted
	int32_t tmpValue;
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_ROLEMETA, tmpValue), S_OK );
	EXPECT_EQ( newValue, tmpValue );
	EXPECT_EQ( storage->AbortTransaction(), S_OK);
	// Test to make sure old value was restored
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_ROLEMETA, tmpValue), S_OK );
	EXPECT_EQ( value, tmpValue );
	EXPECT_EQ( storage->AbortTransaction(), S_OK);

	// SetAttributeValue within a commit transaction (Expect S_OK)
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_ROLEMETA, newValue), S_OK );
	EXPECT_EQ( storage->CommitTransaction(), S_OK);
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_ROLEMETA, tmpValue), S_OK );
	EXPECT_EQ( newValue, tmpValue );
	EXPECT_EQ( storage->AbortTransaction(), S_OK);
}


TEST_F(ICoreStorageTest,RealAttribute)
{
	// GetAttributeValue outside of transaction (Expect E_TRANSACTION)
	double value;
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_FLOATATTR, value), E_TRANSACTION );

	// GetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_FLOATATTR, value), E_INVALID_USAGE );

	// TODO: GetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
//	MetaObjIDPair idPair(113,1);
//	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
//	EXPECT_EQ( storage->GetAttributeValue(12345, value), E_ATTRID );

	// TODO: GetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match Long (Expect E_ATTVALTYPE)
//	EXPECT_EQ( storage->GetAttributeValue(ATTRID_NAME, value), E_ATTVALTYPE );

	// TODO: GetAttributeValue with everything corret (Expect S_OK)
//	EXPECT_EQ( storage->GetAttributeValue(ATTRID_FLOATATTR, value), S_OK );
	EXPECT_EQ( storage->AbortTransaction(), S_OK);
}


TEST_F(ICoreStorageTest,StringAttribute)
{
	// GetAttributeValue outside of transaction (Expect E_TRANSACTION)
	std::string value;
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_FCOPARENT, value), E_TRANSACTION );
	
	// GetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_FCOPARENT, value), E_INVALID_USAGE );
	
	// GetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	MetaObjIDPair idPair(102,31);
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->GetAttributeValue(12345, value), E_ATTRID );
	
	// GetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match Long (Expect E_ATTVALTYPE)
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_META, value), E_ATTVALTYPE );
	
	// GetAttributeValue with everything corret (Expect S_OK)
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_NAME, value), S_OK );
	EXPECT_EQ( storage->AbortTransaction(), S_OK);

	// SetAttributeValue outside of transaction (Expect E_TRANSACTION)
	std::string newValue = "Foo man choo";
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_NAME, newValue), E_TRANSACTION );

	// SetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_NAME, newValue), E_INVALID_USAGE );

	// SetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->SetAttributeValue(12345, newValue), E_ATTRID );

	// SetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match Long (Expect E_ATTVALTYPE)
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_FCOPARENT, newValue), E_ATTVALTYPE );

	// SetAttributeValue with everything corret (Expect S_OK)
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_NAME, newValue), S_OK );
	// Test to make sure value was accepted
	std::string tmpValue;
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_NAME, tmpValue), S_OK );
	EXPECT_EQ( newValue, tmpValue );
	EXPECT_EQ( storage->AbortTransaction(), S_OK);
	// Test to make sure old value was restored
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_NAME, tmpValue), S_OK );
	EXPECT_EQ( value, tmpValue );
	EXPECT_EQ( storage->AbortTransaction(), S_OK);

	// SetAttributeValue within a commit transaction (Expect S_OK)
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_NAME, newValue), S_OK );
	EXPECT_EQ( storage->CommitTransaction(), S_OK);
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_NAME, tmpValue), S_OK );
	EXPECT_EQ( newValue, tmpValue );
	EXPECT_EQ( storage->AbortTransaction(), S_OK);
}


TEST_F(ICoreStorageTest,BinaryAttribute)
{
	// GetAttributeValue outside of transaction (Expect E_TRANSACTION)
	std::vector<unsigned char> value;
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_FLOATATTR, value), E_TRANSACTION );
	
	// GetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_FLOATATTR, value), E_INVALID_USAGE );

	// GetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	MetaObjIDPair idPair(METAID_ROOT,OBJID_ROOT);
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	EXPECT_EQ( storage->GetAttributeValue(12345, value), E_ATTRID );

	EXPECT_EQ( storage->AbortTransaction(), S_OK);
}


TEST_F(ICoreStorageTest,PointerAttribute)
{
	// GetAttributeValue outside of transaction (Expect E_TRANSACTION)
	MetaObjIDPair pointerPair;
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_FCOPARENT, pointerPair), E_TRANSACTION );

	// GetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( storage->BeginTransaction(), S_OK);
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_FCOPARENT, pointerPair), E_INVALID_USAGE );
	
	// Open an object (an Atom) we know has a pointer attribute
	MetaObjIDPair idPair(102,27);
	EXPECT_EQ( storage->OpenObject(idPair), S_OK);
	// Get a non-existant pointer attribute value (Foo attribute) (Expect E_ATTRID)
	EXPECT_EQ( storage->GetAttributeValue(123, pointerPair), E_ATTRID);

	// Get an attribute that isn't a pointer (ATTRID_NAME) (Expect E_ATTVALTYPE)
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_NAME, pointerPair), E_ATTVALTYPE);

	// Get the pointer attribute value (ATTRID_FCOPARENT) (Expect S_OK)
	MetaObjIDPair goodPair;
	EXPECT_EQ( storage->GetAttributeValue(ATTRID_FCOPARENT, goodPair), S_OK);

	// Set a non-existant pointer attribute value (Foo attribute) (Expect E_ATTRID)
	EXPECT_EQ( storage->SetAttributeValue(123, pointerPair), E_ATTRID);

	// Set an attribute that isn't a pointer (ATTRID_NAME) (Expect E_ATTVALTYPE)
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_NAME, pointerPair), E_ATTVALTYPE);

	// Set a pointer attribute to point at an invalid object (7,1) (Expect E_NOTFOUND)
	pointerPair.metaID = 7;
	pointerPair.objID = 1;
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_FCOPARENT, pointerPair), E_NOTFOUND);

	// Set a pointer attribute to point at an object that doesn't have a valid backpointer collection (Expect E_VALTYPE)
	pointerPair.metaID = DTID_STRATTR;
	pointerPair.objID = 17;
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_FCOPARENT, pointerPair), E_VALTYPE);

	// Set pointer attribute correctly (Expect S_OK)
	pointerPair.metaID = DTID_MODEL;
	pointerPair.objID = 13;
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_FCOPARENT, pointerPair), S_OK);
	// Make sure object that was being pointed to no longer has backpointer
	//...
	// Make sure object now being pointed to has backpointer
	//...

	// Set pointer attribute to NONE (Expect S_OK)
	pointerPair.metaID = METAID_NONE;
	pointerPair.objID = OBJID_NONE;
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_FCOPARENT, pointerPair), S_OK);

	// Set pointer attribute from NONE back to original good idPair (Expect S_OK)
	EXPECT_EQ( storage->SetAttributeValue(ATTRID_FCOPARENT, goodPair), S_OK);
	
	// Cleanup and be done
	EXPECT_EQ( storage->CloseObject(), S_OK);
	EXPECT_EQ( storage->AbortTransaction(), S_OK);
}


TEST_F(ICoreStorageTest,Save)
{
	// TODO: Save with "" as filename
	
	// TODO: Save with filename as existing file
	
	// TODO: Save with simple filename (no directory path)
	
	// TODO: Save with full path (directory + filename)
	
	// TODO: Save with objects in cache and scratch file and changes
	
}


TEST_F(ICoreStorageTest,CacheSize)
{
	// TODO: Perform with cacheSize == 1
	
	// TODO: Perform with cacheSize == 2
	
	// TODO: Perform with cacheSize == 8
	
	// TODO: Perform with cacheSize == 64
	
	// TODO: Perform with cacheSize == 256
	
	// TODO: Perform with cacheSize == unlimited	
}


// --------------------------- ICoreStorage Parameterized --------------------------- //


TEST_P(ICoreStorageParamTest,MetaObject)
{
	CoreMetaObject* coreMetaObject = NULL;

	// Get CoreMetaObject outside of transaction (Expect E_TRANSACTION)
	result = storage->MetaObject(coreMetaObject);
	EXPECT_EQ(result, E_TRANSACTION);

	// Get CoreMetaObject without open object (Expect E_INVALID_USAGE)
	result = storage->BeginTransaction();
	EXPECT_EQ(result, S_OK);
	result = storage->MetaObject(coreMetaObject);
	EXPECT_EQ(result, E_INVALID_USAGE);

	// Open an object then try to get its CoreMetaObject (Expect S_OK)
	result = storage->OpenObject(idPair);
	EXPECT_EQ(result, S_OK);
	result = storage->MetaObject(coreMetaObject);
	EXPECT_EQ(result, S_OK);
	MetaID_t tmpMetaID = METAID_NONE;
	EXPECT_EQ(coreMetaObject->MetaID(tmpMetaID), S_OK);
	EXPECT_EQ(tmpMetaID, idPair.metaID);

	// Abort the transaction to clean up
	result = storage->AbortTransaction();
	EXPECT_EQ(result, S_OK);	
}


TEST_P(ICoreStorageParamTest,MetaID)
{
	MetaID_t metaID = METAID_NONE;

	// Get metaID outside of transaction (Expect E_TRANSACTION)
	result = storage->MetaID(metaID);
	EXPECT_EQ(result, E_TRANSACTION);
}


TEST_P(ICoreStorageParamTest,AttributeChangeWithAbort)
{
	int32_t longValue, longValueAlt=457868;
	double realValue, realValueAlt=483.485786;
	std::string stringValue, stringValueAlt="asdkfjhaslkdjfh";
	std::vector<unsigned char> binaryValue, binaryValueAlt;
	std::list<MetaObjIDPair> collectionValue, collectionValueAlt;
	MetaObjIDPair pointerValue, pointerValueAlt;

	// Test every attribute in the object (via its meta attribute list)
	std::list<CoreMetaAttribute*>::iterator metaAttributeIter = attributeList.begin();
	while (metaAttributeIter != attributeList.end())
	{
		AttrID_t attrID;
		ASSERT_EQ((*metaAttributeIter)->AttributeID(attrID), S_OK );
		ValueType attrType = ValueType::None();
		ASSERT_EQ((*metaAttributeIter)->GetValueType(attrType), S_OK);

		// GetAttributeValue(long) outside of transaction (Expect E_TRANSACTION)
		result = storage->GetAttributeValue(attrID, longValue);
		EXPECT_EQ(result, E_TRANSACTION);
		// GetAttributeValue(real) outside of transaction (Expect E_TRANSACTION)
		result = storage->GetAttributeValue(attrID, realValue);
		EXPECT_EQ(result, E_TRANSACTION);
		// GetAttributeValue(string) outside of transaction (Expect E_TRANSACTION)
		result = storage->GetAttributeValue(attrID, stringValue);
		EXPECT_EQ(result, E_TRANSACTION);
		// GetAttributeValue(binary) outside of transaction (Expect E_TRANSACTION)
		result = storage->GetAttributeValue(attrID, binaryValue);
		EXPECT_EQ(result, E_TRANSACTION);
		// GetAttributeValue(collection) outside of transaction (Expect E_TRANSACTION)
		result = storage->GetAttributeValue(attrID, collectionValue);
		EXPECT_EQ(result, E_TRANSACTION);
		// GetAttributeValue(pointer) outside of transaction (Expect E_TRANSACTION)
		result = storage->GetAttributeValue(attrID, pointerValue);
		EXPECT_EQ(result, E_TRANSACTION);

		// SetAttributeValue(long) outside of transaction (Expect E_TRANSACTION)
		result = storage->SetAttributeValue(attrID, longValue);
		EXPECT_EQ(result, E_TRANSACTION);
		// SetAttributeValue(real) outside of transaction (Expect E_TRANSACTION)
		result = storage->SetAttributeValue(attrID, realValue);
		EXPECT_EQ(result, E_TRANSACTION);
		// SetAttributeValue(string) outside of transaction (Expect E_TRANSACTION)
		result = storage->SetAttributeValue(attrID, stringValue);
		EXPECT_EQ(result, E_TRANSACTION);
		// SetAttributeValue(binary) outside of transaction (Expect E_TRANSACTION)
		result = storage->SetAttributeValue(attrID, binaryValue);
		EXPECT_EQ(result, E_TRANSACTION);
		// SetAttributeValue(pointer) outside of transaction (Expect E_TRANSACTION)
		result = storage->SetAttributeValue(attrID, pointerValue);
		EXPECT_EQ(result, E_TRANSACTION);
		
		result = storage->BeginTransaction();
		EXPECT_EQ(result, S_OK);

		// GetAttributeValue(long) without open object (Expect E_INVALID_USAGE)
		result = storage->GetAttributeValue(attrID, longValue);
		EXPECT_EQ(result, E_INVALID_USAGE);
		// GetAttributeValue(real) without open object (Expect E_INVALID_USAGE)
		result = storage->GetAttributeValue(attrID, realValue);
		EXPECT_EQ(result, E_INVALID_USAGE);
		// GetAttributeValue(string) without open object (Expect E_INVALID_USAGE)
		result = storage->GetAttributeValue(attrID, stringValue);
		EXPECT_EQ(result, E_INVALID_USAGE);
		// GetAttributeValue(binary) without open object (Expect E_INVALID_USAGE)
		result = storage->GetAttributeValue(attrID, binaryValue);
		EXPECT_EQ(result, E_INVALID_USAGE);
		// GetAttributeValue(collection) without open object (Expect E_INVALID_USAGE)
		result = storage->GetAttributeValue(attrID, collectionValue);
		EXPECT_EQ(result, E_INVALID_USAGE);
		// GetAttributeValue(pointer) without open object (Expect E_INVALID_USAGE)
		result = storage->GetAttributeValue(attrID, pointerValue);
		EXPECT_EQ(result, E_INVALID_USAGE);

		// SetAttributeValue(long) without open object (Expect E_INVALID_USAGE)
		result = storage->SetAttributeValue(attrID, longValue);
		EXPECT_EQ(result, E_INVALID_USAGE);
		// SetAttributeValue(real) without open object (Expect E_INVALID_USAGE)
		result = storage->SetAttributeValue(attrID, realValue);
		EXPECT_EQ(result, E_INVALID_USAGE);
		// SetAttributeValue(string) without open object (Expect E_INVALID_USAGE)
		result = storage->SetAttributeValue(attrID, stringValue);
		EXPECT_EQ(result, E_INVALID_USAGE);
		// SetAttributeValue(binary) without open object (Expect E_INVALID_USAGE)
		result = storage->SetAttributeValue(attrID, binaryValue);
		EXPECT_EQ(result, E_INVALID_USAGE);
		// SetAttributeValue(pointer) without open object (Expect E_INVALID_USAGE)
		result = storage->SetAttributeValue(attrID, pointerValue);
		EXPECT_EQ(result, E_INVALID_USAGE);

		// Get and Set AttributeValue within transaction and with open object (Expect S_OK)
		result = storage->OpenObject(idPair);
		EXPECT_EQ(result, S_OK);
		// Try changing an object's attribute value
		if (attrType == ValueType::Long())
		{
			result = storage->GetAttributeValue(attrID, longValue);
			EXPECT_EQ(result, S_OK);
			EXPECT_NE(longValue, longValueAlt);
			result = storage->SetAttributeValue(attrID, longValueAlt);
			EXPECT_EQ(result, S_OK);
			result = storage->GetAttributeValue(attrID, longValue);
			EXPECT_EQ(result, S_OK);
			EXPECT_EQ(longValue, longValueAlt);
		}
		else if (attrType == ValueType::Real())
		{
			result = storage->GetAttributeValue(attrID, realValue);
			EXPECT_EQ(result, S_OK);
			EXPECT_NE(realValue, realValueAlt);
			result = storage->SetAttributeValue(attrID, realValueAlt);
			EXPECT_EQ(result, S_OK);
			result = storage->GetAttributeValue(attrID, realValue);
			EXPECT_EQ(result, S_OK);
			EXPECT_EQ(realValue, longValueAlt);
		}
		else if (attrType == ValueType::String())
		{
			result = storage->GetAttributeValue(attrID, stringValue);
			EXPECT_EQ(result, S_OK);
			EXPECT_NE(stringValue, stringValueAlt);
			result = storage->SetAttributeValue(attrID, stringValueAlt);
			EXPECT_EQ(result, S_OK);
			result = storage->GetAttributeValue(attrID, stringValue);
			EXPECT_EQ(result, S_OK);
			EXPECT_EQ(stringValue, stringValueAlt);
		}
		else if (attrType == ValueType::Binary())
		{
//			result = storage->GetAttributeValue(attrID, realValue);
//			EXPECT_EQ(result, S_OK);
//			EXPECT_NE(realValue, realValueAlt);
//			result = storage->SetAttributeValue(attrID, realValueAlt);
//			EXPECT_EQ(result, S_OK);
//			result = storage->GetAttributeValue(attrID, realValue);
//			EXPECT_EQ(result, S_OK);
//			EXPECT_EQ(realValue, longValueAlt);
		}

		// Close the object (though not necessary since abort implies close object)
		result = storage->CloseObject();
		EXPECT_EQ(result, S_OK);
		// Abort the transaction and make sure abort reverts the attribute value
		result = storage->AbortTransaction();
		EXPECT_EQ(result, S_OK);

		// Must being another transaction and open the object
		result = storage->BeginTransaction();
		EXPECT_EQ(result, S_OK);
		result = storage->OpenObject(idPair);
		EXPECT_EQ(result, S_OK);
		// Check to make sure value has been reverted (Expect S_OK)
		if (attrType == ValueType::Long())
		{
			result = storage->GetAttributeValue(attrID, longValue);
			EXPECT_EQ(result, S_OK);
			EXPECT_NE(longValue, longValueAlt);
		}
		else if (attrType == ValueType::Real())
		{
			result = storage->GetAttributeValue(attrID, realValue);
			EXPECT_EQ(result, S_OK);
			EXPECT_NE(realValue, realValueAlt);
		}
		else if (attrType == ValueType::String())
		{
			result = storage->GetAttributeValue(attrID, stringValue);
			EXPECT_EQ(result, S_OK);
			EXPECT_NE(stringValue, stringValueAlt);
		}
		else if (attrType == ValueType::Binary())
		{
//			result = storage->GetAttributeValue(attrID, binaryValue);
//			EXPECT_EQ(result, S_OK);
//			EXPECT_NE(binaryValue, binaryValueAlt);
		}

		// Abort the transaction (though nothing has actually changed)
		result = storage->AbortTransaction();
		EXPECT_EQ(result, S_OK);		
		// Move on to the next attribute in this object
		++metaAttributeIter;
	}
}

/*
TEST_P(ICoreStorageParamTest,StringAttributeChangeWithCommit)
{
	std::string name = "";
	std::string name2 = "lkjahsliuahsd";

	// Try beginning a transaction, making an attribute value change and commiting the transaction
	result = storage->BeginTransaction();
	EXPECT_EQ(result, S_OK);
	result = storage->OpenObject(idPair);
	EXPECT_EQ(result, S_OK);
	result = storage->GetAttributeValue(ATTRID_NAME, name);
	EXPECT_EQ(result, S_OK);
	EXPECT_NE(name, name2);
	result = storage->SetAttributeValue(ATTRID_NAME, name2);
	EXPECT_EQ(result, S_OK);
	result = storage->CommitTransaction();
	EXPECT_EQ(result, S_OK);
	result = storage->BeginTransaction();
	EXPECT_EQ(result, S_OK);
	result = storage->OpenObject(idPair);
	EXPECT_EQ(result, S_OK);
	result = storage->GetAttributeValue(ATTRID_NAME, name);
	EXPECT_EQ(result, S_OK);
	EXPECT_EQ(name, name2);
	result = storage->CommitTransaction();
	EXPECT_EQ(result, S_OK);
}


TEST_P(ICoreStorageParamTest,ObjectDeleteInTransactionWithAbort)
{
	result = storage->BeginTransaction();	
	result = storage->OpenObject(idPair);
//	result = storage->DeleteObject();
	result = storage->CloseObject();
	result = storage->AbortTransaction();
}
*/

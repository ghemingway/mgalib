/*** Included Header Files ***/
#include "CoreStorageTesting.h"


// Initialize the static members outside of the class
CoreProject* ICoreStorageTest::coreProject = NULL;
ICoreStorage* ICoreStorageTest::storage = NULL;
Uuid ICoreStorageTest::atomUuid = Uuid::Null();
Uuid ICoreStorageTest::attributeUuid = Uuid::Null();
//CoreMetaProject* ICoreStorageParamTest::metaProject = NULL;
//CoreProject* ICoreStorageParamTest::coreProject = NULL;
//ICoreStorage* ICoreStorageParamTest::storage = NULL;


// --------------------------- UtilityTests --------------------------- //


TEST(UtilityTests,UuidConversions)
{
	Uuid uuidA;
	// From a Uuid to vector and string
	std::vector<unsigned char> vectorUuidA = uuidA;
	std::string stringUuidA = uuidA;

	// From vector to Uuid and string
	Uuid uuidB = vectorUuidA;
	std::string stringUuidB = Uuid::ToString(vectorUuidA);

	// From string to Uuid and vector
	Uuid uuidC = stringUuidB;
	std::vector<unsigned char> vectorUuidC = Uuid::ToVector(stringUuidB);

	Uuid uuidD;
	EXPECT_NE( uuidA, uuidD );
	EXPECT_EQ( uuidA, uuidB );
	EXPECT_EQ( uuidB, uuidC );
	EXPECT_EQ( stringUuidA, stringUuidB );
	EXPECT_EQ( vectorUuidA, vectorUuidC );
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


TEST(StaticICoreStorageTest,Open)
{
//	Result_t result = Open(const std::string &tag, const std::string &filename,
//						   CoreMetaProject* metaProject, ICoreStorage* &storage);

	// TODO: Open with simple filename (no directory path)

	// TODO: Open with full path (directory + filename)
}


// --------------------------- ICoreStorage Standard --------------------------- //


TEST_F(ICoreStorageTest,CreateObject)
{
	Result_t result;
	Uuid newUuid(Uuid::Null());
	// CreateObject with METAID_NONE (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->CreateObject(METAID_NONE, newUuid), E_INVALID_USAGE ) << GetErrorInfo(result);
	EXPECT_EQ( newUuid, Uuid::Null() );

	// CreateObject outside of transaction (Expect E_TRANSACTION)
	EXPECT_EQ( result = storage->CreateObject(DTID_ATOM, newUuid), E_TRANSACTION ) << GetErrorInfo(result);
	EXPECT_EQ( newUuid, Uuid::Null() );

	EXPECT_EQ( storage->BeginTransaction(), S_OK );
	// CreateObject inside transaction with invalid MetaID (Expect E_METAID)
	EXPECT_EQ( result = storage->CreateObject(123456, newUuid), E_METAID ) << GetErrorInfo(result);
	EXPECT_EQ( newUuid, Uuid::Null() );

	// CreateObject inside transaction with valid MetaID (Expect S_OK)
	EXPECT_EQ( result = storage->CreateObject(DTID_ATOM, newUuid), S_OK ) << GetErrorInfo(result);
	// Make sure objID is not OBJID_NONE
	EXPECT_NE( newUuid, Uuid::Null() );
	// Close the object
	EXPECT_EQ( result = storage->CloseObject(), S_OK ) << GetErrorInfo(result);
	// Try opening the object again
	EXPECT_EQ( result = storage->OpenObject(newUuid), S_OK ) << GetErrorInfo(result);
	// Clean up and exit
	EXPECT_EQ( result = storage->CloseObject(), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK ) << GetErrorInfo(result);

	// Start another transaction and see if we can open the object (Expect E_NOTFOUND)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK );
	EXPECT_EQ( result = storage->OpenObject(newUuid), E_NOTFOUND );
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK );

	// Start another transaction and see if creation persists across commit transaction (Expect S_OK)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CreateObject(DTID_ATOM, newUuid), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(newUuid), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK ) << GetErrorInfo(result);

	// Set up the test Uuid for future tests
	ICoreStorageTest::atomUuid = newUuid;
}


TEST_F(ICoreStorageTest,OpenObject)
{
	Result_t result;
	Uuid uuid = Uuid::Null();
	// OpenObject outside of transaction (Expect E_TRANSACTION)
	EXPECT_EQ( result = storage->OpenObject(uuid), E_TRANSACTION) << GetErrorInfo(result);

	// OpenObject with transaction but with Uuid == NULL (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), E_INVALID_USAGE) << GetErrorInfo(result);

	// OpenObject with transaction but with invalid Uuid (Expect E_NOTFOUND)
	uuid = Uuid("{12345678-1234-1234-1234-123412341234}");
	EXPECT_EQ( result = storage->OpenObject(uuid), E_NOTFOUND) << GetErrorInfo(result);

	// OpenObject with transaction and valid uuid (Expect S_OK)
	uuid = ICoreStorageTest::atomUuid;
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);
}


TEST_F(ICoreStorageTest,CloseObject)
{
	Result_t result;
	Uuid uuid = Uuid::Null();
	// CloseObject outside of transaction (Expect E_TRANSACTION)
	EXPECT_EQ( result = storage->CloseObject(), E_TRANSACTION ) << GetErrorInfo(result);
	
	// CloseObject with transaction but no opened object (Expect S_OK)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CloseObject(), S_OK ) << GetErrorInfo(result);

	// CloseObject with transaction and correctly opened object (Expect S_OK)
	uuid = ICoreStorageTest::atomUuid;
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CloseObject(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);
}


TEST_F(ICoreStorageTest,DeleteObject)
{
	Result_t result;
	Uuid uuid = Uuid::Null();
	// DeleteObject outside of transaction (Expect E_TRANSACTION)
	EXPECT_EQ( result = storage->DeleteObject(), E_TRANSACTION ) << GetErrorInfo(result);

	EXPECT_EQ( result = storage->BeginTransaction(), S_OK );
	// DeleteObject without valid open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->DeleteObject(), E_INVALID_USAGE ) << GetErrorInfo(result);

	uuid = ICoreStorageTest::atomUuid;
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK ) << GetErrorInfo(result);
	// DeleteObject with transaction and valid open object (Expect S_OK)
	EXPECT_EQ( result = storage->DeleteObject(), S_OK );
	// Make sure there is no open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->DeleteObject(), E_INVALID_USAGE ) << GetErrorInfo(result);
	// Make sure we can not open the deleted object
	EXPECT_EQ( result = storage->OpenObject(uuid), E_NOTFOUND ) << GetErrorInfo(result);
/*
	// Make sure objects that were being pointed to no longer have backpointers
	MetaObjIDPair bpPair(101,2);
	EXPECT_EQ( result = storage->OpenObject(bpPair), S_OK ) << GetErrorInfo(result);
	std::list<MetaObjIDPair> bpList;
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_PARENT + ATTRID_COLLECTION, bpList), S_OK ) << GetErrorInfo(result);
	std::list<MetaObjIDPair>::iterator bpListIter = bpList.begin();
	while (bpListIter != bpList.end()) {
		// Make sure this pair does not refer to the idPair
		EXPECT_NE( *bpListIter, idPair );
		++bpListIter;
	}
*/
	// Abort the transaction to restore the deleted object
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK ) << GetErrorInfo(result);
	// Make sure we can open the object
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK ) << GetErrorInfo(result);
	// Make sure objects that are being pointed to now have backpointers
	// ...
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK ) << GetErrorInfo(result);

	// Delete the object through commit and test for it being gone
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->DeleteObject(), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), E_NOTFOUND ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK ) << GetErrorInfo(result);
}


TEST_F(ICoreStorageTest,BeginTransaction)
{
	Result_t result;
	// BeginTransaction with no existing transaction (Expect S_OK)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK ) << GetErrorInfo(result);

	// BeginTransaction with an outstanding transaction (Expect E_TRANSACTION)
	EXPECT_EQ( result = storage->BeginTransaction(), E_TRANSACTION ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK ) << GetErrorInfo(result);

	// BeginTransaction after a commit transaction (Expect S_OK)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK ) << GetErrorInfo(result);

	// BeginTransaction after an abort transaction (Expect S_OK)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK ) << GetErrorInfo(result);
}


TEST_F(ICoreStorageTest,CommitTransaction)
{
	Result_t result;
	// CommitTransaction with no existing transaction (Expect E_TRANSACTION)
	EXPECT_EQ( result = storage->CommitTransaction(), E_TRANSACTION ) << GetErrorInfo(result);

	// TODO: Event (Create, Change, Delete) ordering permutations
	//...
}


TEST_F(ICoreStorageTest,AbortTransaction)
{
	Result_t result;
	// AbortTransaction with no existing transaction (Expect E_TRANSACTION)
	EXPECT_EQ( result = storage->AbortTransaction(), E_TRANSACTION ) << GetErrorInfo(result);

	// TODO: Event (Create, Change, Delete) ordering permutations
	//...
}


TEST_F(ICoreStorageTest,LongAttribute)
{
	Uuid uuid(Uuid::Null());
	Result_t result;
	// GetAttributeValue outside of transaction (Expect E_TRANSACTION)
	int32_t value;
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_FCOPARENT, value), E_TRANSACTION ) << GetErrorInfo(result);

	// GetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_FCOPARENT, value), E_INVALID_USAGE ) << GetErrorInfo(result);

	// GetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	EXPECT_EQ( result = storage->CreateObject(DTID_ATOM, uuid), S_OK ) << GetErrorInfo(result);
	ASSERT_NE( uuid, Uuid::Null() );
	ICoreStorageTest::atomUuid = uuid;
	EXPECT_EQ( result = storage->GetAttributeValue(12345, value), E_ATTRID ) << GetErrorInfo(result);

	// GetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match type (Expect E_ATTVALTYPE)
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_NAME, value), E_ATTVALTYPE ) << GetErrorInfo(result);

	// GetAttributeValue with everything corret (Expect S_OK)
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_PERMISSIONS, value), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);

	// SetAttributeValue outside of transaction (Expect E_TRANSACTION)
	int32_t newValue = 34879;
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_FCOPARENT, newValue), E_TRANSACTION ) << GetErrorInfo(result);

	// SetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_FCOPARENT, newValue), E_INVALID_USAGE ) << GetErrorInfo(result);

	// SetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(12345, newValue), E_ATTRID ) << GetErrorInfo(result);

	// SetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match type (Expect E_ATTVALTYPE)
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_NAME, newValue), E_ATTVALTYPE ) << GetErrorInfo(result);

	// SetAttributeValue with everything corret (Expect S_OK)
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_PERMISSIONS, newValue), S_OK ) << GetErrorInfo(result);
	// Test to make sure value was accepted
	int32_t tmpValue;
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_PERMISSIONS, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( newValue, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);
	// Test to make sure old value was restored
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_PERMISSIONS, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( value, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);

	// SetAttributeValue within a commit transaction (Expect S_OK)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_PERMISSIONS, newValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_PERMISSIONS, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( newValue, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);
}


TEST_F(ICoreStorageTest,RealAttribute)
{
	double value;
	Result_t result;
	Uuid uuid(Uuid::Null());
	// GetAttributeValue outside of transaction (Expect E_TRANSACTION)
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_FLOATATTR, value), E_TRANSACTION ) << GetErrorInfo(result);

	// GetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_FLOATATTR, value), E_INVALID_USAGE ) << GetErrorInfo(result);

	// GetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	EXPECT_EQ( result = storage->CreateObject(DTID_FLOATATTR, uuid), S_OK ) << GetErrorInfo(result);
	ASSERT_NE( uuid, Uuid::Null() );
	ICoreStorageTest::attributeUuid = uuid;
	EXPECT_EQ( result = storage->GetAttributeValue(12345, value), E_ATTRID ) << GetErrorInfo(result);

	// GetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match type (Expect E_ATTVALTYPE)
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_ATTRPARENT, value), E_ATTVALTYPE ) << GetErrorInfo(result);

	// GetAttributeValue with everything corret (Expect S_OK)
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_FLOATATTR, value), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);

	// SetAttributeValue outside of transaction (Expect E_TRANSACTION)
	double newValue = 34879.12341;
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_FCOPARENT, newValue), E_TRANSACTION ) << GetErrorInfo(result);

	// SetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_FCOPARENT, newValue), E_INVALID_USAGE ) << GetErrorInfo(result);

	// SetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(12345, newValue), E_ATTRID ) << GetErrorInfo(result);

	// SetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match type (Expect E_ATTVALTYPE)
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_ATTRPARENT, newValue), E_ATTVALTYPE ) << GetErrorInfo(result);

	// SetAttributeValue with everything corret (Expect S_OK)
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_FLOATATTR, newValue), S_OK ) << GetErrorInfo(result);
	// Test to make sure value was accepted
	double tmpValue;
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_FLOATATTR, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( newValue, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);
	// Test to make sure old value was restored
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_FLOATATTR, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( value, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);

	// SetAttributeValue within a commit transaction (Expect S_OK)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_FLOATATTR, newValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_FLOATATTR, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( newValue, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);
}


TEST_F(ICoreStorageTest,StringAttribute)
{
	Result_t result;
	Uuid uuid(Uuid::Null());
	// GetAttributeValue outside of transaction (Expect E_TRANSACTION)
	std::string value;
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_NAME, value), E_TRANSACTION ) << GetErrorInfo(result);
	
	// GetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_NAME, value), E_INVALID_USAGE ) << GetErrorInfo(result);
	
	// GetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	EXPECT_EQ( result = storage->RootUuid(uuid), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(12345, value), E_ATTRID ) << GetErrorInfo(result);
	
	// GetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match type (Expect E_ATTVALTYPE)
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_PARGUID, value), E_ATTVALTYPE ) << GetErrorInfo(result);
	
	// GetAttributeValue with everything corret (Expect S_OK)
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_NAME, value), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);

	// SetAttributeValue outside of transaction (Expect E_TRANSACTION)
	std::string newValue = "A quick brown fox jumps over the lazy dog.";
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_NAME, newValue), E_TRANSACTION ) << GetErrorInfo(result);

	// SetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_NAME, newValue), E_INVALID_USAGE ) << GetErrorInfo(result);

	// SetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(12345, newValue), E_ATTRID ) << GetErrorInfo(result);

	// SetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match type (Expect E_ATTVALTYPE)
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_PARGUID, newValue), E_ATTVALTYPE ) << GetErrorInfo(result);

	// SetAttributeValue with everything corret (Expect S_OK)
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_NAME, newValue), S_OK ) << GetErrorInfo(result);
	// Test to make sure value was accepted
	std::string tmpValue;
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_NAME, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( newValue, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);
	// Test to make sure old value was restored
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_NAME, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( value, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);

	// SetAttributeValue within a commit transaction (Expect S_OK)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_NAME, newValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_NAME, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( newValue, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);
}


TEST_F(ICoreStorageTest,LongPointerAttribute)
{
	Result_t result;
	Uuid uuid(Uuid::Null());
	// GetAttributeValue outside of transaction (Expect E_TRANSACTION)
	Uuid value;
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_FLOATATTR, value), E_TRANSACTION ) << GetErrorInfo(result);
	
	// GetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_FLOATATTR, value), E_INVALID_USAGE ) << GetErrorInfo(result);

	// GetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	EXPECT_EQ( result = storage->RootUuid(uuid), S_OK ) << GetErrorInfo(result) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(12345, value), E_ATTRID ) << GetErrorInfo(result);

	// GetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match type (Expect E_ATTVALTYPE)
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_NAME, value), E_ATTVALTYPE ) << GetErrorInfo(result);
	
	// GetAttributeValue with everything corret (Expect S_OK)
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_PARGUID, value), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);
	
	// SetAttributeValue outside of transaction (Expect E_TRANSACTION)
	Uuid newValue;
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_FLOATATTR, newValue), E_TRANSACTION ) << GetErrorInfo(result);
	
	// SetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_FLOATATTR, newValue), E_INVALID_USAGE ) << GetErrorInfo(result);
	
	// SetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(12345, newValue), E_ATTRID ) << GetErrorInfo(result);
	
	// SetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match type (Expect E_ATTVALTYPE)
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_NAME, newValue), E_ATTVALTYPE ) << GetErrorInfo(result);
	
	// SetAttributeValue with everything corret (Expect S_OK)
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_PARGUID, newValue), S_OK ) << GetErrorInfo(result);
	// Test to make sure value was accepted
	Uuid tmpValue;
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_PARGUID, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( newValue, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);
	// Test to make sure old value was restored
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_PARGUID, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( value, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);
	
	// SetAttributeValue within a commit transaction (Expect S_OK)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_PARGUID, newValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(uuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_PARGUID, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( newValue, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);
}


TEST_F(ICoreStorageTest,PointerAttribute)
{
	Result_t result;
	Uuid attributeUuid = ICoreStorageTest::attributeUuid;
	// GetAttributeValue outside of transaction (Expect E_TRANSACTION)
	Uuid value(Uuid::Null());
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_ATTRPARENT, value), E_TRANSACTION ) << GetErrorInfo(result);

	// GetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_ATTRPARENT, value), E_INVALID_USAGE ) << GetErrorInfo(result);
	
	// Open an object (a FloatAttribute) we know has a pointer attribute
	EXPECT_EQ( result = storage->OpenObject(attributeUuid), S_OK) << GetErrorInfo(result);
	// Get a non-existant pointer attribute value (Foo attribute) (Expect E_ATTRID)
	EXPECT_EQ( result = storage->GetAttributeValue(123, value), E_ATTRID) << GetErrorInfo(result);

	// Get an attribute that isn't a pointer (ATTRID_FLOATATTR) (Expect E_ATTVALTYPE)
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_FLOATATTR, value), E_ATTVALTYPE) << GetErrorInfo(result);

	// Get the pointer attribute value (ATTRID_ATTRPARENT) (Expect S_OK)
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_ATTRPARENT, value), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( value, Uuid::Null() );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);

	// SetAttributeValue outside of transaction (Expect E_TRANSACTION)
	Uuid atomUuid = ICoreStorageTest::atomUuid;
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_ATTRPARENT, atomUuid), E_TRANSACTION ) << GetErrorInfo(result);
	
	// SetAttributeValue within transaction without open object (Expect E_INVALID_USAGE)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_ATTRPARENT, atomUuid), E_INVALID_USAGE ) << GetErrorInfo(result);

	// SetAttributeValue with transaction and with open object but invalid AttrID (Expect E_ATTRID)
	EXPECT_EQ( result = storage->OpenObject(attributeUuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(12345, atomUuid), E_ATTRID ) << GetErrorInfo(result);
	
	// SetAttributeValue with transaction, open object and valid AttrID, but AttrID does not match type (Expect E_ATTVALTYPE)
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_FLOATATTR, atomUuid), E_ATTVALTYPE ) << GetErrorInfo(result);

	// SetAttributeValue with everything corret (Float attribute now points to atom as parent) (Expect S_OK)
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_ATTRPARENT, atomUuid), S_OK ) << GetErrorInfo(result);
	// Test to make sure value was accepted
	Uuid tmpValue;
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_ATTRPARENT, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( atomUuid, tmpValue );
	// Make sure object now being pointed to has backpointer
	EXPECT_EQ( result = storage->OpenObject(atomUuid), S_OK) << GetErrorInfo(result);
	// Get backpointer list (ATTRID_ATTRPARENT) and make sure it has uuid in it
	std::list<Uuid> backpointers;
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_ATTRPARENT+ATTRID_COLLECTION, backpointers), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( backpointers.size(), 1 );
	EXPECT_EQ( backpointers.front(), attributeUuid );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);

	// Test to make sure old values (both forward and backpointer) were restored
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(attributeUuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_ATTRPARENT, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( value, tmpValue );
	// Make sure object that was being pointed to no longer has backpointer
	EXPECT_EQ( result = storage->OpenObject(atomUuid), S_OK) << GetErrorInfo(result);
	// Get backpointer list (ATTRID_ATTRPARENT) and make sure it is empty
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_ATTRPARENT+ATTRID_COLLECTION, backpointers), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( backpointers.size(), 0 );
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);
	
	// SetAttributeValue within a commit transaction (Expect S_OK)
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(attributeUuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_ATTRPARENT, atomUuid), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(attributeUuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->GetAttributeValue(ATTRID_ATTRPARENT, tmpValue), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( atomUuid, tmpValue );
	EXPECT_EQ( result = storage->AbortTransaction(), S_OK) << GetErrorInfo(result);

	// Clean up the atom's connection to the root object
	Uuid rootUuid;
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->RootUuid(rootUuid), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(atomUuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->SetAttributeValue(ATTRID_FCOPARENT, rootUuid), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);
}


TEST_F(ICoreStorageTest,Save)
{
	Result_t result;
	// Save with "" (existing filename) as filename
	EXPECT_EQ( result = storage->Save(""), S_OK ) << GetErrorInfo(result);
	// Make sure the three objects are there and correct
	Uuid rootUuid(Uuid::Null());
	EXPECT_EQ( result = storage->RootUuid(rootUuid), S_OK ) << GetErrorInfo(result);
	std::vector<Uuid> objectVector;
	EXPECT_EQ( result = storage->ObjectVector(objectVector), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( objectVector.size(), 3 );
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(atomUuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(rootUuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(attributeUuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);
	
	//Save with simple filename (no directory path)
	EXPECT_EQ( result = storage->Save("testOrama.mga"), S_OK ) << GetErrorInfo(result);
	// Make sure the objects are there and correct
	EXPECT_EQ( result = storage->ObjectVector(objectVector), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( objectVector.size(), 3 );
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(atomUuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);

	// Save with full path (directory + filename) from simple path
//	EXPECT_EQ( result = storage->Save("../testOrama.mga"), S_OK ) << GetErrorInfo(result);

	// TODO: Save from full path (directory + filename) to simple path

	// Clean up all of the saves
	EXPECT_EQ( result = storage->Save("tmpfile.mga"), S_OK ) << GetErrorInfo(result);
	remove("testOrama.mga");
	// Make sure the objects are there and correct
	EXPECT_EQ( result = storage->ObjectVector(objectVector), S_OK ) << GetErrorInfo(result);
	EXPECT_EQ( objectVector.size(), 3 );
	EXPECT_EQ( result = storage->BeginTransaction(), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(atomUuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(rootUuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->OpenObject(attributeUuid), S_OK) << GetErrorInfo(result);
	EXPECT_EQ( result = storage->CommitTransaction(), S_OK) << GetErrorInfo(result);
}


TEST_F(ICoreStorageTest,CacheSize)
{
	// TODO: Perform with cacheSize == 1

	// TODO: Perform with cacheSize == 2

	// TODO: Perform with cacheSize == 8

	// TODO: Perform with cacheSize == 64

	// TODO: Perform with cacheSize == 256

	// TODO: Perform with cacheSize == unlimited

	// TODO: Save with objects in cache and scratch file and changes
}


// --------------------------- ICoreStorage Parameterized --------------------------- //

/*
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

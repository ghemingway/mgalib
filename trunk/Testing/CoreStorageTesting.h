#ifndef __CORESTORAGE_TESTING_H__
#define __CORESTORAGE_TESTING_H__


/*** Included Header Files ***/
#include "Core/CoreProject.h"
#include "Core/CoreStorage.h"
#include "Core/CoreMetaProject.h"
#include "Core/BinFile.h"
#include "MGA/MgaGeneric.h"
#include <gtest/gtest.h>


/*** Externally Defined Functions & Variables ***/
extern std::string testFileName;


class ICoreStorageTest : public ::testing::Test {
protected:
	// Static class values
	static ICoreStorage			*storage;
	static CoreProject			*coreProject;
	static Uuid					atomUuid, attributeUuid;
public:
	static void SetUpTestCase()
	{
		Result_t result;
		// Get everything up to storage ready
		CoreMetaProject	*coreMetaProject = NULL;
		EXPECT_EQ( result = CreateMGACoreMetaProject(coreMetaProject), S_OK ) << GetErrorInfo(result);
		ASSERT_TRUE( coreMetaProject != NULL );
		
		// Create a new test coreProject
		EXPECT_EQ( result = CoreProject::Create("MGA=tmpfile.mga", coreMetaProject, ICoreStorageTest::coreProject), S_OK ) << GetErrorInfo(result);
		ASSERT_TRUE( ICoreStorageTest::coreProject != NULL );
		// Get the ICoreStorage pointer
		ICoreStorageTest::coreProject->Storage(ICoreStorageTest::storage);
	}
	
	static void TearDownTestCase()
	{
		// Close the projects and delete the objects
		if (ICoreStorageTest::coreProject != NULL)
		{
			delete ICoreStorageTest::coreProject;
//			remove("tmpfile.mga");
		}
		ICoreStorageTest::atomUuid = Uuid::Null();
		ICoreStorageTest::attributeUuid = Uuid::Null();
		ICoreStorageTest::storage = NULL;
		ICoreStorageTest::coreProject = NULL;
	}
};



// The fixture for testing all Uuid values in any ICoreStorage implementor.
class ICoreStorageParamTest : public ::testing::TestWithParam<Uuid> {
protected:
	//	ICoreStorageParamTest() { }
	//	virtual ~ICoreStorageParamTest() { }
	virtual void SetUp()
	{
//		uuid = GetParam();
//		ASSERT_NE( uuid, Uuid::Null() );
//		result = metaProject->Object(idPair.metaID, metaObject);
//		ASSERT_EQ( result, S_OK );
//		result = metaObject->Attributes(attributeList);
//		ASSERT_EQ( result, S_OK );
	}
	virtual void TearDown()
	{
//		idPair = MetaObjIDPair(METAID_NONE,OBJID_NONE);
//		result = S_OK;
	}
	// Per test class values
//	MetaObjIDPair idPair;
//	Result_t result;
//	CoreMetaObject *metaObject;
//	std::list<CoreMetaAttribute*> attributeList;

	// Static class values
//	static CoreMetaProject		*metaProject;
//	static CoreProject			*coreProject;
//	static ICoreStorage			*storage;
	
public:
	static void SetUpTestCase()
	{
//		// Get everything up to storage ready
//		Result_t result = CreateMGACoreMetaProject(true, ICoreStorageParamTest::metaProject);
//		ASSERT_EQ( result, S_OK );
//		ASSERT_TRUE( ICoreStorageParamTest::metaProject != NULL );
//		result = CoreProject::OpenProject(testFileName, ICoreStorageParamTest::metaProject, ICoreStorageParamTest::coreProject);
//		ASSERT_EQ( result, S_OK );
//		// Get the ICoreStorage pointer
//		ICoreStorageParamTest::coreProject->Storage(ICoreStorageParamTest::storage);
	}
	
	static void TearDownTestCase()
	{
//		// Close the projects and delete the objects
//		delete ICoreStorageParamTest::coreProject;
//		ICoreStorageParamTest::storage = NULL;
//		ICoreStorageParamTest::coreProject = NULL;
//		ICoreStorageParamTest::metaProject = NULL;
	}
};


#endif // __CORESTORAGE_TESTING_H__


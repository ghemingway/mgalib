/*** Included Header Files ***/
#include <gtest/gtest.h>
#include "CoreStorageTesting.h"


// ------------------------------------------------------------------------------------ //


// The following lines pull in the real gtest *.cc files.
#include <src/gtest.cc>
#include <src/gtest-death-test.cc>
#include <src/gtest-filepath.cc>
#include <src/gtest-port.cc>
#include <src/gtest-printers.cc>
#include <src/gtest-test-part.cc>
#include <src/gtest-typed-test.cc>


/*** Externally Defined Functions ***/
std::string testFileName = "";


// ------------------------------------------------------------------------------------ //


int main(int argc, char **argv) {
	// Get the name of the file we want to run against
	testFileName = argv[argc-1];
	// Initialize the gtest framework
	testing::InitGoogleTest(&argc, argv);
	
	// Run all registered tests
	return RUN_ALL_TESTS();

	// Clean up
	ICoreStorage::ClearStorageRegistry();
}


std::vector<MetaObjIDPair> GetPairVector(void)
{
	// Open the test file and get the vector of valid MetaObjIDPairs
	CoreMetaProject *metaProject;
	Result_t result = CreateMGACoreMetaProject(true, metaProject);
	ASSERT( result == S_OK );
	ASSERT( metaProject != NULL );
	CoreProject *coreProject;
	result = ICoreStorage::RegisterStorage("MGA", new BinFileFactory());
	ASSERT( result == S_OK );
	result = CoreProject::OpenProject(testFileName, metaProject, coreProject);
	ASSERT( result == S_OK );
	ICoreStorage* storage;
	result = coreProject->Storage(storage);
	ASSERT( result == S_OK );
	std::vector<MetaObjIDPair> idPairVector;
	result = storage->ObjectVector(idPairVector);
	// Clean up before running any tests...
	delete coreProject;
	ICoreStorage::ClearStorageRegistry();
	return idPairVector;
}


INSTANTIATE_TEST_CASE_P(RangeOfMetaObjIDPairs, ICoreStorageParamTest, ::testing::ValuesIn(GetPairVector()));
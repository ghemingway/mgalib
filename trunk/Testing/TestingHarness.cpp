/*** Included Header Files ***/
#include <gtest/gtest.h>
#include "Core/CoreStorage.h"
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

#ifdef _WIN32
#include "crtdbg.h"

int DebugReportHook(int reportType, char* message, int* returnValue) {
	DWORD written;
	WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message) /* *2 for WideChars*/, &written, NULL);
	DebugBreak();
	return FALSE;
}
#endif


/*** Externally Defined Functions ***/
std::string testFileName = "";
std::string hammerTestCount = "0";


// ------------------------------------------------------------------------------------ //

int main(int argc, char **argv) {
#ifdef _WIN32
	// Don't pop up message boxes when an ASSERT fails or a C runtime fault is detected
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
	_CrtSetReportHook(DebugReportHook);
#endif
	// Get the name of the file we want to run against
	if (argc > 1) hammerTestCount = argv[1];
	if (argc > 2) testFileName = argv[2];

	// Initialize the gtest framework
	testing::InitGoogleTest(&argc, argv);
	
	// Run all registered tests
	return RUN_ALL_TESTS();

	// Clean up
	ICoreStorage::ClearStorageRegistry();
}


std::vector<Uuid> GetUuidVector(void)
{
	// Open the test file and get the vector of valid MetaObjIDPairs
	CoreMetaProject *metaProject;
	Result_t result = CreateMGACoreMetaProject(metaProject);
	ASSERT( result == S_OK );
	ASSERT( metaProject != NULL );
	CoreProject *coreProject;
	result = ICoreStorage::RegisterStorage("MGA", new BinFileFactory());
	ASSERT( result == S_OK );
	result = CoreProject::Open(testFileName, metaProject, coreProject);
	ASSERT( result == S_OK );
	ICoreStorage* storage;
	result = coreProject->Storage(storage);
	ASSERT( result == S_OK );
	std::vector<Uuid> uuidVector;
	result = storage->ObjectVector(uuidVector);
	// Clean up before running any tests...
	delete coreProject;
	ICoreStorage::ClearStorageRegistry();
	return uuidVector;
}


//INSTANTIATE_TEST_CASE_P(RangeOfUuids, ICoreStorageParamTest, ::testing::ValuesIn(GetUuidVector()));

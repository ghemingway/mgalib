/*** Included Header Files ***/
#include "CoreTesting.h"


TEST_F(CoreProjectTest,OpenProject)
{
	// Try opening a non-existant file
	CoreMetaProject* coreMetaProject;
	CoreProject* coreProject = NULL;
	Result_t result;
	ASSERT_EQ( S_OK, result = CreateMGACoreMetaProject(coreMetaProject) ) << GetErrorInfo(result);
	EXPECT_EQ( E_FILEOPEN, result = CoreProject::Open("MGA=notarealmodel.mga", coreMetaProject, coreProject) ) << GetErrorInfo(result);

	// Try opening a valid file
	ASSERT_EQ( S_OK, result = CreateMGACoreMetaProject(coreMetaProject) ) << GetErrorInfo(result);
	ASSERT_EQ( S_OK, result = CoreProject::Open("MGA=tmpfile.mga", coreMetaProject, coreProject) ) << GetErrorInfo(result);
	delete coreProject;
}

/*
TEST_F(CoreProjectTest,CreateProject)
{
}


TEST_F(CoreProjectTest,SaveProject)
{
}


TEST_F(CoreProjectTest,BeginTransaction)
{
}


TEST_F(CoreProjectTest,CommitTransaction)
{
}


TEST_F(CoreProjectTest,AbortTransaction)
{
}


TEST_F(CoreProjectTest,NestedTransactionCount)
{
}


TEST_F(CoreProjectTest,Object)
{
}


TEST_F(CoreProjectTest,CreateObject)
{
}


TEST_F(CoreProjectTest,RootObject)
{
}


TEST_F(CoreProjectTest,UndoTransaction)
{
}


TEST_F(CoreProjectTest,RedoTransaction)
{
}


TEST_F(CoreProjectTest,FlushUndoQueue)
{
}


TEST_F(CoreProjectTest,FlushRedoQueue)
{
}


TEST_F(CoreProjectTest,UndoQueueSize)
{
}


TEST_F(CoreProjectTest,RedoQueueSize)
{
}
*/
/*** Included Header Files ***/
#include "CoreTesting.h"

/*
TEST_F(CoreProjectTest,OpenProject)
{
	// Try opening a non-existant file
	CoreMetaProject* coreMetaProject;
	CoreProject* coreProject = NULL;
	ASSERT_EQ( S_OK, CreateMGACoreMetaProject(true, coreMetaProject) );
	EXPECT_EQ( E_FILEOPEN, CoreProject::OpenProject("MGA=notarealmodel.mga", coreMetaProject, coreProject) )
		<< "Failure on CoreProject::OpenProject with non-existant project file.";
	
	// Try opening an invalid v2 file
	
	// Try opening an invalid v3 file
	
	// Try opening a valid v2 file
	ASSERT_EQ( S_OK, CreateMGACoreMetaProject(true, coreMetaProject) );
	ASSERT_EQ( S_OK, CoreProject::OpenProject("MGA=ESMoLv2.mga", coreMetaProject, coreProject) ) 
		<< "Failure on CoreProject::OpenProject to open ESMoL.mga";
	delete coreProject;

	// Try opening a valid v3 file
	ASSERT_EQ( S_OK, CreateMGACoreMetaProject(true, coreMetaProject) );
	ASSERT_EQ( S_OK, CoreProject::OpenProject("MGA=ESMoLv3.mga", coreMetaProject, coreProject) )
		<< "Failure on CoreProject::OpenProject to open NewESMoL.mga";
	delete coreProject;
}


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
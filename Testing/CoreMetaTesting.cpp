/*** Included Header Files ***/
#include "CoreMetaTesting.h"



TEST(CreateCoreMetaProject,CreateCoreMetaProject)
{
	// Create a new CoreMetaProject using extern function (expect success)
	CoreMetaProject* coreMetaProject;
	EXPECT_EQ(CreateMGACoreMetaProject(true, coreMetaProject), S_OK);
	EXPECT_TRUE(coreMetaProject != NULL) << "Failure on CreateCoreMetaProject";
	delete coreMetaProject;
}
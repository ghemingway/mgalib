/*** Included Header Files ***/
#include "MetaProjectTesting.h"
#include "../Meta/MetaFolder.h"


TEST(MetaProjectTest,CreateMetaGMEv3)
{
	MetaProject* metaProject = NULL;
	Result_t result = MetaProject::Create("MGA=MetaGMEv3.mta", metaProject);
	EXPECT_EQ( result, S_OK );
	EXPECT_TRUE( metaProject != NULL );
}


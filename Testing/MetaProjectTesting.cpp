/*** Included Header Files ***/
#include "MetaProjectTesting.h"
#include "../Meta/MetaFolder.h"


TEST(MetaProjectTest,CreateMetaGMEv3)
{
	// First, create a new MetaGMEv3.mta metaProject for us to work on
	MetaProject* metaProject = NULL;
	Result_t result = MetaProject::Create("MGA=MetaGMEv3.mta", metaProject);
	EXPECT_EQ( result, S_OK );
	EXPECT_TRUE( metaProject != NULL );

	// Set all of the MetaProject attributes
	metaProject->SetName("MetaGME");
	metaProject->SetUuid("{9D3F9884-FE60-409C-8FC1-45789193989B}");
	metaProject->SetCreatedAt("Mon Jun 22 12:03:20 2009");
	metaProject->SetModifiedAt("Mon Jun 22 12:03:20 2009");
	metaProject->SetComment("GME Meta Paradigm");
	metaProject->SetAuthor("A. Ledeczi, P. Volgyesi, J. Sprinkle, T. Paka, Z. Molnar");

	// Save the built up MetaGMEv3 file -- we can use it later
	metaProject->Save("MetaGMEv3.mta");
	delete metaProject;
}


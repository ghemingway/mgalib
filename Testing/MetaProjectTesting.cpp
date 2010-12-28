/*** Included Header Files ***/
#include "MetaProjectTesting.h"
#include "../Meta/MetaFolder.h"


TEST(MetaProjectTest,CreateMetaGMEv3)
{
	// First, create a new MetaGMEv3.mta metaProject for us to work on
	MetaProject* metaProject = NULL;
	EXPECT_EQ( MetaProject::Create("MGA=MetaGMEv3.mta", metaProject), S_OK );
	EXPECT_TRUE( metaProject != NULL );

	// Set all of the MetaProject attributes
	EXPECT_EQ( metaProject->SetName("MetaGME"), S_OK );
	EXPECT_EQ( metaProject->SetUuid("{9D3F9884-FE60-409C-8FC1-45789193989B}"), S_OK );
	EXPECT_EQ( metaProject->SetCreatedAt("Mon Jun 22 12:03:20 2009"), S_OK );
	EXPECT_EQ( metaProject->SetModifiedAt("Mon Jun 22 12:03:20 2009"), S_OK );
	EXPECT_EQ( metaProject->SetComment("GME Meta Paradigm"), S_OK );
	EXPECT_EQ( metaProject->SetAuthor("G. Hemingway, P. Volgyesi, L. Howard, K. Smyth"), S_OK );

	// Get the root folder and start building from there
	MetaFolder *rootFolder;
	EXPECT_EQ( metaProject->RootFolder(rootFolder), S_OK );
	EXPECT_EQ( metaProject->BeginTransaction(), S_OK );
	EXPECT_EQ( rootFolder->SetName("RootFolder"), S_OK );
	EXPECT_EQ( metaProject->CommitTransaction(), S_OK );

	// Save the built up MetaGMEv3 file -- we can use it later
	metaProject->Save("MetaGMEv3.mta");
	delete rootFolder;
	delete metaProject;
}


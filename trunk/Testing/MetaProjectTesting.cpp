/*** Included Header Files ***/
#include "MetaProjectTesting.h"
#include "../Meta/MetaFolder.h"
#include "../Meta/MetaConstraint.h"
#include "../Meta/MetaAttribute.h"
#include "../Meta/MetaFCO.h"


// --------------------------- MetaGMEv3 Built Programatically --------------------------- //


TEST(MetaProjectTest,CreateMetaGMEv3)
{
	// First, create a new MetaGMEv3.mta metaProject for us to work on
	Result_t result;
	MetaProject* metaProject = NULL;
	EXPECT_EQ( S_OK, MetaProject::Create("MGA=MetaGMEv3.mta", metaProject) );
	EXPECT_TRUE( metaProject != NULL );

	// Set all of the MetaProject attributes
	EXPECT_EQ( S_OK, metaProject->SetName("MetaGME") );
	EXPECT_EQ( S_OK, result = metaProject->SetUuid("{9D3F9884-FE60-409C-8FC1-45789193989B}") ) << GetErrorInfo(result);
	EXPECT_EQ( S_OK, metaProject->SetCreatedAt("Mon Jun 22 12:03:20 2009") );
	EXPECT_EQ( S_OK, metaProject->SetModifiedAt("Mon Jun 22 12:03:20 2009") );
	EXPECT_EQ( S_OK, metaProject->SetComment("GME Meta Paradigm") );
	EXPECT_EQ( S_OK, metaProject->SetAuthor("G. Hemingway, P. Volgyesi, L. Howard, K. Smyth") );

	// Get the root folder and start building from there
	MetaFolder *rootFolder;
	EXPECT_EQ( S_OK, metaProject->RootFolder(rootFolder) );
	EXPECT_EQ( S_OK, rootFolder->SetName("RootFolder") );

	MetaConstraint *objectInRootFolder;
	EXPECT_EQ( S_OK, rootFolder->CreateConstraint(objectInRootFolder) );
	EXPECT_EQ( S_OK, objectInRootFolder->SetName("ObjectInRootFolder") );
	EXPECT_EQ( S_OK, objectInRootFolder->SetDisplayedName("RootFolder has to contain at least either a Model or a Folder.") );
	EXPECT_EQ( S_OK, objectInRootFolder->SetEventMask(0x0) );
	EXPECT_EQ( S_OK, objectInRootFolder->SetDepth(CONSTRAINT_DEPTH_ZERO) );
	EXPECT_EQ( S_OK, objectInRootFolder->SetPriority(2) );
	EXPECT_EQ( S_OK, objectInRootFolder->SetExpression("let mFlags = project.allInstancesOf( Model ) -> exists( m : Model | m.InRootFolder ) in let fFlags = project.allInstancesOf( Folder ) -> exists( f : Folder | f.InRootFolder ) in mFlags or fFlags") );
	delete objectInRootFolder;

	MetaConstraint *allAscendants;
	EXPECT_EQ( S_OK, rootFolder->CreateConstraint(allAscendants) );
	EXPECT_EQ( S_OK, allAscendants->SetName("allAscendants") );
	EXPECT_EQ( S_OK, allAscendants->SetExpression("let kinds = Set{ \"Model\", \"Atom\", \"Set\", \"Reference\", \"Connection\",\"FCO\" } in \nif ( kinds.includes( self.kindName )) then \nself.ascendantEndsRecursive( Set{}, inhType ) \nelse \nSet{ } \nendif") );
	delete allAscendants;

	MetaConstraint *allAspects;
	EXPECT_EQ( S_OK, rootFolder->CreateConstraint(allAspects) );
	EXPECT_EQ( S_OK, allAspects->SetName("allAspects") );
	EXPECT_EQ( S_OK, allAspects->SetExpression("method;gme::FCO;;ocl::Set; \nself.allAscendants( 1 ) -> iterate( fco : gme::FCO ; accu = Set{} | accu + fco.memberOfSets() )") );
	delete allAspects;

	MetaAttribute *AddSetEvent;
	EXPECT_EQ( S_OK, rootFolder->CreateAttribute(AddSetEvent) );
	EXPECT_EQ( S_OK, AddSetEvent->SetName("AddSetEvent") );
	EXPECT_EQ( S_OK, AddSetEvent->SetDisplayedName("On include in set") );
	EXPECT_EQ( S_OK, AddSetEvent->SetValueType(ATTVAL_BOOLEAN) );
	EXPECT_EQ( S_OK, AddSetEvent->SetDefaultValue("false") );
	delete AddSetEvent;

	MetaAtom *Atom;
	EXPECT_EQ( S_OK, rootFolder->CreateAtom(Atom) );
	delete Atom;

	// Save the built up MetaGMEv3 file -- we can use it later
	metaProject->Save();
	delete rootFolder;
	delete metaProject;
}


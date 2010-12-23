/*** Included Header Files ***/
#include "MgaProjectTesting.h"
#include "../MGA/MgaFolder.h"
#include "../MGA/MgaRegistrar.h"


/*
TEST(MgaProjectTest,OpenProject)
{
	// First create an XML registry
	MgaRegistrar* registrar;
	Result_t result;
	ASSERT_EQ( result = XMLRegistrar::OpenRegistry("MGAConfig.xml", registrar), S_OK ) << GetErrorInfo(result);
	// Open test project
	MgaProject* mgaProject;
	ASSERT_EQ( result = MgaProject::Open("MGA=ESMoLv3.mga", registrar, false, mgaProject), S_OK ) << GetErrorInfo(result);
	
	// Ok, clean up
	if (mgaProject != NULL) delete mgaProject;
	if (registrar != NULL) delete registrar;
}



TEST(MgaProjectTest,GetRootFolder)
{
	Result_t result;
	// First create an XML registry
	MgaRegistrar* registrar;
	ASSERT_EQ( XMLRegistrar::OpenRegistry("MGAConfig.xml", registrar), S_OK );
	// Open test project
	MgaProject* mgaProject = NULL;
	ASSERT_EQ( MgaProject::Open("MGA=ESMoLv3.mga", registrar, false, mgaProject), S_OK );
	ASSERT_TRUE( mgaProject != NULL );

	// Begin a transaction
	MgaFolder* rootFolder;
	ASSERT_EQ( result = mgaProject->BeginTransaction(NULL, false), S_OK ) << GetMgaErrorInfo(result);
	ASSERT_EQ( result = mgaProject->GetRootFolder(rootFolder), S_OK ) << GetMgaErrorInfo(result);
	ASSERT_TRUE( rootFolder != NULL );
	ASSERT_EQ( result = mgaProject->CommitTransaction(), S_OK ) << GetMgaErrorInfo(result);
	
	// Ok, clean up
	delete rootFolder;
	if (mgaProject != NULL) delete mgaProject;
	if (registrar != NULL) delete registrar;
}


TEST(MgaProjectTest,GetChildFCOs)
{
	Result_t result;
	// First create an XML registry
	MgaRegistrar* registrar;
	ASSERT_EQ( XMLRegistrar::OpenRegistry("MGAConfig.xml", registrar), S_OK );
	// Open test project
	MgaProject* mgaProject = NULL;
	ASSERT_EQ( MgaProject::Open("MGA=ESMoLv3.mga", registrar, false, mgaProject), S_OK );
	ASSERT_TRUE( mgaProject != NULL );
	
	// Begin a transaction
	MgaFolder* rootFolder;
	ASSERT_EQ( result = mgaProject->BeginTransaction(NULL, false), S_OK ) << GetMgaErrorInfo(result);
	ASSERT_EQ( result = mgaProject->GetRootFolder(rootFolder), S_OK ) << GetMgaErrorInfo(result);
	ASSERT_TRUE( rootFolder != NULL );
	std::list<MgaFCO*> fcoList;
	ASSERT_EQ( result = rootFolder->GetChildFCOs(fcoList), S_OK ) << GetMgaErrorInfo(result);
	ASSERT_EQ( result = mgaProject->CommitTransaction(), S_OK ) << GetMgaErrorInfo(result);
	
	// Ok, clean up
	MgaFCO::ReleaseList(fcoList);
	delete rootFolder;
	if (mgaProject != NULL) delete mgaProject;
	if (registrar != NULL) delete registrar;
}
*/
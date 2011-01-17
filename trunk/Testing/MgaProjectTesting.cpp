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
	ASSERT_EQ( S_OK, result = XMLRegistrar::OpenRegistry("MGAConfig.xml", registrar) ) << GetErrorInfo(result);
	// Open test project
	MgaProject* mgaProject;
	ASSERT_EQ( S_OK, result = MgaProject::Open("MGA=ESMoLv3.mga", registrar, false, mgaProject) ) << GetErrorInfo(result);
	
	// Ok, clean up
	if (mgaProject != NULL) delete mgaProject;
	if (registrar != NULL) delete registrar;
}



TEST(MgaProjectTest,GetRootFolder)
{
	Result_t result;
	// First create an XML registry
	MgaRegistrar* registrar;
	ASSERT_EQ( S_OK, XMLRegistrar::OpenRegistry("MGAConfig.xml", registrar) );
	// Open test project
	MgaProject* mgaProject = NULL;
	ASSERT_EQ( S_OK, MgaProject::Open("MGA=ESMoLv3.mga", registrar, false, mgaProject) );
	ASSERT_TRUE( mgaProject != NULL );

	// Begin a transaction
	MgaFolder* rootFolder;
	ASSERT_EQ( S_OK, result = mgaProject->BeginTransaction(NULL, false) ) << GetMgaErrorInfo(result);
	ASSERT_EQ( S_OK, result = mgaProject->GetRootFolder(rootFolder) ) << GetMgaErrorInfo(result);
	ASSERT_TRUE( rootFolder != NULL );
	ASSERT_EQ( S_OK, result = mgaProject->CommitTransaction() ) << GetMgaErrorInfo(result);
	
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
	ASSERT_EQ( S_OK, XMLRegistrar::OpenRegistry("MGAConfig.xml", registrar) );
	// Open test project
	MgaProject* mgaProject = NULL;
	ASSERT_EQ( S_OK, MgaProject::Open("MGA=ESMoLv3.mga", registrar, false, mgaProject) );
	ASSERT_TRUE( mgaProject != NULL );
	
	// Begin a transaction
	MgaFolder* rootFolder;
	ASSERT_EQ( S_OK, result = mgaProject->BeginTransaction(NULL, false) ) << GetMgaErrorInfo(result);
	ASSERT_EQ( S_OK, result = mgaProject->GetRootFolder(rootFolder) ) << GetMgaErrorInfo(result);
	ASSERT_TRUE( rootFolder != NULL );
	std::list<MgaFCO*> fcoList;
	ASSERT_EQ( S_OK, result = rootFolder->GetChildFCOs(fcoList) ) << GetMgaErrorInfo(result);
	ASSERT_EQ( S_OK, result = mgaProject->CommitTransaction() ) << GetMgaErrorInfo(result);
	
	// Ok, clean up
	MgaFCO::ReleaseList(fcoList);
	delete rootFolder;
	if (mgaProject != NULL) delete mgaProject;
	if (registrar != NULL) delete registrar;
}
*/
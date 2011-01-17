/*** Included Header Files ***/
#include "MgaRegistrarTesting.h"



TEST(MgaRegistrarTest,OpenXMLRegistry)
{
	MgaRegistrar* registrar = NULL;
	// Open an XML registrar
	Result_t result;
	ASSERT_EQ( S_OK, XMLRegistrar::OpenRegistry("MGAConfig.xml", registrar) );
	ASSERT_TRUE( registrar != NULL );
	// Get info about the MetaGME paradigm
	std::string connectionString;
	std::string guid = "";
	EXPECT_EQ( S_OK, result = registrar->QueryParadigm("MetaGME", guid, connectionString) );
	if (registrar != NULL) delete registrar;
}
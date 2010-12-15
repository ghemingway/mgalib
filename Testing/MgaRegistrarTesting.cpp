/*** Included Header Files ***/
#include "MgaRegistrarTesting.h"



TEST(MgaRegistrarTest,OpenXMLRegistry)
{
	MgaRegistrar* registrar = NULL;
	// Open an XML registrar
	Result_t result = XMLRegistrar::OpenRegistry("MGAConfig.xml", registrar);
	ASSERT_EQ( result, S_OK );
	ASSERT_TRUE( registrar != NULL );
	// Get info about the MetaGME paradigm
	std::string connectionString;
	std::string guid = "";
	result = registrar->QueryParadigm("MetaGME", guid, connectionString);
	EXPECT_EQ( result, S_OK );
	if (registrar != NULL) delete registrar;
}
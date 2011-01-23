/*** Included Header Files ***/
#include "CoreMetaTesting.h"
#include "MGA/MgaGeneric.h"
#include "Meta/MetaGeneric.h"
#include "Core/CoreMetaAttribute.h"
#include "Core/CoreMetaObject.h"


// Initialize the static members outside of the class
CoreMetaProject* CoreMetaTest::metaCoreMetaProject = NULL;


// --------------------------- CoreMeta Tests --------------------------- //


TEST_F(CoreMetaTest,CreateMgaCoreMetaProject)
{
	// Create a new CoreMetaProject using extern function (expect success)
	Result_t result;
	CoreMetaProject* project = NULL;
	EXPECT_EQ( S_OK, result = CreateMGACoreMetaProject(project) ) << GetErrorInfo(result);
	ASSERT_TRUE(project != NULL);
	delete project;
}


TEST_F(CoreMetaTest,CreateMetaCoreMetaProject)
{
	// Create a new CoreMetaProject using extern function (expect success)
	Result_t result;
	EXPECT_EQ( S_OK, result = CreateMetaCoreMetaProject(CoreMetaTest::metaCoreMetaProject) ) << GetErrorInfo(result);
	ASSERT_TRUE(CoreMetaTest::metaCoreMetaProject != NULL);
}


// --------------------------- CoreMetaProject Tests --------------------------- //


TEST_F(CoreMetaTest,CoreMetaProject)
{
	Result_t result;
	Uuid uuid;
	Uuid metaUuid = "{DC5D7DEE-A590-C947-B707ADB3E98E2879}";
	ASSERT_TRUE( CoreMetaTest::metaCoreMetaProject != NULL );
	// Make sure we can get the Uuid of the project
	EXPECT_EQ( S_OK, result = CoreMetaTest::metaCoreMetaProject->GetUuid(uuid) ) << GetErrorInfo(result);
	EXPECT_EQ( metaUuid, uuid );

	// Test the token
	std::string token;
	std::string metaToken = "MetaCoreMetaProject";
	EXPECT_EQ( S_OK, result = CoreMetaTest::metaCoreMetaProject->GetToken(token) ) << GetErrorInfo(result);
	EXPECT_EQ( metaToken, token );

	// Test the name
	std::string name;
	std::string metaName = "Meta CoreMetaProject";
	EXPECT_EQ( S_OK, result = CoreMetaTest::metaCoreMetaProject->GetName(name) ) << GetErrorInfo(result);
	EXPECT_EQ( metaName, name );

	// Get a list of all objects
	std::list<CoreMetaObject*> objectList;
	EXPECT_EQ( S_OK, result = CoreMetaTest::metaCoreMetaProject->GetObjects(objectList) ) << GetErrorInfo(result);
	EXPECT_EQ( 19, objectList.size() );

	// Get a specific object
	CoreMetaObject *object = NULL;
	MetaID_t metaID = METAID_METAMODEL;
	EXPECT_EQ( S_OK, result = CoreMetaTest::metaCoreMetaProject->GetObject(metaID, object) ) << GetErrorInfo(result);
	EXPECT_TRUE( object != NULL );
}


// --------------------------- CoreMetaObject Tests --------------------------- //


TEST_F(CoreMetaTest,CoreMetaObject)
{
	Result_t result;
	Uuid uuid;
	ASSERT_TRUE( CoreMetaTest::metaCoreMetaProject != NULL );
	// Get an object for use in testing
	CoreMetaObject *object = NULL;
	MetaID_t modelMetaID = METAID_METAMODEL;
	EXPECT_EQ( S_OK, result = CoreMetaTest::metaCoreMetaProject->GetObject(modelMetaID, object) ) << GetErrorInfo(result);
	ASSERT_TRUE( object != NULL );

	// Test GetProject method
	CoreMetaProject *project = NULL;
	EXPECT_EQ( S_OK, result = object->GetProject(project) ) << GetErrorInfo(result);
	EXPECT_TRUE( CoreMetaTest::metaCoreMetaProject == project );

	// Test GetMetaID method
	MetaID_t metaID = METAID_NONE;
	EXPECT_EQ( S_OK, result = object->GetMetaID(metaID) ) << GetErrorInfo(result);
	EXPECT_EQ( modelMetaID, metaID );

	// Test GetToken method
	std::string token = "";
	std::string modelToken = "MetaModel";
	EXPECT_EQ( S_OK, result = object->GetToken(token) ) << GetErrorInfo(result);
	EXPECT_EQ( modelToken, token );

	// Test GetName method
	std::string name = "";
	std::string modelName = "Meta Model";
	EXPECT_EQ( S_OK, result = object->GetName(name) ) << GetErrorInfo(result);
	EXPECT_EQ( modelName, name );

	// Test GetAttribute list method
	std::list<CoreMetaAttribute*> attrList;
	EXPECT_EQ( S_OK, result = object->GetAttributes(attrList) ) << GetErrorInfo(result);
	EXPECT_EQ( 12, attrList.size() );

	// Get a specific attribute
	CoreMetaAttribute* attribute = NULL;
	AttrID_t attrID = ATTRID_REGISTRY;
	EXPECT_EQ( S_OK, result = object->GetAttribute(attrID, attribute) ) << GetErrorInfo(result);
	EXPECT_TRUE( attribute != NULL );
}


// --------------------------- CoreMetaAttribute Tests --------------------------- //


TEST_F(CoreMetaTest,CoreMetaAttribute)
{
	Result_t result;
	Uuid uuid;
	ASSERT_TRUE( CoreMetaTest::metaCoreMetaProject != NULL );
	// Get an attribute for use in testing
	CoreMetaObject *object = NULL;
	MetaID_t modelMetaID = METAID_METAMODEL;
	EXPECT_EQ( S_OK, result = CoreMetaTest::metaCoreMetaProject->GetObject(modelMetaID, object) ) << GetErrorInfo(result);
	ASSERT_TRUE( object != NULL );
	CoreMetaAttribute* attribute = NULL;
	AttrID_t registryAttrID = ATTRID_REGISTRY;
	EXPECT_EQ( S_OK, result = object->GetAttribute(registryAttrID, attribute) ) << GetErrorInfo(result);
	ASSERT_TRUE( attribute != NULL );

	// Test the GetObject method
	CoreMetaObject* metaObject = NULL;
	EXPECT_EQ( S_OK, result = attribute->GetObject(metaObject) ) << GetErrorInfo(result);
	EXPECT_EQ( object, metaObject );

	// Test the GetAttributeID method
	AttrID_t attrID = ATTRID_NONE;
	EXPECT_EQ( S_OK, result = attribute->GetAttributeID(attrID) ) << GetErrorInfo(result);
	EXPECT_EQ( registryAttrID, attrID );

	// Test the GetToken method
	std::string token = "";
	std::string attributeToken = "Registry";
	EXPECT_EQ( S_OK, result = attribute->GetToken(token) ) << GetErrorInfo(result);
	EXPECT_EQ( attributeToken, token );

	// Test GetName method
	std::string name = "";
	std::string attributeName = "Key-Value Registry";
	EXPECT_EQ( S_OK, result = attribute->GetName(name) ) << GetErrorInfo(result);
	EXPECT_EQ( attributeName, name );

	// Test GetValueType method
	ValueType valueType;
	EXPECT_EQ( S_OK, result = attribute->GetValueType(valueType) ) << GetErrorInfo(result);
	EXPECT_EQ( ValueType::Dictionary(), valueType );
}


// --------------------------- CoreMeta Tests --------------------------- //


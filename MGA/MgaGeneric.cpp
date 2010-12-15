/*** Included Header Files ***/
#include "MgaGeneric.h"


/*** Version 2 of the special sauce ***/
const Result_t CreateMGACoreMetaProject(const bool &v2, CoreMetaProject* &metaProject)
{
	GUID_t guid = {0x028F7444,0x2E51,0x11D3,{0xB3,0x6D,0x00,0x62,0x08,0x2D,0xF8,0x74}};
	metaProject = new CoreMetaProject("MgaProject", "MgaProject", guid);
	ASSERT( metaProject != NULL );

// ------- Macros

	Result_t result;
	CoreMetaObject* object;
	CoreMetaAttribute* attribute;

#define CREATE_OBJECT(metaid, token, name) \
	result = metaProject->AddObject(metaid, token, name, object); \
	ASSERT( result == S_OK );

#define CREATE_POINTER(metaid, token, name) \
	ASSERT(metaid == ATTRID_FATHER || metaid >= ATTRID_REF_MIN);\
	result = object->AddAttribute(metaid, token, name, ValueType::Pointer(), attribute); \
	ASSERT( result == S_OK );

#define CREATE_COLLECTION(metaid, token, name) \
	result = object->AddAttribute((metaid) + ATTRID_COLLECTION, token, name, ValueType::Collection(), attribute); \
	ASSERT( result == S_OK );

#define CREATE_ATTRIBUTE(metaid, token, name, valtype) \
	ASSERT(metaid < ATTRID_REF_MIN);\
	result = object->AddAttribute(metaid, token, name, valtype, attribute); \
	ASSERT( result == S_OK );

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// COMMON /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#define COMMON_DEF \
		CREATE_ATTRIBUTE(ATTRID_LOCK, "ObjLock", "Object Lock", ValueType::Lock());

//ROOT
		CREATE_OBJECT(DTID_ROOT, "Root", "Template For RootObject");
		COMMON_DEF
		CREATE_COLLECTION(ATTRID_FATHER, "DummyFather", "Dummy Father");
		CREATE_POINTER(ATTRID_FATHER, "DummyChildren", "Dummy Children");
		CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Project Name",			ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_CDATE, "CDate", "Create Date",				ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_MDATE, "MDate", "Modification Date",		ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_CREATOR, "Author", "Created by",			ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_GUID, "GUID", "GUID",					    ValueType::Binary());
		CREATE_ATTRIBUTE(ATTRID_VERSION, "Version", "Version",				ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_PARGUID, "ParadigmGUID", "Paradigm GUID",	ValueType::Binary());
		CREATE_ATTRIBUTE(ATTRID_PARVERSION, "ParadigmVersion", "Paradigm Version",	ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_PARADIGM, "ExtParadigm", "External Paradigm",ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_EXTDATA, "Comment", "Project Comments",			ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_MGAVERSION, "MGA_Version", "MGA Version Code", ValueType::Long());
		CREATE_COLLECTION(ATTRID_PARENT, "Children", "Child Objects");

//CONSTRAINT
		CREATE_OBJECT(DTID_CONSTRAINT, "Constraint", "Template For Constraint");
		COMMON_DEF
		CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Constraint Name",			ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_CONSTROCL, "ConstraintOCL", "Constraint Text",ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_CONSTRPRIORITY, "ConstrPriority", "Constraint Priority",ValueType::String());
		CREATE_POINTER(ATTRID_CONSTROWNER, "ConstraintOf", "Owner Kind");

//REGNODE
		CREATE_OBJECT(DTID_REGNODE, "RegNode", "Template For Registry Node");
		COMMON_DEF
		CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "RegNode Name",		ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_REGFLAGS, "RegistryFlags", "Registry Flags", ValueType::Long());
		CREATE_POINTER(ATTRID_REGNOWNER, "RegNodeOf", "Parent Object/Regnode");
		CREATE_COLLECTION(ATTRID_REGNOWNER, "RegNodes", "Registry Nodes");
		CREATE_ATTRIBUTE(ATTRID_REGNODEVALUE, "RegNodeValue", "RegNode Value",ValueType::String());
		CREATE_POINTER(ATTRID_XREF,	"FCOref", "Referenced FCO");

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// DATA ///////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#define GUID_ATTRS_DEFS \
		{ CREATE_ATTRIBUTE(ATTRID_GUID1, "GUID1", "FCO GUID1",	ValueType::Long());\
		CREATE_ATTRIBUTE(ATTRID_GUID2, "GUID2", "FCO GUID2",	ValueType::Long());\
		CREATE_ATTRIBUTE(ATTRID_GUID3, "GUID3", "FCO GUID3",	ValueType::Long());\
		CREATE_ATTRIBUTE(ATTRID_GUID4, "GUID4", "FCO GUID4",	ValueType::Long());}

#define STATUS_ATTR_DEF CREATE_ATTRIBUTE(ATTRID_FILESTATUS, "fstate", "Multiuser File Status", ValueType::Long());

//FOLDER
		CREATE_OBJECT(DTID_FOLDER, "Folder", "Template For Folder");
		COMMON_DEF
		CREATE_ATTRIBUTE(ATTRID_CDATE, "CDate", "Create Date",				ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_MDATE, "MDate", "Modification Date",		ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Folder Name",	ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_META, "Meta", "Meta Identifier",ValueType::Long());
		CREATE_ATTRIBUTE(ATTRID_LASTRELID, "LastRelID", "Last Child RelID",	ValueType::Long());
		CREATE_ATTRIBUTE(ATTRID_RELID, "RelID", "RelID",	ValueType::Long());
		CREATE_POINTER(ATTRID_PARENT, "Parent", "Parent Object");
		CREATE_COLLECTION(ATTRID_PARENT, "Children", "Child Objects");
		CREATE_COLLECTION(ATTRID_CONSTROWNER, "Constraints", "Constraints");
		CREATE_COLLECTION(ATTRID_REGNOWNER, "RegNodes", "Registry Nodes");
		CREATE_ATTRIBUTE(ATTRID_PERMISSIONS, "Permissions", "Permissions", ValueType::Long());
		if(v2) GUID_ATTRS_DEFS;
		if(v2) STATUS_ATTR_DEF;
		
//FCO
#define FCO_DEF \
		COMMON_DEF\
		CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "FCO Name",	ValueType::String());\
		CREATE_ATTRIBUTE(ATTRID_RELID, "RelID", "RelID",	ValueType::Long());\
		CREATE_ATTRIBUTE(ATTRID_META, "Meta", "Meta Identifier",ValueType::Long());\
		CREATE_ATTRIBUTE(ATTRID_ROLEMETA, "RoleMeta", "Role Meta Identifier",ValueType::Long());\
		CREATE_POINTER(ATTRID_FCOPARENT, "Parent", "Parent Object");\
		CREATE_COLLECTION(ATTRID_CONSTROWNER, "Constraints", "Constraints");\
		CREATE_COLLECTION(ATTRID_REGNOWNER, "RegNodes", "Registry Nodes");\
		CREATE_COLLECTION(ATTRID_REFERENCE, "References", "Referenced by");\
		CREATE_COLLECTION(ATTRID_XREF,		"XReferences", "Cross refs");\
		CREATE_COLLECTION(ATTRID_ATTRPARENT,"Attributes", "Attributes");\
		CREATE_POINTER(ATTRID_DERIVED, "BaseType", "BaseType");\
		CREATE_COLLECTION(ATTRID_DERIVED, "SubTypes", "SubTypes/Instances"); \
/*		CREATE_COLLECTION(ATTRID_REALOBJECT, "Aliases", "Aliases"); */ \
		CREATE_ATTRIBUTE(ATTRID_PERMISSIONS, "Permissions", "Permissions", ValueType::Long());

//MODEL
		CREATE_OBJECT(DTID_MODEL, "Model", "Template For Model");
		FCO_DEF
		CREATE_ATTRIBUTE(ATTRID_CDATE, "CDate", "Create Date",				ValueType::String());
		CREATE_ATTRIBUTE(ATTRID_MDATE, "MDate", "Modification Date",		ValueType::String());
		CREATE_COLLECTION(ATTRID_PARENT, "Children", "Child Objects");
		CREATE_ATTRIBUTE(ATTRID_LASTRELID, "LastRelID", "Last Child RelID",	ValueType::Long());
		if(v2) GUID_ATTRS_DEFS;
		if(v2) STATUS_ATTR_DEF;

//ATOM		
		CREATE_OBJECT(DTID_ATOM, "Atom", "Template For Atom");
		FCO_DEF
		if(v2) GUID_ATTRS_DEFS;

//REFERENCE		
		CREATE_OBJECT(DTID_REFERENCE, "Reference", "Template For Reference");
		FCO_DEF
		CREATE_POINTER(ATTRID_REFERENCE, "Refers", "Referenced FCO");
		CREATE_COLLECTION(ATTRID_SEGREF, "Segments", "Connection Segments");
		CREATE_POINTER(ATTRID_MASTEROBJ, "MasterObj", "Master Object");
		CREATE_COLLECTION(ATTRID_MASTEROBJ, "MasterOf", "Master Of Objects");
		CREATE_ATTRIBUTE(ATTRID_REFASPECT, "RefAspect", "Ref Aspect", ValueType::Long());
		if(v2) GUID_ATTRS_DEFS;
		
//SET		
		CREATE_OBJECT(DTID_SET, "Set", "Template For Set");
		FCO_DEF
		CREATE_COLLECTION(ATTRID_SETMEMBER, "Members", "Set Members");
		if(v2) GUID_ATTRS_DEFS;
/*
//ALIAS		
		CREATE_OBJECT(DTID_ALIASNODE, "AliasNode", "Template For Aliases");
		COMMON_DEF
		CREATE_ATTRIBUTE(ATTRID_ROLEMETA, "RoleMeta", "Role Meta Identifier",ValueType::MetaRef());\
		CREATE_POINTER(ATTRID_FCOPARENT, "Parent", "Parent of FCO");\
		CREATE_COLLECTION(ATTRID_REGNOWNER, "RegNodes", "Registry Nodes");\
		CREATE_POINTER(ATTRID_REALOBJECT, "RealObject", "Real Object");\
		CREATE_POINTER(ATTRID_DERIVED, "BaseType", "BaseType");\
		CREATE_COLLECTION(ATTRID_DERIVED, "SubTypes", "SubTypes/Instances"); \
		CREATE_ATTRIBUTE(ATTRID_PERMISSIONS, "IsInstance", "Is instance?", ValueType::Long());
*/
//SETNODE		
		CREATE_OBJECT(DTID_SETNODE, "SetNode", "Template For Set Member");
		COMMON_DEF
		CREATE_POINTER(ATTRID_SETMEMBER, "SetOwner", "Owner Set");
		CREATE_POINTER(ATTRID_XREF,		"FCOref", "Member FCO");
		CREATE_POINTER(ATTRID_MASTEROBJ, "MasterObj", "Master Object");
		CREATE_COLLECTION(ATTRID_MASTEROBJ, "MasterOf", "Master Of Objects");


//CONNECTION		
		CREATE_OBJECT(DTID_CONNECTION, "Connection", "Template For Connection");
		FCO_DEF
		CREATE_COLLECTION(ATTRID_CONNROLE, "ConnRoles", "ConnRoles");
		if(v2) GUID_ATTRS_DEFS;
		
//CONNECTION ROLE		
		CREATE_OBJECT(DTID_CONNROLE, "ConnRole", "Template for Connection Role");
		COMMON_DEF
		CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Connrole Name",ValueType::String());
		CREATE_POINTER(ATTRID_XREF,		"FCOref", "Connected FCO");
		CREATE_POINTER(ATTRID_CONNROLE, "CRoleParent", "Owner of ConnRole");
		CREATE_COLLECTION(ATTRID_CONNSEG, "ConnSegments", "Connection Segments");
		CREATE_POINTER(ATTRID_MASTEROBJ, "MasterObj", "Master Object");
		CREATE_COLLECTION(ATTRID_MASTEROBJ, "MasterOf", "Master Of Objects");

//CONNECTION ROLE SEGMENT
		CREATE_OBJECT(DTID_CONNROLESEG, "ConnRoleSeg", "Template for ConnRole Segment");
		COMMON_DEF
		CREATE_ATTRIBUTE(ATTRID_SEGORDNUM, "SegOrdNum", "Segment Order 1,2,3 ", ValueType::Long());
		CREATE_POINTER(ATTRID_CONNSEG, "ConnSRole", "Owner Connection Role");
		CREATE_POINTER(ATTRID_SEGREF, "ReferenceObj", "Reference Involved");

// ATTRIBUTE
#define ATTR_DEF \
		COMMON_DEF\
		CREATE_ATTRIBUTE(ATTRID_META, "Meta", "Meta Identifier",ValueType::Long());\
		CREATE_POINTER(ATTRID_ATTRPARENT,"Owner", "Owner FCO");\
//		CREATE_COLLECTION(ATTRID_CONSTROWNER, "Constraints", "Constraints");\
//		CREATE_COLLECTION(ATTRID_REGNOWNER, "RegNodes", "Registry Nodes");\

//STRATTR
		CREATE_OBJECT(DTID_STRATTR, "StrAttr", "String Attribute");
		ATTR_DEF
		CREATE_ATTRIBUTE(ATTRID_STRATTR, "StrValue", "String Value",ValueType::String());

//INTATTR
		CREATE_OBJECT(DTID_INTATTR, "IntAttr", "LongInt Attribute");
		ATTR_DEF
		CREATE_ATTRIBUTE(ATTRID_INTATTR, "IntValue", "LongInt Value",ValueType::Long());

//FLOATATTR
		CREATE_OBJECT(DTID_FLOATATTR, "FloatAttr", "Float Attribute");
		ATTR_DEF
		CREATE_ATTRIBUTE(ATTRID_FLOATATTR, "FloatValue", "Float Value",ValueType::Real());

//BOOLATTR
		CREATE_OBJECT(DTID_BOOLATTR, "BoolAttr", "Bool Attribute");
		ATTR_DEF
		CREATE_ATTRIBUTE(ATTRID_BOOLATTR, "BoolValue", "Bool Value",ValueType::Long());

//REFATTR
		CREATE_OBJECT(DTID_REFATTR, "RefAttr", "Reference Attribute");
		ATTR_DEF
		CREATE_POINTER(ATTRID_XREF,	"FCOref", "Referenced FCO");

	// Return the project
	return S_OK;
}



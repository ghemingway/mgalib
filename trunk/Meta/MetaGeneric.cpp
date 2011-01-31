/*** Included Header Files ***/
#include "MetaGeneric.h"


const Result_t MGA::CreateMetaCoreMetaProject(CoreMetaProject* &metaProject)
{
	Uuid uuid = "{DC5D7DEE-A590-C947-B707ADB3E98E2879}";
	metaProject = new CoreMetaProject("MetaCoreMetaProject", "Meta CoreMetaProject", uuid);
	ASSERT( metaProject != NULL );

// ------- Macros

	Result_t result;
	CoreMetaObject* object;
	CoreMetaAttribute* attribute;

#define CREATE_OBJECT(metaid, token, name) \
	result = metaProject->AddObject(metaid, token, name, object); \
	ASSERT( result == S_OK );

#define CREATE_POINTER(metaid, token, name) \
	result = object->AddAttribute(metaid, token, name, ValueType::Pointer(), attribute); \
	ASSERT( result == S_OK );
	
#define CREATE_COLLECTION(metaid, token, name) \
	result = object->AddAttribute((metaid) + ATTRID_COLLECTION, token, name, ValueType::Collection(), attribute); \
	ASSERT( result == S_OK );
	
#define CREATE_ATTRIBUTE(metaid, token, name, valtype) \
	result = object->AddAttribute(metaid, token, name, valtype, attribute); \
	ASSERT( result == S_OK );

// ------- Base

#define DECLARE_BASE() \
	CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Name", ValueType::String()); \
	CREATE_ATTRIBUTE(ATTRID_DISPNAME, "DispName", "Displayed Name", ValueType::String()); \
	CREATE_ATTRIBUTE(ATTRID_REGISTRY, "Registry", "Key-Value Registry", ValueType::Dictionary()); \
	CREATE_COLLECTION(ATTRID_CONSTRAINT_PTR, "Constraints", "Constraints");

// ------- Folder

#define DECLARE_FOLDER() \
	DECLARE_BASE(); \
	CREATE_POINTER(ATTRID_FOLDER_PTR, "FolderParent", "Defined In"); \
	CREATE_COLLECTION(ATTRID_FOLDER_PTR, "ChildFolders", "Child Folders"); \
	CREATE_COLLECTION(ATTRID_DEFFCO_PTR, "DefinedFCOs", "Defined FCOs"); \
	CREATE_COLLECTION(ATTRID_DEFATTR_PTR, "DefinedAttributes", "Defined Attributes"); \
	CREATE_COLLECTION(ATTRID_FOLDERLINK_PARENT_PTR, "LegalFolders", "Legal Child Folders"); \
	CREATE_COLLECTION(ATTRID_FOLDERLINK_CHILD_PTR, "LegalParents", "Used In Folders"); \
	CREATE_COLLECTION(ATTRID_ROOTOBJLINK_FOLDER_PTR, "ObjectLinks", "Legal Root Objects");

	CREATE_OBJECT(METAID_METAFOLDER, "MetaFolder", "Meta Folder");
	DECLARE_FOLDER();

	CREATE_OBJECT(METAID_METAFOLDERLINK, "MetaFolderLink", "Meta Folder Link");
	CREATE_POINTER(ATTRID_FOLDERLINK_PARENT_PTR, "Parent", "Parent");
	CREATE_POINTER(ATTRID_FOLDERLINK_CHILD_PTR, "Child", "Child");

	CREATE_OBJECT(METAID_METAROOTOBJLINK, "MetaRootObjLink", "Meta Root Object Link");
	CREATE_POINTER(ATTRID_ROOTOBJLINK_FOLDER_PTR, "Folder", "Folder");
	CREATE_POINTER(ATTRID_ROOTOBJLINK_OBJECT_PTR, "Object", "Object");

// ------- Project

	CREATE_OBJECT(METAID_METAPROJECT, "MetaProject", "Meta Project");
	DECLARE_FOLDER();
	CREATE_ATTRIBUTE(ATTRID_PARNAME, "ParName", "Paradigm Name", ValueType::String());
	CREATE_ATTRIBUTE(ATTRID_PARDISPNAME, "ParDispName", "Displayed Name", ValueType::String());
	CREATE_ATTRIBUTE(ATTRID_UUID, "ParUUID", "Paradigm UUID", ValueType::LongPointer());
	CREATE_ATTRIBUTE(ATTRID_VERSION, "Version", "Paradigm Version", ValueType::String());
	CREATE_ATTRIBUTE(ATTRID_AUTHOR, "Author", "Paradigm Author", ValueType::String());
	CREATE_ATTRIBUTE(ATTRID_COMMENT, "Comment", "Comment", ValueType::String());
	CREATE_ATTRIBUTE(ATTRID_CDATE, "CDate", "Create Date", VALTYPE_DATE);
	CREATE_ATTRIBUTE(ATTRID_MDATE, "MDate", "Modification Date", VALTYPE_DATE);

// ------- FCO

#define DECLARE_FCO() \
	DECLARE_BASE() \
	CREATE_POINTER(ATTRID_DEFFCO_PTR, "ObjectParent", "Defined In"); \
	CREATE_COLLECTION(ATTRID_DEFATTR_PTR, "DefinedAttributes", "Defined Attributes"); \
	CREATE_COLLECTION(ATTRID_KIND_PTR, "UsedInRoles", "Used In Roles"); \
	CREATE_COLLECTION(ATTRID_ROOTOBJLINK_OBJECT_PTR, "FolderLinks", "Used In Folders"); \
	CREATE_COLLECTION(ATTRID_ATTRLINK_USEDIN_PTR, "Attributes", "Attributes");

// ------- Model

	CREATE_OBJECT(METAID_METAMODEL, "MetaModel", "Meta Model");
	DECLARE_FCO();
	CREATE_COLLECTION(ATTRID_DEFFCO_PTR, "DefinedFCOs", "Defined FCOs");
	CREATE_COLLECTION(ATTRID_ROLES_COLL, "Roles", "Roles");
	CREATE_COLLECTION(ATTRID_ASPECTS_COLL, "Aspects", "Aspects");

// ------- Aspect

	CREATE_OBJECT(METAID_METAASPECT, "MetaAspect", "Meta Aspect");
	DECLARE_BASE();
	CREATE_POINTER(ATTRID_ASPECTS_COLL, "Model", "Model");
	CREATE_COLLECTION(ATTRID_PARTASPECT_PTR, "AspectParts", "Parts");
	CREATE_COLLECTION(ATTRID_ATTRLINK_USEDIN_PTR, "Attributes", "Attributes");

// ------- Role

	CREATE_OBJECT(METAID_METAROLE, "MetaRole", "Meta Role");
	DECLARE_BASE();
	CREATE_POINTER(ATTRID_ROLES_COLL, "ParentModel", "Parent Model");
	CREATE_COLLECTION(ATTRID_PARTROLE_PTR, "RoleParts", "Parts");
	CREATE_POINTER(ATTRID_KIND_PTR, "Kind", "Kind");

// ------- Part

	CREATE_OBJECT(METAID_METAPART, "MetaPart", "Meta Part");
	DECLARE_BASE();
	CREATE_POINTER(ATTRID_PARTROLE_PTR, "Role", "Role");
	CREATE_POINTER(ATTRID_PARTASPECT_PTR, "Aspect", "Aspect");
	CREATE_ATTRIBUTE(ATTRID_PARTDATA, "Data", "Part Data", ValueType::Long());
	CREATE_ATTRIBUTE(ATTRID_KINDASPECT, "KindAspect", "Kind Aspect", ValueType::String());

// ------- Atom

	CREATE_OBJECT(METAID_METAATOM, "MetaAtom", "Meta Atom");
	DECLARE_FCO();

// ------- Attribute

	CREATE_OBJECT(METAID_METAATTRIBUTE, "MetaAttribute", "Meta Attribute");
	DECLARE_BASE();
	CREATE_POINTER(ATTRID_DEFATTR_PTR, "AttrParent", "Defined In");
	CREATE_COLLECTION(ATTRID_ATTRLINK_ATTR_PTR, "AttrLinks", "Used In");
	CREATE_ATTRIBUTE(ATTRID_VALUE, "Value", "Default Value", ValueType::String());
	CREATE_ATTRIBUTE(ATTRID_ATTVALTYPE, "ValType", "Value Type", ValueType::Long());
	CREATE_ATTRIBUTE(ATTRID_VIEWABLE, "Viewable", "Viewable", ValueType::Long());
	CREATE_COLLECTION(ATTRID_ENUMITEMS_COLL, "EnumItems", "Enum Items");

	CREATE_OBJECT(METAID_METAATTRLINK, "MetaAttrLink", "Meta Attribute Link");
	CREATE_POINTER(ATTRID_ATTRLINK_USEDIN_PTR, "AttrUsedIn", "Used In");
	CREATE_POINTER(ATTRID_ATTRLINK_ATTR_PTR, "Attribute", "Attribute");

// ------- PointerSpec

#define DECLARE_POINTERSPEC() \
	CREATE_ATTRIBUTE(ATTRID_PTRSPECNAME, "SpecName", "Pointer Spec Name", ValueType::String()); \
	CREATE_COLLECTION(ATTRID_PTRITEMS_COLL, "Items", "Pointer Items");

	CREATE_OBJECT(METAID_METAPOINTERSPEC, "MetaPointerSpec", "Meta Pointer Specification");
	DECLARE_POINTERSPEC();
	CREATE_POINTER(ATTRID_PTRSPECS_COLL, "SpecParent", "Parent");

// ------- PointerItem

	CREATE_OBJECT(METAID_METAPOINTERITEM, "MetaPointerItem", "Meta Pointer Item");
	CREATE_POINTER(ATTRID_PTRITEMS_COLL, "ItemParent", "Parent");
	CREATE_ATTRIBUTE(ATTRID_PTRITEMDESC, "Desc", "Description", ValueType::String());

// ------- Reference

	CREATE_OBJECT(METAID_METAREFERENCE, "MetaReference", "Meta Reference");
	DECLARE_FCO();
	DECLARE_POINTERSPEC();

// ------- Set

	CREATE_OBJECT(METAID_METASET, "MetaSet", "Meta Set");
	DECLARE_FCO();
	DECLARE_POINTERSPEC();

// ------- Connection

	CREATE_OBJECT(METAID_METACONNECTION, "MetaConnection", "Meta Connection");
	DECLARE_FCO();
	CREATE_COLLECTION(ATTRID_CONNJOINTS_COLL, "Joints", "Connection Joints");

// ------- ConnJoint

	CREATE_OBJECT(METAID_METACONNJOINT, "MetaConnJoint", "Meta Connection Joint");
	CREATE_POINTER(ATTRID_CONNJOINTS_COLL, "JointParent", "Parent");
	CREATE_COLLECTION(ATTRID_PTRSPECS_COLL, "Specs", "Pointer Specifications");


// ------- EnumItem

	CREATE_OBJECT(METAID_METAENUMITEM, "MetaEnumItem", "Meta Registry Node");
	CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Displayed Name", ValueType::String());
	CREATE_ATTRIBUTE(ATTRID_VALUE, "Value", "Value", ValueType::String());
	CREATE_POINTER(ATTRID_ENUMITEMS_COLL, "EnumItemParent", "Parent");

// ------- Constraint

	CREATE_OBJECT(METAID_METACONSTRAINT, "MetaConstraint", "Meta Constraint");
	CREATE_POINTER(ATTRID_CONSTRAINT_PTR, "ConstraintParent", "Constraint Parent");
	CREATE_ATTRIBUTE(ATTRID_NAME, "Name", "Name", ValueType::String());
	CREATE_ATTRIBUTE(ATTRID_DISPNAME, "DispName", "Displayed Name", ValueType::String());
	CREATE_ATTRIBUTE(ATTRID_CONSEXPR, "Expression", "Constraint Expression", ValueType::String());
	CREATE_ATTRIBUTE(ATTRID_CONSMASK, "EventMask", "Constraint Event Mask", ValueType::Long());
	CREATE_ATTRIBUTE(ATTRID_CONSDEPTH, "Depth", "Constraint Depth", ValueType::Long());
	CREATE_ATTRIBUTE(ATTRID_CONSPRIORITY, "Priority", "Constraint Priority", ValueType::Long());
	CREATE_ATTRIBUTE(ATTRID_CONSTYPE, "Type", "Constraint Type", ValueType::Long());
	CREATE_ATTRIBUTE(ATTRID_CONSNAMESPC, "DesiredNamespace", "DesiredNamespace", ValueType::String());

	return S_OK;
}


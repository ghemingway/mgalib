/*** Included Header Files ***/
#include "MgaTypes.h"


typedef struct MgaErrorCodes {
	unsigned int		code;
	std::string			helpText;
} MgaErrorCodes;

MgaErrorCodes mga_descs[] = {
	{E_MGA_NOT_IMPLEMENTED, "This method is not yet implemented"},
	{E_MGA_NOT_SUPPORTED, "This mode of operation is not supported"},
	{E_MGA_MODULE_INCOMPATIBILITY, "MGA data structure conflict: error or incompatibility"},
	{E_MGA_PARADIGM_NOTREG, "The paradigm is not registered"},
	{E_MGA_PARADIGM_INVALID, "The paradigm is invalid"},
	{E_MGA_COMPONENT_ERROR, "Some addons cannot be loaded/initialized"},
	{E_MGA_DATA_INCONSISTENCY, "Inconsistent data in database metainfo violated"},
	{E_MGA_META_INCOMPATIBILITY, "Meta incompatibility"},
	{E_MGA_PROJECT_OPEN, "Project must be closed"},
	{E_MGA_PROJECT_NOT_OPEN, "Project must be open"},
	{E_MGA_READ_ONLY_ACCESS, "Database is in read-only mode"},
	{E_MGA_NOT_IN_TERRITORY, "Object is unknown in this territory"},
	{E_MGA_NOT_IN_TRANSACTION, "Operation must execute within transaction"},
	{E_MGA_ALREADY_IN_TRANSACTION, "The project is already in transaction"},
	{E_MGA_MUST_ABORT, "This transaction must be aborted"},
	{E_MGA_TARGET_DESTROYED, "Territory/AddOn already destroyed"},
	{E_MGA_FOREIGN_PROJECT, "This is not the callee object's project"},
	{E_MGA_FOREIGN_OBJECT, "Object does not belong to this project"},
	{E_MGA_OBJECT_DELETED, "The object is deleted partially inaccessible"},
	{E_MGA_OBJECT_ZOMBIE, "The object is deleted and fully inaccessible"},
	{E_MGA_ZOMBIE_NOPROJECT, "The project for this (sub)object has been destroyed"},
	{E_MGA_ZOMBIE_CLOSED_PROJECT, "The project for this (sub)object has been closed"},
	{E_MGA_INVALID_ARG, "invalid argument"},
	{E_MGA_ARG_RANGE, "argument out of range"},
	{E_MGA_INPTR_NULL, "NULL Input COM pointer"},
	{E_MGA_OUTPTR_NONEMPTY, "Nonempty COM pointer supplied for output"},
	{E_MGA_OUTPTR_NULL, "NULL output COM pointer address"},
	{E_MGA_NAME_NOT_FOUND, "Search by name failed"},
	{E_MGA_NAME_DUPLICATE, "The operation caused name uniqueness conflict"},
	{E_MGA_META_VIOLATION, "The operation caused meta violation for object %o"},
	{E_MGA_NOT_CHANGEABLE, "Object cannot be changed"},
	{E_MGA_OP_REFUSED, "Operation refused due to the state of existing data"},
	{E_MGA_LIBOBJECT, "Object belongs to a lib image"},
	{E_META_INVALIDATTR, "Unknown attribute specified"},
	{E_META_INVALIDASPECT, "Unknown aspect specified"},
	{E_MGA_ACCESS_TYPE, "Access method conflicts with attribute type"},
	{E_MGA_BAD_ENUMVALUE, "Invalid value for enum attr"},
	{E_MGA_REFPORTS_USED, "Reference %o is used for connections"},
	{E_MGA_VIRTUAL_NODE, "Node is not a physical, but inherited"},
	{E_MGA_BAD_POSITIONVALUE, "Bad GME position read/written"},
	{E_MGA_ROOTFCO, "Operation not allowed for root FCOs"},
	{E_MGA_NOT_ROOTOBJECT, "Operation not allowed for non-root FCOs"},
	{E_MGA_BAD_COLLENGTH, "Collection parameter counts must match"},
	{E_MGA_INVALID_ROLE, "Bad/NULL metarole or metapart"},
	{E_MGA_BAD_ID, "Object ID format error"},
	{E_MGA_NOT_DERIVABLE, "Non-root object, or derived into itself"},
	{E_MGA_OBJECT_NOT_MEMBER, "%o is not a member in set"},
	{E_MGA_NOT_CONNECTIBLE, "%o is not in a connectible position"},
	{E_MGA_NO_ROLE, "Role not specified"},
	{E_MGA_CONNROLE_USED, "Connrole allowed number exceeded for %o"},
	{E_MGA_NOT_DERIVED, "Object is not derived"},
	{E_MGA_NOT_INSTANCE, "Object is not an instance"},
	{E_MGA_INSTANCE, "Object is a type"},
	{E_MGA_REFERENCE_EXPECTED, "Object %o specified is not a reference"},
	{E_MGA_FILTERFORMAT, "MgaFilter property format error"},
	{E_MGA_GEN_OUT_OF_SPACE, "Cannot generate a unique value"},
	{E_MGA_INVALID_TARGET, "Invalid reference target"},
	{E_MGA_LONG_DERIVCHAIN, "Derive chain too long"},
	{E_MGA_LIB_DIFF, "Invalid changes in library"},
	{E_MGA_BAD_MASKVALUE, "Invalid event mask value"},
	{E_MGA_CONSTRAINT_VIOLATION, "Constraint violation"},
	{E_MGA_ERRCODE_ERROR, "Errcode out of errcode range"}
};


const std::string GetMgaErrorInfo(const Result_t &result)
{
	std::string desc = "Unknown Error Code";
//	if( E_COMMON_FIRST <= result && result <= E_COMMON_LAST ) desc = common_descs[result - E_COMMON_FIRST];
//	else if( E_CORE_FIRST <= result && result <= E_CORE_LAST ) desc = core_descs[result - E_CORE_FIRST];
//	else if( E_META_FIRST <= result && result <= E_META_LAST ) desc = meta_descs[result - E_META_FIRST];
//	else if( E_PARSER_FIRST <= result && result <= E_PARSER_LAST ) desc = parser_descs[result - E_PARSER_FIRST];
//	else if( result == S_OK ) desc = "No Error";
	return desc;
}
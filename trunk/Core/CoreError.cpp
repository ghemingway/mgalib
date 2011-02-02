/*** Included Header Files ***/
#include "CoreError.h"


// --------------------------- Error Codes --------------------------- //


std::string common_descs[] = 
{
	"Improper use of object",
	"Unknown exception",
	"Data conversion has failed",
	"Requested != count in GetAll",
	"[out] parameter is not empty",
	"Passed interface is not from the same project",
	"Object was not found",
	"File could not be opened",
	"VERIFY has failed",
	"COMTHROW test exception thrown"
};


std::string core_descs[] = 
{
	"Object is locked by another Core component",
	"Lock value in Storage is invalid",
	"Attribute is not found",
	"Meta object is not found",
	"Meta project is invalid",
	"Invalid response from Repository",
	"CoreProject - CoreMetaProject mismatch",
	"Object is deleted or unloaded (zombie)",
	"Invalid ValueType",
	"No active transaction",
	"Object already exists",
	"No Territory has been selected",
	"Object is not locked",
	"Invalid data in Repository",
	"Cannot delete Object, has nonempty collection",
	"Cannot resolve the connection string",
	"Invalid binary file storage",
	"Project has no name",
	"String is not valid UTF-8 encoded"
	"Write attempt in a read-only transaction"
};


std::string meta_descs[] = 
{
	"Invalid ATTVAL type",
	"MetaProject is not open",
	"Invalid Path",
	"Invalid MetaRef"
};


std::string parser_descs[] = 
{
	"Invalid XML file"
	"Invalid DTD file",
	"Invalid GUID",
	"Invalid XML filename",
	"Invalid MGA object",
	"Invalid META object",
	"Too many passes while parsing the XML file",
	"Invalid long value in XML file"
};


const std::string MGA::GetErrorInfo(const Result_t &result)
{
	std::string desc = "Unknown Error Code";
	if( E_COMMON_FIRST <= result && result <= E_COMMON_LAST ) desc = common_descs[result - E_COMMON_FIRST];
	else if( E_CORE_FIRST <= result && result <= E_CORE_LAST ) desc = core_descs[result - E_CORE_FIRST];
	else if( E_META_FIRST <= result && result <= E_META_LAST ) desc = meta_descs[result - E_META_FIRST];
	else if( E_PARSER_FIRST <= result && result <= E_PARSER_LAST ) desc = parser_descs[result - E_PARSER_FIRST];
	else if( result == S_OK) desc = "No Error";
	return desc;
}


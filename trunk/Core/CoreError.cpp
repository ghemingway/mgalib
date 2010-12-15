/*** Included Header Files ***/
#include "CoreError.h"


// --------------------------- Error Codes ---------------------------


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
	"Project has no name"
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
	"Invalid DTD file",
	"Invalid GUID",
	"Invalid XML filename",
	"Invalid MGA object",
	"Invalid META object",
	"Too many passes while parsing the XML file",
	"Invalid long value in XML file"
};


const std::string GetErrorInfo(const Result_t &result)
{
	std::string desc = "Unknown Error Code";
	if( E_COMMON_FIRST <= result && result <= E_COMMON_LAST ) desc = common_descs[result - E_COMMON_FIRST];
	else if( E_CORE_FIRST <= result && result <= E_CORE_LAST ) desc = core_descs[result - E_CORE_FIRST];
	else if( E_META_FIRST <= result && result <= E_META_LAST ) desc = meta_descs[result - E_META_FIRST];
	else if( E_PARSER_FIRST <= result && result <= E_PARSER_LAST ) desc = parser_descs[result - E_PARSER_FIRST];
	else if( result == S_OK ) desc = "No Error";
	return desc;
}


// -------------------------


#include "CoreTypes.h"


// Convert from the vector rep to a GUID_t
GUID_t& GUID_t::operator=(const std::vector<unsigned char> &bin)
{
	if (bin.size() != sizeof(GUID_t)) return *this;
	// Copy all 16 bytes into the GUID_t structure
	memcpy(this, &(bin[0]), sizeof(GUID_t));
	return *this;
}


// Convert the GUID_t to a vector rep.
GUID_t::operator std::vector<unsigned char>() const
{
	std::vector<unsigned char> value;
	value.resize(sizeof(GUID_t));
	memcpy(&(value[0]), this, sizeof(GUID_t));
	return value;
}


static inline bool _IsDash(char c)		{ return c == '-'; }
static inline bool _IsOpenBrace(char c)	{ return (c == '{'); }
static inline bool _CheckCloseBrace(char c, char openBrace)
{
	if (openBrace == '{' && c == '}') return true;
	return false;
}
static unsigned char _GetValue(char c)
{
	static char const*const digits_begin = "0123456789abcdefABCDEF";
	static char const*const digits_end = digits_begin + 22;
	static unsigned char const values[] =
	{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,10,11,12,13,14,15
		, static_cast<unsigned char>(-1) };
	char const* d = std::find(digits_begin, digits_end, c);
	return values[d - digits_begin];
}


// Convert from the standardized string rep to a GUID_t
GUID_t& GUID_t::operator=(const std::string &str)
{
	std::string tmpStr = str;
	// Remove all braces and dashes
	size_t pos;
	while ((pos = tmpStr.find_first_of("{-}")) != tmpStr.npos)
	{
		tmpStr.erase(pos, 1);
	}
	ASSERT( tmpStr.size() == 32 );
	// Break the string into the DataX components
	std::string data1 = tmpStr.substr(0, 8);
	std::string data2 = tmpStr.substr(8, 4);
	std::string data3 = tmpStr.substr(12, 4);
	std::string data4_1 = tmpStr.substr(16, 2);
	std::string data4_2 = tmpStr.substr(18, 2);
	std::string data4_3 = tmpStr.substr(20, 2);
	std::string data4_4 = tmpStr.substr(22, 2);
	std::string data4_5 = tmpStr.substr(24, 2);
	std::string data4_6 = tmpStr.substr(26, 2);
	std::string data4_7 = tmpStr.substr(28, 2);
	std::string data4_8 = tmpStr.substr(30, 2);
	// Copy data into the guid structure
	std::stringstream ssDdata1(data1);
	ssDdata1 >> std::hex >> this->Data1;
	std::stringstream ssData2(data2);
	ssData2 >> std::hex >> this->Data2;
	std::stringstream ssData3(data3);
	ssData3 >> std::hex >> this->Data3;
	int16_t tmp;
	std::stringstream ssData4_1(data4_1);
	ssData4_1 >> std::hex >> tmp;
	this->Data4[0] = tmp;
	std::stringstream ssData4_2(data4_2);
	ssData4_2 >> std::hex >> tmp;
	this->Data4[1] = tmp;
	std::stringstream ssData4_3(data4_3);
	ssData4_3 >> std::hex >> tmp;
	this->Data4[2] = tmp;
	std::stringstream ssData4_4(data4_4);
	ssData4_4 >> std::hex >> tmp;
	this->Data4[3] = tmp;
	std::stringstream ssData4_5(data4_5);
	ssData4_5 >> std::hex >> tmp;
	this->Data4[4] = tmp;
	std::stringstream ssData4_6(data4_6);
	ssData4_6 >> std::hex >> tmp;
	this->Data4[5] = tmp;
	std::stringstream ssData4_7(data4_7);
	ssData4_7 >> std::hex >> tmp;
	this->Data4[6] = tmp;
	std::stringstream ssData4_8(data4_8);
	ssData4_8 >> std::hex >> tmp;
	this->Data4[7] = tmp;
	// Off we go
	return *this;
}


// Convert the GUID_t to the standardized string representation (all caps A-F)
GUID_t::operator std::string() const
{
	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	ss << std::uppercase << std::hex << std::setfill('0');
	ss << "{" << std::setw(8) << this->Data1;
	ss << "-" << std::setw(4) << this->Data2;
	ss << "-" << std::setw(4) << this->Data3 << "-";
	// Copy Data4 bytes into stream
	uint16_t c;
	for (int i=0; i<8; ++i)
	{
		// Need to swap byte ordering (use of uint16_t is due to << behaving badly with char)
//		c = (this->Data4[i] >> 4) | ((this->Data4[i] & 0x0F) << 4);
		// Don't swap byte ordering - legacy?
		c = this->Data4[i];
		ss << std::hex << std::setw(2) << c;
		if (i == 1) ss << "-";
	}
	ss << "}";
	// Convert the stringstream to a std::string
	std::string result = ss.str();
//	std::cout << "String Conversion: " << result << std::endl;
	return result;
}


std::vector<unsigned char> GUID_t::ToBinary(const std::string &guid)
{
	// First convert the string to a GUID_t
	GUID_t guidStruct;
	guidStruct = guid;
	// Finally, conver the GUID_t to a vector
	std::vector<unsigned char> value = guidStruct;
	return value;
}


std::string GUID_t::ToString(const std::vector<unsigned char> &guid)
{
	// First convert the vector to a GUID_t
	GUID_t guidStruct;
	guidStruct = guid;
	// Finally, convert the GUID_t to a string
	std::string value = guidStruct;
	return value;
}


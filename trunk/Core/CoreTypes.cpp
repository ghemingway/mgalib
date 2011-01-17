/*** Included Header Files ***/
#include "CoreTypes.h"


// --------------------------- UUID --------------------------- //


Uuid::Uuid()
{
#ifdef __APPLE__
	// Let Apple's implementation create the unique identifier
	CFUUIDRef cfuuidRef = CFUUIDCreate(0);
	CFUUIDBytes cfuuid = CFUUIDGetUUIDBytes(cfuuidRef);
	// Now we copy the 16bytes into our class
	memcpy(this->_data, &cfuuid, sizeof(CFUUIDBytes));
	// Make sure to release 
	CFRelease(cfuuidRef);
#elif _WIN32
	// Needs implementation
	ASSERT(false);
#elif __linux__
	// Needs implementation
	ASSERT(false);
#endif
}


Uuid::Uuid(const std::vector<unsigned char> &vector)
{
	// Just use the equals operator
	*this = vector;
}


Uuid::Uuid(const char* string)
{
	// Convert the char* into a std::string
	std::string strValue(string);
	*this = strValue;
}


Uuid::Uuid(const std::string &string)
{
	// Just use the equals operator
	*this = string;
}


Uuid& Uuid::operator=(const Uuid &uuid)
{
	// Copy all 16 bytes into the Uuid data structure
	memcpy(this, &uuid, sizeof(Uuid));
	return *this;
}


Uuid& Uuid::operator=(const std::vector<unsigned char> &vector)
{
	if (vector.size() != sizeof(Uuid)) return *this;
	// Copy all 16 bytes into the Uuid data structure
	memcpy(this, &(vector[0]), sizeof(Uuid));
	return *this;
}


Uuid& Uuid::operator=(const std::string &string)
{
	std::string tmpStr = string;
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

	// Copy data into the UUID structure
	uint32_t firstValue;
	std::stringstream ssDdata1(data1);
	ssDdata1 >> std::hex >> firstValue;
	memcpy(this->_data, &firstValue, sizeof(firstValue));

	uint16_t secondValue;
	std::stringstream ssData2(data2);
	ssData2 >> std::hex >> secondValue;
	memcpy(this->_data+4, &secondValue, sizeof(secondValue));

	uint16_t thirdValue;
	std::stringstream ssData3(data3);
	ssData3 >> std::hex >> thirdValue;
	memcpy(this->_data+6, &thirdValue, sizeof(thirdValue));

	uint16_t tmp;
	std::stringstream ssData4_1(data4_1);
	ssData4_1 >> std::hex >> tmp;
	this->_data[8] = (uint8_t)tmp;
	std::stringstream ssData4_2(data4_2);
	ssData4_2 >> std::hex >> tmp;
	this->_data[9] = (uint8_t)tmp;
	std::stringstream ssData4_3(data4_3);
	ssData4_3 >> std::hex >> tmp;
	this->_data[10] = (uint8_t)tmp;
	std::stringstream ssData4_4(data4_4);
	ssData4_4 >> std::hex >> tmp;
	this->_data[11] = (uint8_t)tmp;
	std::stringstream ssData4_5(data4_5);
	ssData4_5 >> std::hex >> tmp;
	this->_data[12] = (uint8_t)tmp;
	std::stringstream ssData4_6(data4_6);
	ssData4_6 >> std::hex >> tmp;
	this->_data[13] = (uint8_t)tmp;
	std::stringstream ssData4_7(data4_7);
	ssData4_7 >> std::hex >> tmp;
	this->_data[14] = (uint8_t)tmp;
	std::stringstream ssData4_8(data4_8);
	ssData4_8 >> std::hex >> tmp;
	this->_data[15] = (uint8_t)tmp;
	// Off we go
	return *this;
}


Uuid::operator std::vector<unsigned char>() const
{
	// Create a vector of the correct size
	std::vector<unsigned char> value;
	value.resize(sizeof(Uuid));
	// Copy the memory in and be done
	memcpy(&(value[0]), this, sizeof(Uuid));
	return value;
}


Uuid::operator std::string() const
{
	std::stringstream ss(std::stringstream::in | std::stringstream::out);
	ss << std::uppercase << std::hex << std::setfill('0');

	uint32_t firstValue;
	memcpy(&firstValue, this->_data, sizeof(firstValue));
	ss << "{" << std::setw(8) << firstValue;

	uint16_t secondValue;
	memcpy(&secondValue, this->_data+4, sizeof(secondValue));
	ss << "-" << std::setw(4) << secondValue;

	uint16_t thirdValue;
	memcpy(&thirdValue, this->_data+6, sizeof(thirdValue));
	ss << "-" << std::setw(4) << thirdValue << "-";

	// Copy Data4 bytes into stream
	uint16_t c;
	for (int i=8; i<16; ++i)
	{
		// Need to swap byte ordering (use of uint16_t is due to << behaving badly with char)
//		c = (this->Data4[i] >> 4) | ((this->Data4[i] & 0x0F) << 4);
		// Don't swap byte ordering - legacy?
		c = this->_data[i];
		ss << std::hex << std::setw(2) << c;
		if (i == 1) ss << "-";
	}
	ss << "}";
	// Convert the stringstream to a std::string
	std::string result = ss.str();
	//	std::cout << "String Conversion: " << result << std::endl;
	return result;
}


std::vector<unsigned char> Uuid::ToVector(const std::string &string)
{
	// First convert the string to a GUID_t
	Uuid uuid = string;
	// Finally, conver the Uuid to a vector
	std::vector<unsigned char> vector = uuid;
	return vector;
}


std::string Uuid::ToString(const std::vector<unsigned char> &vector)
{
	// First convert the vector to a Uuid
	Uuid uuid = vector;
	// Finally, convert the Uuid to a string
	std::string string = uuid;
	return string;
}


bool Uuid::operator==(const Uuid &uuid) const
{
	// Just compare the memory
	return (memcmp(this, &uuid, sizeof(Uuid)) == 0);
}


bool Uuid::operator!=(const Uuid &uuid) const
{
	// Just compare the memory
	return (memcmp(this, &uuid, sizeof(Uuid)) != 0);
}


bool Uuid::operator>(const Uuid &uuid) const
{
	// Just compare the memory
	return (memcmp(this, &uuid, sizeof(Uuid)) > 0);
}


bool Uuid::operator<(const Uuid &uuid) const
{
	// Just compare the memory
	return (memcmp(this, &uuid, sizeof(Uuid)) < 0);
}


std::ostream& MGA::operator<<(std::ostream& out, const Uuid &uuid)
{
	// Get a string version of the Uuid
	std::string stringVersion = uuid;
	// Print it
	out << stringVersion;
	return out;
}


Uuid Uuid::Null(void)
{
	Uuid retVal;
	// Return an all zero Uuid (signals Null value)
	memset(&retVal, 0, 16);
	return retVal;
}


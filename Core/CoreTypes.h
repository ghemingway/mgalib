#ifndef __CORE_TYPES_H__
#define __CORE_TYPES_H__


/*** Included Header Files ***/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#ifdef _WIN32
#include <hash_map>
#include <unordered_map>
#include <memory>
#else
#include <ext/hash_map>
#include <tr1/unordered_map>
#include <tr1/memory>
#include <memory>
#endif
#include <assert.h>
#include "CoreError.h"


#ifdef _WIN32
#include "Arch\win32\platform.h"
#endif
#ifdef __APPLE__
#include "Arch/osx/platform.h"
#endif
#ifdef __linux__
#include "Arch/linux64/platform.h"
#endif


/*****************************************************************************/


// Platform Specific Functions
extern Semaphore* _CreateSemaphore( char*, uint32_t );
extern int _PostSemaphore( Semaphore* semaphore );
extern int _WaitSemaphore( Semaphore* semaphore );
extern void _DestroySemaphore( char*, Semaphore* );
extern bool _CreateThread( pthread_t*, void*(*)(void*), void*, uint32_t priority );
extern void _SplitPath(const std::string &path, std::string &directory, std::string &filename);


/*****************************************************************************/


// Will need to be platform independent
#ifdef _WIN32
#define STDEXT stdext
#define STDTR1 std
#else
#define STDEXT __gnu_cxx
#define STDTR1 std::tr1
#endif


/*** Namespace Declaration ***/
namespace MGA {


/******************************************/


typedef int16_t MetaID_t;
typedef int16_t AttrID_t;
typedef std::tr1::unordered_map<std::string,std::string> DictionaryMap;


const MetaID_t METAID_NONE = 0;
const MetaID_t METAID_ROOT = 1;
const AttrID_t ATTRID_NONE = 0;
const AttrID_t ATTRID_NAME = 2;
const AttrID_t ATTRID_FATHER = 3;
const AttrID_t ATTRID_COLLECTION = 10000;


/******************************************/


class Uuid {
private:
	uint8_t		_data[16];								//!< Just 16 bytes of data - that is all
public:
	Uuid();												//!< Default constructor creates new unique Uuid
	Uuid(const std::vector<unsigned char> &vector);		//!< Binary vector constructor
	Uuid(const char* string);							//!< Char string constructor
	Uuid(const std::string &string);					//!< Std::string constructor
	// Coversions
	Uuid& operator=(const Uuid &uuid);
	Uuid& operator=(const std::vector<unsigned char> &vector);
	Uuid& operator=(const std::string &string);
	operator std::vector<unsigned char>() const;
	operator std::string() const;
	static std::vector<unsigned char> ToVector(const std::string &string);
	static std::string ToString(const std::vector<unsigned char> &vector);
	// Operators
	bool operator==(const Uuid &uuid) const;
	bool operator!=(const Uuid &uuid) const;
	bool operator>(const Uuid &uuid) const;
	bool operator<(const Uuid &uuid) const;
	// Friends
	friend std::ostream& operator<<(std::ostream& out, const Uuid &uuid);
	static Uuid Null(void);
};


struct Uuid_HashFunc
{
	size_t operator()(const Uuid &uuid) const
	{
		size_t returnValue;
		memcpy(&returnValue, &uuid, sizeof(size_t));
		return returnValue;
	}
};


/******************************************/


typedef struct AttributeID
{
	Uuid									uuid;
	AttrID_t								attrID;

	bool operator==(const AttributeID &attributeID) const
	{
		return (this->uuid == attributeID.uuid) && (this->attrID == attributeID.attrID);
	}
} AttributeID;


struct AttributeID_HashFunc
{
	size_t operator()(const AttributeID &attributeID) const
	{
		size_t returnValue;
		// Copy in the uuid value for all of the bytes
		memcpy(&returnValue, &(attributeID.uuid), sizeof(size_t));
		// Copy in the attrID value for the first 2 bytes
		memcpy(&returnValue, &(attributeID.attrID), sizeof(AttrID_t));
		return returnValue;
	}
};


/******************************************/


// ValueType class
class ValueType {
private:
	// Hidden Type Enum
	typedef enum _ValueTypeEnum {
		VALUETYPE_NONE			= 0,			//!< No value type
		VALUETYPE_COLLECTION	= 1,			//!< Collection
		VALUETYPE_POINTER		= 2,			//!< Pointer
		VALUETYPE_LONGPOINTER	= 3,			//!< Long-Pointer
		VALUETYPE_LONG			= 4,			//!< Long (32 bit)
		VALUETYPE_STRING		= 5,			//!< String type
		VALUETYPE_REAL			= 6,			//!< Real (64 bit)
		VALUETYPE_DICTIONARY	= 7,			//!< Key-value pair dictionary
	} _ValueTypeEnum;
	_ValueTypeEnum				_type;								//!< Internal type value
	ValueType(const _ValueTypeEnum &type) : _type(type)		{ }		//!< Hidden primary constructor
	ValueType(const uint8_t &type) : _type((_ValueTypeEnum)type)	{ }		//!< Hidden primary constructor
public:
	ValueType() : _type(VALUETYPE_NONE)						{ }		//!< Deny access to default constructor
	ValueType(const ValueType& type) : _type(type._type)	{ }		//!< Copy constructor
	~ValueType()											{ }		//!< Default destructor
	//Static Creation Methods
	inline static ValueType None(void)			{ return ValueType(VALUETYPE_NONE); }		//!< Create a ValueType of type none
	inline static ValueType Collection(void)	{ return ValueType(VALUETYPE_COLLECTION); }	//!< Create a ValueType of type collection
	inline static ValueType Pointer(void)		{ return ValueType(VALUETYPE_POINTER); }	//!< Create a ValueType of type pointer
	inline static ValueType LongPointer(void)	{ return ValueType(VALUETYPE_LONGPOINTER); }//!< Create a ValueType of type long-pointer
	inline static ValueType Long(void)			{ return ValueType(VALUETYPE_LONG); }		//!< Create a ValueType of type long
	inline static ValueType String(void)		{ return ValueType(VALUETYPE_STRING); }		//!< Create a ValueType of type string
	inline static ValueType Real(void)			{ return ValueType(VALUETYPE_REAL); }		//!< Create a ValueType of type real
	inline static ValueType Dictionary(void)	{ return ValueType(VALUETYPE_DICTIONARY); }	//!< Create a ValueType of type dictionary
	inline static ValueType Read(char* &stream)	{ uint8_t type; memcpy(&type, stream, sizeof(uint8_t)); stream += sizeof(uint8_t); return ValueType(type); }
	//Overridden Operators
	inline ValueType& operator=(const ValueType &type)	{ this->_type = type._type; return *this; }	//!< Equals operator
	inline bool operator==(const ValueType &type) const	{ return this->_type == type._type; }		//!< Equality operator
	inline bool operator!=(const ValueType &type) const	{ return this->_type != type._type; }		//!< Inequality operator
	inline void Write(char* &stream) const	{ uint8_t type=(uint8_t)this->_type; memcpy(stream, &type, sizeof(uint8_t)); stream += sizeof(uint8_t); }
	//Friend Methods
	friend std::ostream& operator<<(std::ostream& out, const ValueType &type) {
		switch (type._type) {
			case VALUETYPE_NONE:		out << "ValueType::None"; break;
			case VALUETYPE_COLLECTION:	out << "ValueType::Collection"; break;
			case VALUETYPE_POINTER:		out << "ValueType::Pointer"; break;
			case VALUETYPE_LONGPOINTER:	out << "ValueType::LongPointer"; break;
			case VALUETYPE_LONG:		out << "ValueType::Long"; break;
			case VALUETYPE_STRING:		out << "ValueType::String"; break;
			case VALUETYPE_REAL:		out << "ValueType::Real"; break;
			case VALUETYPE_DICTIONARY:	out << "ValueType::Dictionary"; break;
			default:					out << "ValueType::Error!!!"; break;
		}
		return out;
	}
};


/*** Class Predefinitions ***/
class CoreObject;


/*** End of MGA Namespace ***/
}


/*** Used Namespaces ***/
using namespace MGA;


#endif //__CORE_TYPES_H__

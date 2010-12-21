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
#include <ext/hash_map>
#include <tr1/memory>
#include <assert.h>
#include "CoreError.h"


#ifdef WIN32
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
#define STDEXT __gnu_cxx


//typedef long guid_t;
typedef int16_t MetaID_t;
typedef uint32_t ObjID_t;
typedef int16_t AttrID_t;
typedef uint8_t LockingType_t;
typedef int16_t LockVal_t;
typedef uint8_t Status_t;

const MetaID_t METAID_NONE = 0;
const MetaID_t METAID_ROOT = 1;
const ObjID_t OBJID_NONE = 0;
const ObjID_t OBJID_ROOT = 1;
const AttrID_t ATTRID_NONE = 0;
const AttrID_t ATTRID_LOCK = 1;
const AttrID_t ATTRID_NAME = 2;
const AttrID_t ATTRID_FATHER = 3;
const AttrID_t ATTRID_COLLECTION = 10000;


typedef struct GUID_t {
	uint32_t	Data1;
	uint16_t	Data2;
	uint16_t	Data3;
	uint8_t		Data4[8];
	// Need to do a lot of conversions and comparisons
	GUID_t& operator=(const std::vector<unsigned char> &bin);
	GUID_t& operator=(const std::string &str);
	operator std::string() const;
	operator std::vector<unsigned char>() const;
	static std::vector<unsigned char> ToBinary(const std::string &guid);
	static std::string ToString(const std::vector<unsigned char> &guid);
	inline bool operator==(const GUID_t &guid) const	{ return (memcmp(this, &guid, sizeof(GUID_t)) == 0); }
	inline bool operator!=(const GUID_t &guid) const	{ return (memcmp(this, &guid, sizeof(GUID_t)) != 0); }
} GUID_t;


class UUID {
private:
	uint8_t		_data[16];
public:
	// Constructors
	UUID();
	UUID(const GUID_t &guid);
	UUID(const std::vector<unsigned char> &vector);
	UUID(const char* string);
	UUID(const std::string &string);
	// Coversions
	UUID& operator=(const UUID &uuid);
	UUID& operator=(const std::vector<unsigned char> &vector);
	UUID& operator=(const std::string &string);
	operator std::vector<unsigned char>() const;
	operator std::string() const;
	static std::vector<unsigned char> ToVector(const std::string &string);
	static std::string ToString(const std::vector<unsigned char> &vector);
	// Operators
	bool operator==(const UUID &uuid) const;
	bool operator!=(const UUID &uuid) const;
	bool operator>(const UUID &uuid) const;
	bool operator<(const UUID &uuid) const;
	// Friends
	friend std::ostream& operator<<(std::ostream& out, const UUID &uuid);
};


/******************************************/


typedef struct MetaObjIDPair
{
	MetaID_t metaID;
	ObjID_t objID;
	inline MetaObjIDPair() : metaID(METAID_NONE), objID(OBJID_NONE) { }
	inline MetaObjIDPair(const MetaID_t &metaID, const ObjID_t &objID) : metaID(metaID), objID(objID) { }
	inline MetaObjIDPair(const MetaObjIDPair &idPair) : metaID(idPair.metaID), objID(idPair.objID) { }
	inline MetaObjIDPair& operator=(const MetaObjIDPair &idPair) { this->metaID = idPair.metaID; this->objID = idPair.objID; return *this; }
	// Equality operator
	inline bool operator==(const MetaObjIDPair &idPair) const	{ return (this->metaID == idPair.metaID && this->objID == idPair.objID); }
	inline bool operator!=(const MetaObjIDPair &idPair) const	{ return (this->metaID != idPair.metaID || this->objID != idPair.objID); }
	friend std::ostream& operator<<(std::ostream& out, const MetaObjIDPair &pair) {
		out << "(" << pair.metaID << ", " << pair.objID << ")";
		return out;
	}
} MetaObjIDPair;


struct MetaObjIDPair_HashFunc
{
	size_t operator()(const MetaObjIDPair &idPair) const
	{
		return (((size_t)idPair.metaID) << 8) + ((size_t)idPair.objID);
	}
};


struct MetaObjIDPair_EqualKey
{
	bool operator()(const MetaObjIDPair &a, const MetaObjIDPair &b) const
	{
		return a.objID == b.objID && a.metaID == b.metaID;
	}
};


struct MetaObjIDPair_Less
{
	bool operator()(const MetaObjIDPair& a, const MetaObjIDPair& b) const
	{
        if( a.metaID < b.metaID )
            return true;
        else if( a.metaID > b.metaID )
            return false;
        else if( a.objID < b.objID )
            return true;
        else
            return false;
	}
};


struct MetaObjID2Pair_HashFunc
{
	size_t operator()(const MetaObjIDPair &idPair) const
	{
		return (((size_t)idPair.metaID) << 8) + ((size_t)idPair.objID);
	}
	bool operator()(const MetaObjIDPair &a, const MetaObjIDPair &b) const
	{
		// implement < logic here !!!
		return MetaObjIDPair_Less()(a, b);	
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
		VALUETYPE_LOCK			= 3,			//!< Lock value type, for access negotiation
		VALUETYPE_LONG			= 4,			//!< Long (32 bit)
		VALUETYPE_STRING		= 5,			//!< String type
		VALUETYPE_BINARY		= 6,			//!< Binary type
		VALUETYPE_REAL			= 7,			//!< Real (64 bit)
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
	inline static ValueType Lock(void)			{ return ValueType(VALUETYPE_LOCK); }		//!< Create a ValueType of type lock
	inline static ValueType Long(void)			{ return ValueType(VALUETYPE_LONG); }		//!< Create a ValueType of type long
	inline static ValueType String(void)		{ return ValueType(VALUETYPE_STRING); }		//!< Create a ValueType of type string
	inline static ValueType Binary(void)		{ return ValueType(VALUETYPE_BINARY); }		//!< Create a ValueType of type binary
	inline static ValueType Real(void)			{ return ValueType(VALUETYPE_REAL); }		//!< Create a ValueType of type real
	inline static ValueType Read(std::fstream &stream)	{									//!< Create ValueType from IOStream 
		uint8_t type; stream.read((char*)&type, sizeof(uint8_t)); return ValueType(type); }
	//Overridden Operators
	inline ValueType& operator=(const ValueType &type)	{ this->_type = type._type; return *this; }	//!< Equals operator
	inline bool operator==(const ValueType &type) const	{ return this->_type == type._type; }		//!< Equality operator
	inline bool operator!=(const ValueType &type) const	{ return this->_type != type._type; }		//!< Inequality operator
	inline void Write(std::fstream &stream) const		{											//!< Write ValueType to IOStream
		uint8_t type = (uint8_t)this->_type; stream.write((const char*)&(type), sizeof(uint8_t)); }
	//Friend Methods
	friend std::ostream& operator<<(std::ostream& out, const ValueType &type) {
		switch (type._type) {
			case VALUETYPE_NONE:		out << "ValueType::None"; break;
			case VALUETYPE_COLLECTION:	out << "ValueType::Collection"; break;
			case VALUETYPE_POINTER:		out << "ValueType::Pointer"; break;
			case VALUETYPE_LOCK:		out << "ValueType::Lock"; break;
			case VALUETYPE_LONG:		out << "ValueType::Long"; break;
			case VALUETYPE_STRING:		out << "ValueType::String"; break;
			case VALUETYPE_BINARY:		out << "ValueType::Binary"; break;
			case VALUETYPE_REAL:		out << "ValueType::Real"; break;
			default:					out << "ValueType::Error!!!"; break;
		}
		return out;
	}
};


// LockType class
class LockType {
private:
	// Hidden Type Enum
	typedef enum _LockTypeEnum {
		LOCK_NONE				= 0,			//!< No access, unlimited access for other CoreProjects
		LOCK_READ				= 1,			//!< Read access, only read access for other CoreProjects
		LOCK_WRITE				= 2,			//!< Write access, only write access for other CoreProjects
		LOCK_EXCLUSIVE			= 3				//!< Exclusive access, no access for other CoreProjects
	} _LockTypeEnum;
	_LockTypeEnum				_type;			//!< Internal type value
	inline LockType(const _LockTypeEnum &type) : _type(type)	{ }	//!< Hidden primary constructor
public:
	LockType()					{ }				//!< Deny access to default constructor
	inline LockType(const LockType& type) : _type(type._type)	{ }	//!< Copy constructor
	~LockType()					{ }				//!< Default destructor
	//Static Creation Methods
	inline static LockType	None(void)		{ return LockType(LOCK_NONE); }			//!< Create a LockType of type none
	inline static LockType	Read(void)		{ return LockType(LOCK_READ); }			//!< Create a LockType of type Read
	inline static LockType	Write(void)		{ return LockType(LOCK_WRITE); }		//!< Create a LockType of type Write
	inline static LockType	Exclusive(void)	{ return LockType(LOCK_EXCLUSIVE); }	//!< Create a LockType of type Exclusive
	//Overridden Operators
	inline LockType& operator=(const LockType &type)	{ this->_type = type._type; return *this; }	//!< Equals operator
	inline bool operator==(const LockType &type) const	{ return this->_type == type._type; }	//!< Equality operator
	inline bool operator!=(const LockType &type) const	{ return this->_type != type._type; }	//!< Inequality operator
};


#endif //__CORE_TYPES_H__

#ifndef __CORE_ERROR_H__
#define __CORE_ERROR_H__


/*** Included Header Files ***/
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>


/*** Namespace Declaration ***/
namespace MGA {


/*** Type Definitions ***/
typedef long Result_t;
#define ASSERT	assert


// --------------------------- Common Error Codes

//		One or more arguments are invalid
//*****	E_INVALIDARG
#define E_POINTER						0x80731000
#define S_OK							0

//		Not enough storage is available to complete this operation
//*****	E_OUTOFMEMORY

//		no implementation yet
//***** E_NOTIMPL

//		Class is not properly used
#define E_INVALID_USAGE					0x80731001

//		Unknown exception
#define	E_EXCEPTION						0x80731002

//		conversion failed
#define E_CONVERSION					0x80731003

//		requested != count in GetAll 
#define E_GETALL						0x80731004

//		output parameter is not empty
#define E_OUT_NOT_EMPTY					0x80731005

//		the passed interface is not from this module
#define E_SAMEPROJECT					0x80731006

//		object was not found
#define E_NOTFOUND						0x80731007

//		file open error
#define E_FILEOPEN						0x80731008

//		VERIFY has failed
#define E_VERIFY						0x80731009

//		COMTHROW test exception
#define E_COMTHROW_TEST					0x8073100A

// the description table is in CommonError.cpp
#define E_COMMON_FIRST					E_INVALID_USAGE
#define E_COMMON_LAST					E_COMTHROW_TEST

// --------------------------- Core Error Codes

//		Could not gain lock because an item is already locked
#define E_LOCK_VIOLATION				0x80732001

//		The lock value in the database is not valid
#define E_INVALID_LOCK_VALUE			0x80732002

//		Attribute not found
#define E_ATTRID						0x80732003

//		Meta object not found
#define E_METAID						0x80732004

//		meta project is invalid
#define E_METAPROJECT					0x80732005

//		invalid response from the repository
#define E_REPOSITORY					0x80732006

//		project - metaproject mismatch
#define E_PROJECT_MISMATCH				0x80732007

//		object has been deleted, zombie
#define E_ZOMBIE						0x80732008

//		invalid valtype
#define E_VALTYPE						0x80732009

//		no active transaction
#define E_TRANSACTION					0x8073200A

//		the object already exists
#define E_OBJECTEXISTS					0x8073200B

//		no territory selected
#define E_TERRITORY						0x8073200C

//		not locked
#define E_NOTLOCKED						0x8073200D

//		invalid data in repository
#define E_REPOSITORY_DATA				0x8073200E

//		cannot delete object because of nonempty collection
#define E_NONEMPTY_COLL					0x8073200F

//		cannot resolve the connection string
#define E_UNKNOWN_STORAGE				0x80732010

//		binary file storage is invalid
#define E_BINFILE						0x80732011

//		missing name
#define E_NAMEMISSING					0x80732012

// the description table is in CommonError.cpp
#define E_CORE_FIRST					E_LOCK_VIOLATION
#define E_CORE_LAST						E_NAMEMISSING

// --------------------------- Meta Error Codes

// invalid attval_type
#define E_ATTVALTYPE					0x80733001

// metaproject is not open
#define E_META_NOTOPEN					0x80733002

// invalid path
#define E_INVALID_PATH					0x80733003

// invalid metaref
#define E_METAREF						0x80733004

// the description table is in CommonError.cpp
#define E_META_FIRST					E_ATTVALTYPE
#define E_META_LAST						E_METAREF

// --------------------------- Parser Error Codes

// XML parser exception, WE SET THE DESCRIPTION MANUALLY
#define E_XMLPARSER						0x80734001

// Invalid DTD file
#define E_INVALID_DTD					0x80734002

// Invalid GUID
#define E_INVALID_GUID					0x80734003

// Invalid XML filename
#define E_INVALID_FILENAME				0x80734004

// Invalid MGA object
#define E_INVALID_MGA					0x80734005

// Invalid META object
#define E_INVALID_META					0x80734006

// Too many passes while parsing the XML file
#define E_TOOMANYPASSES					0x80734007

// Invalid long value
#define E_INVALID_XML_LONG				0x80734008

// the description table is in CommonError.cpp
#define E_PARSER_FIRST				E_INVALID_DTD
#define E_PARSER_LAST				E_INVALID_XML_LONG

// XSLT transformation error codes
#define E_XSLT_XERCES_INIT				0x80734009
#define E_XSLT_COMPILE_ERROR			0x8073400A
#define E_XSLT_TRANSF_ERROR				0x8073400B
#define E_XSLT_ERROR					0x8073400C
#define E_XSLTFILESEL_USER_ABORTED		0x8073400D


// Stringify the Error Code
const std::string GetErrorInfo(const Result_t &result);


/*** End of MGA Namespace ***/
}


#endif	//__CORE_ERROR_H__


#ifndef __CORE_METAATTRIBUTE_H__
#define __CORE_METAATTRIBUTE_H__


/*** Included Header Files ***/
#include "CoreTypes.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class CoreMetaObject;


// --------------------------- CoreMetaAttribute ---------------------------


class CoreMetaAttribute
{
private:
	friend class CoreMetaObject;									//!< Only class allowed to create metaAttributes
	CoreMetaAttribute();											//!< Deny access to default constructor
	CoreMetaAttribute(const CoreMetaAttribute &);					//!< Deny access to copy constructor
	CoreMetaAttribute& operator=(const CoreMetaAttribute &);		//!< Deny access to equals operator
	CoreMetaAttribute(CoreMetaObject* object, const AttrID_t &attrID, const std::string &token,
					  const std::string &name, const ValueType &valueType);

protected:
	CoreMetaObject								*_object;			//!< Object this attribute belongs to
	AttrID_t									_attributeID;		//!< AttrID of this object
	std::string									_token;				//!< Short description of this attribute
	std::string									_name;				//!< Full description of this attribute
	ValueType									_valueType;			//!< Type of the attribute

public:
	~CoreMetaAttribute()								{ }

	inline const Result_t Object(CoreMetaObject* &metaObject) const throw()	{ metaObject = this->_object; return S_OK; }
	inline const Result_t AttributeID(AttrID_t &attrID) const throw()		{ attrID = this->_attributeID; return S_OK; }
	inline const Result_t Token(std::string &token) const throw()			{ token = this->_token; return S_OK; }
	inline const Result_t Name(std::string &name) const throw()				{ name = this->_name; return S_OK; }
	inline const Result_t GetValueType(ValueType &valueType) const throw()	{ valueType = this->_valueType; return S_OK; }
	friend std::ostream& operator<<(std::ostream& out, const CoreMetaAttribute *attrib)
	{
		// Print out the attribute's info
		out << "\t(" << attrib->_attributeID << ":" << attrib->_valueType << ") " << attrib->_name << " --" << attrib->_token << ".\n";
		return out;
	}
};


/*** End of MGA Namespace ***/
}


#endif	//__CORE_METAATTRIBUTE_H__


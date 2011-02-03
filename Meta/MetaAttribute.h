#ifndef __META_ATTRIBUTE_H_
#define __META_ATTRIBUTE_H_


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaFCO;
class MetaEnumItem;


// --------------------------- MetaAttribute --------------------------- //


class MetaAttribute : public MetaBase
{
private:
	MetaAttribute();											//!< Deny access to default constructor
	MetaAttribute(const MetaAttribute &);						//!< Deny access to copy constructor
	MetaAttribute& operator=(const MetaAttribute &);			//!< Deny access to equals operator

	friend class MetaBase;
	friend class MetaEnumItem;
	MetaAttribute(CoreObject &coreObject, MetaProject* const &metaProject) : ::MetaBase(coreObject, metaProject) { }

public:
	virtual ~MetaAttribute() { }
	
	const Result_t GetDefinedIn(MetaFCO* &fco) const throw();						//!< Get FCO this attribute is defined in
	const Result_t GetEnumItems(std::list<MetaEnumItem*> &enumList) const throw();	//!< Get list of child EnumItems
	const Result_t GetUsedIn(std::list<MetaFCO*> &fcoList) const throw();			//!< Get list of FCOs this attribute is used in
	const Result_t GetValueType(AttVal_t &valueType) const throw();					//!< Get the type of this attribute
	const Result_t SetValueType(const AttVal_t &valueType) throw();					//!< Set the type of this attribute
	const Result_t GetDefaultValue(std::string &value) const throw();				//!< Get the default value of this attribute
	const Result_t SetDefaultValue(const std::string &value) throw();				//!< Set the default value of this attribute
	const Result_t GetViewable(bool &flag) const throw();							//!< Is this attribute viewable
	const Result_t SetViewable(const bool &flag) throw();							//!< Set if this attribute is viewable
	const Result_t CreateEnumItem(MetaEnumItem* &enumItem) throw();					//!< Create a child EnumItem
};


/*** End of MGA Namespace ***/
}


#endif // __META_ATTRIBUTE_H_


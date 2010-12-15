#ifndef __META_ENUMITEM_H__
#define __META_ENUMITEM_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Class Predefinitions ***/
class MetaAttribute;


// --------------------------- MetaEnumItem --------------------------- //


class MetaEnumItem
{
private:
	MetaEnumItem();											//!< Deny access to default constructor
	MetaEnumItem(const MetaEnumItem &);						//!< Deny access to copy constructor
	MetaEnumItem& operator=(const MetaEnumItem &);			//!< Deny access to equals operator

public:
	const Result_t GetParent(MetaAttribute* &metaAttribute) const throw();	//{ return ComGetPointerValue(GetUnknown(), ATTRID_ENUMITEMS_COLL, p); }
	const Result_t GetDisplayedName(std::string &name) const throw();		//{ return ComGetAttrValue(GetUnknown(), ATTRID_NAME, p); }
	const Result_t GetValue(std::string &value) const throw();				//{ return ComGetAttrValue(GetUnknown(), ATTRID_VALUE, p); }
//	const Result_t SetDisplayedName(const std::string &name) throw();		//{ return ComPutAttrValue(GetUnknown(), ATTRID_NAME, p); }
//	const Result_t SetValue(const std::string &value) throw();				//{ return ComPutAttrValue(GetUnknown(), ATTRID_VALUE, p); }
};


#endif // __META_ENUMITEM_H__


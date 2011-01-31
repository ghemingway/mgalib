#ifndef __META_ENUMITEM_H__
#define __META_ENUMITEM_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaAttribute;


// --------------------------- MetaEnumItem --------------------------- //


class MetaEnumItem
{
private:
	MetaEnumItem();											//!< Deny access to default constructor
	MetaEnumItem(const MetaEnumItem &);						//!< Deny access to copy constructor
	MetaEnumItem& operator=(const MetaEnumItem &);			//!< Deny access to equals operator

	CoreObject					_coreObject;				//!<
	MetaProject					*_metaProject;				//!<

	friend class MetaBase;
	MetaEnumItem(CoreObject &coreObject, MetaProject* &metaProject);

public:
	const Result_t GetParent(MetaAttribute* &metaAttribute) throw();	//!<
	const Result_t GetDisplayedName(std::string &name) const throw();	//!<
	const Result_t SetDisplayedName(const std::string &name) throw();	//!<
	const Result_t GetValue(std::string &value) const throw();			//!<
	const Result_t SetValue(const std::string &value) throw();			//!<
};


/*** End of MGA Namespace ***/
}


#endif // __META_ENUMITEM_H__


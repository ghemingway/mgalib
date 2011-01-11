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

	friend class MetaFolder;
	friend class MetaFCO;
	MetaAttribute(CoreObject &coreObject, MetaProject* &metaProject) : ::MetaBase(coreObject, metaProject) { }

public:
	const Result_t GetDefinedIn(MetaFCO* &fco) const throw();						//!<
	const Result_t GetEnumItems(std::list<MetaEnumItem*> &enumList) const throw();	//!<
	const Result_t GetUsedIn(std::list<MetaFCO*> &fcoList) const throw();			//!<
	const Result_t GetValueType(AttVal_t &valueType) const throw();					//!<
	const Result_t SetValueType(const AttVal_t &type) throw();						//!<
	const Result_t GetDefaultValue(bool &value) const throw();						//!<
	const Result_t SetDefaultValue(const std::string &value) throw();				//!<
	const Result_t GetViewable(bool &flag) const throw();							//!<
	const Result_t SetViewable(const bool &flag) throw();							//!<
	const Result_t CreateEnumItem(std::list<int> &list) throw();					//!<
};


/*** End of MGA Namespace ***/
}


#endif // __META_ATTRIBUTE_H_


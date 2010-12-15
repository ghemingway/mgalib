#ifndef __META_ATTRIBUTE_H_
#define __META_ATTRIBUTE_H_

/*** Included Header Files ***/
#include "MetaBase.h"


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

public:
	static void Traverse(MetaProject *metaproject, CoreObject* &coreObject);
	
	const Result_t GetDefinedIn(MetaFCO* &fco) const throw();
	const Result_t GetEnumItems(std::list<MetaEnumItem*> &enumList) const throw();
	const Result_t GetUsedIn(std::list<MetaFCO*> &fcoList) const throw();
	const Result_t GetValueType(AttVal_t &valueType) const throw();
	const Result_t GetDefaultValue(bool &value) const throw();
	const Result_t GetViewable(bool &flag) const throw();	
//	const Result_t SetValueType(const AttVal_t &type) throw();			//{ return ComPutAttrValue(GetUnknown(), ATTRID_ATTVALTYPE, static_cast<long>(p)); }
//	const Result_t SetDefaultValue(const std::string &value) throw();	//{ return ComPutAttrValue(GetUnknown(), ATTRID_VALUE, p); }
//	const Result_t SetViewable(const bool &flag) throw();				//{ return ComPutAttrValue(GetUnknown(), ATTRID_VIEWABLE, (p == VARIANT_FALSE)?0L:1L); }
//	const Result_t CreateEnumItem(std::list<int> &list) throw();		//{ return ComCreateMetaObj(GetUnknown(), METAID_METAENUMITEM, ATTRID_ENUMITEMS_COLL, p); }
};


#endif // __META_ATTRIBUTE_H_


#ifndef __META_SET_H__
#define __META_SET_H__


/*** Included Header Files ***/
#include "MetaFCO.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaProject;
class MetaPointerSpec;


// --------------------------- MetaSet --------------------------- //


class MetaSet : public MetaFCO
{
private:
	MetaSet();												//!< Deny access to default constructor
	MetaSet(const MetaSet &);								//!< Deny access to copy constructor
	MetaSet& operator=(const MetaSet &);					//!< Deny access to equals operator

	friend class MetaBase;
	MetaSet(CoreObject &coreObject, MetaProject* const &metaProject) : ::MetaFCO(coreObject, metaProject) { }

public:
	virtual ~MetaSet() { }

	const Result_t GetMemberSpec(MetaPointerSpec* &metaPointerSpec) const throw();
	const Result_t CheckPath(const std::string &path, bool &valid) const throw();
};


/*** End of MGA Namespace ***/
}


#endif // __META_SET_H__


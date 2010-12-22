#ifndef __META_REFERENCE_H__
#define __META_REFERENCE_H__


/*** Included Header Files ***/
#include "MetaFCO.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaPointerSpec;


// --------------------------- MetaReference --------------------------- //


class MetaReference : public MetaFCO
{
private:
	MetaReference();											//!< Deny access to default constructor
	MetaReference(const MetaReference &);						//!< Deny access to copy constructor
	MetaReference& operator=(const MetaReference &);			//!< Deny access to equals operator

public:
	static void Traverse(MetaProject* &metaProject, CoreObject* &coreObject);

	const Result_t GetRefSpec(MetaPointerSpec* &pointerSpec) const throw();
	const Result_t CheckPath(const std::string &path, bool &valid) const throw();
};


/*** End of MGA Namespace ***/
}


#endif // __META_REFERENCE_H__


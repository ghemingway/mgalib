#ifndef __META_REFERENCE_H__
#define __META_REFERENCE_H__


/*** Included Header Files ***/
#include "MetaFCO.h"
#include "MetaPointerSpec.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
// None


// --------------------------- MetaReference --------------------------- //


class MetaReference : public MetaFCO, public IMetaPointerSpec
{
private:
	MetaReference();											//!< Deny access to default constructor
	MetaReference(const MetaReference &);						//!< Deny access to copy constructor
	MetaReference& operator=(const MetaReference &);			//!< Deny access to equals operator

	friend class MetaBase;
	MetaReference(CoreObject &coreObject, MetaProject* const &metaProject) : ::MetaFCO(coreObject, metaProject) { }

public:
	virtual ~MetaReference() { }

	virtual const Result_t GetItems(std::list<MetaPointerItem*> &pointerList) const throw();//!<
	virtual const Result_t CreateItem(MetaPointerItem* &pointerItem) throw();				//!<
};


/*** End of MGA Namespace ***/
}


#endif // __META_REFERENCE_H__


#ifndef __META_SET_H__
#define __META_SET_H__


/*** Included Header Files ***/
#include "MetaFCO.h"
#include "MetaPointerSpec.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
// None


// --------------------------- MetaSet --------------------------- //


class MetaSet : public MetaFCO, public IMetaPointerSpec
{
private:
	MetaSet();												//!< Deny access to default constructor
	MetaSet(const MetaSet &);								//!< Deny access to copy constructor
	MetaSet& operator=(const MetaSet &);					//!< Deny access to equals operator

	friend class MetaBase;
	MetaSet(CoreObject &coreObject, MetaProject* const &metaProject) : ::MetaFCO(coreObject, metaProject) { }

public:
	virtual ~MetaSet() { }

	virtual const Result_t GetItems(std::list<MetaPointerItem*> &pointerList) const throw();//!<
	virtual const Result_t CreateItem(MetaPointerItem* &pointerItem) throw();				//!<
};


/*** End of MGA Namespace ***/
}


#endif // __META_SET_H__


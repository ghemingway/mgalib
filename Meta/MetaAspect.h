#ifndef __META_ASPECT_H__
#define __META_ASPECT_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaModel;
class MetaPart;
class MetaAttribute;
class MetaRole;


// --------------------------- MetaAspect --------------------------- //


class MetaAspect : public MetaBase
{
private:
	MetaAspect();											//!< Deny access to default constructor
	MetaAspect(const MetaAspect &);							//!< Deny access to copy constructor
	MetaAspect& operator=(const MetaAspect &);				//!< Deny access to equals operator

	friend class MetaBase;
	MetaAspect(CoreObject &coreObject, MetaProject* const &metaProject) : ::MetaBase(coreObject, metaProject) { }

public:
	virtual ~MetaAspect() { }

	const Result_t GetParentModel(MetaModel* &metaModel) const throw();					//!< Get the parent metaModel
	const Result_t GetParts(std::list<MetaPart*> &partList)	const throw();				//!< Get all child parts
	const Result_t GetAttributes(std::list<MetaAttribute*> &attribList) const throw();	//!< Get all child attributes
	const Result_t AddAttribute(MetaAttribute* &attrib) throw();						//!< Add a child attribute
	const Result_t CreatePart(MetaRole* &role, MetaPart* &part) throw();				//!< Create a child part
};


/*** End of MGA Namespace ***/
}


#endif // __META_ASPECT_H__


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


// --------------------------- MetaAspect --------------------------- //


class MetaAspect : public MetaBase
{
private:
	MetaAspect();											//!< Deny access to default constructor
	MetaAspect(const MetaAspect &);							//!< Deny access to copy constructor
	MetaAspect& operator=(const MetaAspect &);				//!< Deny access to equals operator

public:
	static void Traverse(MetaProject* &metaProject, CoreObject* &coreObject);

	const Result_t GetParentModel(MetaModel* &metaModel) const throw();
	const Result_t GetParts(std::list<MetaPart*> &partList)	const throw();
	const Result_t GetAttributes(std::list<MetaAttribute*> &attribList) const throw();	//{ return ComGetSortedLinkCollValue<IMgaMetaAttribute>(GetUnknown(), ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p); }
//	const Result_t AddAttribute(MetaAttribute* &attrib) throw();			//{ return ComAddLink(this, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR,	ATTRID_ATTRLINK_ATTR_PTR, p); }
//	const Result_t RemoveAttribute(MetaAttribute* &attrib) throw();			//{ return ComRemoveLink(this, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p); }
//	const Result_t CreatePart(MetaRole* &role, MetaPart* &part) throw();
};


/*** End of MGA Namespace ***/
}


#endif // __META_ASPECT_H__


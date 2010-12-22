#ifndef __META_ROLE_H__
#define __META_ROLE_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaModel;
class MetaFCO;


// --------------------------- MetaRole --------------------------- //


class MetaRole : public MetaBase
{
private:
	MetaRole();												//!< Deny access to default constructor
	MetaRole(const MetaRole &);								//!< Deny access to copy constructor
	MetaRole& operator=(const MetaRole &);					//!< Deny access to equals operator

public:
	const Result_t GetParentModel(MetaModel* &metaModel) throw();		//{ return ComGetPointerValue(GetUnknown(), ATTRID_ROLES_COLL, metaModel); }
	const Result_t GetParts(std::list<MetaPart*> &partList) throw();	//{ return ComGetCollectionValue<IMgaMetaPart>(GetUnknown(), ATTRID_PARTROLE_PTR, partList); }
	const Result_t GetKind(MetaFCO* &metaFCO) throw();					//{ return ComGetPointerValue(GetUnknown(), ATTRID_KIND_PTR, metaFCO); }
};


/*** End of MGA Namespace ***/
}


#endif // __META_ROLE_H__


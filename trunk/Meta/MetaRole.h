#ifndef __META_ROLE_H__
#define __META_ROLE_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaModel;
class MetaFCO;
class MetaPart;
class MetaModel;


// --------------------------- MetaRole --------------------------- //


class MetaRole : public MetaBase
{
private:
	MetaRole();												//!< Deny access to default constructor
	MetaRole(const MetaRole &);								//!< Deny access to copy constructor
	MetaRole& operator=(const MetaRole &);					//!< Deny access to equals operator

	friend class MetaBase;
	MetaRole(CoreObject &coreObject, MetaProject* const &metaProject) : ::MetaBase(coreObject, metaProject) { }

public:
	virtual ~MetaRole() { }

	const Result_t GetParentModel(MetaModel* &metaModel) throw();		//!<
	const Result_t GetParts(std::list<MetaPart*> &partList) throw();	//!<
	const Result_t GetKind(MetaFCO* &metaFCO) throw();					//!<
};


/*** End of MGA Namespace ***/
}


#endif // __META_ROLE_H__


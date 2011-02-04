#ifndef __META_PART_H__
#define __META_PART_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaRole;
class MetaAspect;


// --------------------------- MetaPart --------------------------- //


class MetaPart : public MetaBase
{
private:
	MetaPart();											//!< Deny access to default constructor
	MetaPart(const MetaPart &);							//!< Deny access to copy constructor
	MetaPart& operator=(const MetaPart &);				//!< Deny access to equals operator

	friend class MetaAspect;
	MetaPart(CoreObject &coreObject, MetaProject* const &metaProject) : ::MetaBase(coreObject, metaProject) { }

public:
	virtual ~MetaPart() { }

	inline const Result_t GetDisplayedName(std::string &name) const throw()						{ return E_INVALID_USAGE; }
	inline const Result_t GetConstraints(std::list<MetaConstraint*> *constraintList) throw()	{ return E_INVALID_USAGE; }
	inline const Result_t SetName(const std::string &name) throw()								{ return E_INVALID_USAGE; }
	inline const Result_t SetDisplayedName(const std::string &dispName) throw()					{ return E_INVALID_USAGE; }
	inline const Result_t CreateConstraint(MetaConstraint* &metaConstraint) throw()				{ return E_INVALID_USAGE; }

	const Result_t GetRole(MetaRole* &metaRole) const throw();				//!<
	const Result_t GetParentAspect(MetaAspect* &MetaAspect) const throw();	//!<
	const Result_t GetKindAspect(std::string &kind) const throw();			//!<
	const Result_t SetKindAspect(const std::string &kind) throw();			//!<
	const Result_t GetName(std::string &name) const throw();				//!<
	const Result_t GetIsPrimary(bool &isPrimary) const throw();				//!<
	const Result_t SetIsPrimary(const bool &isPrimary) throw();				//!<
	const Result_t GetIsLinked(bool &isLinked) const throw();				//!<
	const Result_t SetIsLinked(const bool &isLinked) throw();				//!<
};


/*** End of MGA Namespace ***/
}


#endif // __META_PART_H__


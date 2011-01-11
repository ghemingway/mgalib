#ifndef __META_CONSTRAINT_H__
#define __META_CONSTRAINT_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
// None


/*** Type Definitions ***/
typedef enum ConstraintDepth
{
	CONSTRAINT_DEPTH_ZERO,
	CONSTRAINT_DEPTH_ONE,
	CONSTRAINT_DEPTH_ANY
} ConstraintDepth;

typedef enum ConstraintType
{
	CONSTRAINT_TYPE_EVENTBASED,
	CONSTRAINT_TYPE_ONDEMAND,
	CONSTRAINT_TYPE_FUNCTION
} ConstraintType;


// --------------------------- MetaConstraint --------------------------- //


class MetaConstraint
{
private:
	MetaConstraint();											//!< Deny access to default constructor
	MetaConstraint(const MetaConstraint &);						//!< Deny access to copy constructor
	MetaConstraint& operator=(const MetaConstraint &);			//!< Deny access to equals operator

	CoreObject					_coreObject;					//!<
	MetaProject					*_metaProject;					//!<

	friend class MetaBase;
	MetaConstraint(CoreObject &coreObject, MetaProject* &metaProject);

public:
	const Result_t GetName(std::string &name) const throw();				//!<
	const Result_t SetName(const std::string &name) throw();				//!<
	const Result_t GetDisplayedName(std::string &name) const throw();		//!<
	const Result_t SetDisplayedName(const std::string &name) throw();		//!<
	const Result_t GetExpression(std::string &exp) const throw();			//!<
	const Result_t SetExpression(const std::string &expression) throw();	//!<
	const Result_t GetEventMask(unsigned long &mask) const throw();			//!<
	const Result_t SetEventMask(const unsigned long &mask) throw();			//!<
	const Result_t GetDepth(ConstraintDepth &depth) const throw();			//!<
	const Result_t SetDepth(const ConstraintDepth &depth) throw();			//!<
	const Result_t GetPriority(long &priority) const throw();				//!<
	const Result_t SetPriority(const long &priority) throw();				//!<
	const Result_t GetType(ConstraintType &type) const throw();				//!<
	const Result_t SetType(const ConstraintType &type) throw();				//!<
	const Result_t GetDefinedForNamespace(std::string &value) const throw();//!<
	const Result_t SetDefinedForNamespace(const std::string &value) throw();//!<
//	const Result_t Remove(void)												//{ return ComDeleteObject(GetUnknown()); }
};


/*** End of MGA Namespace ***/
}


#endif // __META_CONSTRAINT_H__


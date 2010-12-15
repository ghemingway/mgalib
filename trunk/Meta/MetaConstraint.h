#ifndef __META_CONSTRAINT_H__
#define __META_CONSTRAINT_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Class Predefinitions ***/
// None


// --------------------------- MetaConstraint --------------------------- //


class MetaConstraint
{
private:
	MetaConstraint();											//!< Deny access to default constructor
	MetaConstraint(const MetaConstraint &);						//!< Deny access to copy constructor
	MetaConstraint& operator=(const MetaConstraint &);			//!< Deny access to equals operator

protected:
	CoreObject					*_coreObject;					//!<
	MetaProject					*_metaProject;					//!<
	
public:
	MetaConstraint(CoreObject* &coreObject, MetaProject* &metaProject);

	const Result_t GetName(std::string &name) const throw();
	const Result_t GetDisplayedName(std::string &name) const throw();
	const Result_t GetExpression(std::string &exp) const throw();
	const Result_t GetEventMask(unsigned long &mask) const throw();
//	const Result_t GetDepth(constraint_depth_enum &depth) const throw();
	const Result_t GetPriority(long &priority) const throw();
//	const Result_t GetType(constraint_type_enum *p);
	const Result_t GetDefinedForNamespace(std::string &value) const throw();
//	const Result_t SetName(const std::string &name) throw();				//{ return ComPutAttrValue(GetUnknown(), ATTRID_NAME, p); }
//	const Result_t SetDisplayedName(const std::string &name) throw();		//{ return ComPutAttrValue(GetUnknown(), ATTRID_DISPNAME, p); }
//	const Result_t SetExpression(std::string p)								//{ return ComPutAttrValue(GetUnknown(), ATTRID_CONSEXPR, p); }
//	const Result_t SetEventMask(unsigned long p)							//{ return ComPutAttrValue(GetUnknown(), ATTRID_CONSMASK, (long)p); }
//	const Result_t SetDepth(constraint_depth_enum p)						//{ return ComPutAttrValue(GetUnknown(), ATTRID_CONSDEPTH, static_cast<long>(p)); }
//	const Result_t SetPriority(long p)										//{ return ComPutAttrValue(GetUnknown(), ATTRID_CONSPRIORITY, p); }
//	const Result_t SetType(constraint_type_enum p)							//{ return ComPutAttrValue(GetUnknown(), ATTRID_CONSTYPE, static_cast<long>(p)); }
//	const Result_t Remove(void)												//{ return ComDeleteObject(GetUnknown()); }
//	const Result_t SetDefinedForNamespace( std::string pStr);
};


#endif // __META_CONSTRAINT_H__


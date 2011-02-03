#ifndef __META_CONNJOINT_H__
#define __META_CONNJOINT_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaPointerSpec;
class MetaConnection;


// --------------------------- MetaConnJoint --------------------------- //


class MetaConnJoint
{
private:
	MetaConnJoint();											//!< Deny access to default constructor
	MetaConnJoint(const MetaConnJoint &);						//!< Deny access to copy constructor
	MetaConnJoint& operator=(const MetaConnJoint &);			//!< Deny access to equals operator
	
	CoreObject					_coreObject;					//!< Associated coreObject
	MetaProject					*_metaProject;					//!< Contained in this metaProject
	
	friend class MetaBase;
	MetaConnJoint(CoreObject &coreObject, MetaProject* const &metaProject);
	
public:
	const Result_t GetParent(MetaConnection* &parent) const throw();											//!<
	const Result_t GetPointerSpecs(std::list<MetaPointerSpec*> &pointerSpecList) const throw();					//!<
	const Result_t GetPointerSpecByName(const std::string &name, MetaPointerSpec* &pointerSpec) const throw();	//!<
	const Result_t CreatePointerSpec(MetaPointerSpec* &pointerSpec) throw();									//!<
};


/*** End of MGA Namespace ***/
}


#endif	// __META_CONNJOINT_H__


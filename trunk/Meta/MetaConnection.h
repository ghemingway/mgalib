#ifndef __META_CONNECTION_H__
#define __META_CONNECTION_H__


/*** Included Header Files ***/
#include "MetaFCO.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaConnJoint;


// --------------------------- MetaConnection --------------------------- //


class MetaConnection : public MetaFCO
{
private:
	MetaConnection();											//!< Deny access to default constructor
	MetaConnection(const MetaConnection &);						//!< Deny access to copy constructor
	MetaConnection& operator=(const MetaConnection &);			//!< Deny access to equals operator

	friend class MetaBase;
	MetaConnection(CoreObject &coreObject, MetaProject* const &metaProject) : ::MetaFCO(coreObject, metaProject) { }

public:
	virtual ~MetaConnection() { }
	inline const Result_t IsSimple(bool &value)	const throw()	{ value = true; return S_OK; }

	const Result_t GetJoints(std::list<MetaConnJoint*> &jointList) const throw();	//!<
	const Result_t CheckPaths(const std::string &paths, bool &valid) const throw();	//!<
	const Result_t CreateJoint(MetaConnJoint* &connJoint) throw();					//!<
};


/*** End of MGA Namespace ***/
}


#endif // __META_CONNECTION_H__


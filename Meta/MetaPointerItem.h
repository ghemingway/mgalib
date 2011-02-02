#ifndef __META_POINTERITEM_H__
#define __META_POINTERITEM_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Namespace Declaration ***/
namespace MGA {
	
	
/*** Class Predefinitions ***/
class MetaPointerSpec;


// --------------------------- MetaPointerItem --------------------------- //


class MetaPointerItem
{
private:
	MetaPointerItem();												//!< Deny access to default constructor
	MetaPointerItem(const MetaPointerItem &);						//!< Deny access to copy constructor
	MetaPointerItem& operator=(const MetaPointerItem &);			//!< Deny access to equals operator

	CoreObject					_coreObject;						//!< Associated coreObject
	MetaProject					*_metaProject;						//!< Contained in this metaProject

	friend class MetaBase;
	MetaPointerItem(CoreObject &coreObject, MetaProject* &metaProject);

public:
//	static bool CheckToken(CComBstrObj &token, bstr_const_iterator i, bstr_const_iterator &e);
//	static bool CheckPath(CCoreObjectPtr &self, pathitems_type &pathitems, bool global);

	const Result_t GetDescription(std::string &description) const throw();	//!<
	const Result_t SetDescription(const std::string &description) throw();	//<!
	const Result_t GetParent(MetaPointerSpec* &parent) throw();				//!<
};


/*** End of MGA Namespace ***/
}


#endif	// __META_POINTERITEM_H__


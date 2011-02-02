#ifndef __META_POINTERSPEC_H__
#define __META_POINTERSPEC_H__


/*** Included Header Files ***/
#include "MetaBase.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaProject;
class MetaPointerItem;


// --------------------------- MetaPointerSpec --------------------------- //


class MetaPointerSpec
{
private:
	MetaPointerSpec();												//!< Deny access to default constructor
	MetaPointerSpec(const MetaPointerSpec &);						//!< Deny access to copy constructor
	MetaPointerSpec& operator=(const MetaPointerSpec &);			//!< Deny access to equals operator

	CoreObject					_coreObject;						//!< Associated coreObject
	MetaProject					*_metaProject;						//!< Contained in this metaProject

	friend class MetaBase;
	MetaPointerSpec(CoreObject &coreObject, MetaProject* &metaProject);

public:
//	static bool CheckPath(CoreObject* &coreObject, pathitems_type &pathitems, bool &global);
	
	const Result_t GetParent(MetaBase* &parent) throw();							//!<
	const Result_t GetName(std::string &name) const throw();						//!<
	const Result_t SetName(const std::string &name) throw();						//!<
	const Result_t GetItems(std::list<MetaPointerItem*> &pointerList) const throw();//!<
	const Result_t CreateItem(MetaPointerItem* &pointerItem) throw();				//!<
};


/*** End of MGA Namespace ***/
}


#endif // __META_POINTERSPEC_H__


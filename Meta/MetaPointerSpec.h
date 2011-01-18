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


class MetaPointerSpec {
private:
	MetaPointerSpec();											//!< Deny access to default constructor
	MetaPointerSpec(const MetaPointerSpec &);					//!< Deny access to copy constructor
	MetaPointerSpec& operator=(const MetaPointerSpec &);		//!< Deny access to equals operator

public:
//	static bool CheckPath(CoreObject* &coreObject, pathitems_type &pathitems, bool &global);
	
//	const Result_t GetParent(IDispatch **p);
	const Result_t GetName(std::string &name) const throw();
	const Result_t GetItems(std::list<MetaPointerItem*> &pointerList) const throw();
//	const Result_t SetName(const std::string &name) throw();			//{ return ComPutAttrValue(GetUnknown(), ATTRID_PTRSPECNAME, p); }
//	const Result_t CreateItem(MetaPointerItem* &pointerItem) throw();	//{ return ComCreateMetaObj(GetUnknown(), METAID_METAPOINTERITEM, ATTRID_PTRITEMS_COLL, p); }
};


/*** End of MGA Namespace ***/
}


#endif // __META_POINTERSPEC_H__

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

public:
	static void Traverse(MetaProject* &metaProject, CoreObject* &coreObject);

	inline const Result_t GetDisplayedName(std::string &name) const throw()						{ return E_INVALID_USAGE; }
	inline const Result_t GetConstraints(std::list<MetaConstraint*> *constraintList) throw()	{ return E_INVALID_USAGE; }
	const Result_t GetRole(MetaRole* &metaRole) const throw();
	const Result_t GetParentAspect(MetaAspect* &MetaAspect) const throw();
	const Result_t GetKindAspect(std::string &kind) const throw();
	const Result_t GetName(std::string &name) const throw();
	const Result_t GetObjType(ObjType_t &objType) const throw();
	const Result_t GetIsPrimary(bool value) const throw();
	const Result_t GetIsLinked(bool &value) const throw();
//	const Result_t put_MetaRef)(metaref_type p)	{ HRESULT hr = PutMetaRef(p); if(hr == S_OK) hr = ComPutAttrValue(GetUnknown(), ATTRID_METAREF, p); return hr; }
//	const Result_t put_Name)(BSTR p)							{ return E_NOTIMPL; }
//	const Result_t put_DisplayedName)(BSTR p)				{ return E_NOTIMPL; }
//	const Result_t CreateConstraint)(IMgaMetaConstraint **p) { return E_NOTIMPL; }
//	const Result_t Delete)()									{ return ComDeleteObject(GetUnknown()); }
//	const Result_t put_KindAspect)(BSTR p)					{ return ComPutAttrValue(GetUnknown(), ATTRID_KINDASPECT, p); }
//	const Result_t put_IsPrimary)(VARIANT_BOOL p);
//	const Result_t put_IsLinked)(VARIANT_BOOL p);
};


/*** End of MGA Namespace ***/
}


#endif // __META_PART_H__


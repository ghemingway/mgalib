#ifndef __MGA_ATTRIBUTE_H__
#define __MGA_ATTRIBUTE_H__


/*** Included Header Files ***/
#include "MgaTypes.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class CoreObject;
class MetaAttribute;
class MgaFCOBase;
class MgaProject;


// --------------------------- MgaAttribute --------------------------- //


class MgaAttribute
{
private:
	MetaRef_t				_metaRef;
	MgaFCOBase				*_fcoBase;
	//CoreObject			*_coreObject;
	MgaProject				*_mgaProject;
	std::string				_regPrefix;
	MetaAttribute			*_metaAttribute;
	AttVal_t				_attribType, _inputType;
	long					_loadStatus;
	MgaAttribute			**_prevPtr, *_next;

	MgaAttribute();
	MgaAttribute(const MgaAttribute &);

public:
	MgaAttribute(MetaRef_t &metaRef, MgaFCOBase* &fcoBase, MgaProject* &mgaProject);
	~MgaAttribute()
	{
		// Remove object from hash
		if(_next) _next->_prevPtr = _prevPtr;
		*_prevPtr = _next;
	}
	
	inline const Result_t GetMeta(MetaAttribute* &metaAttribute)
	{
//		return this->_mgaProject->FindMetaRef(this->_metaRef);
		return S_OK;
	}
//	STDMETHOD(get_Owner)( IMgaFCO **pVal);
//	STDMETHOD(get_Status)( long *status);
//	STDMETHOD(get_Value)( VARIANT *pVal);
//	STDMETHOD(put_Value)( VARIANT newVal);
//	STDMETHOD(get_StringValue)(BSTR *pVal);
//	STDMETHOD(put_StringValue)( BSTR newVal);
//	STDMETHOD(get_OrigValue)( VARIANT *pVal);
//	STDMETHOD(get_HasChanged)( VARIANT_BOOL *pVal);
//	STDMETHOD(get_IntValue)( long *pVal);
//	STDMETHOD(put_IntValue)( long newVal);
//	STDMETHOD(get_BoolValue)( VARIANT_BOOL *pVal);
//	STDMETHOD(put_BoolValue)( VARIANT_BOOL newVal);
//	STDMETHOD(get_FloatValue)( double *pVal);
//	STDMETHOD(put_FloatValue)( double newVal);
//	STDMETHOD(get_FCOValue)( IMgaFCO **pVal);
//	STDMETHOD(put_FCOValue)( IMgaFCO * newVal);
//	STDMETHOD(get_RegistryNode)( BSTR path,  IMgaRegNode **pVal); 
//	STDMETHOD(GetRegistryNodeDisp)( BSTR path,  IMgaRegNode **pVal) { return get_RegistryNode( path, pVal); }
//	STDMETHOD(get_Registry)(VARIANT_BOOL virtuals, IMgaRegNodes **pVal);
//	STDMETHOD(GetRegistryDisp)(VARIANT_BOOL virtuals, IMgaRegNodes **pVal) { return get_Registry( virtuals, pVal); }
//	STDMETHOD(get_RegistryValue)( BSTR path,  BSTR *pVal);
//	STDMETHOD(put_RegistryValue)( BSTR path,  BSTR newval);
//	STDMETHOD(GetRegistryValueDisp)( BSTR path,  BSTR *pVal) { return get_RegistryValue( path, pVal); }
//	STDMETHOD(SetRegistryValueDisp)( BSTR path,  BSTR newval) { return put_RegistryValue( path, newval); }
//	STDMETHOD(Clear)();
};

/*
#define APOOL_HASHSIZE 8
#define apool_hash(x) ((x)%APOOL_HASHSIZE)


class attrpool {
	CMgaAttribute::hashobp pool[APOOL_HASHSIZE];
public:
	attrpool() { 
		int i; 
		for(i = 0; i < APOOL_HASHSIZE;i++) pool[i] = NULL;
	}

	~attrpool() {
		int i; 
		for(i = 0; i < APOOL_HASHSIZE;i++) ASSERT(pool[i] == NULL);
	}

	// Throws (allocates)!!!!
	CComPtr<IMgaAttribute> getpoolobj(metaref_type mref, FCO *o, CMgaProject *pr) {
		CMgaAttribute::hashobp &k = pool[apool_hash(mref)], *kk;
		for(kk = &k; *kk != NULL; kk = &((*kk)->next)) {
			if((*kk)->mref == mref) {
				return (*kk);
			}
		}
		CComPtr<CMgaAttribute> s;
		CreateComObject(s);
		s->prevptr = &k;				// Insert to the front
		s->next = k;
		if(k) k->prevptr = &(s->next);
		k = s;

		s->Initialize(mref, o, pr);  
		CComPtr<IMgaAttribute> retval = s;
		return retval;
	}
};

void MergeAttrs(const CoreObj &src, CoreObj &dst);
*/


// --------------------------- MgaRegNode --------------------------- //


class MgaRegNode
{
private:
	MgaRegNode				**prevptr, *next;
	long					load_status;
	CoreObject				*valueobj;
	MgaFCOBase				*_fcoBase;
	std::string				_mypath;
	MgaProject				*_mgaProject;
	
	void Initialize(const std::string &path, MgaFCOBase *fcoBase, MgaProject* &mgaProject)
	{
		this->_mypath = path;		
		this->_fcoBase = fcoBase;
		this->_mgaProject = mgaProject;
	}
	void markchg();

public:
	MgaRegNode()	: prevptr(NULL), next(NULL), load_status(/*ATTSTATUS_INVALID*/) { }
	~MgaRegNode()
	{
		// Remove object from hash
		if(next) next->prevptr = prevptr;
		*prevptr = next;
	}
	
//	STDMETHOD(get_Name)( BSTR *pVal) { 
//		COMTRY {
//			CHECK_OUTPAR(pVal);
//			LPCOLESTR p = wcsrchr(mypath, '/');
//			if(p) p += 1; // skip '/'
//			else p = mypath;
//			CComBSTR rval(p);
//			*pVal = rval.Detach();
//		} COMCATCH(;); 
//	};
//	STDMETHOD(get_Path)( BSTR *pVal) { 
//		COMTRY {
//			CHECK_OUTPAR(pVal);
//			*pVal = mypath.Copy();
//		} COMCATCH(;); 
//	};
//	STDMETHOD(get_Object)( IMgaObject **pVal);
//	STDMETHOD(get_Value)( BSTR *pVal);
//	STDMETHOD(put_Value)( BSTR newVal);
//	STDMETHOD(get_FCOValue)( IMgaFCO **pVal);
//	STDMETHOD(put_FCOValue)( IMgaFCO *newVal);
//	STDMETHOD(get_SubNodes)( VARIANT_BOOL virtuals, IMgaRegNodes **pVal);
//	STDMETHOD(GetSubNodesDisp)( VARIANT_BOOL virtuals, IMgaRegNodes **pVal) { return get_SubNodes( virtuals, pVal); }
//	STDMETHOD(get_SubNodeByName)(BSTR name, IMgaRegNode **pVal);
//	STDMETHOD(GetSubNodeByNameDisp)(BSTR name, IMgaRegNode **pVal) { return get_SubNodeByName( name, pVal); }
//	STDMETHOD(get_ParentNode)( IMgaRegNode **pVal);
//	STDMETHOD(get_Status)( long *status);
//	STDMETHOD(get_Opacity)( VARIANT_BOOL *pVal);
//	STDMETHOD(put_Opacity)( VARIANT_BOOL newVal);
//	STDMETHOD(Clear)();
//	STDMETHOD(RemoveTree)();
};

/*
#define RPOOL_HASHSIZE 8


class regnpool {
	CMgaRegNode::hashobp pool[RPOOL_HASHSIZE];
public:
	regnpool() { 
		int i; 
		for(i = 0; i < RPOOL_HASHSIZE;i++) pool[i] = NULL;
	}

	~regnpool() {
		int i; 
		for(i = 0; i < RPOOL_HASHSIZE;i++) ASSERT(pool[i] == NULL);
	}

	int rpool_hash(BSTR nam) {
		int i = SysStringLen(nam);
		int hash = i;
		while(i) hash ^= nam[--i];
		return hash % RPOOL_HASHSIZE;
	}

	// Throws (allocates)!!!!
	CComPtr<IMgaRegNode>  getpoolobj(BSTR nam, FCO *o, CMgaProject *pr) {
		CMgaRegNode::hashobp &k = pool[rpool_hash(nam)], *kk;
		for(kk = &k; *kk != NULL; kk = &((*kk)->next)) {
			if((*kk)->mypath == nam) {
				return (*kk);
			}
		}
		CComPtr<CMgaRegNode > s;
		CreateComObject(s);
		s->prevptr = &k;				// Insert to the front
		s->next = k;
		if(k) k->prevptr = &(s->next);
		k = s;

		s->Initialize(nam, o, pr);  
		CComPtr<IMgaRegNode> retval = s;
		return retval;
	}
};

void RegistryChildrenRemove(CoreObj &t);
void MergeRegs(const CoreObj &src, CoreObj &dst);
*/


// --------------------------- MgaPart --------------------------- //


class MgaPart
{
protected:
	MgaProject				*mgaproject;  // Holds no reference
	MgaPart					**prevptr, *next;
	MetaRef_t				mref;
	long					load_status;
//	FCOPtr					fco;
//	CComBSTR(regprefix);
//	CComBSTR(aspname);

	void Initialize(const MetaRef_t &metaRef, MgaFCOBase* &fcoBase, MgaProject* &project);

public:
	MgaPart();
	~MgaPart();

//	STDMETHOD(get_Model)(struct IMgaModel **pVal );
//	STDMETHOD(get_Meta)(struct IMgaMetaPart **pVal );
//	STDMETHOD(get_MetaAspect)(struct IMgaMetaAspect **pVal );
//	STDMETHOD(get_MetaRole)(struct IMgaMetaRole **pVal );
//	STDMETHOD(get_FCO)(struct IMgaFCO **pVal);
//	STDMETHOD(get_AccessMask)(long *);
//	STDMETHOD(GetGmeAttrs)(BSTR *icon,long *x,long *y);
//	STDMETHOD(SetGmeAttrs)(BSTR icon,long x,long y);
//	STDMETHOD(get_RegistryMode)( VARIANT_BOOL *own);
//	STDMETHOD(put_RegistryMode)( VARIANT_BOOL own);
//	STDMETHOD(get_RegistryNode)( BSTR path,  IMgaRegNode **pVal);
//	STDMETHOD(GetRegistryNodeDisp)( BSTR path,  IMgaRegNode **pVal) { return get_RegistryNode( path, pVal); }
//	STDMETHOD(get_Registry)( VARIANT_BOOL virtuals, IMgaRegNodes **pVal);
//	STDMETHOD(GetRegistryDisp)( VARIANT_BOOL virtuals, IMgaRegNodes **pVal) { return get_Registry( virtuals, pVal); }
//	STDMETHOD(get_RegistryValue)( BSTR path,  BSTR *pVal);
//	STDMETHOD(put_RegistryValue)( BSTR path,  BSTR newval);  
//	STDMETHOD(GetRegistryValueDisp)( BSTR path,  BSTR *pVal) { return get_RegistryValue( path, pVal); }
//	STDMETHOD(SetRegistryValueDisp)( BSTR path,  BSTR newval) { return put_RegistryValue( path, newval); }
};

/*
#define PPOOL_HASHSIZE 4
#define ppool_hash(x) ((x)%PPOOL_HASHSIZE)


class partpool {
	CMgaPart::hashobp pool[PPOOL_HASHSIZE];
public:
	partpool() { 
		int i; 
		for(i = 0; i < PPOOL_HASHSIZE;i++) pool[i] = NULL;
	}

	~partpool() {
		int i; 
		for(i = 0; i < PPOOL_HASHSIZE;i++) ASSERT(pool[i] == NULL);
	}

	// Throws (allocates)!!!!
	CComPtr<IMgaPart> getpoolobj(metaref_type mref, FCO *o, CMgaProject *pr) {
		CMgaPart::hashobp &k = pool[ppool_hash(mref)], *kk;
		for(kk = &k; *kk != NULL; kk = &((*kk)->next)) {
			if((*kk)->mref == mref) {
				return (*kk);
			}
		}
		CComPtr<CMgaPart > s;
		CreateComObject(s);
		s->prevptr = &k;				// Insert to the front
		s->next = k;
		if(k) k->prevptr = &(s->next);
		k = s;

		s->Initialize(mref, o, pr);  
		CComPtr<IMgaPart> retval = s;
		return retval;
	}
};
*/

/*** End of MGA Namespace ***/
}


#endif // __MGA_ATTRIBUTE_H__


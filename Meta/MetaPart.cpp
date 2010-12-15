/*** Included Header Files ***/
#include "MetaPart.h"
#include "MetaGeneric.h"
#include "MetaRole.h"


// --------------------------- MetaPart --------------------------- //


#define MGAMETAPART_PRIMARY		0x0001
#define MGAMETAPART_LINKED		0x0002


void MetaPart::Traverse(MetaProject* &metaProject, CoreObject* &coreObject)
{
	ASSERT( metaProject != NULL );
	ASSERT( coreObject != NULL );
	// Traverse the base class
	MetaBase::Traverse(metaProject, coreObject);
	// Traverse any children
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_REGNODES_COLL);
}


const Result_t MetaPart::GetRole(MetaRole* &metaRole) const throw()
{
//	return ComGetPointerValue(GetUnknown(), ATTRID_PARTROLE_PTR, p);
	return S_OK;
}


const Result_t MetaPart::GetParentAspect(MetaAspect* &MetaAspect) const throw()
{
//	return ComGetPointerValue(GetUnknown(), ATTRID_PARTASPECT_PTR, p);
	return S_OK;
}


const Result_t MetaPart::GetKindAspect(std::string &kind) const throw()
{
//	return ComGetAttrValue(GetUnknown(), ATTRID_KINDASPECT, p);
	return S_OK;
}


const Result_t MetaPart::GetName(std::string &name) const throw()
{
	MetaRole* role;
	Result_t result = this->GetRole(role);
	if (result != S_OK) return result;
	return role->GetName(name);
}


const Result_t MetaPart::GetObjType(ObjType_t &objType) const throw()
{
//	return ComGetObjType(GetUnknown(), p);
	return S_OK;
}


const Result_t MetaPart::GetIsPrimary(bool value) const throw()
{
//	CCoreObjectPtr self(GetUnknown());
//	ASSERT( self != NULL );
//	long a = self.GetLongValue(ATTRID_PARTDATA);
//	*p = (a & MGAMETAPART_PRIMARY) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}


const Result_t MetaPart::GetIsLinked(bool &value) const throw()
{
//	CCoreObjectPtr self(GetUnknown());
//	ASSERT( self != NULL );
//	long a = self.GetLongValue(ATTRID_PARTDATA);
//	*p = (a & MGAMETAPART_LINKED) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

/*
STDMETHODIMP CMgaMetaPart::put_IsPrimary(VARIANT_BOOL p)
{
	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());
		ASSERT( self != NULL );

		long a = self.GetLongValue(ATTRID_PARTDATA);

		if( p == VARIANT_FALSE )
			a &= (~MGAMETAPART_PRIMARY);
		else
			a |= MGAMETAPART_PRIMARY;

		self.PutLongValue(ATTRID_PARTDATA, a);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaMetaPart::put_IsLinked(VARIANT_BOOL p)
{
	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());
		ASSERT( self != NULL );

		long a = self.GetLongValue(ATTRID_PARTDATA);

		if( p == VARIANT_FALSE )
			a &= (~MGAMETAPART_LINKED);
		else
			a |= MGAMETAPART_LINKED;

		self.PutLongValue(ATTRID_PARTDATA, a);
	}
	COMCATCH(;)
}
*/

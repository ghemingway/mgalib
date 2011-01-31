/*** Included Header Files ***/
#include "MetaPart.h"
#include "MetaGeneric.h"
#include "MetaRole.h"


// --------------------------- MetaPart --------------------------- //


#define MGAMETAPART_PRIMARY		0x0001
#define MGAMETAPART_LINKED		0x0002


const Result_t MetaPart::GetRole(MetaRole* &metaRole) const throw()
{
	// Use the MetaBase helper function to get an object from this pointer attribute
	return this->ObjectFromAttribute(ATTRID_PARTROLE_PTR, metaRole);
}


const Result_t MetaPart::GetParentAspect(MetaAspect* &MetaAspect) const throw()
{
	// Use the MetaBase helper function to get an object from this pointer attribute
	return this->ObjectFromAttribute(ATTRID_PARTASPECT_PTR, MetaAspect);
}


const Result_t MetaPart::GetKindAspect(std::string &kind) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_KINDASPECT, kind);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaPart::GetName(std::string &name) const throw()
{
	MetaRole* role;
	Result_t result = this->GetRole(role);
	if (result != S_OK) return result;
	result = role->GetName(name);
	delete role;
	return result;
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

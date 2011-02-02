/*** Included Header Files ***/
#include "MetaFCO.h"
#include "MetaAttribute.h"
#include "MetaGeneric.h"


// --------------------------- Static MetaFCO Methods --------------------------- //


const Result_t MetaFCO::GetAttributeByName(MetaFCO* &metaFCO, const std::string &name, MetaAttribute* &attrib) throw()
{
	ASSERT(false);
/*	ASSERT( me != NULL );
	CCoreObjectPtr self(me);
	coreobjects_type coreobjects;
	self.GetCollectionValue(ATTRID_ATTRLINK_USEDIN_PTR, coreobjects);
	coreobjects_iterator i = coreobjects.begin();
	coreobjects_iterator e = coreobjects.end();
	while( i != e )
	{
		CCoreObjectPtr obj;
		(*i).GetPointerValue(ATTRID_ATTRLINK_ATTR_PTR, obj);
		CComBstrObj n;
		obj.GetStringValue(ATTRID_NAME, PutOut(n));
		if( n == name )
			COMRETURN( ::QueryInterface(obj, p) );
		++i;
	}
	HR_THROW(E_NOTFOUND);
 */
	return S_OK;
 }


const Result_t MetaFCO::GetAttributeByRef(MetaFCO *metaFCO, const Uuid &uuid, MetaAttribute* &metaAttribute) throw()
{
	ASSERT(false);
/*	ASSERT( me != NULL );
	CHECK_OUT(p);

	COMTRY 
	{
		CCoreObjectPtr self(me);

		coreobjects_type coreobjects;
		self.GetCollectionValue(ATTRID_ATTRLINK_USEDIN_PTR, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CCoreObjectPtr obj;
			(*i).GetPointerValue(ATTRID_ATTRLINK_ATTR_PTR, obj);

			if( obj.GetLongValue(ATTRID_METAREF) == mref )
			{
				COMTHROW( ::QueryInterface(obj, p) );
				return S_OK;
			}

			++i;
		}

		HR_THROW(E_NOTFOUND);
	}
	COMCATCH(;)
*/
	return S_OK;
}


const Result_t MetaFCO::GetDefinedAttributeByName(MetaFCO *metaFCO, const std::string &name, const bool &inscope, MetaAttribute* &metaAttribute) throw()
{
	ASSERT(false);
/*	ASSERT( me != NULL );
	CHECK_OUT(p);

	COMTRY
	{
		CCoreObjectPtr self(me);

		coreobjects_type coreobjects;
		self.GetCollectionValue(ATTRID_DEFATTR_PTR, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComBstrObj n;
			(*i).GetStringValue(ATTRID_NAME, PutOut(n));
			
			if( n == name )
			{
				COMTHROW( ::QueryInterface(*i, p) );
				return S_OK;
			}

			++i;
		}

		if( inscope == VARIANT_FALSE )
			COMRETURN(E_NOTFOUND);

		CComObjPtr<IMgaMetaBase> parent;
		COMTHROW( me->get_DefinedIn(PutOut(parent)) );
		ASSERT( parent != NULL );

		CComObjPtr<IMgaMetaFolder> folder;
		if( SUCCEEDED(parent.QueryInterface(folder)) )
			COMRETURN( folder->get_DefinedAttributeByName(name, inscope, p) );

		CComObjPtr<IMgaMetaFCO> fco;
		COMTHROW( parent.QueryInterface(fco) );
		COMRETURN( fco->get_DefinedAttributeByName(name, inscope, p) );
	}
	COMCATCH(;)
*/
	return S_OK;
}


const Result_t MetaFCO::GetAliasingEnabled(MetaFCO *metaFCO, bool &value) throw()
{
	ASSERT(false);
/*	ASSERT( me != NULL );
	if( p == NULL ) COMRETURN(E_POINTER);
	COMTRY
	{
		CCoreObjectPtr self(me);
		*p = 0; //self.GetBoolValue(ATTRID_ALIASENABLED);

	}
	COMCATCH(;)
*/
	return S_OK;
}


const Result_t MetaFCO::SetAliasingEnabled(MetaFCO *metaFCO, const bool &value) throw()
{
	ASSERT(false);
/*	ASSERT( me != NULL );
	COMTRY
	{
		CCoreObjectPtr self(me);
//		self.PutBoolValue(ATTRID_ALIASENABLED,p);

	}
	COMCATCH(;)
*/
	return S_OK;
}


const Result_t MetaFCO::GetDefinedIn(MetaBase* &metaBase) const throw()
{
	ASSERT(false);
//	return ComGetPointerValue(GetUnknown(), ATTRID_DEFFCO_PTR, p);
	return S_OK;
}


const Result_t MetaFCO::GetDefinedAttributes(std::list<MetaAttribute*> &attribList) const throw()
{
	ASSERT(false);
//	return ComGetSortedCollValue<IMgaMetaAttribute>(GetUnknown(), ATTRID_DEFATTR_PTR, p);
	return S_OK;
}


const Result_t MetaFCO::GetDefinedAttributeByName(const std::string &name, const bool &inScope, MetaAttribute* &attrib) const throw()
{
	ASSERT(false);
	return S_OK;
}


const Result_t MetaFCO::GetUsedInRoles(std::list<MetaRole*> &rolesList) const throw()
{
	ASSERT(false);
//	return ComGetCollectionValue<IMgaMetaRole>(GetUnknown(), ATTRID_KIND_PTR, p);
	return S_OK;
}


const Result_t MetaFCO::GetUsedInFolders(std::list<MetaFolder*> &folderList) const throw()
{
	ASSERT(false);
//	return ComGetLinkCollectionValue<IMgaMetaFolder>(GetUnknown(), ATTRID_ROOTOBJLINK_OBJECT_PTR, ATTRID_ROOTOBJLINK_FOLDER_PTR, p);
	return S_OK;
}


const Result_t MetaFCO::GetAttributes(std::list<MetaAttribute*> &attribList) const throw()
{
	ASSERT(false);
//	return ComGetSortedLinkCollValue<IMgaMetaAttribute>(GetUnknown(),	ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p);
	return S_OK;
}


const Result_t MetaFCO::GetAttributeByName(const std::string &name, MetaAttribute* &attrib) const throw()
{
	ASSERT(false);
	return S_OK;
}


const Result_t MetaFCO::GetAttributeByRef(const Uuid &uuid, MetaAttribute* &attrib) const throw()
{
	ASSERT(false);
	return S_OK;
}


const Result_t MetaFCO::CreateAttribute(MetaAttribute* &metaAttribute) throw()
{
	// Use the MetaBase helper function to create a new attribute
	return this->CreateObject(METAID_METAATTRIBUTE, ATTRID_DEFATTR_PTR, metaAttribute);
}


const Result_t MetaFCO::AddAttribute(MetaAttribute* &attrib) throw()
{
	ASSERT(false);
//	return ComAddLink(this, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p);
	return S_OK;
}


const Result_t MetaFCO::RemoveAttribute(MetaAttribute* &attrib) throw()
{
	ASSERT(false);
//	return ComRemoveLink(this, METAID_METAATTRLINK, ATTRID_ATTRLINK_USEDIN_PTR, ATTRID_ATTRLINK_ATTR_PTR, p);
	return S_OK;
}


const Result_t MetaFCO::GetAliasingEnabled(bool &value) const throw()
{
	ASSERT(false);
	return S_OK;
}


const Result_t MetaFCO::SetAliasingEnabled(const bool &value) throw()
{
	ASSERT(false);
	return S_OK;
}


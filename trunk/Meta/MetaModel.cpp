/*** Included Header Files ***/
#include "MetaModel.h"
#include "MetaGeneric.h"
//#include "MetaPointerSpec.h"
//#include "MetaConnection.h"


// --------------------------- MetaModel --------------------------- //


const Result_t MetaModel::GetDefinedFCOs(std::list<MetaFCO*> &fcoList) const throw()
{
//	return ComGetSortedCollValue<IMgaMetaFCO>(GetUnknown(), ATTRID_DEFFCO_PTR, p);
	return S_OK;
}


const Result_t MetaModel::GetRoles(std::list<MetaRole*> &roleList) const throw()
{
//	return ComGetSortedCollValue<IMgaMetaRole>(GetUnknown(), ATTRID_ROLES_COLL, p);
	return S_OK;
}


const Result_t MetaModel::GetAspects(std::list<MetaAspect*> &aspectList) const throw()
{
//	return ComGetSortedCollValue<IMgaMetaAspect>(GetUnknown(), ATTRID_ASPECTS_COLL, p);
	return S_OK;
}


const Result_t MetaModel::GetDefinedFCOByName(const std::string &name, bool &inScope, MetaFCO* &metaFCO) const throw()
{
/*		CCoreObjectPtr self(GetUnknown());
		CComBstrObj_lightequal equal( GetUnknown());
		coreobjects_type coreobjects;
		self.GetCollectionValue(ATTRID_DEFFCO_PTR, coreobjects);
		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComBstrObj n;
			(*i).GetStringValue(ATTRID_NAME, PutOut(n));
			
			if( equal(n, name))//if( n == name )
				COMRETURN( ::QueryInterface(*i, p) );

			++i;
		}

		if( inscope == VARIANT_FALSE )
			COMRETURN(E_NOTFOUND);

		CComObjPtr<IMgaMetaBase> parent;
		COMTHROW( get_DefinedIn(PutOut(parent)) );
		ASSERT( parent != NULL );

		CComObjPtr<IMgaMetaFolder> folder;
		if( SUCCEEDED(parent.QueryInterface(folder)) )
			COMRETURN( folder->get_DefinedFCOByName(name, inscope, p) );

		CComObjPtr<IMgaMetaModel> model;
		COMTHROW( parent.QueryInterface(model) );
		COMRETURN( model->get_DefinedFCOByName(name, inscope, p) );
	}
*/
	return S_OK;
}


const Result_t MetaModel::GetRoleByName(const std::string &name, MetaRole* &metaRole) const throw()
{
/*
	CCoreObjectPtr self(GetUnknown());
	CComBstrObj_lightequal equal( GetUnknown());
	coreobjects_type coreobjects;
	self.GetCollectionValue(ATTRID_ROLES_COLL, coreobjects);
	
	coreobjects_iterator i = coreobjects.begin();
	coreobjects_iterator e = coreobjects.end();
	while( i != e )
	{
		CComBstrObj n;
		(*i).GetStringValue(ATTRID_NAME, PutOut(n));
		
		if( equal(n, name))//if( n == name )
			COMRETURN( ::QueryInterface(*i, p) );
		
		++i;
	}
	
	COMRETURN(E_NOTFOUND);
*/
	return S_OK;
}


const Result_t MetaModel::GetAspectByName(const std::string &name, MetaAspect* &metaAspect) const throw()
{
/*
	CCoreObjectPtr self(GetUnknown());
	coreobjects_type coreobjects;
	self.GetCollectionValue(ATTRID_ASPECTS_COLL, coreobjects);
	coreobjects_iterator i = coreobjects.begin();
	coreobjects_iterator e = coreobjects.end();
	while( i != e )
	{
		CComBstrObj n;
		(*i).GetStringValue(ATTRID_NAME, PutOut(n));
		
		if( n == name )
			COMRETURN( ::QueryInterface(*i, p) );
		
		++i;
	}
	
	COMRETURN(E_NOTFOUND);
*/
	return S_OK;
}


const Result_t MetaModel::LegalConnectionRoles(const std::string &paths, std::list<MetaRole*> &roleList) const throw()
{
/*
	// create the collection
	CComObjPtr<RolesExCOMType> coll;
	CreateComObject(coll);
	// prepare the jointpaths
	jointpaths_type jointpaths;
	ASSERT( metaproject != NULL );
	metaproject->CreateJointPaths(paths, jointpaths);
	// traverse the roles
	CCoreObjectPtr self(GetUnknown());
	coreobjects_type roles;
	self.GetCollectionValue(ATTRID_ROLES_COLL, roles);
	coreobjects_iterator i = roles.begin();
	coreobjects_iterator e = roles.end();
	while( i != e )
	{
		CCoreObjectPtr kind;
		i->GetPointerValue(ATTRID_KIND_PTR, kind);
		if( kind.GetMetaID() == METAID_METACONNECTION &&
		   CMgaMetaConnection::CheckPaths(kind, jointpaths) )
		{
			CComObjPtr<IMgaMetaRole> role;
			COMTHROW( ::QueryInterface(*i, role) );
			coll->Add(role);
		}
		++i;
	}
	// return
	MoveTo(coll, p);
*/
	return S_OK;
}


const Result_t MetaModel::LegalReferenceRoles(const std::string &path, std::list<MetaRole*> &roleList) const throw()
{
/*
	// create the collection
	CComObjPtr<RolesExCOMType> coll;
	CreateComObject(coll);
	
	// prepare the pathitems
	pathitems_type pathitems;
	
	ASSERT( metaproject != NULL );
	metaproject->CreatePathItems(begin(path), end(path), pathitems);
	
	// traverse the roles
	CCoreObjectPtr self(GetUnknown());
	
	coreobjects_type roles;
	self.GetCollectionValue(ATTRID_ROLES_COLL, roles);
	
	coreobjects_iterator i = roles.begin();
	coreobjects_iterator e = roles.end();
	while( i != e )
	{
		CCoreObjectPtr kind;
		i->GetPointerValue(ATTRID_KIND_PTR, kind);
		
		if( kind.GetMetaID() == METAID_METAREFERENCE &&
		   CMgaMetaPointerSpec::CheckPath(kind, pathitems, true) )
		{
			CComObjPtr<IMgaMetaRole> role;
			COMTHROW( ::QueryInterface(*i, role) );
			
			coll->Add(role);
		}
		
		++i;
	}
*/
	return S_OK;
}


const Result_t MetaModel::LegalSetRoles(const std::string &path, std::list<MetaRole*> &roleList) const throw()
{
/*
	// create the collection
	CComObjPtr<RolesExCOMType> coll;
	CreateComObject(coll);
	
	// prepare the pathitems
	pathitems_type pathitems;
	
	ASSERT( metaproject != NULL );
	metaproject->CreatePathItems(begin(path), end(path), pathitems);
	
	// traverse the roles
	CCoreObjectPtr self(GetUnknown());
	
	coreobjects_type roles;
	self.GetCollectionValue(ATTRID_ROLES_COLL, roles);
	
	coreobjects_iterator i = roles.begin();
	coreobjects_iterator e = roles.end();
	while( i != e )
	{
		CCoreObjectPtr kind;
		i->GetPointerValue(ATTRID_KIND_PTR, kind);
		
		if( kind.GetMetaID() == METAID_METASET &&
		   CMgaMetaPointerSpec::CheckPath(kind, pathitems, false) )
		{
			CComObjPtr<IMgaMetaRole> role;
			COMTHROW( ::QueryInterface(*i, role) );
			
			coll->Add(role);
		}
		
		++i;
	}
*/
	return S_OK;
}


const Result_t MetaModel::LegalRoles(MetaFCO* &kind, std::list<MetaRole*> &roleList) const throw()
{
	if( kind == NULL ) return E_INVALID_USAGE;
/*
	CComObjPtr<RolesExCOMType> coll;
	CreateComObject(coll);
	
	// traverse the roles
	CCoreObjectPtr self(GetUnknown());
	
	coreobjects_type roles;
	self.GetCollectionValue(ATTRID_ROLES_COLL, roles);
	
	coreobjects_iterator i = roles.begin();
	coreobjects_iterator e = roles.end();
	while( i != e )
	{
		CCoreObjectPtr rolekind;
		i->GetPointerValue(ATTRID_KIND_PTR, rolekind);
		
		if( IsEqualObject(rolekind,kind) )
		{
			CComObjPtr<IMgaMetaRole> role;
			COMTHROW( ::QueryInterface(*i, role) );
			
			coll->Add(role);
		}
		
		++i;
	}
*/	
	return S_OK;
}

const Result_t MetaModel::CreateRole(const MetaFCO* &kind, MetaRole* &role) throw()
{
	ASSERT(false);
	return S_OK;
}
/*
STDMETHODIMP CMgaMetaModel::CreateRole(IMgaMetaFCO *kind, IMgaMetaRole **p)
{
	CHECK_OUT(p);

	if( kind == NULL )
		COMRETURN(E_POINTER);

	ASSERT( metaproject != NULL );

	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());
		ASSERT( self != NULL );

		CCoreObjectPtr other(kind);
		ASSERT( other != NULL );

		CCoreObjectPtr role;
		metaproject->CreateMetaBase(METAID_METAROLE, role);

		role.PutPointerValue(ATTRID_ROLES_COLL, self);
		role.PutPointerValue(ATTRID_KIND_PTR, other);

		COMTHROW( ::QueryInterface(role, p) );
	}
	COMCATCH(;)
}
*/

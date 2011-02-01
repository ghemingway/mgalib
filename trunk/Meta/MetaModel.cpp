/*** Included Header Files ***/
#include "MetaModel.h"
#include "MetaGeneric.h"
#include "MetaConnection.h"
#include "MetaAspect.h"
#include "MetaRole.h"
#include "MetaReference.h"
#include "MetaSet.h"

//#include "MetaPointerSpec.h"


// --------------------------- MetaModel --------------------------- //


const Result_t MetaModel::GetDefinedFCOs(std::list<MetaFCO*> &fcoList) const throw()
{
//	return ComGetSortedCollValue<IMgaMetaFCO>(GetUnknown(), ATTRID_DEFFCO_PTR, p);
	return S_OK;
}


const Result_t MetaModel::GetRoles(std::list<MetaRole*> &roleList) const throw()
{
	// Use the MetaBase helper function to get collection of objects
	return this->CollectionFromAttribute(ATTRID_ROLES_COLL, roleList);
}


const Result_t MetaModel::GetAspects(std::list<MetaAspect*> &aspectList) const throw()
{
	// Use the MetaBase helper function to get collection of objects
	return this->CollectionFromAttribute(ATTRID_ASPECTS_COLL, aspectList);
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


const Result_t MetaModel::CreateModel(MetaModel* &metaModel) throw()
{
	// Use the MetaBase helper function to create a new object
	return this->CreateObject(METAID_METAMODEL, ATTRID_DEFFCO_PTR, metaModel);
}


const Result_t MetaModel::CreateAtom(MetaAtom* &metaAtom) throw()
{
	// Use the MetaBase helper function to create a new object
	return this->CreateObject(METAID_METAATOM, ATTRID_DEFFCO_PTR, metaAtom);
}


const Result_t MetaModel::CreateReference(MetaReference* &metaReference) throw()
{
	// Use the MetaBase helper function to create a new object
	return this->CreateObject(METAID_METAREFERENCE, ATTRID_DEFFCO_PTR, metaReference);
}


const Result_t MetaModel::CreateSet(MetaSet* &metaSet) throw()
{
	// Use the MetaBase helper function to create a new object
	return this->CreateObject(METAID_METASET, ATTRID_DEFFCO_PTR, metaSet);
}


const Result_t MetaModel::CreateConnection(MetaConnection* &metaConnection) throw()
{
	// Use the MetaBase helper function to create a new object
	return this->CreateObject(METAID_METACONNECTION, ATTRID_DEFFCO_PTR, metaConnection);
}


const Result_t MetaModel::CreateAspect(MetaAspect* &metaAspect) throw()
{
	// Use the MetaBase helper function to create a new object
	return this->CreateObject(METAID_METAASPECT, ATTRID_ASPECTS_COLL, metaAspect);
}


const Result_t MetaModel::CreateRole(const MetaFCO* &kind, MetaRole* &metaRole) throw()
{
	ASSERT(false);
	// Use the MetaBase helper function to create a new object
	return this->CreateObject(METAID_METAROLE, ATTRID_ROLES_COLL, metaRole);
//	role.PutPointerValue(ATTRID_ROLES_COLL, self);
//	role.PutPointerValue(ATTRID_KIND_PTR, other);
	return S_OK;
}


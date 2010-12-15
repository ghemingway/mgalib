/*** Included Header Files ***/
#include "MetaConnection.h"
#include "MetaGeneric.h"
//#include "MetaConnJoint.h"


// --------------------------- MetaConnection --------------------------- //


void MetaConnection::Traverse(MetaProject* &metaProject, CoreObject* &coreObject)
{
	ASSERT( metaProject != NULL );
	ASSERT( coreObject != NULL );
	// Traverse the base class
	MetaFCO::Traverse(metaProject, coreObject);
	// Traverse any children
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_CONNJOINTS_COLL);
}

/*
bool MetaConnection::CheckPaths(CCoreObjectPtr &self, jointpaths_type jointpaths)
{
	coreobjects_type objects;
	self.GetCollectionValue(ATTRID_CONNJOINTS_COLL, objects);
	
	coreobjects_iterator i = objects.begin();
	coreobjects_iterator e = objects.end();
	while( i != e )
	{
		if( CMgaMetaConnJoint::CheckPaths(*i, jointpaths) )
			return true;
		
		++i;
	}
	
	return false;
}
*/

const Result_t MetaConnection::GetJoints(std::list<MetaConnJoint*> &jointList) const throw()
{
	// Use the MetaBase helper function to get collection from the attribute
	//	return this->CollectionFromAttribute(ATTRID_CONNJOINTS_COLL, enumList);
	return S_OK;
}


const Result_t MetaConnection::CheckPaths(const std::string &paths, bool &valid) const throw()
{
//	jointpaths_type jointpaths;
//	metaproject->CreateJointPaths(paths, jointpaths);
//	CCoreObjectPtr self(GetUnknown());
//	*p = CheckPaths(self, jointpaths) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

/*
STDMETHODIMP MetaConnection::CreateJoint(IMgaMetaConnJoint **p)
{
	CHECK_OUT(p);

	ASSERT( metaproject != NULL );

	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());
		ASSERT( self != NULL );

		CCoreObjectPtr joint;
		metaproject->CreateMetaObj(METAID_METACONNJOINT, joint);

		joint.PutPointerValue(ATTRID_CONNJOINTS_COLL, self);

		COMTHROW( ::QueryInterface(joint, p) );
	}
	COMCATCH(;)
}
*/

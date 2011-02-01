/*** Included Header Files ***/
#include "MetaConnection.h"
#include "MetaGeneric.h"
#include "MetaConnJoint.h"


// --------------------------- MetaConnection --------------------------- //


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
	// Use the MetaBase helper function to get collection from an attribute
	return this->CollectionFromAttribute(ATTRID_CONNJOINTS_COLL, jointList);
}


const Result_t MetaConnection::CheckPaths(const std::string &paths, bool &valid) const throw()
{
//	jointpaths_type jointpaths;
//	metaproject->CreateJointPaths(paths, jointpaths);
//	CCoreObjectPtr self(GetUnknown());
//	*p = CheckPaths(self, jointpaths) ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}


const Result_t MetaConnection::CreateJoint(MetaConnJoint* &connJoint) throw()
{
	// Use the MetaBase helper function to create a new MetaConnJoint
	return this->CreateObject(METAID_METACONNJOINT, ATTRID_CONNJOINTS_COLL, connJoint);
}


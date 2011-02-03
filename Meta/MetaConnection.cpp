/*** Included Header Files ***/
#include "MetaConnection.h"
#include "MetaGeneric.h"
#include "MetaConnJoint.h"


// --------------------------- MetaConnection --------------------------- //


const Result_t MetaConnection::GetJoints(std::list<MetaConnJoint*> &jointList) const throw()
{
	// Use the MetaBase helper function to get collection from an attribute
	return MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_CONNJOINTS_COLL, jointList);
}


const Result_t MetaConnection::CreateJoint(MetaConnJoint* &connJoint) throw()
{
	// Use the MetaBase helper function to create a new MetaConnJoint
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METACONNJOINT, ATTRID_CONNJOINTS_COLL, connJoint);
}


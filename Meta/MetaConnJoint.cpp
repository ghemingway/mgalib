/*** Included Header Files ***/
#include "MetaConnJoint.h"
#include "MetaPointerSpec.h"
#include "MetaGeneric.h"


// --------------------------- MetaConnJoint --------------------------- //


MetaConnJoint::MetaConnJoint(CoreObject &coreObject, MetaProject* const &metaProject) :
	_coreObject(coreObject), _metaProject(metaProject)
{
	ASSERT(coreObject != NULL);
	ASSERT(metaProject != NULL);
}


const Result_t MetaConnJoint::GetPointerSpecs(std::list<MetaPointerSpec*> &pointerSpecList) const throw()
{
	// Use the MetaBase helper function to get collection of objects
	return MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_PTRSPECS_COLL, pointerSpecList);
}


const Result_t MetaConnJoint::GetParent(MetaConnection* &parent) const throw()
{
	// Use the MetaBase helper function to get this pointer object
	return MetaBase::ObjectFromAttribute(this->_coreObject, this->_metaProject, ATTRID_CONNJOINTS_COLL, parent);
}


const Result_t MetaConnJoint::GetPointerSpecByName(const std::string &name, MetaPointerSpec* &pointerSpec) const throw()
{
	// Use the MetaBase helper function to get this pointer object by name
//	return MetaBase::ObjectFromCollectionByName(this->_coreObject, this->_metaProject, ATTRID_PTRSPECS_COLL, ATTRID_PTRSPECNAME, pointerSpec);
	return S_OK;
}


const Result_t MetaConnJoint::CreatePointerSpec(MetaPointerSpec* &pointerSpec) throw()
{
	// Use the MetaBase helper function to create a new object
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METAPOINTERSPEC, ATTRID_PTRSPECS_COLL, pointerSpec);
	return S_OK;
}


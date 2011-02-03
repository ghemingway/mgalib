/*** Included Header Files ***/
#include "MetaConnJoint.h"
#include "MetaPointerSpec.h"


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
//	return this->CollectionFromAttribute(ATTRID_PTRSPECS_COLL, pointerSpecList);
	return S_OK;
}


const Result_t MetaConnJoint::GetParent(MetaConnection* &parent) const throw()
{
	// Use the MetaBase helper function to get this pointer object
//	return this->ObjectFromAttribute(ATTRID_CONNJOINTS_COLL, parent);
	return S_OK;
}


const Result_t MetaConnJoint::GetPointerSpecByName(const std::string &name, MetaPointerSpec* &pointerSpec) const throw()
{
	ASSERT(false);
//	return ComGetCollValueByName(name, GetUnknown(), ATTRID_PTRSPECS_COLL, ATTRID_PTRSPECNAME, p);
	return S_OK;
}


const Result_t MetaConnJoint::CreatePointerSpec(MetaPointerSpec* &pointerSpec) throw()
{
	// Use the MetaBase helper function to create a new object
//	return this->CreateObject(METAID_METAPOINTERSPEC, ATTRID_PTRSPECS_COLL, pointerSpec);
	return S_OK;
}


/*** Included Header Files ***/
#include "MetaRole.h"
#include "MetaPart.h"
#include "MetaGeneric.h"


// --------------------------- MetaRole --------------------------- //


const Result_t  MetaRole::GetParentModel(MetaModel* &metaModel) throw()
{
	// Use the MetaBase helper function to get a parent MetaModel from this pointer attribute
	return MetaBase::ObjectFromAttribute(this->_coreObject, this->_metaProject, ATTRID_ROLES_COLL, metaModel);
}


const Result_t MetaRole::GetParts(std::list<MetaPart*> &partList) throw()
{
	// Use the MetaBase helper function to get collection of MetaParts
	return this->CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_PARTROLE_PTR, partList);
}


const Result_t MetaRole::GetKind(MetaFCO* &metaFCO) throw()
{
	// Use the MetaBase helper function to get a kind MetaModel from this pointer attribute
	return MetaBase::ObjectFromAttribute(this->_coreObject, this->_metaProject, ATTRID_KIND_PTR, metaFCO);
}


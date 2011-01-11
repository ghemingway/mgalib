/*** Included Header Files ***/
#include "MetaRole.h"
#include "MetaPart.h"


// --------------------------- MetaConstraint --------------------------- //


const Result_t MetaRole::GetParentModel(MetaModel* &metaModel) throw()
{
	ASSERT(false);
//	return ComGetPointerValue(GetUnknown(), ATTRID_ROLES_COLL, metaModel);
	return S_OK;
}


const Result_t MetaRole::GetParts(std::list<MetaPart*> &partList) throw()
{
	ASSERT(false);
//	return ComGetCollectionValue<IMgaMetaPart>(GetUnknown(), ATTRID_PARTROLE_PTR, partList);
	return S_OK;
}


const Result_t MetaRole::GetKind(MetaFCO* &metaFCO) throw()
{
	ASSERT(false);
//	return ComGetPointerValue(GetUnknown(), ATTRID_KIND_PTR, metaFCO);
	return S_OK;
}
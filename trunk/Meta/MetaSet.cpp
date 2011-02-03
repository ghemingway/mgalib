/*** Included Header Files ***/
#include "MetaSet.h"
#include "MetaPointerItem.h"
#include "MetaGeneric.h"


// --------------------------- MetaSet --------------------------- //


const Result_t MetaSet::GetItems(std::list<MetaPointerItem*> &pointerList) const throw()
{
	// Use the MetaBase helper function to get collection of objects
	return MetaBase::CollectionFromAttribute(this->_coreObject, this->_metaProject, ATTRID_PTRITEMS_COLL, pointerList);
}


const Result_t MetaSet::CreateItem(MetaPointerItem* &pointerItem) throw()
{
	// Use the helper function to create a new object
	return MetaBase::CreateObject(this->_coreObject, this->_metaProject, METAID_METAPOINTERITEM, ATTRID_PTRITEMS_COLL, pointerItem);
}


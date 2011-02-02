/*** Included Header Files ***/
#include "MetaPointerSpec.h"
#include "MetaPointerItem.h"
#include "MetaGeneric.h"


// --------------------------- MetaPointerSpec --------------------------- //

/*
bool MetaPointerSpec::CheckPath(CCoreObjectPtr &self, pathitems_type &pathitems, bool global)
{
	ASSERT( self != NULL );
	
	coreobjects_type coreobjects;
	self.GetCollectionValue(ATTRID_PTRITEMS_COLL, coreobjects);
	
	coreobjects_iterator i = coreobjects.begin();
	coreobjects_iterator e = coreobjects.end();
	while( i != e )
	{
		if( CMgaMetaPointerItem::CheckPath(*i, pathitems, global) )
			return true;
		
		++i;
	}
	
	return false;
}
*/

const Result_t MetaPointerSpec::GetParent(MetaBase* &parent) throw()
{
	ASSERT(false);
	// Use the MetaBase helper function to get an object from this pointer attribute
//	return this->ObjectFromAttribute(ATTRID_PTRSPECS_COLL, parent);
	return S_OK;
}


const Result_t MetaPointerSpec::GetName(std::string &name) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_PTRSPECNAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}

const Result_t MetaPointerSpec::SetName(const std::string &name) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_PTRSPECNAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaPointerSpec::GetItems(std::list<MetaPointerItem*> &pointerList) const throw()
{
	ASSERT(false);
	// Use the MetaBase helper function to get collection of MetaPointerItems
//	return this->CollectionFromAttribute(ATTRID_PTRITEMS_COLL, pointerList);
	return S_OK;
}


const Result_t MetaPointerSpec::CreateItem(MetaPointerItem* &pointerItem) throw()
{
	ASSERT(false);
	// Use the MetaBase helper function to create a new attribute
//	return this->CreateObject(METAID_METAPOINTERITEM, ATTRID_PTRITEMS_COLL, pointerItem);
	return S_OK;
}
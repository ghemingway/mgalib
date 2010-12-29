/*** Included Header Files ***/
#include "CoreObject.h"
#include "CoreAttribute.h"
#include "CoreProject.h"
#include "CoreMetaProject.h"
#include "CoreMetaObject.h"


// --------------------------- CoreObject Private Methods ---------------------------


CoreObjectBase::CoreObjectBase(CoreProject *project, CoreMetaObject *metaObject, const Uuid &uuid) :
	_project(project), _metaObject(metaObject), _uuid(uuid), _attributeMap(), _isDirty(false), _refCount(0)
{
	// Make sure project and metaObject are valid
	ASSERT( project != NULL );
	ASSERT( metaObject != NULL );
	ASSERT( uuid != Uuid::Null() );
	// Must regiter object with parent project
	this->_project->RegisterObject(this->_uuid, this);

	// Create all of the attributes
	std::list<CoreMetaAttribute*> metaAttributes;
	ASSERT( this->_metaObject->GetAttributes(metaAttributes)== S_OK );
	std::list<CoreMetaAttribute*>::iterator metaAttrIter = metaAttributes.begin();
	while( metaAttrIter != metaAttributes.end() )
	{
		CoreAttributeBase::Create(this, *metaAttrIter);
		++metaAttrIter;
	}
}

/*
STDMETHODIMP CoreObjectBase::SearchCollection(attrid_type coll_attrid, attrid_type search_attrid,
	VARIANT search_value, ICoreObject **p)
{
	CHECK_OUT(p);

	if( IsZombie() )
		COMRETURN(E_ZOMBIE);

	CCoreAttribute *attribute = FindAttribute(coll_attrid);
	if( attribute == NULL || attribute->GetValType() != VALTYPE_COLLECTION )
		return E_INVALIDARG;

	COMTRY
	{
		CCoreCollectionAttribute *collection = 
			static_cast<CCoreCollectionAttribute*>(attribute);
		
		CComObjPtr<CCoreObject> ret = collection->SearchCollection(search_attrid, search_value);
		MoveTo(ret, p);
	}
	COMCATCH(;)
}


STDMETHODIMP CoreObjectBase::get_IsDeleted(VARIANT_BOOL *p)
{
	CHECK_OUT(p);

	if( IsZombie() )
		COMRETURN(E_ZOMBIE);

	*p = VARIANT_FALSE;

	COMTRY
	{
		CCoreAttribute *lock = FindAttribute(ATTRID_LOCK);
		ASSERT( lock != NULL && lock->GetValType() == VALTYPE_LOCK );
		
		if( static_cast<CCoreLockAttribute*>(lock)->IsLoaded() &&
			HasEmptyPointers() && 
			(objid != OBJID_ROOT || GetMetaID() != METAID_ROOT) )
		{
			*p = VARIANT_TRUE;
		}
	}
	COMCATCH(;)
}


void CoreObjectBase::Delete()
{
	ASSERT( !this->IsZombie() );
	CoreAttribute *lock = this->Attribute(ATTRID_LOCK);
	ASSERT( lock != NULL && lock->GetValType() == VALTYPE_LOCK );
	COMTHROW( lock->put_Value(PutInVariant(locking_type(LOCKING_EXCLUSIVE))) );
	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		if( (*i)->GetValueType() == ValueType::Collection() )
		{
			if( !((CCoreCollectionAttribute*)(*i))->IsEmptyFromStorage() )
				HR_THROW(E_NONEMPTY_COLL);
		}
			++i;
	}
		i = attributes.begin();
	while( i != e )
	{
		if( (*i)->GetValueType() == ValueType::Pointer() )
			COMTHROW( (*i)->put_Value(PutInVariant((IDispatch*)NULL)) );
			++i;
	}
		COMTHROW( lock->put_Value(PutInVariant(locking_type(LOCKING_NONE))) );
}
*/

// --------------------------- CoreObjectBase Public Methods ---------------------------


const Result_t CoreObjectBase::Create(CoreProject *project, const Uuid &uuid, CoreObject &coreObject) throw()
{
	if( project == NULL ) return E_INVALID_USAGE;
	if( uuid == Uuid::Null() ) return E_INVALID_USAGE;
	
	CoreMetaProject *metaProject = NULL;
	ASSERT( project->MetaProject(metaProject) == S_OK );
	// Set storage to point at this object and get the coreMetaObject
	ICoreStorage* storage;
	storage = project->SetStorageObject(uuid);
	ASSERT( storage != NULL );
	CoreMetaObject *metaObject = NULL;
	ASSERT( storage->MetaObject(metaObject) == S_OK );
	ASSERT( metaObject != NULL );
	// Create the actual coreObjectBase (new a shiny new & unique Uuid)
	CoreObjectBase* coreObjectBase = new CoreObjectBase(project, metaObject, uuid);
	ASSERT( coreObjectBase != NULL );
	// Set the CoreObject (refPointer) to the base object
	coreObject = coreObjectBase;
	return S_OK;
}


CoreObjectBase::~CoreObjectBase()
{
	// Make sure all outstanding CoreObjects have been deleted
	ASSERT( this->_refCount == 0 );
	
	// The attribute will remove itself from the list
	while( !this->_attributeMap.empty() )
	{
		std::map<AttrID_t,CoreAttributeBase*>::iterator attrIter = this->_attributeMap.begin();
		if (attrIter->second != NULL) delete attrIter->second;
	}
	// Unregister the object from the project
	ASSERT( this->_project != NULL );
	this->_project->UnregisterObject(this->_uuid);
}


void CoreObjectBase::RegisterAttribute(const AttrID_t &attrID, CoreAttributeBase *attribute) throw()
{
	ASSERT( attrID != ATTRID_NONE );
	ASSERT( attribute != NULL );
	// We should really make sure this attribute belongs and is not already in the list
	ASSERT( this->_attributeMap.insert( std::make_pair(attrID, attribute) ).second );
}


void CoreObjectBase::UnregisterAttribute(const AttrID_t &attrID) throw()
{
	ASSERT( attrID != ATTRID_NONE );
	// Try to find the attribute
	std::map<AttrID_t,CoreAttributeBase*>::iterator attrIter = this->_attributeMap.find(attrID);
	ASSERT( attrIter != this->_attributeMap.end() );
	// Remove the attribute from the Map
	this->_attributeMap.erase(attrIter);
}


ICoreStorage* CoreObjectBase::SetStorageObject(void) const
{
	// Set the project storage to point to this object
	return this->_project->SetStorageObject( this->_uuid );
}


const Result_t CoreObjectBase::Attribute(const AttrID_t &attrID, CoreAttribute* &attribute) const throw()
{
	ASSERT(false);
	// Make sure we are ready
	if( attrID == ATTRID_NONE ) return E_INVALID_USAGE;
	// Iterate through attrib list and find matchind ID
	std::map<AttrID_t,CoreAttributeBase*>::const_iterator attrIter = this->_attributeMap.find(attrID);
	if (attrIter == this->_attributeMap.end()) return E_NOTFOUND;
/*
	// Found it, now create a nice CoreAttribute object that points at it
	attribute = new CoreAttribute(attrIter->second);
	ASSERT( attribute != NULL );
*/
	// Never found it, return error
	return S_OK;
}


const Result_t CoreObjectBase::InTransaction(bool &flag) const throw()
{
	// Just call to the parent project to find out
	ASSERT( this->_project != NULL );
	return this->_project->InTransaction(flag);
}


const Result_t CoreObjectBase::InWriteTransaction(bool &flag) const throw()
{
	// Just call to the parent project to find out
	ASSERT( this->_project != NULL );
	return this->_project->InWriteTransaction(flag);
}


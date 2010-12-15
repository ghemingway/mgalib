/*** Included Header Files ***/
#include "CoreObject.h"
#include "CoreAttribute.h"
#include "CoreProject.h"
#include "CoreMetaProject.h"
#include "CoreMetaObject.h"


// --------------------------- CoreObject Private Methods ---------------------------


CoreObjectBase::CoreObjectBase(CoreProject *project, CoreMetaObject *metaObject, const ObjID_t &objID) :
	_project(project), _metaObject(metaObject), _idPair(METAID_NONE, objID), _attributeMap(), _isDirty(false), _refCount(0)
{
	// Make sure project and metaObject are valid
	ASSERT( project != NULL );
	ASSERT( metaObject != NULL );
	ASSERT( metaObject->MetaID(this->_idPair.metaID) == S_OK );
	// Must regiter object with parent project
	this->_project->RegisterObject(this->_idPair, this);

	// Create all of the attributes
	std::list<CoreMetaAttribute*> metaAttributes;
	ASSERT( this->_metaObject->Attributes(metaAttributes)== S_OK );
	std::list<CoreMetaAttribute*>::iterator metaAttrIter = metaAttributes.begin();
	while( metaAttrIter != metaAttributes.end() )
	{
		CoreAttributeBase::Create(this, *metaAttrIter);
		++metaAttrIter;
	}
}

/*
STDMETHODIMP CoreObjectBase::get_Attributes(ICoreAttributes **p)
{
	CHECK_OUT(p);

	COMTRY
	{
		typedef CCoreCollection<ICoreAttributes, std::vector<ICoreAttribute*>,
			ICoreAttribute, CCoreAttribute> COMTYPE;

		CComObjPtr<COMTYPE> q;
		CreateComObject(q);

		q->Fill(attributes.begin(), attributes.end());
		MoveTo(q,p);
	}
	COMCATCH(;)
}


STDMETHODIMP CoreObjectBase::get_LoadedAttrValue(attrid_type attrid, VARIANT *p)
{
	CHECK_OUT(p);

	if( IsZombie() )
		COMRETURN(E_ZOMBIE);

	CCoreAttribute *attribute = FindAttribute(attrid);
	if( attribute == NULL )
		return E_INVALIDARG;

	return attribute->get_LoadedValue(p);
}


STDMETHODIMP CCoreObject::get_PreviousAttrValue(attrid_type attrid, VARIANT *p)
{
	CHECK_OUT(p);

	if( IsZombie() )
		COMRETURN(E_ZOMBIE);

	CCoreAttribute *attribute = FindAttribute(attrid);
	if( attribute == NULL )
		return E_INVALIDARG;

	return attribute->get_PreviousValue(p);
}


STDMETHODIMP CoreObjectBase::get_PeerLockValue(attrid_type attrid, locking_type *p)
{
	CHECK_OUT(p);

	if( IsZombie() )
		COMRETURN(E_ZOMBIE);

	CCoreAttribute *attribute = FindAttribute(attrid);
	if( attribute == NULL || attribute->GetValType() != VALTYPE_LOCK )
		return E_INVALIDARG;

	return ((CCoreLockAttribute*)attribute)->get_PeerLockValue(p);
}


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


// ------- Creation 


void CoreObjectBase::FillAfterCreateObject()
{
	ASSERT( !attributes.empty() );

	attributes_iterator i = attributes.begin();
	attributes_iterator e = attributes.end();
	while( i != e )
	{
		(*i)->FillAfterCreateObject();
		++i;
	}

	RegisterFinalTrItem();
}
*/

// --------------------------- CoreObjectBase Public Methods ---------------------------


const Result_t CoreObjectBase::Create(CoreProject *project, const MetaObjIDPair &idPair, CoreObject* &coreObject) throw()
{
	if( project == NULL ) return E_INVALID_USAGE;
	if( idPair.metaID == METAID_NONE || idPair.objID == OBJID_NONE ) return E_INVALID_USAGE;
	
	CoreMetaProject *metaProject = NULL;
	Result_t result = project->MetaProject(metaProject);
	ASSERT( result == S_OK );
	CoreMetaObject *metaObject = NULL;
	// Find the metaObject (tests if metaID is valid)
	result = metaProject->Object(idPair.metaID, metaObject);
	if( result != S_OK ) return E_METAID;
	ASSERT( metaObject != NULL );
	// Create the actual coreObjectBase
	CoreObjectBase* coreObjectBase = new CoreObjectBase(project, metaObject, idPair.objID);
	ASSERT( coreObjectBase != NULL );
	// Create the CoreObject (refPointer) to the base object
	coreObject = coreObjectBase->Reference();
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
	if( this->_project != NULL )
		this->_project->UnregisterObject(this->_idPair);
}


CoreObject* CoreObjectBase::Reference(void)
{
	// Increase refCount
	this->_refCount++;
	// Create a new CoreObject
	CoreObject* newObject = new CoreObject(this);
	ASSERT( newObject != NULL );
	return newObject;
}


void CoreObjectBase::Release(void)
{
	// Decrement the refCount
	this->_refCount--;
	// Is this object no longer needed
	if (this->_refCount == 0) delete this;
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
	return this->_project->SetStorageObject( this->_idPair );
}


const Result_t CoreObjectBase::Attribute(const AttrID_t &attrID, CoreAttribute* &attribute) const throw()
{
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

/*  
void CoreObjectBase::GetAttributes(CoreLockAttribute *lockAttribute, std::list<CoreAttribute*> &controlled) throw()
{
//	ASSERT( lockAttribute != NULL );
	ASSERT( controlled.empty() );
//	AttrID_t attrID = lockAttribute->GetAttrID();
	CoreAttributeListIterator i = this->_attributes.begin();
	CoreAttributeListIterator e = this->_attributes.end();
	while( i != e )
	{
//		AttrID_t lockattrid = ATTRID_NONE;
//		if( (*i)->GetLockAttr() == lockAttribute )
//		{
//			ASSERT( lockattrid == lockAttribute->GetAttrID() );
//			controlled.push_back( *i );
//		}
		++i;
	}
}


void CoreObjectBase::LoadAttributes(CoreLockAttribute *lockAttribute)
{
	ASSERT( lockAttribute != NULL );
	std::list<CoreAttribute*> controlled;
	this->GetAttributes(lockAttribute, controlled);

	CoreAttributeListIterator i = controlled.begin();
	CoreAttributeListIterator e = controlled.end();

	while( i != e )
	{
		(*i)->Load();
		++i;
	}
//	catch(result_exception &)
//	{
//		e = i;
//		i = controlled.begin();
//		while( i != e )
//		{
//			(*i)->Unload();
//			++i;
//		}
//		throw;
//	}
}


void CoreObjectBase::UnloadAttributes(CoreLockAttribute *lockAttribute) throw()
{
	ASSERT( lockAttribute != NULL );

	std::list<CoreAttribute*> controlled;
	this->GetAttributes(lockAttribute, controlled);

	CoreAttributeListIterator i = controlled.begin();
	CoreAttributeListIterator e = controlled.end();
	while( i != e )
	{
		(*i)->Unload();

		++i;
	}
}
*/


const Result_t CoreObjectBase::InTransaction(bool &flag) const throw()
{
	ASSERT( this->_project != NULL );
	return this->_project->InTransaction(flag);
}

/*
bool CoreObjectBase::InWriteTransaction(void) const throw()
{
	ASSERT( !IsZombie() );
	ASSERT( this->_project != NULL );

	return this->_project->InWriteTransaction();
}


bool CoreObjectBase::HasEmptyPointers(void) const throw()
{
	ASSERT( this->_project != NULL );
	
	CoreAttributeList::const_iterator i = this->_attributes.begin();
	CoreAttributeList::const_iterator e = this->_attributes.end();
	while( i != e )
	{
		if( (*i)->GetValueType() == ValueType::Pointer() )
		{
			if( !((CorePointerAttributeBase*)(*i))->IsEmpty() )
				return false;
		}

		++i;
	}

	return true;
}
*/

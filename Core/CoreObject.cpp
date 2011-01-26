/*** Included Header Files ***/
#include "CoreObject.h"
#include "CoreProject.h"
#include "CoreMetaProject.h"


// --------------------------- CoreObjectBase Private Methods ---------------------------


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
	Result_t result = this->_metaObject->GetAttributes(metaAttributes);
	ASSERT( result == S_OK );
	std::list<CoreMetaAttribute*>::iterator metaAttrIter = metaAttributes.begin();
	while( metaAttrIter != metaAttributes.end() )
	{
		CoreAttributeBase::Create(this, *metaAttrIter);
		++metaAttrIter;
	}
}


// --------------------------- CoreObjectBase Public Methods ---------------------------


const Result_t CoreObjectBase::Create(CoreProject *project, const Uuid &uuid, CoreObject &coreObject) throw()
{
	if( project == NULL ) return E_INVALID_USAGE;
	if( uuid == Uuid::Null() ) return E_INVALID_USAGE;
	
	CoreMetaProject *metaProject = NULL;
	Result_t result = project->MetaProject(metaProject);
	ASSERT( result == S_OK );
	ASSERT( metaProject != NULL );
	// Set storage to point at this object and get the coreMetaObject
	ICoreStorage* storage;
	storage = project->SetStorageObject(uuid);
	ASSERT( storage != NULL );
	CoreMetaObject *metaObject = NULL;
	result = storage->MetaObject(metaObject);
	ASSERT( result == S_OK );
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
	// TODO: We should really make sure this attribute belongs and is not already in the list
	bool retVal = this->_attributeMap.insert( std::make_pair(attrID, attribute) ).second;
	ASSERT( retVal );
}


void CoreObjectBase::UnregisterAttribute(const AttrID_t &attrID) throw()
{
	ASSERT( attrID != ATTRID_NONE );
	// Try to find the attribute
	std::map<AttrID_t,CoreAttributeBase*>::iterator attrIter = this->_attributeMap.find(attrID);
	// TODO: Turn this into an non-S_OK return
	ASSERT( attrIter != this->_attributeMap.end() );
	// Remove the attribute from the Map
	this->_attributeMap.erase(attrIter);
}


ICoreStorage* CoreObjectBase::SetStorageObject(void) const
{
	// Set the project storage to point to this object
	return this->_project->SetStorageObject( this->_uuid );
}


const Result_t CoreObjectBase::Attribute(const AttrID_t &attrID, CoreAttributeBase* &attribute) const throw()
{
	// Make sure we are ready
	if( attrID == ATTRID_NONE ) return E_INVALID_USAGE;
	// Iterate through attrib list and find matchind ID
	std::map<AttrID_t,CoreAttributeBase*>::const_iterator attrIter = this->_attributeMap.find(attrID);
	if (attrIter == this->_attributeMap.end()) return E_NOTFOUND;
	// Found it, now create a nice CoreAttribute object that points at it
	attribute = attrIter->second;
	ASSERT( attribute != NULL );
	// All is good
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


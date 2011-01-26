//*** Included Header Files ***/
#include "CoreAttribute.h"
#include "CoreProject.h"
#include "CoreObject.h"


// --------------------------- Private CoreAttributeBase Methods --------------------------- //


const Result_t CoreAttributeBase::Create(CoreObjectBase *parent, CoreMetaAttribute *metaAttribute) throw()
{
	// Make sure we have good inputs
	if( parent == NULL ) return E_INVALID_USAGE;
	if( metaAttribute == NULL ) return E_INVALID_USAGE;
	// Create the attribute, base it on the correct type
	CoreAttributeBase* attribute = NULL;
	ValueType valueType;
	Result_t result = metaAttribute->GetValueType(valueType);
	ASSERT( result == S_OK );
	// Now actually create the attribute
	if(valueType == ValueType::Long())
		attribute = new CoreAttributeLong(parent, metaAttribute);
	else if (valueType == ValueType::Real())
		attribute = new CoreAttributeReal(parent, metaAttribute);
	else if (valueType == ValueType::String())
		attribute = new CoreAttributeString(parent, metaAttribute);
	else if (valueType == ValueType::LongPointer())
		attribute = new CoreAttributeTemplateBase<Uuid>(parent, metaAttribute);
	else if (valueType == ValueType::Collection())
		attribute = new CoreAttributeCollection(parent, metaAttribute);
	else if (valueType == ValueType::Pointer())
		attribute = new CoreAttributeTemplateBase<Uuid>(parent, metaAttribute);
	else if (valueType == ValueType::Dictionary())
		attribute = new CoreAttributeTemplateBase<DictionaryEntry>(parent, metaAttribute);
	ASSERT( attribute != NULL );
	// All must be good
	return S_OK;
}


ICoreStorage* CoreAttributeBase::SetStorageObject(void) const
{
	// Call to the parent CoreObjectBase
	return this->_parent->SetStorageObject();
}



void CoreAttributeBase::RegisterTransactionItem(void)
{
	// Get to the parent CoreProject
	CoreProject* coreProject = NULL;
	Result_t result = this->_parent->Project(coreProject);
	ASSERT( result == S_OK );
	ASSERT( coreProject != NULL );
	// And, register this attribute as changing
	coreProject->RegisterTransactionItem(this);
}


void CoreAttributeBase::MarkDirty(void) throw()
{
	// Mark this attribute as dirty
	this->_isDirty = true;
	// And, mark the parent object as dirty
	this->_parent->MarkDirty();
}


const Result_t CoreAttributeBase::InTransaction(bool &flag) const throw()
{
	// Just call to the parent
	return this->_parent->InTransaction(flag);
}


const Result_t CoreAttributeBase::InWriteTransaction(bool &flag) const throw()
{
	// Just call to the parent
	return this->_parent->InWriteTransaction(flag);
}


const Result_t CoreAttributeBase::ResolveBackpointer(const AttrID_t &attrID, const Uuid &newValue, const Uuid &oldValue) throw()
{
	CoreProject* coreProject = NULL;
	Result_t result = this->_parent->Project(coreProject);
	ASSERT( result == S_OK );
	ASSERT( coreProject != NULL );
	// Make sure value is valid (either a valid object or NULL - and pointed to has good backpointer collection)
	CoreAttributeTemplateBase< std::list<Uuid> >* attribute = NULL;
	if (newValue != Uuid::Null())
	{
		// Can we get object - if not it is an error
		CoreObject backpointerObject;
		Result_t result = coreProject->Object(newValue, backpointerObject);
		if (result != S_OK) return result;
		// Does the object have a correct backpointer collection
		CoreAttributeBase *basePointer;
		result = backpointerObject->Attribute(attrID + ATTRID_COLLECTION, basePointer);
		// Make sure the backpointer collection exists
		if (result != S_OK) return result;
		// Get the attribute
		attribute = (CoreAttributeTemplateBase< std::list<Uuid> >*)basePointer;
	}
	
	// Update the current pointed-to object's backpointer collection (if idPair is valid)
	if (oldValue != Uuid::Null())
	{
		// Can we get object - if not it is an error
		CoreObject oldObject;
		Result_t result = coreProject->Object(oldValue, oldObject);
		if (result != S_OK) return result;
		// Does the object have a correct backpointer collection
		CoreAttributeBase *basePointer;
		result = oldObject->Attribute(attrID + ATTRID_COLLECTION, basePointer);
		// Make sure the backpointer collection exists
		if (result != S_OK) return result;
		CoreAttributeTemplateBase< std::list<Uuid> >* oldAttr = (CoreAttributeTemplateBase< std::list<Uuid> >*)basePointer;
		// Make sure attribute's values have been loaded
		std::list<Uuid> tmpList;
		result = oldAttr->GetValue(tmpList);
		ASSERT( result == S_OK );
		// Remove oldValue from backpointerCollection and set attribute in oldObject (all by reference)
		std::list<Uuid> &oldList = oldAttr->_values.back();
		oldList.remove(oldValue);
	}
	// Update the pointed-to object's backpointer collection (if it points to something valid)
	if (attribute != NULL)
	{
		// Make sure attribute's values have been loaded
		std::list<Uuid> tmpList;
		result = attribute->GetValue(tmpList);
		ASSERT( result == S_OK );
		// Get the attributes list of backpointers (notice it is by reference)
		std::list<Uuid> &bpList = attribute->_values.back();
		// Add newValue to backpointer list (still by reference here)
		bpList.push_back(newValue);
	}
	// We are all done
	return S_OK;
}


CoreAttributeBase::CoreAttributeBase(CoreObjectBase* parent, CoreMetaAttribute *metaAttribute) :
_parent(parent), _metaAttribute(metaAttribute), _isDirty(false), _refCount(0)
{
	ASSERT( parent != NULL );
	ASSERT( metaAttribute != NULL );
	// Register the attribute
	AttrID_t attrID = ATTRID_NONE;
	Result_t result = metaAttribute->GetAttributeID(attrID);
	ASSERT( result == S_OK );
	ASSERT( attrID != ATTRID_NONE );
	parent->RegisterAttribute(attrID, this);
}


// --------------------------- Public CoreAttributeBase Methods --------------------------- //


CoreAttributeBase::~CoreAttributeBase()
{
	ASSERT( this->_metaAttribute != NULL );
	ASSERT( this->_parent != NULL );
	// Unregister the attribute from the parent
	AttrID_t attrID = ATTRID_NONE;
	Result_t result = this->_metaAttribute->GetAttributeID(attrID);
	ASSERT( result == S_OK );
	ASSERT( attrID != ATTRID_NONE );	
	this->_parent->UnregisterAttribute(attrID);
}


/*** Included Header Files ***/
#include "CoreMetaObject.h"
#include "CoreMetaProject.h"
#include "CoreMetaAttribute.h"


// --------------------------- Public CoreMetaObject Methods ---------------------------


CoreMetaObject::~CoreMetaObject()
{
	// While we still have attributes...
	while( !this->_attributes.empty() )
	{
		// Delete the first attribute
		delete this->_attributes.front();
		// Remove the deleted attribute from the list
		this->_attributes.pop_front();
	}
}


const Result_t CoreMetaObject::Attribute(const AttrID_t &attrID, CoreMetaAttribute* &attribute) const throw()
{
	if ( attrID == ATTRID_NONE ) return E_INVALID_USAGE;
	// Start at the beginning of the attribute list
	std::list<CoreMetaAttribute*>::const_iterator iter = this->_attributes.begin();
	// Look until the end of the list
	while( iter != this->_attributes.end() )
	{
		// Make sure attribute is not null
		ASSERT( (*iter) != NULL );
		// Is this the attribute we are looking for
		AttrID_t tmpAttrID = ATTRID_NONE;
		ASSERT( (*iter)->AttributeID(tmpAttrID) == S_OK );
		if( tmpAttrID == attrID )
		{
			// Yes it is...
			attribute = (*iter);
			return S_OK;
		}
		// Move on to the next attribute
		++iter;
	}
	return E_NOTFOUND;
}


const Result_t CoreMetaObject::AddAttribute(const AttrID_t &attrID, const std::string &token, const std::string &name,
											const ValueType &valueType, CoreMetaAttribute* &attribute) throw()
{
	if ( attrID == ATTRID_NONE ) return E_INVALID_USAGE;
	if ( valueType == ValueType::None() ) return E_INVALID_USAGE;
	// Create a new attribute
	CoreMetaAttribute *newAttribute = new CoreMetaAttribute(this, attrID, token, name, valueType);
	ASSERT( newAttribute != NULL );
	// Add the attribute to the object's list
	this->_attributes.push_front(newAttribute);
	// Return pointer to the new attribute
	attribute = newAttribute;
	return S_OK;
}


std::ostream& operator<<(std::ostream& out, const CoreMetaObject *object)
{
	out << "(" << object->_metaID << ") " << object->_name << " -- " << object->_token << ".\n";
	// Just print out all of the attribute info
	std::list<CoreMetaAttribute*>::const_iterator attribIter = object->_attributes.begin();
	while (attribIter != object->_attributes.end())
	{
		out << (*attribIter);
		++attribIter;
	}
	return out;
}


/*** Included Header Files ***/
#include "CoreMetaObject.h"
#include "CoreMetaAttribute.h"


// --------------------------- Public CoreMetaAttribute Methods ---------------------------


CoreMetaAttribute::CoreMetaAttribute(CoreMetaObject* object, const AttrID_t &attrID, const std::string &token,
									 const std::string &name, const ValueType &valueType) :
	_object(object), _attributeID(attrID), _token(token), _name(name), _valueType(valueType)
{
	ASSERT( object != NULL );
	ASSERT( attrID != ATTRID_NONE );
	ASSERT( valueType != ValueType::None() );
	// Nothing else to be done here
}


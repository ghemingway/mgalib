%module MGALib
%{


/*** Included Header Files ***/
#include "../CoreMetaAttribute.h"
#include "../CoreMetaObject.h"
#include "../CoreMetaProject.h"
//#include "../CoreStorage.h"
//#include "../BinFile.h"


/*** Externally Defined Functions ***/
extern CoreMetaProject* CreateCoreMetaProject(const bool &v2=true);


%}


/*** Included SWIG Libraries ***/
%include <std_string.i>
%include <std_list.i>
%include <std_map.i>

//namespace std {
//	%template(CoreMetaObjectList)		list<CoreMetaObject*>;
//	%template(CoreMetaAttributeList)	list<CoreMetaAttribute*>;
//}


// --------------------------- CoreMetaAttribute --------------------------- //


extern CoreMetaProject* CreateCoreMetaProject(const bool &v2=true);


class CoreMetaAttribute
{
private:
	CoreMetaAttribute();
	CoreMetaAttribute(const CoreMetaAttribute &);
	CoreMetaAttribute& operator=(const CoreMetaAttribute &);
	CoreMetaAttribute(CoreMetaObject* object, const AttrID_t &attrID, const std::string &token,
					  const std::string &name, const ValueType &valueType);
public:
%extend {
	CoreMetaObject* Object(void)		{ CoreMetaObject* obj; $self->Object(obj); return obj; }
	AttrID_t AttributeID(void)			{ AttrID_t attrID; $self->AttributeID(attrID); return attrID; }
	std::string Token(void)				{ std::string token; $self->Token(token); return token; }
	std::string Name(void)				{ std::string name; $self->Name(name); return name; }
	ValueType GetValueType(void)		{ ValueType vt = ValueType::None(); $self->GetValueType(vt); return vt; }
}
};


// --------------------------- CoreMetaObject --------------------------- //


class CoreMetaObject
{
private:
	CoreMetaObject();
	CoreMetaObject(const CoreMetaObject &);
	CoreMetaObject& operator=(const CoreMetaObject &);
	CoreMetaObject(CoreMetaProject *project, const MetaID_t &metaID, const std::string &token, const std::string &name);
public:
%extend {
	CoreMetaProject* Project(void)		{ CoreMetaProject* prj; $self->Project(prj); return prj; }
	MetaID_t MetaID(void)				{ MetaID_t metaID; $self->MetaID(metaID); return metaID; }
	std::string Token(void)				{ std::string token; $self->Token(token); return token; }
	std::string Name(void)				{ std::string name; $self->Name(name); return name; }
	std::list<CoreMetaAttribute*> Attributes(void)	{ std::list<CoreMetaAttribute*> attrList; $self->Attributes(attrList); return attrList; }
	CoreMetaAttribute* Attribute(const AttrID_t &attrID)	{ CoreMetaAttribute* atr; $self->Attribute(attrID, atr); return atr; }
	CoreMetaAttribute* AddAttribute(const AttrID_t &attrID, const std::string &token, const std::string &name, const ValueType &valueType) {
									CoreMetaAttribute* atr; $self->AddAttribute(attrID, token, name, valueType, atr); return atr; }
}
};


// --------------------------- CoreMetaProject --------------------------- //


class CoreMetaProject 
{
private:
	CoreMetaProject();
	CoreMetaProject(const CoreMetaProject &);
	CoreMetaProject& operator=(const CoreMetaProject &);
public:
%extend {
	GUID_t GUID(void)								{ GUID_t guid; $self->GUID(guid); return guid; }
	std::string Token(void)							{ std::string token; $self->Token(token); return token; }
	std::string Name(void)							{ std::string name; $self->Name(name); return name; }
	std::list<CoreMetaObject*> Objects(void)		{ std::list<CoreMetaObject*> objects; $self->Objects(objects); return objects; }
	CoreMetaObject* Object(const MetaID_t &metaID)	{ CoreMetaObject *obj; $self->Object(metaID, obj); return obj; }
	CoreMetaObject* AddObject(const MetaID_t &metaID, const std::string &token, const std::string &name) { 
							  CoreMetaObject *obj; $self->AddObject(metaID, token, name, obj); return obj;}
}
};


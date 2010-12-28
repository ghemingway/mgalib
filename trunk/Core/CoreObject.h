#ifndef __CORE_OBJECT_H__
#define __CORE_OBJECT_H__


/*** Included Header Files ***/
#include "CoreTypes.h"
#include "CoreAttribute.h"
#include "CoreMetaObject.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class CoreProject;
class ICoreStorage;


/*** Type Definitions ***/
// None


// --------------------------- CoreObject ---------------------------


class CoreObjectBase
{
private:
	CoreObjectBase();
	CoreObjectBase(CoreProject *project, CoreMetaObject *metaObject, const Uuid &uuid);
	
protected:
	CoreProject								*_project;
	CoreMetaObject							*_metaObject;
	Uuid									_uuid;
	std::map<AttrID_t,CoreAttributeBase*>	_attributeMap;
	bool									_isDirty;
	uint32_t								_refCount;

	void CreateAttributes(void);

public:
	static const Result_t Create(CoreProject *project, const Uuid &uuid, CoreObject &coreObject) throw();
	virtual ~CoreObjectBase();

	// ------- Hidden (Core only) Interface ------------ //

//	CoreObject* Reference(void);
//	void Release(void);
	void RegisterAttribute(const AttrID_t &attrID, CoreAttributeBase *attribute) throw();
	void UnregisterAttribute(const AttrID_t &attrID) throw();
	ICoreStorage* SetStorageObject(void) const;

	// ------- ICoreObject Interface ------------ //

	inline const Result_t Project(CoreProject* &project) const	throw()	{ project = this->_project; return S_OK; }
	inline const Result_t MetaObject(CoreMetaObject* &coreMetaObject) const throw()	{ coreMetaObject = this->_metaObject; return S_OK; }
	inline const Result_t GetUuid(Uuid &uuid) const throw()				{ uuid = this->_uuid; return S_OK; }
	inline const Result_t GetMetaID(MetaID_t &metaID) const throw()		{ return this->_metaObject->GetMetaID(metaID); }
	inline const Result_t IsDirty(bool &flag) const throw()				{ flag = this->_isDirty; return S_OK; }
	inline const Result_t MarkDirty(void) throw()						{ this->_isDirty = true; return S_OK; }
	const Result_t InTransaction(bool &flag) const throw();

	const Result_t Attributes(std::list<CoreAttribute*> &controlled) throw();
	const Result_t Attribute(const AttrID_t &attrID, CoreAttribute* &attribute) const throw();

	template<class T>
	const Result_t SetAttributeValue(const AttrID_t &attrID, const T &value) throw()
	{
		bool inTransaction;
		ASSERT( this->InTransaction(inTransaction) == S_OK );
		if (!inTransaction) return E_TRANSACTION;
		CoreAttributeBase *attribute = this->_attributeMap.find(attrID)->second;
		if( attribute == NULL ) return E_ATTRID;
		((CoreAttributeTemplateBase<T>*)attribute)->SetValue(value);
		return S_OK;
	}

	template<class T>
	const Result_t GetAttributeValue(const AttrID_t &attrID, T &value) const throw()
	{
		// Make sure we are in a transaction
		bool inTransaction;
		ASSERT( this->InTransaction(inTransaction) == S_OK );
		if (!inTransaction) return E_TRANSACTION;
		// Make sure we can find the desired attribute
		std::map<AttrID_t,CoreAttributeBase*>::const_iterator mapIter = this->_attributeMap.find(attrID);
		if (mapIter == this->_attributeMap.end()) return E_ATTRID;
		CoreAttributeBase *attribute = mapIter->second;
		ASSERT( attribute != NULL );
		// Get the value
		return ((CoreAttributeTemplateBase<T>*)attribute)->GetValue(value);
	}

	template<class T>
	const Result_t LoadedAttributeValue(const AttrID_t &attrID, const T &value) throw()
	{
		bool inTransaction;
		ASSERT( this->InTransaction(inTransaction) == S_OK );
		if (!inTransaction) return E_TRANSACTION;
		CoreAttributeBase *attribute = this->_attributeMap.find(attrID)->second;
		if( attribute == NULL ) return E_ATTRID;
		return ((CoreAttributeTemplateBase<T>*)attribute)->GetLoadedValue(value);
	}

	template<class T>
	const Result_t PreviousAttributeValue(const AttrID_t &attrID, const T &value) throw()
	{
		bool inTransaction;
		ASSERT( this->InTransaction(inTransaction) == S_OK );
		if (!inTransaction) return E_TRANSACTION;
		CoreAttributeBase *attribute = this->_attributeMap.find(attrID)->second;
		if( attribute == NULL ) return E_ATTRID;
		return ((CoreAttributeTemplateBase<T>*)attribute)->GetPreviousValue(value);
	}

	template<class T>
	const Result_t SearchCollection(const AttrID_t &collectionAttrID, const AttrID_t &attrID, const T &value, CoreObjectBase* &object) throw();

	friend std::ostream& operator<<(std::ostream& out, const CoreObjectBase *object)
	{
		out << object->_metaObject;
		return out;
	}
};

/*
class CoreObject
{
private:
	CoreObjectBase			*_base;

	friend class CoreObjectBase;
	CoreObject();
	CoreObject(CoreObjectBase* base) : _base(base)				{ ASSERT( base != NULL ); }

	friend class CoreProject;
	CoreObjectBase* Base(void) const							{ return this->_base; }
public:
	~CoreObject()												{ this->_base->Release(); }

	// ------- ICoreObject Interface ------------ //

	inline CoreAttribute* operator[](const AttrID_t &attrID) const throw() {
		CoreAttribute* attribute; Result_t result = this->_base->Attribute(attrID, attribute); if (result == S_OK) return attribute; else return NULL; }
	inline const Result_t Project(CoreProject* &project) const	throw()		{ return this->_base->Project(project); }
	inline const Result_t MetaObject(CoreMetaObject* obj) const throw()		{ return this->_base->MetaObject(obj); }
	inline const Result_t GetUuid(Uuid &uuid) const throw()					{ return this->_base->GetUuid(uuid); }
	inline const Result_t GetMetaID(MetaID_t &metaID) const throw()			{ return this->_base->GetMetaID(metaID); }
	inline const Result_t IsDirty(bool &flag) const throw()					{ return this->_base->IsDirty(flag); }
	inline const Result_t MarkDirty(void) throw()							{ return this->_base->MarkDirty(); }
	inline const Result_t InTransaction(bool &flag) const throw()			{ return this->_base->InTransaction(flag); }
	
	inline const Result_t Attributes(std::list<CoreAttribute*> &controlled) throw() {
		return this->_base->Attributes(controlled); }
	inline const Result_t Attribute(const AttrID_t &attrID, CoreAttribute* &attribute) throw() {
		return this->_base->Attribute(attrID, attribute); }

	template<class T>
	inline const Result_t GetAttributeValue(const AttrID_t &attrID, T &value) const throw() {
		return this->_base->GetAttributeValue(attrID, value); }
	template<class T>
	const Result_t SetAttributeValue(const AttrID_t &attrID, const T &value) throw() {
		return this->_base->SetAttributeValue<T>(attrID, value); }
	template<class T>
	const Result_t LoadedAttributeValue(const AttrID_t &attrID, const T &value) throw();
	template<class T>
	const Result_t PreviousAttributeValue(const AttrID_t &attrID, const T &value) throw();
	template<class T>
	const Result_t SearchCollection(const AttrID_t &collectionAttrID, const AttrID_t &attrID, const T &value, CoreObject* &object) throw();

	friend std::ostream& operator<<(std::ostream& out, const CoreObject *object) { return out << object->_base; }
};
*/

/*** End of MGA Namespace ***/
}


#endif	//__CORE_OBJECT_H__


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
	friend class CoreObject;
	friend class CoreAttributeBase;
	inline uint32_t Reference(void)			{ return ++this->_refCount; }
	inline uint32_t Release(void)			{ return --this->_refCount; }
	void RegisterAttribute(const AttrID_t &attrID, CoreAttributeBase *attribute) throw();
	void UnregisterAttribute(const AttrID_t &attrID) throw();
	ICoreStorage* SetStorageObject(void) const;

public:
	static const Result_t Create(CoreProject *project, const Uuid &uuid, CoreObject &coreObject) throw();
	virtual ~CoreObjectBase();

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
		out << "(" << object->_refCount << ")" << object->_metaObject;
		return out;
	}
};


class CoreObject
{
private:
	CoreObjectBase					*_base;
	inline void Release(void)
	{
		uint32_t count = 0;
		if(this->_base != NULL) count = this->_base->Release();
		if (count == 0) delete this->_base;
	}
	inline void Reference(void)
	{
		if( this->_base != NULL ) this->_base->Reference();
	}
public:
	CoreObject() : _base(NULL)									{ }
	CoreObject(CoreObjectBase* base) : _base(base)				{ this->Reference(); }
	CoreObject(const CoreObject &obj) : _base(obj._base)		{ this->Reference(); }
	~CoreObject()												{ this->Release(); }

	inline CoreObjectBase* operator->() const					{ return this->_base; }
	inline CoreObject& operator=(CoreObjectBase* base)			{ this->Release(); this->_base = base; this->Reference(); return *this; }
	inline CoreObject& operator=(const CoreObject &obj)			{ this->Release(); this->_base = obj._base; this->Reference(); return *this; }
	inline bool operator==(const CoreObjectBase* base) const	{ return (this->_base == base); }
	inline bool operator!=(const CoreObjectBase* base) const	{ return (this->_base != base); }
	inline bool operator==(const CoreObject &obj) const			{ return (this->_base == obj._base); }
	inline bool operator!=(const CoreObject &obj) const			{ return (this->_base != obj._base); }
};


/*** End of MGA Namespace ***/
}


#endif	//__CORE_OBJECT_H__


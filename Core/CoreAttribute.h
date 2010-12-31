#ifndef __CORE_ATTRIBUTE_H__
#define __CORE_ATTRIBUTE_H__


/*** Included Header Files ***/
#include "CoreTypes.h"
#include "CoreMetaAttribute.h"
#include "CoreStorage.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class CoreObjectBase;


// --------------------------- CoreAttributeBase --------------------------- //


class CoreAttributeBase
{
private:
	CoreAttributeBase();
	CoreAttributeBase(const CoreAttributeBase &);
	
protected:
	CoreObjectBase							*_parent;			//!<
	CoreMetaAttribute						*_metaAttribute;	//!<
	bool									_isDirty;			//!<
	uint32_t								_refCount;			//!<

	friend class CoreObjectBase;
	static const Result_t Create(CoreObjectBase *parent, CoreMetaAttribute *metaAttribute) throw();

	ICoreStorage* SetStorageObject(void) const;										//!<
	void RegisterTransactionItem(void);												//!<
	void MarkDirty(void) throw();													//!<
	const Result_t InTransaction(bool &flag) const throw();							//!<
	const Result_t InWriteTransaction(bool &flag) const throw();					//!<
	const Result_t ResolveBackpointer(const AttrID_t &attrID, const Uuid &newValue, const Uuid &oldValue) throw(); //!<
	CoreAttributeBase(CoreObjectBase *parent, CoreMetaAttribute *metaAttribute);	//!<

public:
	virtual ~CoreAttributeBase();

	inline CoreMetaAttribute* MetaAttribute(void) const throw()			{ return this->_metaAttribute; }
	inline const Result_t AttributeID(AttrID_t &attrID) const throw()	{ return this->_metaAttribute->GetAttributeID(attrID); }
	inline bool IsDirty(void) const throw()								{ return this->_isDirty; }
	inline const Result_t GetValueType(ValueType &valueType) const throw(){ return this->_metaAttribute->GetValueType(valueType); }

	virtual const Result_t CommitTransaction(void) throw()=0;
	virtual const Result_t AbortTransaction(void) throw()=0;
};


// --------------------------- CoreAttributeTemplateBase --------------------------- //


template<class T>
class CoreAttributeTemplateBase : public CoreAttributeBase
{
protected:
	friend class CoreAttributeBase;
	std::list<T>					_values;

public:
	CoreAttributeTemplateBase(CoreObjectBase* parent, CoreMetaAttribute* coreMetaAttribute) :
		::CoreAttributeBase(parent, coreMetaAttribute), _values() { }

	inline const Result_t SetValue(const T &value) throw()
	{
		// Make sure we are in a write transaction
		bool flag;
		ASSERT( this->InWriteTransaction(flag) == S_OK ); 
		if (!flag) return E_READONLY;
		// First, make sure the storage value has been loaded
		if (this->_values.empty())
		{
			T tmpValue;
			ASSERT( this->GetValue(tmpValue) == S_OK );
		}
		// Is the value actually changing?
		if (value == this->_values.back()) return S_OK;
		// Set the value
		this->_values.push_back(value);
		// Mark the attribute as dirty
		this->MarkDirty();
		// Add attribute to the project transaction change list
		this->RegisterTransactionItem();
		return S_OK;
	}

	inline const Result_t GetValue(T &value) throw()
	{
		// Make sure we are in a transaction
		bool flag;
		ASSERT( this->InTransaction(flag) == S_OK ); 
		if (!flag) return E_TRANSACTION;
		// Have we not read this attribute previously
		if (this->_values.empty())
		{
			// Set the storage to the parent object
			ICoreStorage* storage = this->SetStorageObject();
			// Read the value from storage
			AttrID_t attrID = ATTRID_NONE;
			ASSERT( this->_metaAttribute->GetAttributeID(attrID) == S_OK );
			Result_t result = storage->GetAttributeValue(attrID, value);
			if (result != S_OK) return result;
			// Save the value
			this->_values.push_back(value);
		}
		// Otherwise, just get the most recent value
		else value = this->_values.back();
		return S_OK;
	}

	inline const Result_t GetLoadedValue(T &value) throw()
	{
		ASSERT(false);
		return E_INVALID_USAGE;
	}

	inline const Result_t GetPreviousValue(T &value) throw()
	{
		ASSERT(false);
		return E_INVALID_USAGE;
	}

	virtual const Result_t CommitTransaction(void) throw()
	{
		ASSERT( !this->_values.empty() );
		ASSERT( this->_isDirty );
		// Are there changes, if not we are good
		if (this->_values.size() == 1) return S_OK;
		// Set the storage to the parent object
		ICoreStorage* storage = this->SetStorageObject();
		ASSERT( storage != NULL );
		// Write the value into storage
		AttrID_t attrID = ATTRID_NONE;
		ASSERT( this->_metaAttribute->GetAttributeID(attrID) == S_OK );
		T value = this->_values.back();
		Result_t result = storage->SetAttributeValue(attrID, value);
		if (result != S_OK) return result;
		// Collapse the value list to one
		this->_values.clear();
		this->_values.push_back(value);
		return S_OK;
	}

	virtual const Result_t AbortTransaction(void) throw()
	{
		// There must be more than one value here (loaded + change)
		ASSERT( this->_values.size() > 1 );
		ASSERT( this->_isDirty );
		// Just remove the back value
		this->_values.pop_back();
		return S_OK;
	}
};


// Specialization of the SetValue method for Uuid (Pointer and LongPointer)
template<>
inline const Result_t CoreAttributeTemplateBase<Uuid>::SetValue(const Uuid &value) throw()
{
	// Make sure we are in a write transaction
	bool flag;
	ASSERT( this->InWriteTransaction(flag) == S_OK ); 
	if (!flag) return E_READONLY;
	// First, make sure the storage value has been loaded
	if (this->_values.empty())
	{
		Uuid tmpValue = Uuid::Null();
		ASSERT( this->GetValue(tmpValue) == S_OK );
	}
	// Is the value actually changing?
	if (value == this->_values.back()) return S_OK;
	// Now, are we dealing with a pointer or longPointer
	ValueType valueType = ValueType::None();
	ASSERT( this->_metaAttribute->GetValueType(valueType) == S_OK );
	if (valueType != ValueType::LongPointer() && valueType != ValueType::Pointer()) return E_ATTVALTYPE;
	// Pointers, those are special because we also have to change the backpointer collection
	if (valueType == ValueType::Pointer())
	{
		AttrID_t attrID = ATTRID_NONE;
		ASSERT( this->_metaAttribute->GetAttributeID(attrID) == S_OK );
		// Get the current value
		Uuid oldValue = this->_values.back();
		// Resolve all backpointer issues
		Result_t result = this->ResolveBackpointer(attrID, value, oldValue);
		if (result != S_OK) return result;
	}
	// Set the value
	this->_values.push_back(value);
	// Mark the attribute as dirty
	this->MarkDirty();
	// Add attribute to the project transaction change list
	this->RegisterTransactionItem();
	// Or we have an error condition
	return S_OK;
}


// Specialization of the SetValue method for std::list<Uuid> (Collection)
template<>
inline const Result_t CoreAttributeTemplateBase<std::list<Uuid> >::SetValue(const std::list<Uuid> &value) throw()
{
	// Just take the new value - no questions asked
	this->_values.clear();
	this->_values.push_back(value);
	return S_OK;
}

/*** Simple Attribute Type Definitions ***/
typedef CoreAttributeTemplateBase<int32_t>						CoreAttributeLong;
typedef CoreAttributeTemplateBase<double>						CoreAttributeReal;
typedef CoreAttributeTemplateBase<std::string>					CoreAttributeString;
typedef CoreAttributeTemplateBase<Uuid>							CoreAttributeLongPointer;
typedef CoreAttributeTemplateBase< std::list<Uuid> >			CoreAttributeCollection;
typedef CoreAttributeTemplateBase<Uuid>							CoreAttributePointer;


/*** End of MGA Namespace ***/
}


#endif	// __CORE_ATTRIBUTE_H__


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
		Result_t result = this->InWriteTransaction(flag);
		ASSERT( result == S_OK ); 
		if (!flag) return E_READONLY;
		// First, make sure the storage value has been loaded
		if (this->_values.empty())
		{
			T tmpValue;
			result = this->GetValue(tmpValue); 
			ASSERT( result == S_OK );
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
		Result_t result = this->InWriteTransaction(flag);
		ASSERT( result == S_OK ); 
		if (!flag) return E_TRANSACTION;
		// Have we not read this attribute previously
		if (this->_values.empty())
		{
			// Set the storage to the parent object
			ICoreStorage* storage = this->SetStorageObject();
			// Read the value from storage
			AttrID_t attrID = ATTRID_NONE;
			result = this->_metaAttribute->GetAttributeID(attrID);
			ASSERT( result == S_OK );
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
		Result_t result = this->_metaAttribute->GetAttributeID(attrID);
		ASSERT( result == S_OK );
		T value = this->_values.back();
		result = storage->SetAttributeValue(attrID, value);
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
	Result_t result = this->InWriteTransaction(flag);
	ASSERT( result == S_OK ); 
	if (!flag) return E_READONLY;
	// First, make sure the storage value has been loaded
	if (this->_values.empty())
	{
		Uuid tmpValue = Uuid::Null();
		result = this->GetValue(tmpValue);
		ASSERT( result == S_OK );
	}
	// Is the value actually changing?
	if (value == this->_values.back()) return S_OK;
	// Now, are we dealing with a pointer or longPointer
	ValueType valueType = ValueType::None();
	result = this->_metaAttribute->GetValueType(valueType);
	ASSERT( result == S_OK );
	if (valueType != ValueType::LongPointer() && valueType != ValueType::Pointer()) return E_ATTVALTYPE;
	// Pointers, those are special because we also have to change the backpointer collection
	if (valueType == ValueType::Pointer())
	{
		AttrID_t attrID = ATTRID_NONE;
		result = this->_metaAttribute->GetAttributeID(attrID);
		ASSERT( result == S_OK );
		// Get the current value
		Uuid oldValue = this->_values.back();
		// Resolve all backpointer issues
		result = this->ResolveBackpointer(attrID, value, oldValue);
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


// Specialization of the entire CoreAttributeTemplateBase class for std::pair<std::string,std::string> (Dictionary)
typedef std::pair<std::string,std::string> DictionaryEntry;
template<>
class CoreAttributeTemplateBase<DictionaryEntry> : public CoreAttributeBase
{
protected:
	friend class CoreAttributeBase;
	bool										_isLoaded;
	DictionaryMap								_dictionary;
	std::list<std::pair<DictionaryEntry,std::string> > _values;

	inline const Result_t LoadValue(void)
	{
		// Have we not read this attribute previously
		if (!this->_isLoaded)
		{
			// Set the storage to the parent object
			ICoreStorage* storage = this->SetStorageObject();
			// Read the value from storage
			AttrID_t attrID = ATTRID_NONE;
			Result_t result = this->_metaAttribute->GetAttributeID(attrID);
			ASSERT( result == S_OK );
			ASSERT( attrID == ATTRID_NONE );
			result = storage->GetAttributeValue(attrID, this->_dictionary);
			if (result != S_OK) return result;
			// Mark as loaded
			this->_isLoaded = true;
		}
		return S_OK;
	}

public:
	CoreAttributeTemplateBase<DictionaryEntry>(CoreObjectBase* parent, CoreMetaAttribute* coreMetaAttribute) :
		::CoreAttributeBase(parent, coreMetaAttribute), _isLoaded(false), _dictionary(), _values() { }

	inline const Result_t SetValue(const DictionaryEntry &value) throw()
	{
		// Make sure we are in a write transaction
		bool flag;
		Result_t result = this->InWriteTransaction(flag);
		ASSERT( result == S_OK ); 
		if (!flag) return E_READONLY;
		// Make sure we are loaded
		result = this->LoadValue();
		ASSERT( result == S_OK );

		// Does this key alread exist
		std::string currentValue = "";
		DictionaryMapIter mapIter = this->_dictionary.find(value.first);
		// Key exists
		if (mapIter != this->_dictionary.end())
		{
			// Get the current value
			currentValue = mapIter->second;
			// Update the map entry value
			mapIter->second = value.second;
		}
		// Key does not exist
		else
		{
			// Insert the value into the dictionary
			this->_dictionary.insert(value);
		}
		// Record the change into the list of values
		this->_values.push_back(std::make_pair(value,currentValue));

		// Mark the attribute as dirty
		this->MarkDirty();
		// Add attribute to the project transaction change list
		this->RegisterTransactionItem();
		return S_OK;
	}

	inline const Result_t GetValue(DictionaryEntry &value) throw()
	{
		// Make sure we are in a transaction
		bool flag;
		Result_t result = this->InWriteTransaction(flag);
		ASSERT( result == S_OK ); 
		if (!flag) return E_TRANSACTION;
		// Make sure we are loaded
		ASSERT( this->LoadValue() == S_OK );
		// Now, get the value from the dictionary
		DictionaryMapIter mapIter = this->_dictionary.find(value.first);
		if (mapIter == this->_dictionary.end()) value.second = "";
		else value.second = mapIter->second;
		return S_OK;
	}

	inline const Result_t GetLoadedValue(DictionaryEntry &value) throw()
	{
		ASSERT(false);
		return E_INVALID_USAGE;
	}

	inline const Result_t GetPreviousValue(DictionaryEntry &value) throw()
	{
		ASSERT(false);
		return E_INVALID_USAGE;
	}

	virtual const Result_t CommitTransaction(void) throw()
	{
		ASSERT( !this->_values.empty() );
		ASSERT( this->_isDirty );
		ASSERT( this->_isLoaded );
		// Set the storage to the parent object
		ICoreStorage* storage = this->SetStorageObject();
		ASSERT( storage != NULL );
		// Write the value into storage
		AttrID_t attrID = ATTRID_NONE;
		Result_t result = this->_metaAttribute->GetAttributeID(attrID);
		ASSERT( result == S_OK );
		result = storage->SetAttributeValue(attrID, this->_dictionary);
		if (result != S_OK) return result;
		// Clear the intermediate values list
		this->_values.clear();
		return S_OK;
	}

	virtual const Result_t AbortTransaction(void) throw()
	{
		// There must be more than one value here (loaded + change)
		ASSERT( !this->_values.empty() );
		ASSERT( this->_isDirty );
		ASSERT( this->_isLoaded );
		// Find the correct dictionary entry
		std::pair<DictionaryEntry,std::string> value = this->_values.back();
		DictionaryMapIter mapIter = this->_dictionary.find(value.first.first);
		ASSERT( mapIter != this->_dictionary.end() );
		// Change the value back to before the change
		mapIter->second = value.second;
		// Remove this entry from the list of changes
		this->_values.pop_back();		
		return S_OK;
	}
};


/*** Simple Attribute Type Definitions ***/
typedef CoreAttributeTemplateBase<int32_t>						CoreAttributeLong;
typedef CoreAttributeTemplateBase<double>						CoreAttributeReal;
typedef CoreAttributeTemplateBase<std::string>					CoreAttributeString;
typedef CoreAttributeTemplateBase<Uuid>							CoreAttributeLongPointer;
typedef CoreAttributeTemplateBase< std::list<Uuid> >			CoreAttributeCollection;
typedef CoreAttributeTemplateBase<Uuid>							CoreAttributePointer;
typedef CoreAttributeTemplateBase<DictionaryEntry>				CoreAttributeDictionary;


/*** End of MGA Namespace ***/
}


#endif	// __CORE_ATTRIBUTE_H__


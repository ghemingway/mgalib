#ifndef __CORE_ATTRIBUTE_H__
#define __CORE_ATTRIBUTE_H__


/*** Included Header Files ***/
#include "CoreTypes.h"
#include "CoreTransactionItem.h"
#include "CoreMetaAttribute.h"
#include "CoreStorage.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class CoreObjectBase;


// --------------------------- CoreAttributeBase --------------------------- //


class CoreAttributeBase
//	: public class ICoreFinalTransactionItem
{
protected:
	CoreObjectBase							*_parent;
	CoreMetaAttribute						*_metaAttribute;	
	bool									_isDirty;
	uint16_t								_refCount;

	friend class CoreObjectBase;
	static const Result_t Create(CoreObjectBase *parent, CoreMetaAttribute *metaAttribute) throw();
	ICoreStorage* SetStorageObject(void) const;

	friend class CoreProject;
	void CommitTransaction(void)	{ }
	void UndoTransaction(void)		{ }
	void RedoTransaction(void)		{ }
	void Save(void)					{ }

	CoreAttributeBase();
	CoreAttributeBase(const CoreAttributeBase &);
	CoreAttributeBase(CoreObjectBase *parent, CoreMetaAttribute *metaAttribute);
	
	void RegisterTransactionItem(void);

public:
	virtual ~CoreAttributeBase();

	inline CoreMetaAttribute* MetaAttribute(void) const throw()			{ return this->_metaAttribute; }
	inline const Result_t AttributeID(AttrID_t &attrID) const throw()	{ return this->_metaAttribute->AttributeID(attrID); }
	inline bool IsDirty(void) const throw()								{ return this->_isDirty; }
	inline void MarkDirty(void) throw();
	bool InTransaction(void) const throw();
	inline const Result_t GetValueType(ValueType &valueType) const throw(){ return this->_metaAttribute->GetValueType(valueType); }
};


// --------------------------- CoreAttribute --------------------------- //


class CoreAttribute
{
private:
	CoreAttributeBase			*_base;
public:
	CoreAttribute(CoreObjectBase* parent, const AttrID_t &attributeID) : _base(NULL)
	{
	}
};


// --------------------------- CoreAttributeTemplateBase --------------------------- //


template<class T>
class CoreAttributeTemplateBase : public CoreAttributeBase
{
protected:
	std::list<T>					_values;

public:
	CoreAttributeTemplateBase(CoreObjectBase* parent, CoreMetaAttribute* coreMetaAttribute) :
		::CoreAttributeBase(parent, coreMetaAttribute), _values() { }

	inline const Result_t SetValue(const T &value) throw()
	{
		this->_parent->MarkDirty(); 
		this->_values.push_back(value);
		// Add attribute to the project transaction change list
		this->RegisterTransactionItem();
		return S_OK;
	}

	inline const Result_t GetValue(T &value) throw()
	{
		// Have we not read this attribute previously
		if (this->_values.empty())
		{
			// Set the storage to the parent object
			ICoreStorage* storage = this->SetStorageObject();
			// Read the value from storage
			AttrID_t attrID;
			ASSERT( this->_metaAttribute->AttributeID(attrID) == S_OK );
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
		// TODO: Setup CoreStorage for a read
		value = this->_values.front();
		return S_OK;
	}

	inline const Result_t GetPreviousValue(T &value) throw()
	{
		return E_NOTFOUND;
	}
};


/*** Simple Attribute Type Definitions ***/
typedef CoreAttributeTemplateBase<int32_t>						CoreAttributeLong;
typedef CoreAttributeTemplateBase<double>						CoreAttributeReal;
typedef CoreAttributeTemplateBase<std::string>					CoreAttributeString;
typedef CoreAttributeTemplateBase<std::vector<unsigned char> >	CoreAttributeBinary;
typedef CoreAttributeTemplateBase<std::list<MetaObjIDPair> >	CoreAttributeCollection;
typedef CoreAttributeTemplateBase<MetaObjIDPair>				CoreAttributePointer;


// --------------------------- CoreDataAttribute

/*
template<class BASE, const int VALTYPE>
class ATL_NO_VTABLE CCoreDataAttribute : 
	public //typename// BASE,//z!
	public CCoreTransactionItem,
	public CCoreUndoItem
{
public:
	typedef typename BASE::value_type value_type;

// ------- Methods

public:
	virtual valtype_type GetValType() const throw() { return VALTYPE; }

	STDMETHODIMP get_Value(VARIANT *p);
	STDMETHODIMP put_Value(VARIANT p);
	STDMETHODIMP get_LoadedValue(VARIANT *p);
	STDMETHODIMP get_PreviousValue(VARIANT *p);

	const value_type &GetValue() const { ASSERT( !values.empty() ); return values.front(); }

	// if type mismatch then returns false
	virtual bool DoesMatch(bool do_load, const VARIANT &v);

// ------- Storage

public:
	void Save(value_type &value);

	virtual void FillAfterCreateObject() throw();

// ------- NestedTrItem

public:
	virtual bool IsDirty() const throw() { return CCoreAttribute::IsDirty(); }
	virtual void ChangeDirty(bool dirty) throw() { CCoreAttribute::ChangeDirty(dirty); }

	virtual void AbortNestedTransaction() throw();
	virtual void DiscardPreviousValue() throw();

// ------- FinalTrItem

public:
	virtual void AbortFinalTransaction() throw();
	virtual void CommitFinalTransaction();
	virtual void CommitFinalTransactionFinish(bool undo) throw();

// ------- UndoItem

public:
	virtual void UndoTransaction();
	virtual void UndoTransactionFinish() throw();

	virtual void RedoTransaction();
	virtual void RedoTransactionFinish() throw();

	virtual void DiscardLastItem();
	virtual void DiscardLastItemFinish() throw();
	virtual void DiscardLastItemCancel() throw();
};
*/


/*** End of MGA Namespace ***/
}


#endif	// __CORE_ATTRIBUTE_H__


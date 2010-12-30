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
	ASSERT( metaAttribute->GetValueType(valueType) == S_OK );
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
	CoreProject* coreProject;
	ASSERT( this->_parent->Project(coreProject) == S_OK );
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


CoreAttributeBase::CoreAttributeBase(CoreObjectBase* parent, CoreMetaAttribute *metaAttribute) :
_parent(parent), _metaAttribute(metaAttribute), _isDirty(false), _refCount(0)
{
	ASSERT( parent != NULL );
	ASSERT( metaAttribute != NULL );
	// Register the attribute
	AttrID_t attrID;
	ASSERT( metaAttribute->GetAttributeID(attrID) == S_OK );
	parent->RegisterAttribute(attrID, this);
}


// --------------------------- Public CoreAttributeBase Methods --------------------------- //


CoreAttributeBase::~CoreAttributeBase()
{
	ASSERT( this->_metaAttribute != NULL );
	ASSERT( this->_parent != NULL );
	// Unregister the attribute from the parent
	AttrID_t attrID;
	ASSERT( this->_metaAttribute->GetAttributeID(attrID) == S_OK );	
	this->_parent->UnregisterAttribute(attrID);
}


/*
STDMETHODIMP CCoreAttribute::get_Object(ICoreObject **p)
{
	CHECK_OUT(p);

	CopyTo(GetObject(), p);

	return S_OK;
}


inline bool CCoreAttribute::InTransaction() const
{
	return GetObject()->InTransaction();
}

 
inline bool CCoreAttribute::InWriteTransaction() const
{
	return GetObject()->InWriteTransaction();
}


// ------- Methods

template<class BASE, const int VALTYPE>
STDMETHODIMP CCoreDataAttribute<BASE, VALTYPE>::get_Value(VARIANT *p)
{
	CHECK_OUT(p);
	CHECK_ZOMBIE();

	COMTRY
	{
		if( InTransaction() )
		{
			if( LOCKING_GETVALUE > LOCKING_NONE )
				GetTerritory()->RaiseLocking(GetLockAttr(), LOCKING_GETVALUE);

			if( !IsLoaded() )
				GetLockAttr()->Load();

            // BGY: this hack makes sure that the sourcecontrol status is always
            // read from the storage.
			if( GetValType() == VALTYPE_LONG)
			{
				ICoreStorage *storage = SetStorageThisAttribute();
				ASSERT( storage != NULL );
				COMTHROW( storage->LockObject() );

				attrid_type a_id;
				COMTHROW( storage->get_AttrID( &a_id));
				if( a_id == ATTRID_FILESTATUS)
				{
					COMTHROW( storage->get_AttributeValue( p));
					return S_OK;
				}
			}
        }
		else
		{
//			if( GetTerritory()->GetLocking(GetLockAttr()) == LOCKING_NONE )
//				HR_THROW(E_NOTLOCKED);

			if( !IsLoaded() )
				HR_THROW(E_NOTLOCKED);
		}

		ASSERT( !values.empty() );
		UserCopyTo(values.front(), p);
	}
	COMCATCH(;)
}

template<class BASE, const int VALTYPE>
STDMETHODIMP CCoreDataAttribute<BASE, VALTYPE>::put_Value(VARIANT p)
{
	CHECK_ZOMBIE();

	if( !InWriteTransaction() )
		COMRETURN(E_TRANSACTION);

	COMTRY
	{
		if( LOCKING_PUTVALUE > LOCKING_NONE )
			GetTerritory()->RaiseLocking(GetLockAttr(), LOCKING_PUTVALUE);

		if( !IsLoaded() )
			GetLockAttr()->Load();

		ASSERT( !values.empty() );

		if( !IsDirty() )
		{
			InsertFrontValue(p);

			// nothing will throw here

			GetObject()->RegisterFinalTrItem();
			GetProject()->RegisterTransactionItem(this);
			SetDirty();
		}
		else
			ChangeFrontValue(p);
	}
	COMCATCH(;)
}

template<class BASE, const int VALTYPE>
STDMETHODIMP CCoreDataAttribute<BASE, VALTYPE>::get_LoadedValue(VARIANT *p)
{
	CHECK_OUT(p);
	CHECK_ZOMBIE();

	COMTRY
	{
		if( values.empty() )
			HR_THROW(E_NOTLOCKED);

		UserCopyTo(values.front(), p);
	}
	COMCATCH(;)
}

template<class BASE, const int VALTYPE>
STDMETHODIMP CCoreDataAttribute<BASE, VALTYPE>::get_PreviousValue(VARIANT *p)
{
	CHECK_OUT(p);
	CHECK_ZOMBIE();

	COMTRY
	{
		if( GetTerritory()->GetLocking(GetLockAttr()) == LOCKING_NONE )
			HR_THROW(E_NOTLOCKED);

		if( IsDirty() )
		{
			ASSERT( values.size() >= 2 );
			UserCopyTo(*(++values.begin()), p);
		}
		else
		{
			ASSERT( !values.empty() );
			UserCopyTo(values.front(), p);
		}
	}
	COMCATCH(;)
}


template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::Save(value_type &value)
{
	ASSERT( !IsZombie() );

	ICoreStorage *storage = SetStorageThisAttribute();
	ASSERT( storage != NULL );

	CComVariant v;
	StorageCopyTo(value, &v);

	COMTHROW( storage->put_AttributeValue(v) );
}


// ------- NestedTrItem


template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::AbortNestedTransaction()
{
	ASSERT( IsDirty() );
	ASSERT( values.size() >= 2 );

	RemoveValueDo(values.begin());

	ResetDirty();
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::DiscardPreviousValue()
{
	ASSERT( IsDirty() );
	ASSERT( values.size() >= 3 );

	RemoveValueDo(++values.begin());
}

// ------- FinalTrItem

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::AbortFinalTransaction()
{
	ASSERT( IsDirty() );
	ASSERT( values.size() >= 2 );

	// RemoveValueDo may call Unload back
	// so we have to reset dirty first

	ResetDirty();

	RemoveValueDo(values.begin());
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::CommitFinalTransaction()
{
	ASSERT( IsDirty() );
	ASSERT( values.size() >= 2 );

	Save( values.front() );
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::CommitFinalTransactionFinish(bool undo)
{
	ASSERT( IsDirty() );
	ASSERT( values.size() >= 2 );

	GetProject()->RegisterUndoItem(this);

	ResetDirty();
}

// ------- UndoItem

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::UndoTransaction()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	Save( *(++values.begin()) );
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::UndoTransactionFinish()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	SpliceValue(values.end(), values.begin());
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::RedoTransaction()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	Save( values.back() );
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::RedoTransactionFinish()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	SpliceValue(values.begin(), --values.end());
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::DiscardLastItem()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	RemoveValueTry(--values.end());
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::DiscardLastItemFinish()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	RemoveValueFinish(--values.end());
}

template<class BASE, const int VALTYPE>
void CCoreDataAttribute<BASE, VALTYPE>::DiscardLastItemCancel()
{
	ASSERT( values.size() >= 2 );
	ASSERT( !IsDirty() );

	RemoveValueCancel(--values.end());
}
*/

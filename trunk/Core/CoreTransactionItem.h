#ifndef __CORE_TRANSACTIONITEM_H__
#define __CORE_TRANSACTIONITEM_H__


// --------------------------- CoreTransactionItem


// IMPORTANT: Each transaction item cares only about itself and those
// of its parts which are not transaction items themselves.
// Never make any modification to other transaction items during
// abort, discard, commit or commit finish.

// Call ICoreProject::RegisterTransactionItem to get into the
// latest nested transaction, be dirty when registering

class ICoreTransactionItem
{
public:
	// Dirty if modified in the latest transaction
	virtual bool IsDirty(void) const throw()=0;

	// We clear the flag at the beggining of a nested transaction
	// We set it at abort, or committing a nested tr and gaining focus
	virtual void MarkDirty(void) throw()=0;

	// Must be dirty, discard the latest transaction value, clear dirty
	virtual void AbortNestedTransaction(void) throw()=0;

	// Must be dirty, discard the previous transaction value, keep dirty
	// called only for nested transactions when we merge the latest two values
	virtual void DiscardPreviousValue(void) throw()=0;
};


// --------------------------- CoreFinalTransactionItem


// Call ICoreProject::RegisterFinalTransactionItem to register, call it only once
// CoreObjects register to group database access of their CoreAttributes

class ICoreFinalTransactionItem
{
public:
	// Called for final abort, clear dirty
	virtual void AbortFinalTransaction(void) throw()=0;

	// Should do the work, may be called several times,
	virtual void CommitFinalTransaction(void)=0;

	// Clean up, clear dirty
	virtual void CommitFinalTransactionFinish(const bool &undo) throw()=0;
};


// --------------------------- CoreUndoItem


// Call ICoreProject::RegisterUndoItem to register,
// Call it only from CommitFinalTransactionFinish

class ICoreUndoItem
{
public:
	// Do the work here, called only once for each level
	// This is not a regular transaction, do not register new
	// transaction items and final transaction items
	virtual void UndoTransaction(void)=0;
	virtual void UndoTransactionFinish(void) throw()=0;

	virtual void RedoTransaction(void)=0;
	virtual void RedoTransactionFinish(void) throw()=0;

	// Not a regular transaction, only locking attributes can register
	virtual void DiscardLastItem(void)=0;
	virtual void DiscardLastItemFinish(void) throw()=0;
	virtual void DiscardLastItemCancel(void) throw()=0;
};

#endif	//__CORE_TRANSACTIONITEM_H__


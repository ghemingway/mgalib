#ifndef __META_BASE_H__
#define __META_BASE_H__


/*** Included Header Files ***/
#include "MetaProject.h"
#include "../Core/CoreProject.h"
#include "../Core/CoreObject.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaConstraint;


// --------------------------- MetaBase --------------------------- //


class MetaBase
{
private:
	MetaBase();													//!< Deny access to default constructor
	MetaBase(const MetaBase &);									//!< Deny access to copy constructor
	MetaBase& operator=(const MetaBase &);						//!< Deny access to equals operator
	
protected:
	CoreObject					_coreObject;					//!< Pointer to wrapped core object
	MetaProject					*_metaProject;					//!< Pointer to parent project

public:
	template <class T>
	static inline const Result_t ObjectFromAttribute(const CoreObject &coreObject, MetaProject* const &metaProject, const AttrID_t &attrID, T* &obj) throw()
	{
		// Start us off with a transaction
		Result_t txResult = metaProject->BeginTransaction();
		ASSERT( txResult == S_OK );
		Uuid uuid = Uuid::Null();
		Result_t result = coreObject->GetAttributeValue(attrID, uuid);
		ASSERT( result == S_OK );
		// Is the Uuid valid
		if (uuid == Uuid::Null())
		{
			obj = NULL;
			result = E_NOTFOUND;
		}
		// Convert this Uuid into an object
		else
		{
			// Get the coreProject and coreObject for the calling object
			CoreProject *coreProject = NULL;
			result = coreObject->Project(coreProject);
			ASSERT( result == S_OK );
			ASSERT( coreProject != NULL );
			CoreObject newCoreObject;
			result = coreProject->Object(uuid, newCoreObject);
			ASSERT( result == S_OK );
			ASSERT( newCoreObject != NULL );
			// Create the object with the coreObject and metaProject
			obj = new T(newCoreObject, metaProject);
			ASSERT( obj != NULL );
		}
		// Close the transaction and be done
		txResult = metaProject->CommitTransaction();
		ASSERT( txResult == S_OK );
		return result;
	}

	template <class T>
	static inline const Result_t CollectionFromAttribute(const CoreObject &coreObject, MetaProject* const &metaProject, const AttrID_t &attrID, std::list<T> &objList) throw()
	{
		ASSERT(false);
/*		objList.clear();
		std::list<MetaObjIDPair> pairList;
		Result_t result = this->_coreObject->GetAttributeValue(attrID, pairList);
		if ( result != S_OK ) return result;
		// Get the coreProject
		CoreProject* coreProject;
		CoreObject* coreObject;
		MetaBase* metaBase;
		ASSERT( this->_coreObject->Project(coreProject) == S_OK );
		ASSERT( coreProject != NULL );
		// Iterate over the list
		std::list<MetaObjIDPair>::iterator listIter = pairList.begin();
		while (listIter != pairList.end())
		{
			ASSERT( coreProject->Object(*listIter, coreObject) == S_OK );
			ASSERT( coreObject != NULL );
			int32_t value;
			// What metaRef does this object have (210 == ATTRID_METAREF - just dont want to include MetaGeneric.h)
			ASSERT( coreObject->GetAttributeValue(210, value) == S_OK );
			MetaRef_t metaRef = (MetaRef_t)value;
			delete coreObject;
			// Get the MetaBase from the MetaProject metaObjHash
			result = this->_metaProject->FindObject(metaRef, metaBase);
			ASSERT( metaBase != NULL );
			if (result != S_OK) return result;
			objList.push_back( (T)metaBase );
			// Move to the next pair
			++listIter;
		} */
		return S_OK;
	}

	template<class T>
	static inline const Result_t CreateObject(const CoreObject &coreObject, MetaProject* const &metaProject, const MetaID_t &metaID, const AttrID_t &attrID, T* &metaObject) throw()
	{
		// Get the associated coreProject
		CoreProject* coreProject = NULL;
		Result_t result = coreObject->Project(coreProject);
		ASSERT( result == S_OK );
		ASSERT( coreProject != NULL );
		// Start a transaction
		result = coreProject->BeginTransaction(false);
		ASSERT( result == S_OK );
		// Create a new coreObject
		CoreObject newCoreObject;
		result = coreProject->CreateObject(metaID, newCoreObject);
		ASSERT( result == S_OK );
		ASSERT( newCoreObject != NULL );
		// Link the new child to this object as parent
		Uuid uuid = Uuid::Null();
		result = coreObject->GetUuid(uuid);
		ASSERT( result == S_OK );
		ASSERT( uuid != Uuid::Null() );
		result = newCoreObject->SetAttributeValue(attrID, uuid);
		ASSERT( result == S_OK );
		// Commit transaction at the CoreProject level
		result = coreProject->CommitTransaction();
		ASSERT( result == S_OK );
		// Now use the core object to create a new metaObject of type T
		metaObject = new T(newCoreObject, metaProject);
		ASSERT( metaObject != NULL );
		return S_OK;
	}

	template<class T>
	static inline const Result_t ObjectFromCollectionByName(const CoreObject &coreObject, MetaProject* const &metaProject, const AttrID_t &objAttrID, const AttrID_t &nameAttrID, const std::string &name, T* &obj) throw()
	{
		ASSERT(false);
		// Use the helper function to get collection of objects
		std::list<T*> objList;
		Result_t result = MetaBase::CollectionFromAttribute(coreObject, metaProject, objAttrID, objList);
		ASSERT( result == S_OK );
		// Iterate through the list to find the one with name
		obj = NULL;
		typename std::list<T*>::iterator objIter = objList.begin();
		while (objIter != objList.end())
		{
			// Does the name match
			std::string objName;
			result = (*objIter)->GetName(objName);
			ASSERT( result == S_OK );
			if (objName == name) obj = *objIter;
			// Otherwise, delete this item
			else delete *objIter;
			// Move on to the next item in the list
			++objIter;
		}
		// Did we find it
		if (obj == NULL) return E_NOTFOUND;
		return S_OK;
	}

	template<class T>
	static inline const Result_t AddLink(const CoreObject &coreObject, MetaProject* const &metaProject, const MetaID_t &metaID, const AttrID_t &attrHere, const AttrID_t &attrThere, T* &obj) throw()
	{
		ASSERT(false);
//		CCoreObjectPtr self(me);
//		CCoreObjectPtr other(p);
//		CCoreObjectPtr link;
//		metaproject->CreateMetaObj(metaid, link);
//		link.PutPointerValue(here, self);
//		link.PutPointerValue(there, other);
		return S_OK;
	}

	MetaBase(CoreObject &coreObject, MetaProject* const &metaProject);
	virtual ~MetaBase();

	virtual inline const Result_t GetUuid(Uuid &uuid) const throw()						{ ASSERT( this->_coreObject != NULL ); return this->_coreObject->GetUuid(uuid); }
	virtual inline const Result_t GetMetaProject(MetaProject* &project) const throw()	{ project = this->_metaProject; return S_OK; }
	virtual const Result_t GetName(std::string &name) const throw();
	virtual const Result_t SetName(const std::string &name) throw();
	virtual const Result_t GetDisplayedName(std::string &name) const throw();
	virtual const Result_t SetDisplayedName(const std::string &name) throw();
	virtual const Result_t GetRegistryValue(const std::string &key, std::string &value) const throw();
	virtual const Result_t SetRegistryValue(const std::string &key, const std::string &value) throw();
	virtual const Result_t GetObjType(ObjType_t &objType) const throw();
	virtual const Result_t GetConstraints(std::list<MetaConstraint*> &cList) throw();
	virtual const Result_t CreateConstraint(MetaConstraint* &constraint) throw();
};


/*** End of MGA Namespace ***/
}


#endif // __META_BASE_H__


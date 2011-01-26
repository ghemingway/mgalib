#ifndef __META_BASE_H__
#define __META_BASE_H__


/*** Included Header Files ***/
#include "MetaRegNode.h"
#include "MetaProject.h"
#include "../Core/CoreProject.h"
#include "../Core/CoreObject.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaConstraint;


// --------------------------- MetaBase --------------------------- //


class MetaBase : public MetaRegNodes
{
private:
	MetaBase();													//!< Deny access to default constructor
	MetaBase(const MetaBase &);									//!< Deny access to copy constructor
	MetaBase& operator=(const MetaBase &);						//!< Deny access to equals operator
	
protected:
	CoreObject					_coreObject;					//!< Pointer to wrapped core object
	MetaProject					*_metaProject;					//!< Pointer to parent project

	template <class T>
	const Result_t ObjectFromAttribute(const AttrID_t &attrID, T* &obj) const throw()
	{
		Uuid uuid;
		Result_t result = this->_coreObject->GetAttributeValue(attrID, uuid);
		if ( result != S_OK ) return result;
		// Get a coreObject for the uuid
		CoreProject* coreProject = NULL;
		result = this->_coreObject->Project(coreProject);
		ASSERT( result == S_OK );
		ASSERT( coreProject != NULL );
		CoreObject coreObject;
		result = coreProject->Object(uuid, coreObject);
		ASSERT( result == S_OK );
		ASSERT( coreObject != NULL );
		// Create a new MetaBase object and cast to the correct type
		obj = (T*)new MetaBase(coreObject, this->_metaProject);
		ASSERT(obj != NULL);
		return S_OK;
	}

	template <class T>
	const Result_t CollectionFromAttribute(const AttrID_t &attrID, std::list<T> &objList) const throw()
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

public:
	MetaBase(CoreObject &coreObject, MetaProject* const &metaProject);
	virtual ~MetaBase();

	virtual const Result_t GetUuid(Uuid &uuid) const throw();
	virtual const Result_t GetMetaProject(MetaProject* &project) const throw();
	virtual const Result_t GetName(std::string &name) const throw();
	virtual const Result_t GetDisplayedName(std::string &name) const throw();
	virtual const Result_t GetObjType(ObjType_t &objType) const throw();
	virtual const Result_t GetConstraints(std::list<MetaConstraint*> &cList) throw();
	virtual const Result_t SetName(const std::string &name) throw();
	virtual const Result_t SetDisplayedName(const std::string &name) throw();
	virtual const Result_t CreateConstraint(MetaConstraint* &constraint) throw();
//	virtual const Result_t Delete(void) throw();
};


/*** End of MGA Namespace ***/
}


#endif // __META_BASE_H__


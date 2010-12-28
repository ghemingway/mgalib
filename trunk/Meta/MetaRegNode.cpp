/*** Included Header Files ***/
#include "MetaRegNode.h"
#include "MetaGeneric.h"
#include "MetaBase.h"
#include "../Core/CoreProject.h"
#include "../Core/CoreObject.h"


// --------------------------- MetaRegNodes --------------------------- //


const Result_t MetaRegNodes::GetRegistryNode(CoreObject &coreObject, const std::string &path, MetaRegNode* &regNode)
{
	ASSERT( coreObject != NULL );
	CoreProject* coreProject;
	coreObject->Project(coreProject);
	ASSERT( coreProject != NULL );
	for(;;)
	{
		// pos will be either in range 0..len-1 or will be equal with string::npos 
		int pos = path.find('/');
		std::string name(path, 0, pos);
		// Get the collection of backpointers to registry nodes
		CoreObject node;
		std::list<Uuid> coreObjectIDs;
		coreObject->GetAttributeValue(ATTRID_REGNODES_COLL, coreObjectIDs);
		
		std::list<Uuid>::iterator idIter = coreObjectIDs.begin();
		while( idIter != coreObjectIDs.end() )
		{
			std::string tmpName;
			CoreObject tmpObject;
			coreProject->Object(*idIter, tmpObject);
			ASSERT(tmpObject != NULL);
			tmpObject->GetAttributeValue(ATTRID_NAME, tmpName);
			if( name == tmpName )
			{
				node = tmpObject;
				break;
			}
			// Make sure to delete the tmpObject
			tmpObject.reset();
			// Move on to the next object
			++idIter;
		}
		// Did we find the node we were looking for
		if( node == NULL )
		{
			// Create the node
			Result_t result = coreProject->CreateObject(METAID_METAREGNODE, node);
			if ( result == E_TRANSACTION ) return E_NOTFOUND;
			if ( result != S_OK ) return result;
			ASSERT( node != NULL );
			node->SetAttributeValue(ATTRID_REGNODES_COLL, coreObject);
			node->SetAttributeValue(ATTRID_NAME, name);
		}
		ASSERT( node != NULL );
		
		if( pos == std::string::npos) //<-- correct test, instead of "if( (unsigned int) pos >= path.length() )"
		{
//			COMTHROW( ::QueryInterface(node, p) );
			return S_OK;
		}
		
//		regNode = node;
//		path.erase(0, pos+1);
	}
	return S_OK;
}

/*
HRESULT CMgaMetaRegNodes::get_RegistryValue(IUnknown *me, BSTR path, BSTR *p)
{
	CComObjPtr<IMgaMetaRegNode> node;

	HRESULT hr;
	if( FAILED(hr = get_RegistryNode(me, path, PutOut(node))) )
	{
		if( hr == E_NOTFOUND )
			hr = S_OK;

		return hr;
	}

	ASSERT( node != NULL );
	return node->get_Value(p);
}

HRESULT CMgaMetaRegNodes::put_RegistryValue(IUnknown *me, BSTR path, BSTR p)
{
	CComObjPtr<IMgaMetaRegNode> node;

	HRESULT hr;
	if( FAILED(hr = get_RegistryNode(me, path, PutOut(node))) )
	{
		if( hr == E_NOTFOUND )
			hr = S_OK;

		return hr;
	}

	ASSERT( node != NULL );
	return node->put_Value(p);
}
*/

// --------------------------- MetaRegNode --------------------------- //

/*
void MetaRegNode::Traverse(MetaProject* &metaProject, CoreObject* &coreObject)
{
	ASSERT( metaProject != NULL );
	ASSERT( coreObject != NULL );
	// Traverse any children
	MetaBase::TraverseCollection(metaProject, coreObject, ATTRID_REGNODES_COLL);
}
*/

const Result_t MetaRegNode::GetRegistryNodes(std::list<MetaRegNode*> &nodeList) const throw()
{
//	return ComGetCollectionValue<MetaRegNode>(this, ATTRID_REGNODES_COLL, p);
	return S_OK;
}


const Result_t MetaRegNode::GetName(std::string &name) const throw()
{
//	return ComGetAttrValue(this, ATTRID_NAME, name);
	return S_OK;
}


const Result_t MetaRegNode::GetValue(std::string &value) const throw()
{
//	return ComGetAttrValue(this, ATTRID_VALUE, value);
	return S_OK;
}

/* 
STDMETHODIMP CMgaMetaRegNode::Delete()
{
	COMTRY
	{
		CCoreObjectPtr self(GetUnknown());
	
		self.PutLockValue(ATTRID_LOCK, LOCKING_EXCLUSIVE);

		coreobjects_type coreobjects;
		self.LoadCollectionValue(ATTRID_REGNODES_COLL, coreobjects);

		coreobjects_iterator i = coreobjects.begin();
		coreobjects_iterator e = coreobjects.end();
		while( i != e )
		{
			CComObjPtr<IMgaMetaRegNode> node;
			COMTHROW( ::QueryInterface(*i, node) );

			COMTHROW( node->Delete() );

			++i;
		}

		COMTHROW( self->Delete() );
	}
	COMCATCH(;)
}
*/

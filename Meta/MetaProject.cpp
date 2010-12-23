/*** Included Header Files ***/
#include "MetaProject.h"
#include "MetaGeneric.h"
#include "MetaFolder.h"
#include "../Core/CoreProject.h"
#include "../Core/CoreObject.h"
#include <time.h>


static std::string _CurrentTime(void)
{
	time_t t;
	time(&t);
	char *p = ctime(&t);
	ASSERT( p != NULL );
	p[strlen(p)-1] = 0;
	return p;	
}


// --------------------------- Private MetaProject Methods --------------------------- //


MetaProject::MetaProject(CoreProject* &coreProject) : _maxMetaRef(1000), _coreProject(coreProject),
	_rootObject(NULL), _namespace("")
{
	ASSERT( this->_coreProject != NULL );
	// Setup the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	ASSERT( this->_coreProject->RootObject(this->_rootObject) == S_OK );
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );	
}


const Result_t MetaProject::RegisterMetaBase(const MetaRef_t &metaRef, MetaBase* obj)
{
	ASSERT( obj != NULL );
	// Try to find metaRef in the hash
	if( this->_metaObjectHash.find(metaRef) != this->_metaObjectHash.end() )
		return E_METAREF;
	// Make sure to check against the max MetaRef
	if( metaRef > this->_maxMetaRef ) this->_maxMetaRef = metaRef;
	// Update the value in the hash
	this->_metaObjectHash[metaRef] = obj;
	return S_OK;
}


const Result_t MetaProject::UnregisterMetaBase(const MetaRef_t &metaRef, MetaBase* obj)
{
	ASSERT( obj != NULL );
	ASSERT( this->_metaObjectHash.find(metaRef) != this->_metaObjectHash.end() );
	ASSERT( this->_metaObjectHash[metaRef] == obj );
	this->_metaObjectHash.erase(metaRef);
	return S_OK;
}


// --------------------------- Public MetaProject Methods --------------------------- //


MetaProject::~MetaProject()
{
	ASSERT( this->_rootObject != NULL );
	ASSERT( this->_coreProject != NULL );
	// Need to delete all objects in the metaObjHash
	while (!this->_metaObjectHash.empty())
	{
		// Get a MetaBase object
		MetaBase* metaBase = this->_metaObjectHash.begin()->second;
		// Try to delete it
		if (metaBase != NULL) delete metaBase;
	}
	ASSERT( this->_metaObjectHash.empty() );
	// Now we can delete the core project
	delete this->_coreProject;
}


const Result_t MetaProject::Open(const std::string &connection, MetaProject* &project) throw()
{
	if (connection == "") return E_INVALID_USAGE;
	// Try to open the project
	CoreMetaProject* coreMetaProject;
	ASSERT( CreateMetaCoreMetaProject(true, coreMetaProject) == S_OK );
	CoreProject *coreProject;
	Result_t result = CoreProject::OpenProject(connection, coreMetaProject, coreProject);
	if (result != S_OK)
	{
		// Clean up and exit
//		delete coreMetaProject;
		return result;
	}
	ASSERT( coreProject != NULL );
	// Create the metaProject
	MetaProject* metaProject = new MetaProject(coreProject);
	ASSERT( metaProject != NULL );
	// Traverse and register the entire project (must be inside a transaction
	ASSERT( coreProject->BeginTransaction(true) == S_OK );
	MetaFolder::Traverse(metaProject, metaProject->_rootObject);
	ASSERT( coreProject->CommitTransaction() == S_OK );
	project = metaProject;
	return S_OK;
}


const Result_t MetaProject::Create(const std::string &connection, MetaProject* &project) throw()
{
	if (connection == "") return E_INVALID_USAGE;
	// Try to create a CoreProject
	CoreMetaProject* coreMetaProject;
	ASSERT( CreateMetaCoreMetaProject(true, coreMetaProject) == S_OK );
	CoreProject *coreProject;
	Result_t result = CoreProject::CreateProject(connection, coreMetaProject, coreProject);
	if (result != S_OK)
	{
		// Clean up and exit
		delete coreMetaProject;
		return result;
	}
	ASSERT( coreProject != NULL );
	// Create the new MetaProject
	MetaProject* metaProject = new MetaProject(coreProject);
	ASSERT( metaProject != NULL );
	// Create the root object
	ASSERT( metaProject->_coreProject->BeginTransaction(false) == S_OK );
	metaProject->_rootObject->SetAttributeValue(ATTRID_CDATE, _CurrentTime());
	metaProject->_rootObject->SetAttributeValue(ATTRID_MDATE, _CurrentTime());
	metaProject->_rootObject->SetAttributeValue(ATTRID_NAME, std::string(""));
	metaProject->_rootObject->SetAttributeValue(ATTRID_METAREF, 1000);
	MetaFolder::Traverse(metaProject, metaProject->_rootObject);
	ASSERT( metaProject->_coreProject->CommitTransaction() == S_OK );
	// MetaProject is ready
	project = metaProject;
	return S_OK;
}


const Result_t MetaProject::BeginTransaction(void) throw()
{
	ASSERT( this->_coreProject != NULL );
	// BeingTransaction on underlying CoreProject
	return this->_coreProject->BeginTransaction(false);
}


const Result_t MetaProject::CommitTransaction(void) throw()
{
	ASSERT( this->_coreProject != NULL );
	// Set the modify date on the root object if the coreProject is dirty
	bool flag;
	ASSERT( this->_coreProject->IsDirty(flag) == S_OK );
	if (flag) this->_rootObject->SetAttributeValue(ATTRID_MDATE, _CurrentTime());
	// Commit the transaction
	return this->_coreProject->CommitTransaction();
}


const Result_t MetaProject::AbortTransaction(void) throw()
{
	ASSERT( this->_coreProject != NULL );
	// AbortTransaction on underlying CoreProject
	return this->_coreProject->AbortTransaction();
}


const Result_t MetaProject::GetGUID(GUID_t &guid) const throw()
{
	std::vector<unsigned char> value;
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_GUID, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	if (result != S_OK) return result;
	// Convert GUID from bin value to GUID_t
	guid = value;
	return S_OK;
}

/*
const Result_t MetaProject::SetGUID(const GUID_t &guid) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->SetAttributeValue(ATTRID_GUID, guid);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}
*/

const Result_t MetaProject::GetName(std::string &name) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_PARNAME, name);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}

/*
const Result_t MetaProject::SetName(const std::string &name) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->SetAttributeValue(ATTRID_PARNAME, name);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}
*/

const Result_t MetaProject::GetDisplayedName(std::string &displayedName) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_DISPNAME, displayedName);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}

/*
const Result_t MetaProject::SetDisplayedName(const std::string &value) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->SetAttributeValue(ATTRID_PARDISPNAME, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}
*/

const Result_t MetaProject::GetVersion(std::string &version) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_VERSION, version);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}

/*
const Result_t MetaProject::SetVersion(const std::string &value) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->SetAttributeValue(ATTRID_VERSION, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}
*/

const Result_t MetaProject::GetAuthor(std::string &author) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_AUTHOR, author);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}

/*
const Result_t MetaProject::SetAuthor(const std::string &value) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->SetAttributeValue(ATTRID_AUTHOR, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}
*/

const Result_t MetaProject::GetComment(std::string &comment) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_COMMENT, comment);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}

/*
const Result_t MetaProject::SetComment(const std::string &value) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result =  this->_rootObject->SetAttributeValue(ATTRID_COMMENT, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}
*/

const Result_t MetaProject::GetCreatedAt(std::string &createdAt) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_CDATE, createdAt);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}

/*
const Result_t MetaProject::SetCreatedAt(const std::string &value) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result =  this->_rootObject->SetAttributeValue(ATTRID_CDATE, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}
*/

const Result_t MetaProject::GetModifiedAt(std::string &modifiedAt) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_MDATE, modifiedAt);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}

/*
const Result_t MetaProject::SetModifiedAt(const std::string &value) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->SetAttributeValue(ATTRID_MDATE, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}
*/

const Result_t MetaProject::FindObject(const MetaRef_t &metaRef, MetaBase* &metaBase) throw()
{
	MetaObjHashIterator metaHashIter = this->_metaObjectHash.find(metaRef);
	if( metaHashIter == this->_metaObjectHash.end() ) return E_NOTFOUND;
	metaBase = metaHashIter->second;
	ASSERT( metaBase != NULL );
	return S_OK;
}

/*
void MetaProject::CreateMetaBase(metaid_type metaid, CCoreObjectPtr &obj)
{
	ASSERT( obj == NULL );
	ASSERT( coreproject != NULL );

	COMTHROW( coreproject->CreateObject(metaid, PutOut(obj)) );
	COMTHROW( obj->put_AttributeValue(ATTRID_METAREF, PutInVariant(max_metaref + 1)) );
	CMgaMetaBase::Traverse(this, obj);
	
	++max_metaref;
}


void MetaProject::CreateMetaObj(metaid_type metaid, CCoreObjectPtr &obj)
{
	ASSERT( obj == NULL );
	ASSERT( coreproject != NULL );

	COMTHROW( coreproject->CreateObject(metaid, PutOut(obj)) );
}


// ------- CheckPath


void MetaProject::CreatePathItems(bstr_const_iterator i, bstr_const_iterator e, pathitems_type &pathitems)
{
	ASSERT( pathitems.empty() );

	while( i != e && *(e-1) == L' ' )
		--e;

	while( i < e )
	{
		wchar_t *j = 0;
		long metaref = wcstol(i, &j, 10);

		if( i == j || j == 0 )
			COMTHROW(E_INVALID_PATH);

		metaobj_lookup_iterator k = metaobj_lookup.find(metaref);
		if( k == metaobj_lookup.end() )
			COMTHROW(E_INVALID_PATH);

		pathitems.push_back(pathitem_type());
		
		COMTHROW( k->second->get_Name(PutOut(pathitems.back().continual_name)) );

		CComObjPtr<IMgaMetaRole> role;
		if( SUCCEEDED( ::QueryInterface(k->second, role)) )
		{
			CComObjPtr<IMgaMetaFCO> fco;
			COMTHROW( role->get_Kind(PutOut(fco)) );
			COMTHROW( fco->get_Name(PutOut(pathitems.back().terminal_name)) );
		}
		else
		{
			pathitems.back().terminal_name = pathitems.back().continual_name;
		}

		i = j;
	}
}

void MetaProject::CreateJointPaths(BSTR paths, jointpaths_type &jointpaths)
{
	ASSERT( jointpaths.empty() );

	bstr_const_iterator i = begin(paths);
	bstr_const_iterator e = end(paths);
	while( i != e )
	{
		i = std::find_if(i, e, std::bind2nd( std::not_equal_to<wchar_t>(), L' '));
		if( i == e )
			break;
		
		bstr_const_iterator j = std::find(i, e, L' ');
		bstr_const_iterator k = std::find(j, e, L',');

		jointpaths.push_back(jointpath_type());
		CopyTo(i, j, jointpaths.back().ptrspec_name);

		CreatePathItems(j, k, jointpaths.back().pathitems);

		i = k;
		if( i != e )
		{
			ASSERT( *i == L',' );
			++i;
		}
	}
}
*/

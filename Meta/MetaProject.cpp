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


MetaProject::MetaProject(CoreProject* &coreProject) : _coreProject(coreProject),
	_rootObject(), _namespace("")
{
	ASSERT( this->_coreProject != NULL );
	// Setup the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	ASSERT( this->_coreProject->RootObject(this->_rootObject) == S_OK );
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );	
}


// --------------------------- Public MetaProject Methods --------------------------- //


MetaProject::~MetaProject()
{
	ASSERT( this->_rootObject != NULL );
	ASSERT( this->_coreProject != NULL );
	// Remove the reference to the rootObject
	this->_rootObject = NULL;
	// Now we can delete the core project
	delete this->_coreProject;
}


const Result_t MetaProject::Open(const std::string &connection, MetaProject* &project) throw()
{
	if (connection == "") return E_INVALID_USAGE;
	// Try to open the project
	CoreMetaProject* coreMetaProject;
	ASSERT( CreateMetaCoreMetaProject(coreMetaProject) == S_OK );
	CoreProject *coreProject;
	Result_t result = CoreProject::Open(connection, coreMetaProject, coreProject);
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
//	MetaFolder::Traverse(metaProject, metaProject->_rootObject);
	ASSERT( coreProject->CommitTransaction() == S_OK );
	project = metaProject;
	return S_OK;
}


const Result_t MetaProject::Create(const std::string &connection, MetaProject* &project) throw()
{
	if (connection == "") return E_INVALID_USAGE;
	// Try to create a CoreProject
	CoreMetaProject* coreMetaProject;
	ASSERT( CreateMetaCoreMetaProject(coreMetaProject) == S_OK );
	ASSERT( coreMetaProject != NULL );
	CoreProject *coreProject;
	Result_t result = CoreProject::Create(connection, coreMetaProject, coreProject);
	if (result != S_OK)
	{
		std::cout << GetErrorInfo(result);
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
//	MetaFolder::Traverse(metaProject, metaProject->_rootObject);
	ASSERT( metaProject->_coreProject->CommitTransaction() == S_OK );
	// MetaProject is ready
	project = metaProject;
	return S_OK;
}


const Result_t MetaProject::Save(const std::string &filename) throw()
{
	ASSERT( this->_coreProject != NULL );
	// Ask the coreProject to save itself
	return this->_coreProject->Save(filename);
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


const Result_t MetaProject::GetUuid(Uuid &uuid) const throw()
{
	Uuid value = Uuid::Null();
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_UUID, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	if (result != S_OK) return result;
	// Convert GUID from bin value to GUID_t
	uuid = value;
	return S_OK;
}


const Result_t MetaProject::SetUuid(const Uuid &uuid) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->SetAttributeValue(ATTRID_UUID, uuid);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::GetName(std::string &name) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_PARNAME, name);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::SetName(const std::string &name) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->SetAttributeValue(ATTRID_PARNAME, name);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::GetDisplayedName(std::string &displayedName) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_DISPNAME, displayedName);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::SetDisplayedName(const std::string &value) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->SetAttributeValue(ATTRID_PARDISPNAME, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::GetVersion(std::string &version) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_VERSION, version);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::SetVersion(const std::string &value) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->SetAttributeValue(ATTRID_VERSION, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::GetAuthor(std::string &author) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_AUTHOR, author);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::SetAuthor(const std::string &value) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->SetAttributeValue(ATTRID_AUTHOR, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::GetComment(std::string &comment) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_COMMENT, comment);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::SetComment(const std::string &value) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result =  this->_rootObject->SetAttributeValue(ATTRID_COMMENT, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::GetCreatedAt(std::string &createdAt) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_CDATE, createdAt);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::SetCreatedAt(const std::string &value) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result =  this->_rootObject->SetAttributeValue(ATTRID_CDATE, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::GetModifiedAt(std::string &modifiedAt) const throw()
{
	// Get the value from the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->GetAttributeValue(ATTRID_MDATE, modifiedAt);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::SetModifiedAt(const std::string &value) throw()
{
	// Set the value in the root object
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	Result_t result = this->_rootObject->SetAttributeValue(ATTRID_MDATE, value);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return result;
}


const Result_t MetaProject::RootFolder(MetaFolder* &folder) throw()
{
	// Traverse and register the entire project (must be inside a transaction
	ASSERT( this->_coreProject->BeginTransaction(true) == S_OK );
	// The rootObject is the rootFolder - just need to make a copy of it
	CoreObject rootObject;
	Uuid rootUuid;
	this->_rootObject->GetUuid(rootUuid);
	this->_coreProject->Object(rootUuid, rootObject);
	MetaProject* metaProject = this;
	MetaBase* metaBase = new MetaBase(rootObject, metaProject);
	ASSERT( metaBase != NULL );
	folder = (MetaFolder*)metaBase;
	ASSERT( folder != NULL );
	// Wrap up the transaction
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	return S_OK;
}


const Result_t MetaProject::FindObject(const Uuid &uuid, MetaBase* &metaBase) throw()
{
//	MetaObjectHashIterator metaHashIter = this->_metaObjectHash.find(uuid);
//	if( metaHashIter == this->_metaObjectHash.end() ) return E_NOTFOUND;
//	metaBase = metaHashIter->second;
	ASSERT( metaBase != NULL );
	return S_OK;
}


void MetaProject::CreateMetaBase(const MetaID_t &metaID, CoreObject &object)
{
	ASSERT( this->_coreProject != NULL );
//	ASSERT( obj == NULL );
	ASSERT( this->_coreProject->CreateObject(metaID, object) == S_OK );
//	COMTHROW( obj->put_AttributeValue(ATTRID_METAREF, PutInVariant(max_metaref + 1)) );
//	MetaBase::Traverse(this, obj);
}

/*
void MetaProject::CreateMetaObj(metaid_type metaid, CCoreObjectPtr &obj)
{
	ASSERT( obj == NULL );
	ASSERT( coreproject != NULL );

	COMTHROW( coreproject->CreateObject(metaid, PutOut(obj)) );
}


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

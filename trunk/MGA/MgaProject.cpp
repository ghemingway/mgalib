// MgaProject.cpp : Implementation of CMgaProject
#include "MgaProject.h"
#include "MgaGeneric.h"
#include "MgaRegistrar.h"
#include "MgaTerritory.h"
#include "MgaFolder.h"
#include "../Core/CoreMetaProject.h"
#include "../Core/CoreProject.h"
#include "../Core/CoreObject.h"
#include "../Core/CoreAttribute.h"
#include "../Meta/MetaProject.h"


// --------------------------- MgaProject Private Methods --------------------------- //


MgaProject::MgaProject(CoreProject *coreProject, MetaProject *metaProject, const bool &readOnly) :
	_coreProject(coreProject), _metaProject(metaProject), _mgaVersion(2), _preferences(0),
	_opMask(0x0026662A), _isReadOnly(readOnly), _addOnList(), _clientList(), _inAutoAddOnCreate(false), _allTerritories(),
	_baseTerritory(NULL), _activeTerritory(NULL), _reserveTerritory(NULL)
{
	ASSERT( coreProject != NULL );
	ASSERT( metaProject != NULL );
//	this->_rot = 0;
//	this->_checkoff = false;
//	this->_transactioncount = 0;
//	this->_autoaddons = false;
//	this->_aurcnt = 0;
//	this->_guidstat = CLEAN;

	Uuid nUuid;
	ASSERT( this->_metaProject->GetUuid(nUuid) == S_OK );
/*
	if(pGUID == nGUID)
	{
/*		this->BeginTransaction(lm, TRANSACTION_READ_ONLY));
		MgaFolder *rf;
		this->GetRootFolder(rootFolder);
		(ObjFor(rf)->CheckTree());
		this->CommitTransaction();

		// If we are not in readOnly Mode, 
		if (*ro_mode == VARIANT_FALSE)
		{
			COMTHROW(BeginTransaction(lm, TRANSACTION_GENERAL));
			CoreObject* rootObject;
			this->_coreProject->RootObject(rootObject);
			dataroot[ATTRID_PARGUID] = nGUID;
			this->CommitTransaction();
		}
	}
*/
	// Start all automatic addOns
	this->StartAutoAddOns();
}

/*
const Result_t MgaProject::GetMetaObj(const Uuid &metaRef, MetaBase* &metaBase)
{
	if( this->_metaProject == NULL ) return E_MGA_PROJECT_NOT_OPEN;
	this->SetNamespaceInMeta();
	return this->_metaProject->FindObject(metaRef, metaBase);
}


HRESULT CMgaProject::CreateSetupData(BSTR rootname, BSTR paradigmname, VARIANT guid) {
		COMTRY_IN_TRANSACTION {	

// check if meta is valid
			CoreObj  dataroot, rootfolder;
			CComBSTR parversion;

// set values in 			
			COMTHROW(dataproject->get_RootObject(&dataroot.ComPtr()));
			dataroot[ATTRID_CDATE] = dataroot[ATTRID_MDATE]  = Now();
			dataroot[ATTRID_PARADIGM] = paradigmname;
			dataroot[ATTRID_MGAVERSION] = mgaversion;
			COMTHROW(metapr->get_Version(&parversion));
			dataroot[ATTRID_PARVERSION] = parversion;
			dataroot[ATTRID_NAME]    = rootname;
			if( guid.vt != (VT_UI1 | VT_ARRAY) || GetArrayLength(guid) != sizeof(GUID) )
					COMTHROW(E_INVALIDARG);
			dataroot[ATTRID_PARGUID] = guid;
			dataroot[ATTRID_VERSION] = CComBSTR(""); // default version string

// create data root things
		    COMTHROW(dataproject->CreateObject(DTID_FOLDER,&rootfolder.ComPtr()));
			assignGuid( this, rootfolder); // assign a guid to the root folder
			rootfolder[ATTRID_NAME]    = rootname;
			rootfolder[ATTRID_FPARENT] = dataroot;
			if(!(preferences & MGAPREF_MANUAL_RELIDS))  rootfolder[ATTRID_RELID] = 1;
			CComPtr<IMgaMetaFolder> mf;
			metaref_type mr;
			COMTHROW(metapr->get_RootFolder(&mf));
			COMTHROW(mf->get_MetaRef(&mr));
			rootfolder[ATTRID_META] = mr;
			notifyqueueprocessed = true;
		} COMCATCH_IN_TRANSACTION(;);

}
*/

const Result_t MgaProject::OpenParadigm(MgaRegistrar* &registrar, const std::string &paradigm, const std::string &uuidStr, MetaProject* &metaProject)
{
	// Get the connection string for the paradigm
	std::string connectionString = "";
	Result_t result = registrar->QueryParadigm(paradigm, uuidStr, connectionString);
	if (result != S_OK || connectionString == "") return E_MGA_PARADIGM_NOTREG;
	// Try to open the file and create a MetaProject
	result = MetaProject::Open(connectionString, metaProject);
	if (result != S_OK || metaProject == NULL) return E_MGA_PARADIGM_INVALID;
	Uuid metaUuid;
	ASSERT( metaProject->GetUuid(metaUuid) == S_OK );
	// Make sure Uuids match
	std::string metaUuidStr = metaUuid;
	if(uuidStr != metaUuidStr)
	{
		delete metaProject;
		metaProject = NULL;
		return E_MGA_PARADIGM_INVALID;
	}
	// All is good
	return S_OK;
}


void MgaProject::StartAutoAddOns(void)
{
	if(!this->_autoAddOns) return;
	// Get the list of associated components from the registrar
	std::string paradigmName;
	//	this->_metaProject->GetName(paradigmName);
	std::vector<std::string> programIDs;
	this->_registrar->AssociatedComponents(paradigmName, COMPONENTTYPE_ADDON, programIDs);
	ASSERT(programIDs.size() >= 0);
	for(std::vector<std::string>::iterator idIter = programIDs.begin(); idIter < programIDs.end(); ++idIter)
	{
//		MgaComponent* addOn;
//		this->CreateMgaComponent(addOn, idIter);
//		ASSERT( addon != NULL );
//		CComQIPtr<IGMEVersionInfo> vv=addon;
//		GMEInterfaceVersion_enum v = GMEInterfaceVersion_None;
//		if(vv) COMTHROW(vv->get_version(&v));
//		if(v != GMEInterfaceVersion_Current)
//			HR_THROW(E_MGA_COMPONENT_ERROR);
//		addon->Initialize(this);
//		autocomps.push_front(addon.Detach());
	}
	this->_inAutoAddOnCreate = false;
}


// --------------------------- Public MgaProject Methods --------------------------- //


MgaProject::~MgaProject() {
	// Send global notification of project close (must be within transaction context)
	MgaTerritory* territory = NULL;
	ASSERT( this->BeginTransaction(territory, false) == S_OK );
	ASSERT( this->GlobalNotify(GLOBALEVENT_CLOSE_PROJECT) == S_OK );
	ASSERT( this->CommitTransaction() == S_OK );
	// PETER: Moved these two lines here, otherwise addons won't receive notifications
	this->StopAutoAddOns();
	this->RemoveClients();
	// Clean up the coreProject and the metaProject
	if ( this->_coreProject != NULL ) delete this->_coreProject;
	if ( this->_metaProject != NULL ) delete this->_metaProject;
}


const Result_t MgaProject::Create(const std::string &projectName, const std::string &paradigmName,
								  const Uuid &paradigmUuid, MgaRegistrar* &registrar, MgaProject* &mgaProject) throw()
{
//	CoreMetaProject* genericProject;
//	Result_t result = CreateMGACoreMetaProject(true, genericProject);
//	ASSERT( result == S_OK );
	mgaProject = NULL;
	return S_OK;
}
/*
STDMETHODIMP CMgaProject::CreateEx(BSTR projectname, BSTR paradigmname, VARIANT paradigmGUID) {
		CComVariant connGUID;
		if(paradigmGUID.vt != VT_EMPTY) connGUID = paradigmGUID;
		COMTHROW(OpenParadigm(paradigmname, &connGUID));
	
		int undosize = getMaxUndoSize();
		COMTHROW(dataproject->CreateProject2(projectname, undosize, genericproject));
		opened = UNCHANGED;
		guidstat = DIRTY;

	    CComPtr<IMgaTerritory> lm;
	    COMTHROW(CreateTerritory(NULL, &lm));
		COMTHROW(BeginTransaction(lm, TRANSACTION_GENERAL));
		
		HRESULT suhr = S_OK;
		try {
			CComBSTR   fname("RootFolder");
            // BGY: commented out because xml backend has different connection string format
//			LPCOLESTR p = wcsrchr(projectname, '\\');
//			if(p) {
//				p++;
//				LPCOLESTR p2 = wcschr(p, '.');
//				if(p2 == NULL) p2 = p + wcslen(p);
//				fname = CComBSTR(p2-p, p);
//			}
			suhr = CreateSetupData(fname, paradigmname, connGUID);
			COMTHROW(lm->Flush());
			COMTHROW(CommitTransaction());
		} catch(hresult_exception &e) {
			lm->Flush();
			AbortTransaction();
			throw e;
		}
		COMTHROW(dataproject->FlushUndoQueue());
		COMTHROW(suhr);
		COMTHROW(RegisterActiveObject((IMgaProject *)this,CLSID_MgaProject,ACTIVEOBJECT_STRONG,&rot));
		projconn = projectname;
		StartAutoAddOns();
		try {
			COMTHROW(BeginTransaction(lm, TRANSACTION_READ_ONLY));
			GlobalNotify(GLOBALEVENT_OPEN_PROJECT);
			COMTHROW(lm->Flush());
			COMTHROW(CommitTransaction());
		} catch(hresult_exception &e) {
			lm->Flush();
			AbortTransaction();
			throw e;
		}
		MARKSIG('2');
	}
	COMCATCH(
		opened = CLOSED;
		guidstat = CLEAN;
		if(dataproject) dataproject->CloseProject();
		if(metapr) metapr->Close();
		metapr = 0;
		projconn.Empty(); 
		parconn.Empty();
		// We've already called SetErrorInfo, don't call it again
		if (e.hr == E_MGA_COMPONENT_ERROR) {
			return e.hr;
		}
	)
}
*/

const Result_t MgaProject::Open(const std::string &projectName, MgaRegistrar* &registrar, const bool &readOnly, MgaProject* &project) throw()
{
	if (projectName == "") return E_INVALID_USAGE;
	// Create a coreMetaProject and open the coreProject
	CoreMetaProject* mgaCoreMetaProject;
	Result_t result = CreateMGACoreMetaProject(mgaCoreMetaProject);
	ASSERT( result == S_OK  );
	CoreProject* coreProject;
	result = CoreProject::Open(projectName, mgaCoreMetaProject, coreProject);
	// Handle case of not being able to open file
	if (result != S_OK)
	{
		project = NULL;
		delete mgaCoreMetaProject;
		return result;
	}
	// Try to get the paradigm guid from the open coreProject
	result = coreProject->BeginTransaction(true);
	ASSERT( result == S_OK );
	CoreObject rootObject;
	result = coreProject->RootObject(rootObject);
	ASSERT( result == S_OK );
	ASSERT( rootObject != NULL );
//	ASSERT( rootObject->Project() == coreProject );
	std::string paradigm, paradigmVersion;
	Uuid paradigmUuid;
	result = rootObject->GetAttributeValue(ATTRID_PARADIGM, paradigm);
	ASSERT( result == S_OK );
	result = rootObject->GetAttributeValue(ATTRID_PARADIGMUUID, paradigmUuid);
	ASSERT( result == S_OK );
	result = rootObject->GetAttributeValue(ATTRID_PARADIGMVERSION, paradigmVersion);
	ASSERT( result == S_OK );
	result = coreProject->CommitTransaction();
	ASSERT( result == S_OK );
	rootObject = NULL;

	// See if we can open the paradigm
	if (paradigm == "") return E_MGA_MODULE_INCOMPATIBILITY;
	MetaProject* metaProject = NULL;
	if (paradigmVersion != "")
	{
		// Version string has precedence over Uuid - get the guid for the current version
		std::string uuidStr;
		Result_t result = registrar->UuidFromVersion(paradigm, paradigmVersion, uuidStr);
		ASSERT( result == S_OK );
		result = MgaProject::OpenParadigm(registrar, paradigm, uuidStr, metaProject);
		if ( result != S_OK ) return result;
		ASSERT( metaProject != NULL );
	}
	else
	{
		// Convert paradigmUuid to string and open paradigm
		std::string uuidStr = paradigmUuid;
		result = MgaProject::OpenParadigm(registrar, paradigm, uuidStr, metaProject);
		if ( result != S_OK ) return result;
		ASSERT( metaProject != NULL );
	}
	// Create the MgaProject
	project = new MgaProject(coreProject, metaProject, readOnly);
	if( project == NULL ) return E_MGA_MUST_ABORT;
	// Finally, signal everyone that we are ready to run
	MgaTerritory* territory = NULL;
	result = project->BeginTransaction(territory, true);
	ASSERT( result == S_OK );
	result = project->GlobalNotify(GLOBALEVENT_OPEN_PROJECT);
	ASSERT( result == S_OK );
	result = project->CommitTransaction();
	ASSERT( result == S_OK );
	// And we are done
	return S_OK;
}

/*
STDMETHODIMP CMgaProject::QueryProjectInfo(BSTR projectname, long *mgaversion, 
										   BSTR *paradigmname, 
										   BSTR *parversion,
										   VARIANT *paradigmGUID,
										   VARIANT_BOOL *ro_mode)
{
	CComPtr<ICoreProject> dp;
	COMTRY {
		CHECK_INSTRPAR(projectname);
		CHECK_OUTPAR(mgaversion);
		CHECK_OUTSTRPAR(paradigmname);
		CHECK_OUTSTRPAR(parversion);
		CHECK_OUTPAR(paradigmGUID);
		CHECK_OUTPAR(ro_mode);

		CComPtr<ICoreMetaProject> genericproject;
		CreateCoreMetaProject(genericproject); // use mgaversion = 1 project model 

		COMTHROW(dp.CoCreateInstance(CLSID_CoreProject));
		COMTHROW(dp->OpenProject(projectname, genericproject, ro_mode));
		
		CComPtr<ICoreTerritory> tt;
		COMTHROW(dp->CreateTerritory(&tt));
		COMTHROW(dp->BeginTransaction(TRANSTYPE_READFIRST));

		try {	
			COMTHROW(dp->PushTerritory(tt));

			CoreObj  dataroot;
			COMTHROW(dp->get_RootObject(&dataroot.ComPtr()));
		    *paradigmname = CComBSTR(dataroot[ATTRID_PARADIGM]).Detach();
		    *parversion = CComBSTR(dataroot[ATTRID_PARVERSION]).Detach();
		    CComVariant(dataroot[ATTRID_PARGUID]).Detach(paradigmGUID);
			*mgaversion = dataroot[ATTRID_MGAVERSION];
			dp->PopTerritory();
		} catch(hresult_exception &) {
			;
		}
		dp->AbortTransaction();
		dp->CloseProject();
	}
	COMCATCH(
		if(dp) dp->CloseProject();
	)
}


STDMETHODIMP CMgaProject::Save(BSTR newname, VARIANT_BOOL keepoldname)
{
	COMTRY {
		if(baseterr) COMTHROW(E_MGA_ALREADY_IN_TRANSACTION);
		{
			CComPtr<IMgaTerritory> t;
			COMTHROW(CreateTerritory(NULL, &t));
			// if mga_ver<=1 the Core layer changed the project by adding ATTRID_GUID1..4
			// (mgaversion <= 1L) -> (opened >= CHANGED)
			ASSERT( !(mgaversion <= 1L) || opened >= CHANGED);

			if(opened >= CHANGED) {
				COMTHROW(BeginTransaction(t, TRANSACTION_GENERAL));
				try {
					CoreObj self;
					COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
					UpdateMGAVersion( self);
					FixupGUID();
					locking_type ppp;
					COMTHROW(self->get_PeerLockValue(ATTRID_LOCK, &ppp));
					//if(!(ppp & LOCKING_READ)) self[ATTRID_MDATE] = Now();
                    // BGY: this modifies the root every save time, which is bad for the
                    // xmlbackend (it always tries to check out the root)
					GlobalNotify(GLOBALEVENT_SAVE_PROJECT);
					COMTHROW(CommitTransaction());
				} catch(hresult_exception &) {
					t->Flush();
					AbortTransaction();
				}
			}
		}
		COMTHROW(dataproject->SaveProject(newname, keepoldname));
		if(CComBSTR(newname).Length()) {
			if (!keepoldname) {
				projconn = newname;
				opened = UNCHANGED;
				transactioncount = 0;
			}

		} else {
			opened = UNCHANGED;
			transactioncount = 0;
		}
	}
	COMCATCH(;);
}

#undef mgaproject
*/

const Result_t MgaProject::GetRootFolder(MgaFolder* &rootFolder) throw()
{
	// TODO: Need to make sure we are in a transaction
	CoreObject rootObject;
	rootFolder = NULL;
	if( this->_coreProject->RootObject(rootObject) != S_OK ) return E_MGA_ROOTFCO;
	std::list<Uuid> objList;
	if( rootObject->GetAttributeValue(ATTRID_FPARENT + ATTRID_COLLECTION, objList) != S_OK ) return E_MGA_ROOTFCO;
	// No longer need rootObject - make sure to delete it
	rootObject = NULL;
	std::list<Uuid>::iterator objListIter = objList.begin();
	while (objListIter != objList.end())
	{
/*		if (objListIter->metaID == DTID_FOLDER)
		{
			// Get a coreObject for this item
			CoreObject coreObject;
			this->_coreProject->Object(*objListIter, coreObject);
			ASSERT( coreObject != NULL );
			// Create the MgaFolder object corresponding to this CoreObject
			rootFolder = new MgaFolder(this->_activeTerritory, this, coreObject);
			ASSERT( rootFolder != NULL );
			return S_OK;
		}
*/
		++objListIter;
	}
	// Did we did not find it
	return E_MGA_MODULE_INCOMPATIBILITY;
}

/*
STDMETHODIMP CMgaProject::get_RootMeta(IMgaMetaProject **pVal)
{
	COMTRY {
		if(metapr == 0) COMTHROW( E_MGA_PROJECT_NOT_OPEN );
		SetNmspaceInMeta();
		*pVal = metapr; (*pVal)->AddRef();
	} COMCATCH(;);
}

// ----------------------------------------
// Filter and search functions
// ----------------------------------------
HRESULT CMgaProject::CreateFilter(IMgaFilter **newfilter) {
	COMTRY {
		CHECK_OUTPTRPAR(newfilter);
		CComPtr<CMgaFilter> filter;
		CreateComObject(filter);
		filter->setproject(this);
		*newfilter = filter.Detach();
	} COMCATCH(;);
}

void recursefolders(CoreObj folder, CMgaFilter * filter, EXCOLLECTIONTYPE_FOR(MgaFCO) *q) {
	{
		ITERATE_THROUGH(folder[ATTRID_FCOPARENT+ATTRID_COLLECTION]) {
			if(ITER.IsFCO()) filter->searchFCOs(ITER, q);
			else recursefolders(ITER, filter, q);
		}
	}
}

STDMETHODIMP CMgaProject::AllFCOs(IMgaFilter *filter, IMgaFCOs ** fcos) {
	COMTRY {
		CHECK_OUTPTRPAR(fcos);
		CHECK_INPTRPAR(filter);   // project is checked in the following lines
		CComPtr<IMgaProject> p;
		COMTHROW(filter->get_Project(&p));
		if(!COM_EQUAL(p, (IMgaProject *)this)) COMTHROW(E_MGA_FOREIGN_PROJECT);

		CREATEEXCOLLECTION_FOR(MgaFCO, q);
		CComPtr<IMgaFolder> rootf;
		get_RootFolder(&rootf);
		recursefolders(CoreObj(rootf), reinterpret_cast<CMgaFilter *>(filter),  q);
		*fcos = q.Detach();
    }
    COMCATCH(;);
}

STDMETHODIMP CMgaProject::GetFCOByID(BSTR id, IMgaFCO **pVal) {
	COMTRY {
		CComPtr<IMgaObject> p;
		COMTHROW(GetObjectByID(id,&p));
		COMTHROW(p.QueryInterface(pVal));
    } COMCATCH(;);
}

STDMETHODIMP CMgaProject::GetObjectByID(BSTR id, IMgaObject **pVal)
{

	COMTRY {
		CHECK_INSTRPAR(id);
		CHECK_OUTPTRPAR(pVal);
		metaid_type mm; // short
		objid_type ss;  // long
		if( swscanf(id,OLESTR("id-%04hx-%08lx"), &mm, &ss) != 2 || 
			mm < DTID_MODEL || mm > DTID_FOLDER) COMTHROW(E_MGA_BAD_ID);
		CoreObj obj;
		COMTHROW(dataproject->get_Object(mm,ss,&obj.ComPtr()));
		if(obj) ObjForCore(obj)->getinterface(pVal);
		else COMTHROW(E_MGA_BAD_ID);
    } COMCATCH(;);
}

STDMETHODIMP CMgaProject::GetFCOsByName(BSTR name, IMgaFCOs **pVal)
{

	COMTRY {
// ADD CODE
//				(*pVal)->Open()
		COMTHROW(E_MGA_NOT_IMPLEMENTED);
    }
    COMCATCH(;);
}

STDMETHODIMP CMgaProject::GetFolderByPath(BSTR path, IMgaFolder **pVal)
{

	COMTRY {
// ADD CODE
//				(*pVal)->Open()
		COMTHROW(E_MGA_NOT_IMPLEMENTED);
    }
    COMCATCH(;);
}

// see FCO::get_ObjectByPath for details in MgaFolder.cpp 
STDMETHODIMP CMgaProject::get_ObjectByPath(BSTR path, IMgaObject **pVal) {
	COMTRY {
		CHECK_INSTRPAR(path);
		CHECK_OUTPTRPAR(pVal);
		CComPtr<IMgaFolder> f;
		COMTHROW(get_RootFolder(&f));
		COMTHROW(f->get_ObjectByPath(path, pVal));
	} COMCATCH(;);
}

STDMETHODIMP CMgaProject::get_NthObjectByPath(long n_th, BSTR path, IMgaObject **pVal) {
	COMTRY {
		CHECK_INSTRPAR(path);
		CHECK_OUTPTRPAR(pVal);
		CComPtr<IMgaFolder> f;
		COMTHROW(get_RootFolder(&f));
		COMTHROW(f->get_NthObjectByPath(n_th, path, pVal));
	} COMCATCH(;);
}

// ----------------------------------------
// Access project properties
// ----------------------------------------
STDMETHODIMP CMgaProject::GetStatistics(BSTR *statstr)
{

	COMTRY {
// ADD CODE
    }
    COMCATCH(;);
}



STDMETHODIMP CMgaProject::get_CreateTime(BSTR *pVal)
{

	COMTRY {
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		*pVal = CComBSTR(self[ATTRID_CDATE]).Detach();
	}
    COMCATCH(;);
}

STDMETHODIMP CMgaProject::get_ChangeTime(BSTR *pVal)
{

	COMTRY {
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		*pVal = CComBSTR(self[ATTRID_MDATE]).Detach();
    }
    COMCATCH(;);
}

STDMETHODIMP CMgaProject::get_Author(BSTR *pVal)
{

	COMTRY {
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		*pVal = CComBSTR(self[ATTRID_CREATOR]).Detach();
    }
    COMCATCH(;);
}

STDMETHODIMP CMgaProject::put_Author(BSTR newVal)
{

	COMTRY_IN_TRANSACTION {
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		self[ATTRID_CREATOR] = newVal;
		notifyqueueprocessed = true;
		COMTHROW(GlobalNotify(GLOBALEVENT_PROJECT_PROPERTIES));
    }
    COMCATCH_IN_TRANSACTION(;);
}

STDMETHODIMP CMgaProject::get_MetaGUID(VARIANT *pVal)
{

	COMTRY
	{
		CHECK_OUTPAR(pVal);
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		CComVariant p = self[ATTRID_PARGUID];

		if( p.vt != (VT_UI1 | VT_ARRAY) || GetArrayLength(p) != sizeof(GUID) )
		{
			GUID guid;
			memset(&guid, 0, sizeof(GUID));

			CopyTo(guid, p);
		}
		p.Detach(pVal);
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaProject::get_MetaName(BSTR *pVal)
{

	COMTRY
	{
		CHECK_OUTSTRPAR(pVal);
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		CComBSTR p = self[ATTRID_PARADIGM];

		*pVal=p.Detach();
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaProject::get_MetaVersion(BSTR *pVal)
{

	COMTRY
	{
		CHECK_OUTSTRPAR(pVal);
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		CComBSTR p = self[ATTRID_PARVERSION];

		*pVal=p.Detach();
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaProject::get_GUID(VARIANT *pVal)
{

	COMTRY
	{
		CHECK_OUTPAR(pVal);
		
		FixupGUID(false);

		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		CComVariant p = self[ATTRID_GUID];

		
		if (guidstat == PENDING) {
			p = pendingguid;
		}
		
		if( p.vt != (VT_UI1 | VT_ARRAY) || GetArrayLength(p) != sizeof(GUID) )
		{
			GUID guid;
			memset(&guid, 0, sizeof(GUID));

			CopyTo(guid, p);
		}
		p.Detach(pVal);
	}
	COMCATCH(;)
}


STDMETHODIMP CMgaProject::put_GUID(VARIANT newVal)
{
	COMTRY_IN_TRANSACTION {
		CHECK_INPAR(newVal);
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));

		if( newVal.vt != (VT_UI1 | VT_ARRAY) || GetArrayLength(newVal) != sizeof(GUID) )
			COMTHROW(E_INVALIDARG);

		self[ATTRID_GUID] = newVal;
		guidstat = MANUAL;
		notifyqueueprocessed = true;
		COMTHROW(GlobalNotify(GLOBALEVENT_PROJECT_PROPERTIES));
    }
    COMCATCH_IN_TRANSACTION(;);
}


STDMETHODIMP CMgaProject::get_Comment(BSTR *pVal)
{

	COMTRY {
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		*pVal = CComBSTR(self[ATTRID_EXTDATA]).Detach();
    }
    COMCATCH(;);
}

STDMETHODIMP CMgaProject::put_Comment(BSTR newVal)
{

	COMTRY_IN_TRANSACTION {
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		self[ATTRID_EXTDATA] = newVal;
		notifyqueueprocessed = true;
		COMTHROW(GlobalNotify(GLOBALEVENT_PROJECT_PROPERTIES));
    }
    COMCATCH_IN_TRANSACTION(;);
}

STDMETHODIMP CMgaProject::put_Name(BSTR newVal)
{

	COMTRY_IN_TRANSACTION {
		CHECK_INSTRPAR(newVal);
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		self[ATTRID_NAME] = newVal;
		notifyqueueprocessed = true;
		COMTHROW(GlobalNotify(GLOBALEVENT_PROJECT_PROPERTIES));
    }
    COMCATCH_IN_TRANSACTION(;);
}

STDMETHODIMP CMgaProject::put_Version(BSTR newVal)
{

	COMTRY_IN_TRANSACTION {
		CHECK_INSTRPAR(newVal);
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		self[ATTRID_VERSION] = newVal;
		notifyqueueprocessed = true;
		COMTHROW(GlobalNotify(GLOBALEVENT_PROJECT_PROPERTIES));
    }
    COMCATCH_IN_TRANSACTION(;);
}


STDMETHODIMP CMgaProject::get_Name(BSTR *pVal)
{

	COMTRY {
		CHECK_OUTSTRPAR(pVal);
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		CComBSTR s = self[ATTRID_NAME];
		*pVal = s.Detach();
    }
    COMCATCH(;);
}

STDMETHODIMP CMgaProject::get_Version(BSTR *pVal)
{

	COMTRY {
		CHECK_OUTSTRPAR(pVal);
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		CComBSTR s = self[ATTRID_VERSION];
		*pVal = s.Detach();
    }
    COMCATCH(;);
}


STDMETHODIMP CMgaProject::get_ProjectConnStr(BSTR *pVal)
{
	COMTRY {
		CHECK_OUTSTRPAR(pVal);
		CComBSTR s = projconn;
		*pVal = s.Detach();
    }
    COMCATCH(;);
}

STDMETHODIMP CMgaProject::get_ParadigmConnStr(BSTR *pVal)
{
	COMTRY {
		CHECK_OUTSTRPAR(pVal);
		CComBSTR s = parconn;
		*pVal = s.Detach();
    }
    COMCATCH(;);
}
*/

const Result_t MgaProject::CreateTerritory(IMgaEventSink *sink, MgaTerritory* &territory, IMgaEventSink *rwSink) throw()
{
	// Try to create a new territory
	Result_t result = MgaTerritory::CreateTerritory(this, sink, rwSink, territory);
	// Check for error conditions
	if (result != S_OK)
	{
		territory = NULL;
		return result;
	}
	// All is good, add territory to the list
	this->_allTerritories.push_front(territory);
	return S_OK;
}


const Result_t MgaProject::CreateAddOn(IMgaEventSink *handler, MgaAddOn* &addOn) throw()
{
	if (handler == NULL) return E_MGA_INPTR_NULL;
	// Checking for something
	if(!this->_reserveTerritory)
	{
		MgaTerritory* territory;
		Result_t result = this->CreateTerritory(NULL, territory);
		if (result != S_OK) return result;
		this->_reserveTerritory = territory;
	}
	// Create the new addOn and add it to the project list
	MgaAddOn* newAddOn = new MgaAddOn(this, handler);
	this->_addOnList.push_front(newAddOn);
	// Are addOns being set to automatic
	if(this->_inAutoAddOnCreate)
	{
		newAddOn->SetAutomatic();
	}
	addOn = newAddOn;
	return S_OK;
}

/*
STDMETHODIMP CMgaProject::get_AddOns(IMgaAddOns **addons) {
	COMTRY {
		CHECK_OUTPTRPAR(addons);
		CREATECOLLECTION_FOR(IMgaAddOn, q);
		for(addoncoll::iterator j = alladdons.begin(); j != alladdons.end(); ++j) {
			q->Add(*j);
		}
		*addons = q.Detach();
	} COMCATCH(;)
}

STDMETHODIMP CMgaProject::get_Clients(IMgaClients **clients) {
	COMTRY {
		CHECK_OUTPTRPAR(clients);
		CREATECOLLECTION_FOR(IMgaClient, q);
		for(clientcoll::iterator j = allclients.begin(); j != allclients.end(); ++j) {
			q->Add(*j);
		}
		*clients = q.Detach();
	} COMCATCH(;)
}

STDMETHODIMP CMgaProject::RegisterClient(BSTR name, IDispatch *OLEServer, IMgaClient **pVal) {
	COMTRY {
		CHECK_INPTRPAR(OLEServer);
		CHECK_INSTRPAR(name);

		CComPtr< CMgaClient > client;
		CreateComObject(client);
		client->active = true;
		client->mgaproject = this;
		client->ole_server = OLEServer;
		client->name = name;
		allclients.push_front(client);
		*pVal = client.Detach();
		// TODO register only if not registered
    }
    COMCATCH(;);
}

STDMETHODIMP CMgaProject::GetClientByName(BSTR name, IMgaClient **pVal) {
	COMTRY {
		CHECK_INSTRPAR(name);
		clientcoll::iterator j;
		for(j = allclients.begin(); j != allclients.end(); ++j) {
			CComPtr< CMgaClient > client(*j);
			if (client->name == name) {
				*pVal = client.Detach();
				break;
			}
		}
		if(j == allclients.end()) COMTHROW(E_MGA_NAME_NOT_FOUND);
	}
    COMCATCH(;);
}


STDMETHODIMP CMgaProject::get_Territories(IMgaTerritories **terrs) {
	COMTRY {
		CHECK_OUTPTRPAR(terrs);
		CREATECOLLECTION_FOR2(IMgaTerritory, IMgaTerritories, q);
		for(tercoll::iterator j = allterrs.begin(); j != allterrs.end(); ++j) {
			q->Add(*j);
		}
		*terrs = q.Detach();
	} COMCATCH(;)
}

STDMETHODIMP CMgaProject::get_AddOnComponents(IMgaComponents **comps) {
	COMTRY {
		CHECK_OUTPTRPAR(comps);
		CREATECOLLECTION_FOR(IMgaComponent, q);
		for(compcoll::iterator j = autocomps.begin(); j != autocomps.end(); ++j) {
			q->Add(*j);
		}
		*comps = q.Detach();
	} COMCATCH(;)
}

STDMETHODIMP CMgaProject::EnableAutoAddOns(VARIANT_BOOL bEnable) {
	COMTRY {
		if(baseterr) COMTHROW(E_MGA_ALREADY_IN_TRANSACTION);
		bool bnew = (bEnable == VARIANT_TRUE);
		if(bnew == autoaddons) return S_OK;
		autoaddons = bnew;
		if(opened != CLOSED) {
			if(autoaddons) StartAutoAddOns();
			else StopAutoAddOns();
		}
	} COMCATCH(;)
}
*/

void MgaProject::StopAutoAddOns(void)
{
//	while(!autocomps.empty())
//	{
//		CComPtr<IMgaComponent> addon;
//		addon.Attach(autocomps.front());
//		autocomps.pop_front();
//		addon.Release();
//	}
//	this->CoFreeUnusedLibraries();
}


void MgaProject::RemoveClients()
{
//	while(!allclients.empty())
//	{
//		CComPtr< CMgaClient > client;
//		client = allclients.front();
//		client->active = false;
//		allclients.pop_front();
//	}
}

/*
STDMETHODIMP CMgaProject::get_ProjectStatus(long *status) {
	COMTRY {
		CHECK_OUTPAR(status);
		if(opened == CLOSED) {
			*status  = 0;
		}	
		else if(opened == CLOSEERROR) {
			*status = 0x80000000;
		}
		else {
			*status = 1 + (opened == CHANGED ? 4 : 0) + (baseterr ? 8 : 0) + (read_only ? 16 : 0);
		}
	} COMCATCH(;);
}
*/

const Result_t MgaProject::BeginTransaction(MgaTerritory* territory, const bool &mode) throw()
{
	if(this->_baseTerritory != NULL) return E_MGA_ALREADY_IN_TRANSACTION;
	// Create a territory if one is needed
	if(territory == NULL)
	{
		ASSERT( this->CreateTerritory(NULL, territory) == S_OK );
	}
	// Otherwise, verify that the territory is valid for this project
	else
	{
		MgaProject* project = NULL;
		ASSERT( territory->Project(project) == S_OK );
		if( project != this) return E_MGA_FOREIGN_OBJECT;
	}
	ASSERT( territory != NULL );
//	read_only = (mode == TRANSACTION_READ_ONLY);
//	non_nestable = (mode == TRANSACTION_NON_NESTED);
	// BeginTransaction at the coreProject level
	ASSERT( this->_coreProject->BeginTransaction(mode) == S_OK );
//	checkofftemporary = false;
//	in_nested = false;
//	must_abort = false;
	// Set the base and active territories
	this->_baseTerritory = this->_activeTerritory = territory;
//	notifyqueueprocessed = false;
	return S_OK;
}


const Result_t MgaProject::CommitTransaction(void) throw()
{
//	ASSERT(!this->_inNested);
	if(!this->_baseTerritory) return E_MGA_NOT_IN_TRANSACTION;
//	if(checkoff) COMTHROW(CheckSupress(VARIANT_FALSE));
//	if(!notifyobjs.empty()) {
//		CoreObj self;
//		COMTHROW(dataproject->GetRootObject(&self.ComPtr()));
//		//			self[ATTRID_MDATE] = Now();
//	}
//	this->CommitNotify();
	short nestedCount;
	this->_coreProject->NestedTransactionCount(nestedCount);
	if (nestedCount == 1 && !this->_isReadOnly)
		this->GlobalNotify(GLOBALEVENT_COMMIT_TRANSACTION);
	ASSERT( this->_coreProject->CommitTransaction() == S_OK );
	this->_baseTerritory = this->_activeTerritory = NULL;
//	read_only = false;
//	if(notifyqueueprocessed) {
//		transactioncount++;
//		opened = CHANGED;
//		if (guidstat == MANUAL) {
//			guidstat = CLEAN;
//		}
//		else {
//			guidstat = DIRTY;
//		}
//	}
	return S_OK;
}


const Result_t MgaProject::AbortTransaction(void) throw()
{
/*
	ASSERT(!in_nested);
	if(!baseterr) COMTHROW(E_MGA_NOT_IN_TRANSACTION);
	aurcnt++;
	//COMTHROW(dataproject->PopTerritory()); BGY: see committransaction, it is already popped
	COMTHROW(dataproject->AbortTransaction(read_only ? TRANSTYPE_READFIRST:TRANSTYPE_FIRST));
	baseterr = activeterr= NULL;
	deferredchecks.clear();
	checkoff = false;
	while(!changedobjs.empty()) {
		changedobjs.front()->objforgetrwnotify();
		changedobjs.pop();
	}
	while(!notifyobjs.empty()) {
		notifyobjs.front()->objforgetnotify();
		notifyobjs.pop();
	}
	read_only = true;
	{
		CComPtr<IMgaTerritory> t;
		COMTHROW(CreateTerritory(NULL, &t));
		COMTHROW(BeginTransaction(t, TRANSACTION_READ_ONLY));
		GlobalNotify(GLOBALEVENT_ABORT_TRANSACTION);
		COMTHROW(CommitTransaction());
	}
	read_only = false;
*/
	return S_OK;
}

/*
STDMETHODIMP CMgaProject::CheckSupress(VARIANT_BOOL s) {
	COMTRY_IN_TRANSACTION {
		if(s) checkoff = true;
		else { 
			for(objhash::iterator i = deferredchecks.begin(); i != deferredchecks.end(); ++i) {
				if(GetMetaID((*i)->self) == DTID_FOLDER) COMTHROW((*i)->Check());
				else COMTHROW((*i)->CheckRCS());
			}
			deferredchecks.clear();
			checkoff = false;
		}
	} COMCATCH_IN_TRANSACTION(;);
}
*/

const Result_t MgaProject::GlobalNotify(const GlobalEvent_t &msg) throw()
{
	// Are there any add-ons?
	AddOnList::iterator addOnIter = this->_addOnList.begin();
	if (addOnIter != this->_addOnList.end())
	{
		//
//		this->PushTerritory(this->_reserveTerritory);
// The addon may Destroy() itself or release a reference while being notified
// But other addons are expected to remain in the list
		while(addOnIter != this->_addOnList.end())
		{
			// Set the iterator to the next addon (since it may be deleted)
			MgaAddOn* addOn = (*addOnIter++);
			if(msg == GLOBALEVENT_NOTIFICATION_READY && addOn->Notified() == false) continue;
			if(addOn->Handler()->GlobalEvent(msg) != S_OK)
			{
				ASSERT(("Global notification failed", false));
			}
		}
//		this->PopTerritory();
	}
	
//	tercoll::iterator i = allterrs.begin(), end = allterrs.end();
//	for(;i != end; ++i)
//	{
//		if(msg == GLOBALEVENT_NOTIFICATION_READY && (*i)->notified == false) continue;
//		else (*i)->notified = false;
//		if((*i)->handler) {
//			COMTHROW(pushterr(**i));
//			(*i)->handler->GlobalEvent(msg); // Silently ignore failures
//			COMTHROW(popterr());
//		}
//	}
	return S_OK;
}


/*
STDMETHODIMP CMgaProject::Notify(globalevent_enum event) {
	COMTRY {
		if(baseterr) COMTHROW(E_MGA_ALREADY_IN_TRANSACTION);
		{
			CComPtr<IMgaTerritory> t;
			COMTHROW(CreateTerritory(NULL, &t));
			COMTHROW(BeginTransaction(t, TRANSACTION_READ_ONLY));
			GlobalNotify(event);
			COMTHROW(CommitTransaction());
		}
    }
    COMCATCH(;);
}


STDMETHODIMP CMgaProject::CommitNotify() {
  if(read_only) {
                ASSERT(changedobjs.empty() && notifyobjs.empty());
                return S_OK;
  }
  COMTRY {
                if(!baseterr) COMTHROW(E_MGA_NOT_IN_TRANSACTION);

                if(!changedobjs.empty()) notifyqueueprocessed = true;
                while(!changedobjs.empty()) {
                                FCOPtr f = changedobjs.front();
                                changedobjs.pop();

#if(1)
                                COMTHROW(f->objrwnotify());
#else
                                if(f->objrwnotify() != S_OK) {
                                        ASSERT(("Notification failed", false));
                                }
#endif
                }

                read_only = true;
                while(!notifyobjs.empty()) {
                                FCOPtr f = notifyobjs.front();
                                notifyobjs.pop();
                                if(f->objnotify() != S_OK) {
                                        ASSERT(("Notification failed", false));
                                }
                }
                COMTHROW(GlobalNotify(GLOBALEVENT_NOTIFICATION_READY));
                read_only = false;
  } COMCATCH( read_only = false;);
}



bool CMgaProject::alreadynested() { return in_nested; }

HRESULT CMgaProject::beginnested() {
	ASSERT(!in_nested);

	ASSERT(objstocheck.empty());
	objstocheck.clear();		

	HRESULT hr;
	if(non_nestable) hr = must_abort ? E_MGA_MUST_ABORT : S_OK;
	else hr = dataproject->BeginTransaction(TRANSTYPE_NESTED);

    MARKSIG('4');
	if(hr == 0) in_nested = true;
	return hr;
}

HRESULT CMgaProject::commitnested() {
	ASSERT(in_nested);

	ASSERT(objstocheck.empty());
	objstocheck.clear();

	HRESULT hr = S_OK;
	if(!non_nestable) hr = dataproject->CommitTransaction(TRANSTYPE_NESTED);
    MARKSIG('6');
	if(hr != S_OK) 	abortnested();
	else { 
		in_nested = false;
		while(!temporalobjs.empty()) {
			temporalobjs.front()->objrecordchange();
			temporalobjs.pop();
		}
	}
	return hr;
}

HRESULT CMgaProject::abortnested() {
	ASSERT(in_nested);
	objstocheck.clear();
	in_nested = false;
	while(!temporalobjs.empty()) {
			temporalobjs.front()->objforgetchange();
			temporalobjs.pop();
	}
    MARKSIG('5');
	must_abort = true;
	return dataproject->AbortTransaction(TRANSTYPE_NESTED);
}


HRESULT CMgaProject::pushterr(CMgaTerritory &ter) {
	COMTRY {
		ASSERT(("Territorys overwrite each other",activeterr==baseterr));
		activeterr = &ter;
		COMTHROW(dataproject->PushTerritory(ter.coreterr));
	} COMCATCH(;);
}

HRESULT CMgaProject::popterr() {
	COMTRY {
		activeterr= baseterr;
		COMTHROW(dataproject->PopTerritory());
	} COMCATCH(;);
}



STDMETHODIMP CMgaProject::Undo() {
	COMTRY {
		if(baseterr) COMTHROW(E_MGA_ALREADY_IN_TRANSACTION);
		COMTHROW(dataproject->UndoTransaction());
		if(!--transactioncount) {
			opened = UNCHANGED;
			guidstat = CLEAN;
		}
		{
			aurcnt++;
			CComPtr<IMgaTerritory> t;
			COMTHROW(CreateTerritory(NULL, &t));
			COMTHROW(BeginTransaction(t, TRANSACTION_READ_ONLY));
			GlobalNotify(GLOBALEVENT_UNDO);
			COMTHROW(CommitTransaction());
		}
    }
    COMCATCH(;);
}

STDMETHODIMP CMgaProject::Redo() {

	COMTRY {
		if(baseterr) COMTHROW(E_MGA_ALREADY_IN_TRANSACTION);
		COMTHROW(dataproject->RedoTransaction());
		transactioncount++;
		opened = CHANGED;
		guidstat = DIRTY;
		{
			aurcnt++;
			CComPtr<IMgaTerritory> t;
			COMTHROW(CreateTerritory(NULL, &t));
			COMTHROW(BeginTransaction(t, TRANSACTION_READ_ONLY));
			GlobalNotify(GLOBALEVENT_REDO);
			COMTHROW(CommitTransaction());
		}
    }
    COMCATCH(;);
}

STDMETHODIMP CMgaProject::UndoRedoSize(short *undosize, short *redosize ) {
	COMTRY {
		COMTHROW(dataproject->get_UndoQueueSize(undosize));
		COMTHROW(dataproject->get_RedoQueueSize(redosize));
    }
    COMCATCH(;);
}


STDMETHODIMP CMgaProject::FlushUndoQueue() {

	if( dataproject == NULL )
		COMRETURN(E_FAIL);

	COMTRY {
		if(baseterr) COMTHROW(E_MGA_ALREADY_IN_TRANSACTION);
		COMTHROW( dataproject->FlushUndoQueue() );
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaProject::UpdateSourceControlInfo( BSTR param)
{
	CComPtr<IMgaTerritory> lm;
	COMTRY {
		COMTHROW(CreateTerritory(NULL, &lm));
		COMTHROW(BeginTransaction(lm, TRANSACTION_GENERAL));

		// hack: core will recognize this action as a source control status update
		CComBSTR hack_str = "UpdateSourceControlInfo";
		CComBSTR para_str( param);
		if( para_str.Length() > 0)
			hack_str.Append( para_str);
		CoreObj  dataroot;
		COMTHROW(dataproject->get_RootObject(&dataroot.ComPtr()));
		dataroot[ATTRID_MDATE] = hack_str;

		COMTHROW(CommitTransaction());
	}
	COMCATCH(;)
}

STDMETHODIMP CMgaProject::SourceControlActiveUsers()
{
	CComPtr<IMgaTerritory> lm;
	COMTRY {
		COMTHROW(CreateTerritory(NULL, &lm));
		COMTHROW(BeginTransaction(lm, TRANSACTION_GENERAL));

		// hack: core will recognize this value as a command, will show a dlg with user names
		CComBSTR hack_str = "ShowActiveUsers";

		CoreObj  dataroot;
		COMTHROW(dataproject->get_RootObject(&dataroot.ComPtr()));
		dataroot[ATTRID_MDATE] = hack_str;

		COMTHROW(CommitTransaction());

	}
	COMCATCH(;)
}

STDMETHODIMP CMgaProject::SourceControlObjectOwner( BSTR p_optionalID)
{
	CComPtr<IMgaTerritory> lm;
	COMTRY {
		COMTHROW(CreateTerritory(NULL, &lm));
		COMTHROW(BeginTransaction(lm, TRANSACTION_GENERAL));

		// hack: core will recognize this value as a command, will show a dlg with owneruser
		CComBSTR hack_str = "WhoControlsThisObj";
		CComBSTR para_str( p_optionalID);
		if( para_str.Length() > 0)
			hack_str.Append( para_str);
		CoreObj  dataroot;
		COMTHROW(dataproject->get_RootObject(&dataroot.ComPtr()));
		dataroot[ATTRID_MDATE] = hack_str;

		COMTHROW(CommitTransaction());
	}
	COMCATCH(;)
}

void CMgaProject::ObjMark(IMgaObject *s, long mask) {
	ObjFor(s)->SelfMark(mask);
}

void CMgaProject::FixupGUID(bool write) {
	if (guidstat == DIRTY) {
		GUID newGUID;
		COMTHROW(CoCreateGuid(&newGUID));
		CopyTo(newGUID, pendingguid);
		guidstat = PENDING;
	}
	if ((guidstat == PENDING) && write) {
		ASSERT(baseterr);
		CoreObj self;
		COMTHROW(dataproject->get_RootObject(&self.ComPtr()));
		self[ATTRID_GUID] = pendingguid;
		guidstat = CLEAN;
	}
}

void CMgaProject::UpdateMGAVersion(CoreObj& p_dataroot)
{
	mgaversion = p_dataroot[ATTRID_MGAVERSION];
	if( mgaversion <= 1L)
	{
		// update done by CreateCoreMetaProject so update the ATTRID_MGAVERSION too
		mgaversion = p_dataroot[ATTRID_MGAVERSION] = 2L;
	}
}
*/

void MgaProject::SetNamespaceInMeta(void)
{
	ASSERT( this->_metaProject != NULL );
//	this->_metaProject->SetNamespace( this->GetNamespace() );
}


std::string MgaProject::GetNamespace(void)
{
	std::string name = "";
	MgaTerritory* territory;
	this->ActiveTerritory(territory);
	if(territory != NULL)
		territory->GetNamespace(name);
	return name;
}


std::string MgaProject::PrefixWithNamespace(const std::string &kindName)
{
	std::string name = this->GetNamespace();
	if( name.length() > 0)// or if not found('::')
	{
//		kindname_m.AppendBSTR( nm);
//		kindname_m.Append( "::");
	}
//	kindname_m.AppendBSTR( pKindname);
	return name;
}

/*
STDMETHODIMP CMgaProject::CheckCollection(IMgaFCOs *coll) {
	COMTRY {
		MGACOLL_ITERATE(IMgaFCO, coll) {
			HRESULT s;
			if((s = MGACOLL_ITER->CheckProject(this)) != S_OK) return s;
		}
		MGACOLL_ITERATE_END;
	}
	COMCATCH(;)
}

// by ZolMol
STDMETHODIMP CMgaProject::CheckFolderCollection(IMgaFolders *coll) {
	COMTRY {
		MGACOLL_ITERATE(IMgaFolder, coll) {
			HRESULT s;
			if((s = MGACOLL_ITER->CheckProject(this)) != S_OK) return s;
		}
		MGACOLL_ITERATE_END;
	}
	COMCATCH(;)
}


// ----------------------------------------
// OBSOLETE stuff: Clearlocks
// ----------------------------------------

#include <set>
#include "../core/CoreUtilities.h"

bool std::less<metaobjidpair_type>::operator ()(const metaobjidpair_type &a,const metaobjidpair_type &b) const {
	if(a.metaid == b.metaid) return a.objid < b.objid;
	return a.metaid < b.metaid;
}

void ClearLocks(ICoreStorage * storage, std::set<metaobjidpair_type> &mset, short mi, long oi, bool clear) {
	
	metaobjidpair_type mm; 
	mm.metaid = mi, mm.objid = oi;

	if(mset.find(mm) != mset.end()) return;
	mset.insert(mm);
	CComPtr<ICoreMetaProject> mp;
	COMTHROW(storage->get_MetaProject(&mp));
	CComPtr<ICoreMetaObject> mo;
	COMTHROW(mp->get_Object(mi, &mo));
	CComPtr<ICoreMetaAttributes> atts;
	COMTHROW(mo->get_Attributes(&atts));
	long count = 0;								
	COMTHROW( atts->get_Count(&count) );		
	CComPtr<ICoreMetaAttribute> *arrptr, *arrend, *array = new CComPtr<ICoreMetaAttribute>[count]; 
	if(count > 0) { 
		COMTHROW( atts->GetAll((unsigned long)count, &(*array)) ); 
	} 
	arrend = array+count; 
	for(arrptr = array; arrptr != arrend; arrptr++)  {
		unsigned char t;
		COMTHROW((*arrptr)->get_ValueType(&t)); 
		switch(t) {
		case VALTYPE_LOCK:
				COMTHROW(storage->put_MetaID(mi));
				COMTHROW(storage->OpenObject(oi));
				COMTHROW(storage->put_MetaAttribute((*arrptr)));
				{
				CComVariant vv;
				COMTHROW(storage->get_AttributeValue(&vv));
				ASSERT(vv.vt == VT_I2);
				if(vv.iVal) {
					COMTHROW(storage->put_AttributeValue(CComVariant(0)));
				}
				}
				break;
		case VALTYPE_COLLECTION:
				COMTHROW(storage->put_MetaID(mi));
				COMTHROW(storage->OpenObject(oi));
				COMTHROW(storage->put_MetaAttribute((*arrptr)));
				{
					CComVariant v;
					COMTHROW(storage->get_AttributeValue(&v));
					metaobjidpair_type *i = NULL;
					metaobjidpair_type *e = NULL;
					GetArrayBounds(v, i, e);
					while( i != e )	{
		 				ClearLocks(storage, mset, (*i).metaid, (*i).objid, clear);
						i++;
					}
				}
		}
	}
	delete []array;
}



STDMETHODIMP CMgaProject::CheckLocks(BSTR filename, VARIANT_BOOL clearlocks) {
  CComPtr<ICoreStorage> storage;
  CComPtr<ICoreMetaProject> genericproject;
  COMTRY {
	CreateCoreMetaProject(genericproject); // use mgaversion = 1 project model
	COMTHROW(storage.CoCreateInstance(	OLESTR("MGA.CoreRepository")));
	COMTHROW(storage->put_MetaProject(genericproject));
	COMTHROW(storage->OpenProject(filename, NULL));
	COMTHROW(storage->BeginTransaction());
	std::set<metaobjidpair_type> ccc;
	ClearLocks(storage, ccc, 1, 1, clearlocks ? true : false);
	COMTHROW(storage->CommitTransaction());
  } COMCATCH(;)
}

int CMgaProject::getMaxUndoSize()
{
	int retval = 10; // keep in sync with the default value in CoreProject.h
	try
	{
		CComPtr<IMgaRegistrar> mgareg;
		COMTHROW(mgareg.CoCreateInstance(OLESTR("MGA.MgaRegistrar")));
		CComBSTR undo_size;
		COMTHROW( mgareg->GetUndoQueueSize( REGACCESS_USER, &undo_size));
		TCHAR p[3] = { 0, 0, 0}; // max = 99
		unsigned int len = (undo_size)? undo_size.Length(): 0;
		if( len != 0 && len <= sizeof( p) - 1)
		{
			CopyTo( undo_size, len, p, len); // strcpy
			int val = _tstoi( p); // atoi
			if( val > 0 && val < 100) // requirement is to be above 0 and below 100
				retval = val;
		}
	}
	catch( hresult_exception&)
	{
		retval = 10;
	}

	return retval;
}

STDMETHODIMP CMgaProject::GetTopLibraries(BSTR pDispGuid, IMgaFolders **pVal)
{
	COMTRY {
		CHECK_INSTRPAR( pDispGuid);
		CREATECOLLECTION_FOR(IMgaFolder,q);

		CComPtr<IMgaFolder> rootf;
		COMTHROW( get_RootFolder( &rootf));
		CoreObj crf( rootf);
		CoreObjs children = crf[ATTRID_FPARENT+ATTRID_COLLECTION];
		ITERATE_THROUGH(children) {
			if( ITER.GetMetaID() != DTID_FOLDER) continue;
			long perm = ITER[ATTRID_PERMISSIONS];
			if( perm & LIBROOT_FLAG)
			{
				FCO *lib = ObjForCore( ITER);
				if( lib) 
				{
					CComBSTR gd;
					COMTHROW( lib->GetGuidDisp( &gd));

					if( gd == pDispGuid)
					{
						CComPtr<IMgaFolder> fld;
						lib->getinterface( &fld);
						q->Add( fld);
					}
				}
			}
		}
		*pVal = q.Detach();
	}
    COMCATCH(;);
}
*/

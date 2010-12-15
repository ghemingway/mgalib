#ifndef __MGA_PROJECT_H__
#define __MGA_PROJECT_H__


/*** Included Header Files ***/
#include "MgaTypes.h"
#include "MgaEventSink.h"


/*** Class Predefinitions ***/
class CoreProject;
class MgaRegistrar;
class MgaTerritory;
class MgaAddOn;
class MgaComponent;
class MgaClient;
class MetaProject;
class MgaFCOBase;
class MgaFolder;
class MetaBase;


typedef std::list<MgaTerritory*>	TerritoryList;	//!< elements do not hold a reference!!
typedef std::list<MgaAddOn*>		AddOnList;		//!< elements do not hold a reference!!
typedef std::list<MgaComponent*>	ComponentList;	//!< this list holds a reference!!!!
typedef std::list<MgaClient*>		ClientList;		//!< elements do not hold a reference!!
//typedef std::queue<FCOPtr>		FCOQueue;		//!<
//typedef stdext::hash_set<FCOPtr, FCOPtr_hashfunc> objhash;
//enum guidmodes { CLEAN, DIRTY, PENDING, MANUAL};


// --------------------------- MgaProject --------------------------- //


class MgaProject
{
private:
	CoreProject			*_coreProject;
	MetaProject			*_metaProject;
	int					_mgaVersion;
	long				_preferences;
	unsigned long		_opMask;
	bool				_isReadOnly;
	AddOnList			_addOnList;
	ClientList			_clientList;
	ComponentList		_autoComponents;
	bool				_autoAddOns;								//!< automatic addon instantiation enabled
	bool				_inAutoAddOnCreate;							//!< Addons are just being instantiated
	MgaRegistrar		*_registrar;
	TerritoryList		_allTerritories;
	MgaTerritory		*_baseTerritory, *_activeTerritory, *_reserveTerritory;
/*
	objcoll				changedobjs, notifyobjs, temporalobjs;
	objhash				deferredchecks;
	bool				_checkoff;
	bool				_checkoffTemporary;
	bool				_inNested;
	bool				_isNonNestable;
	bool				_mustAbort;
	int					_transactionCount;
	bool				_notifyQueueProcessed;
	int					_aurCount;   // Abort/undo/redo count
	coreobjhash			_objstocheck;	
	std::string			_paradigmConnection;
	CComVariant			_pendingguid;

	friend class Transaction;
	bool alreadynested();
	int getMaxUndoSize();
	HRESULT beginnested();
	HRESULT abortnested();
	HRESULT commitnested(); 
	void ObjMark(IMgaObject *s, long mask);
	void FixupGUID(bool write = true);
	void UpdateMGAVersion(CoreObj& p_dataroot);
	HRESULT pushterr(CMgaTerritory &terr);
	HRESULT popterr();
*/

	MgaProject();
	MgaProject(CoreProject *coreProject, MetaProject *metaProject, const bool &readOnly);

	static const Result_t OpenParadigm(MgaRegistrar* &registrar, const std::string &paradigm, const std::string &guid, MetaProject* &metaProject);
	void StartAutoAddOns(void);												// start if enabled
	void StopAutoAddOns(void);												// stop them always
	void RemoveClients(void);
	void SetNamespaceInMeta(void);

	friend class MgaFCOBase;
	std::string GetNamespace(void);
	std::string PrefixWithNamespace(const std::string &kindName);

public:
	~MgaProject();

	static const Result_t Create(const std::string &projectName, const std::string &paradigmName,
								 const GUID_t &paradigmGUID, MgaRegistrar* &registrar, MgaProject* &mgaProject) throw();
	static const Result_t Open(const std::string &projectName, MgaRegistrar* &registrar, const bool &readOnly, MgaProject* &project) throw();
/*
	STDMETHOD(OpenEx)(BSTR projectname, BSTR paradigmname, VARIANT paradigmGUID);
	STDMETHOD(Save)(BSTR name = NULL, VARIANT_BOOL keepoldname = VARIANT_FALSE);
*/
	inline const Result_t GetPreferences(long &prefMask) const throw()	{ prefMask = this->_preferences; return S_OK; }
	inline const Result_t SetPreferences(const long &prefMask) throw()	{ this->_preferences = prefMask; return S_OK; }
	inline const Result_t GetOperationsMask(long &mask) const throw()	{ mask = this->_opMask; return S_OK; }
	inline const Result_t SetOperationsMask(const long &mask) throw()	{ this->_opMask = mask; return S_OK; }

	const Result_t CreateTerritory(IMgaEventSink *handler, MgaTerritory* &territory, IMgaEventSink *handler2=NULL) throw();
	const Result_t ActiveTerritory(MgaTerritory* &territory) throw()	{ territory = this->_baseTerritory;	return S_OK; }
	const Result_t CreateAddOn(IMgaEventSink* handler, MgaAddOn* &addOn) throw();
	const Result_t GetMetaObj(const MetaRef_t &metaRef, MetaBase* &metaBase);
/*
	STDMETHOD(CheckLocks)(BSTR filename, VARIANT_BOOL clearlocks);
	STDMETHOD(get_Territories)(IMgaTerritories **terrs);
	STDMETHOD(get_AddOns)(IMgaAddOns **addons);
	STDMETHOD(get_AddOnComponents)(IMgaComponents **comps);
	STDMETHOD(EnableAutoAddOns)(VARIANT_BOOL bEnable);
*/
	const Result_t BeginTransaction(MgaTerritory* territory, const bool &mode) throw();
	const Result_t CommitTransaction(void) throw();
	const Result_t AbortTransaction(void) throw();
/*
	STDMETHOD(get_ProjectStatus)(long *stat);
*/

/*
	STDMETHOD(CommitNotify)();
	STDMETHOD(CheckSupress)(VARIANT_BOOL);
*/
	const Result_t GlobalNotify(const GlobalEvent_t &msg) throw();
/*
	STDMETHOD(Notify)(globalevent_enum event);
	STDMETHOD(Redo)();
	STDMETHOD(Undo)();
	STDMETHOD(UndoRedoSize)(short *undosize, short *redosize);
	STDMETHOD(FlushUndoQueue)();
	STDMETHOD(UpdateSourceControlInfo)( BSTR param);
	STDMETHOD(SourceControlActiveUsers)( );
	STDMETHOD(SourceControlObjectOwner)( BSTR p_optionalID);
*/
	const Result_t GetRootFolder(MgaFolder* &rootFolder) throw();
/*
	STDMETHOD(get_RootMeta)(IMgaMetaProject **pVal);
	STDMETHOD(GetStatistics)(BSTR *statstr);
 	STDMETHOD(get_ObjectByPath)(BSTR path, IMgaObject **pVal);
 	STDMETHOD(get_NthObjectByPath)(long n_th, BSTR path, IMgaObject **pVal);
	STDMETHOD(GetFolderByPath)(BSTR path, IMgaFolder **pVal);
	STDMETHOD(GetFCOsByName)(BSTR name, IMgaFCOs **pVal);
	STDMETHOD(GetFCOByID)(BSTR id, IMgaFCO **pVal);
	STDMETHOD(GetObjectByID)(BSTR id, IMgaObject **pVal);
	STDMETHOD(EnumExtReferences)(IMgaFCOs *fcos, IMgaFCOs **pVal);
    STDMETHOD(CreateFilter)(IMgaFilter **);
	STDMETHOD(AllFCOs)(IMgaFilter *, IMgaFCOs **pVal);
*/
	const Result_t GetName(std::string &name) const throw();
	const Result_t PutName(const std::string &name) throw();
	const Result_t GetVersion(std::string &version) const throw();
	const Result_t PutVersion(const std::string &version) throw();
	const Result_t GetComment(std::string &comment) const throw();
	const Result_t PutComment(const std::string &comment) throw();
	const Result_t GetAuthor(std::string &author) const throw();
	const Result_t PutAuthor(const std::string &author) throw();
	const Result_t GetChangeTime(std::string &changeTime) const throw();
	const Result_t GetCreateTime(std::string &createTime) const throw();
/*
	STDMETHOD(get_MetaGUID)(VARIANT *pVal);
	STDMETHOD(get_MetaVersion)(std::string *pVal);
	STDMETHOD(get_MetaName)(std::string *pVal);
	STDMETHOD(get_GUID)(VARIANT *pVal);
	STDMETHOD(put_GUID)(VARIANT newVal);
	STDMETHOD(get_ProjectConnStr)(std::string *pVal);
	STDMETHOD(get_ParadigmConnStr)(std::string *pVal);
	STDMETHOD(QueryProjectInfo)(std::string projectname, long *mgaversion, 
										   std::string *paradigmname, 
										   std::string *parversion,
										   VARIANT *paradigmGUID,
										   VARIANT_BOOL *ro_mode);
	STDMETHOD(CheckCollection)(IMgaFCOs *coll);
	STDMETHOD(CheckFolderCollection)(IMgaFolders *coll);
	STDMETHOD(get_Clients)(IMgaClients **clients);
	STDMETHOD(RegisterClient)(std::string name, IDispatch *OLEServer, IMgaClient **client);
	STDMETHOD(GetClientByName)(std::string name, IMgaClient **client);
	STDMETHOD(GetTopLibraries)(std::string name, IMgaFolders **res);
*/
};


// --------------------------- MgaProject --------------------------- //


class MgaClient
{
private:
	std::string				_name;
//	CComPtr<IDispatch>		ole_server;
	MgaProject				*_mgaProject;
	bool					_isActive;

public:
	MgaClient() { }
	~MgaClient() {
//		if(this->_isActive) this->Destroy();
	}

	inline const Result_t Name(std::string &name) const throw()			{ name = this->_name; return S_OK; }
/*	STDMETHOD(get_OLEServer)( IDispatch **pServer ) {
		COMTRY {
			CHECK_OUTPTRPAR(pServer);
			*pServer = ole_server;
			(*pServer)->AddRef();
		} COMCATCH(;);
	}*/
	inline const Result_t Project(MgaProject* &project) const throw()	{ project = this->_mgaProject; return S_OK; }
/*	STDMETHOD(Destroy)() {
		COMTRY {
			CMgaProject::clientcoll::iterator i = mgaproject->allclients.begin(), 
											  end = mgaproject->allclients.end();
			for(;i != end; ++i) {
				if(*i == this) {
					mgaproject->allclients.erase(i);
					active = false;
					return S_OK;
				}
			}
		    ASSERT(("Client was not found among project clients",false));	
		} COMCATCH(;);
	}*/
};


#endif //__MGA_PROJECT_H__


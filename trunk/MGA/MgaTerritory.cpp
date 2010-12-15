/*** Included Header Files ***/
#include "MgaTerritory.h"
#include "MgaProject.h"


// --------------------------- MgaTerritory --------------------------- //


const Result_t MgaTerritory::CreateTerritory(MgaProject* project, IMgaEventSink* handler,
									  IMgaEventSink *rwHandler, MgaTerritory* &territory) throw()
{
	// Make sure inputs are valid
	if ( project == NULL ) return E_MGA_INVALID_ARG;
	// TODO: Validate that a NULL handler is OK
//	if ( handler == NULL ) return E_MGA_INVALID_ARG;
	// Allocate the new territory and we are off
	territory = new MgaTerritory(project, handler, rwHandler);
	ASSERT( territory != NULL );
	return S_OK;
}


/*
const Result_t MgaTerritory::Associate(MgaObject* &object, VARIANT userdata)
{ 
	if(!coreterr) COMTHROW( E_MGA_TARGET_DESTROYED);
			ObjFor(obj)->objsetuserdata(this, userdata);
}

 
STDMETHODIMP CMgaTerritory::OpenObj(MgaObject* &oldptr, MgaObject* &newptr)
{ 
	COMTRY {
		CHECK_MYINPTRPAR(oldptr); 
		CHECK_OUTPAR(newptr); 
		CComPtr<IMgaObject> np;
		if(!coreterr) COMTHROW(E_MGA_TARGET_DESTROYED);
		ObjFor(oldptr)->getinterface(&np,this);
		if(*newptr) (*newptr)->Release();
		*newptr = np.Detach();
	} COMCATCH(;) 
}


const Result_t MgaTerritory::OpenFCO(MgaFCO* &oldptr, MgaFCO* &newptr) throw()
{ 
	CComPtr<IMgaFCO> np;
	ObjFor(oldptr)->getinterface(&np,this);
	if(*newptr) (*newptr)->Release();
	*newptr = np.Detach();
	return S_OK;
}


STDMETHODIMP CMgaTerritory::OpenFCOs(std::list<MgaFCO*> &oldList, std::list<MgaFCO*> &newList)
{ 
	CREATEEXCOLLECTION_FOR(MgaFCO, q);
	if(!coreterr) COMTHROW(E_MGA_TARGET_DESTROYED);
		MGACOLL_ITERATE(IMgaFCO, oldList) {
			CComPtr<IMgaFCO> bb;
		    ObjFor(MGACOLL_ITER)->getinterface(&bb);
			q->Add(bb);
		}
		MGACOLL_ITERATE_END;
		if(*newList) (*newList)->Release();
		*newList = q.Detach();
	} COMCATCH(;) 
}


STDMETHODIMP CMgaTerritory::CloseObj(MgaObject *obj)
{
	COMTRY {
		CHECK_MYINPTRPAR(obj); 
		if(!coreterr) COMTHROW(E_MGA_TARGET_DESTROYED);
		COMTHROW(obj->Close());
	} COMCATCH(;) 
}
*/

const Result_t MgaTerritory::Flush(void) throw()
{
/*
	bool trwas = false;
	bool pushwas = false;
*/
/*
	if(!coreterr) COMTHROW(E_MGA_TARGET_DESTROYED);
	MGA_TRACE("Terr flush. Lock count: %d\n", refcnt);
	objsetcoll::iterator f;
	while((f =inobjs.begin()) != inobjs.end()) (*f)->removeterrfromlist(this);
	ASSERT(("Territory is still locked by some objects", refcnt == 0));
	COMTHROW(coreterr->Clear());
*/
	return S_OK;
}

/*
const Result_t MgaTerritory::Destroy(void)
{
	COMTRY {
		if(!coreterr) COMTHROW(E_MGA_TARGET_DESTROYED);
		COMTHROW(Flush());
		CMgaProject::tercoll::iterator i = mgaproject->allterrs.begin(), end = mgaproject->allterrs.end();
		for(;i != end; ++i) {
			if(*i == this) {
				mgaproject->allterrs.erase(i);
				coreterr = NULL;   // release CoreTerritory object
				handler = NULL;	// release sink
				rwhandler = NULL;	// release sink
				return S_OK;
			}
			
		}
		ASSERT(false);	// not found among project territories
	} COMCATCH(;)
}
*/

const Result_t MgaTerritory::SetNamespace(const std::string &name) throw()
{
//	m_bstrCurrNamespace = pNmsp;
//	this->_mgaProject->SetNamespaceInMeta(); // set into the metaproject if open
	return S_OK;
}


// --------------------------- MgaAddOn --------------------------- //


const Result_t MgaAddOn::CheckProject(MgaProject *project) {
	return (project == this->_mgaProject? S_OK : E_MGA_FOREIGN_PROJECT);
}


const Result_t MgaAddOn::Destroy(void) {
	if(!this->_handler) return E_MGA_TARGET_DESTROYED;
//	AddOnList::iterator addOnIter = this->_mgaProject->_alladdons.begin();
//	for(; addOnIter != end; ++i)
//	{
//		if(*addOnIter == this)
//		{
//			this->_mgaProject->alladdons.erase(i);
//			// Release
//			if(this->_mgaProject->alladdons.empty()) this->_mgaProject->reserveterr = NULL; 
//			// Release sink
//			this->_handler = NULL;
//			return S_OK;
//		}
//	}
//	ASSERT(("addon not found among project addons",false));	
	this->_active = false;
	return S_OK;
}


const Result_t MgaAddOn::GetPriority(long &priority)		{ return E_MGA_NOT_IMPLEMENTED; }
const Result_t MgaAddOn::SetPriority(const long &priority)	{ return E_MGA_NOT_IMPLEMENTED; }


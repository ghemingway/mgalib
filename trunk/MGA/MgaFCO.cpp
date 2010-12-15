/*** Included Header Files ***/
#include "MgaFCO.h"
#include "../Core/CoreProject.h"
#include "../Core/CoreObject.h"
#include "MgaFolder.h"
#include "MgaGeneric.h"
#include "MgaAttribute.h"
#include "MgaProject.h"
#include "../Meta/MetaFCO.h"
#include "../Meta/MetaAttribute.h"
//#include "MgaModel.h"
//#include "MgaConnection.h"
//#include "MgaReference.h"
//#include "MgaSet.h"


// --------------------------- MgaFCOBase --------------------------- //


MgaFCOBase::MgaFCOBase(MgaProject* &project, CoreObject* &coreObject) :
	_mgaProject(project), _coreObject(coreObject)
{ 
//	notifymask = changemask = temporalmask = 0;
}


MgaFCOBase::~MgaFCOBase()
{ 
//	ASSERT(pubfcos.begin() == pubfcos.end());
	if (this->_coreObject != NULL) delete this->_coreObject;
}

/*
// return an IMgaObject for this FCO. By default, use the active territory to create it
template <>
void FCO::getinterface<IMgaObject>(IMgaObject **p, CMgaTerritory *terr) {
	ASSERT(mgaproject->activeterr);
	if(terr == NULL) terr = mgaproject->activeterr;
	pubfcohash::iterator ii = pubfcos.find(terr);
	if(ii != pubfcos.end())  ((*p) = (*ii).second)->AddRef();
	else {
		switch(GetMetaID(self)) {
		case DTID_MODEL:		{ CComPtr< CMgaModel > np; CreateComObject(np); np->Initialize(terr, this); *p = np.Detach(); } break;
		case DTID_ATOM:			{ CComPtr< CMgaAtom > np; CreateComObject(np); np->Initialize(terr, this); *p = np.Detach(); } break;
		case DTID_REFERENCE:	{ CComPtr< CMgaReference > np; CreateComObject(np); np->Initialize(terr, this); *p = np.Detach(); } break;
		case DTID_CONNECTION:	{ CComPtr< CMgaConnection > np; CreateComObject(np); np->Initialize(terr, this); *p = np.Detach(); } break;
		case DTID_SET:			{ CComPtr< CMgaSet > np; CreateComObject(np); np->Initialize(terr, this); *p = np.Detach(); } break;
		case DTID_FOLDER:		{ CComPtr< CMgaFolder > np; CreateComObject(np); np->Initialize(terr, this); *p = np.Detach(); } break;
		}
	}
	ASSERT(("Could not create MGA object",p));
}

// ----------------------------------------
// FCO status information methods: readibility, writability, IsEqual
// ----------------------------------------
//throws!!
long FCO::getstatus() {
		if(!mgaproject) COMTHROW(E_MGA_ZOMBIE_NOPROJECT);
		if(!mgaproject->opened) COMTHROW(E_MGA_ZOMBIE_CLOSED_PROJECT);
		if(!mgaproject->activeterr) COMTHROW(E_MGA_NOT_IN_TRANSACTION);
		VARIANT_BOOL pp;
		if(self->get_IsDeleted(&pp) != S_OK) return OBJECT_ZOMBIE;
		return pp ? OBJECT_DELETED  :  OBJECT_EXISTS;
}

//throws!!
void FCO::CheckDeletedRead() {
		if(getstatus() == OBJECT_ZOMBIE) COMTHROW(E_MGA_OBJECT_ZOMBIE);
}

//throws!!
void FCO::CheckRead() {
		if(getstatus() != OBJECT_EXISTS) COMTHROW(E_MGA_OBJECT_DELETED);
}

//throws!!
void FCO::CheckWrite() {
		CheckRead();
		ASSERT(mgaproject->alreadynested());
		if(mgaproject->read_only) COMTHROW(E_MGA_READ_ONLY_ACCESS);
		if(self[ATTRID_PERMISSIONS] & LIBRARY_FLAG) COMTHROW(E_MGA_LIBOBJECT);
		if(self[ATTRID_PERMISSIONS] & READONLY_FLAG) COMTHROW(E_MGA_READ_ONLY_ACCESS);
}

HRESULT FCO::get_IsEqual(IMgaObject *o, VARIANT_BOOL *pVal) {
	COMTRY {
		CHECK_OUTPAR(pVal);
		if(o == NULL) *pVal = VARIANT_FALSE;
		else {
			CHECK_MYINPTRPAR(o);
			*pVal = (ObjFor(o) == this) ? VARIANT_TRUE : VARIANT_FALSE;
		}
	} COMCATCH(;)
}


HRESULT FCO::get_Status(long *p) {
	COMTRY {
		CHECK_OUTPAR(p);
		*p = getstatus(); 
	} COMCATCH(;);
}

HRESULT FCO::get_IsWritable(VARIANT_BOOL *p) {
	COMTRY {
		CHECK_OUTPAR(p);
		locking_type ppp;
		COMTHROW(self->get_PeerLockValue(ATTRID_LOCK, &ppp));
		*p =  (ppp & LOCKING_READ) ? VARIANT_FALSE : VARIANT_TRUE;
	} COMCATCH(;)
}


// is its metaobject a simpleconnection??
bool FCO::simpleconn() {
	CComPtr<IMgaMetaFCO> meta;
	if(get_Meta(&meta) != S_OK) return false;
	CComQIPtr<IMgaMetaConnection> metac = meta;
	if(!metac) return false;
	VARIANT_BOOL s;
	if(metac->get_IsSimple(&s) != S_OK) return false;
	return s ? true : false;
}
*/

const Result_t MgaFCOBase::GetID(std::string &id) const throw()
{
	MetaObjIDPair idPair;
	ASSERT( this->_coreObject->IDPair(idPair) == S_OK );
//	wchar_t str[20];
//	swprintf(str, 20, "id-%04lx-%08lx", idPair.metaID, idPair.objID);
//	id = std::string(str);
	id = "id-";
	return S_OK;
}


const Result_t MgaFCOBase::GetRelID(int32_t &value) const throw()
{
	// Just call through to the core object - will return issues if not in transaction
	return this->_coreObject->GetAttributeValue(ATTRID_RELID, value);
}


const Result_t MgaFCOBase::SetRelID(const int32_t &newVal) throw()
{
//	CheckWrite();
//	if(newVal <= 0 || newVal >= RELIDSPACE) return E_MGA_ARG_RANGE;
	return this->_coreObject->SetAttributeValue(ATTRID_RELID, newVal);
}


const Result_t MgaFCOBase::GetName(std::string &name) const throw()
{
	// Just call through to the core object - will return issues if not in transaction
	return this->_coreObject->GetAttributeValue(ATTRID_NAME, name);
}


const Result_t MgaFCOBase::SetName(const std::string &name) throw()
{ 
	// Make sure we are ready to write info
	//CheckWrite();
	// See if we are working with a library
	int32_t permissions;
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_PERMISSIONS, permissions);
	if (result != S_OK ) return result;
	if (permissions & LIBROOT_FLAG) return E_MGA_LIBOBJECT;
	// Get the current name
	std::string currentName;
	ASSERT( this->_coreObject->GetAttributeValue(ATTRID_NAME, currentName) == S_OK );
	// Is this a name change?
	if (currentName != name)
	{
		// Make sure name is unique (if it needs to be) with in parent (containment level)
		//		put_NameTask(CComBSTR(self[ATTRID_NAME]), newVal).DoWithDeriveds(self);
	}
	return S_OK;
}


// gives back the path to an object starting from the rootfolder (does not include project name)
const Result_t MgaFCOBase::GetAbsolutePath(std::string &path) const throw()
{ 
	std::string fullPath = "";
	// Get the coreProject
	CoreProject* coreProject;
	this->_coreObject->Project(coreProject);
	// Get the first parent
	MetaObjIDPair idPair;
	this->_coreObject->GetAttributeValue(ATTRID_PARENT, idPair);
	CoreObject* parent;
	coreProject->Object(idPair, parent);
	CoreObject* current = this->_coreObject;
	// Iterate upwards until the root folder is found
	while(idPair.metaID != DTID_ROOT)
	{
		/*		std::string tp = "/@";
		 std::string name;
		 current->GetAttributeValue(ATTRID_NAME, name);
		 tp.Append(name);
		 tp.Append("|kind=");
		 std::string metakind;
		 COMTHROW( mgaproject->FindMetaRef( cur[ATTRID_META])->get_Name( &metakind));
		 tp.Append( metakind);
		 int n;
		 giveme( mgaproject, par, cur, metakind, &n);
		 char p[10]; sprintf( p, "%d", n);
		 CComBSTR relative_pos;
		 tp.Append("|relpos=");tp.Append( p);
		 
		 tp.Append( path);
		 path = tp;
		 */
		// Make sure to delete the level we are done with before we move on
		if (current != this->_coreObject) delete current;
		// Move up one level in the hierarchy
		current = parent;
		this->_coreObject->GetAttributeValue(ATTRID_PARENT, idPair);
		coreProject->Object(idPair, parent);
	}
	// Clean up by deleting the rootObject
	if (current != this->_coreObject) delete current;
	// Set the return and go...
	path = fullPath;
	return S_OK;
}


const Result_t MgaFCOBase::GetMeta(MetaFCO* &metaFCO) const throw()
{
	// Set the meta namespace
	this->_mgaProject->SetNamespaceInMeta();
	int32_t metaID;
	ASSERT( this->_coreObject->GetAttributeValue(ATTRID_META, metaID) == S_OK );
//	Result_t result = this->_mgaProject->FindMetaRef(metaID);
//		} COMCATCH(e.hr = E_MGA_META_INCOMPATIBILITY;)
	// unreachable code ? really?
//		if(!(*pVal)) return E_MGA_META_INCOMPATIBILITY;
	return S_OK;
}


const Result_t MgaFCOBase::GetParentModel(MgaModel* &parent) const throw()
{
//	CoreObj par;
//	par = self[ATTRID_FCOPARENT];
//	if(par.GetMetaID() == DTID_MODEL)
//	{
//		CComPtr<IMgaModel> pv;
//		ObjForCore(par)->getinterface(&pv);
//		*pVal = pv.Detach();
//		COMTHROW((*pVal)->Open());
//	}
	return S_OK;
}


const Result_t MgaFCOBase::GetParentFolder(MgaFolder* &Folder) const throw()
{
//	CoreObj par;
//	par = self[ATTRID_FCOPARENT];
//	if(par.GetMetaID() == DTID_FOLDER) {
//		CComPtr<IMgaFolder> pv;
//		ObjForCore(par)->getinterface(&pv);
//		*pVal = pv.Detach();
//		COMTHROW((*pVal)->Open());
//	}
	return S_OK;
}


const Result_t MgaFCOBase::GetMetaBase(MetaBase* &metaBase) const throw()
{ 
	this->_mgaProject->SetNamespaceInMeta();
//	*pVal = mgaproject->FindMetaRef(self[ATTRID_META]).Detach();
	return S_OK;
}


const Result_t MgaFCOBase::GetObjType(ObjType_t& type) const throw()
{ 
	//	CheckDeletedRead();
	MetaObjIDPair idPair;
	ASSERT( this->_coreObject->IDPair(idPair) == S_OK );
	type = static_cast<ObjType_t>(idPair.metaID - DTID_BASE);
	return S_OK;
}

/*
HRESULT FCO::get_MetaRole(IMgaMetaRole **pVal) {
		COMTRY {
			CheckDeletedRead();
			CHECK_OUTPTRPAR(pVal);
			metaref_type role = self[ATTRID_ROLEMETA];
			if(role) {
				mgaproject->SetNmspaceInMeta();
				*pVal = CComQIPtr<IMgaMetaRole>(mgaproject->FindMetaRef(role)).Detach();
				if(!(*pVal)) COMTHROW(E_MGA_META_INCOMPATIBILITY);
			}

		} COMCATCH(;);
}

// ----------------------------------------
// Parent and grandparent access
// ----------------------------------------

HRESULT FCO::GetParent(IMgaContainer **pVal, objtype_enum *l) { 
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPARVALIDNULL(pVal);
			CComPtr<IMgaContainer> pv;
			CoreObj par;
			par = self[ATTRID_PARENT];
			if(par.GetMetaID() != DTID_ROOT) {
				ObjForCore(par)->getinterface(&pv);
				if(l != NULL) pv->get_ObjType(l);
				if(pVal != NULL) {
					*pVal = pv.Detach();
					COMTHROW((*pVal)->Open());
				}
			}
			else {
				if(l != NULL) *l = OBJTYPE_NULL;
				if(pVal != NULL) *pVal = NULL;
			}
		} COMCATCH(;)
	};


HRESULT FCO::get_RootParent(IMgaFCO **parent, long *distance) {       // retuns itself if root model
	COMTRY {
		long d = 0;
		CComPtr<IMgaFCO> cur;
		getinterface(&cur);
		while(true) {
			CComPtr<IMgaContainer> f;
			objtype_enum t;
			COMTHROW(cur->GetParent(&f, &t));
			if(t != OBJTYPE_MODEL) break;
			cur = NULL;
			f.QueryInterface(&cur);
			d++;
		}
		if(parent) {
			CHECK_OUTPTRPAR(parent);
			*parent = cur.Detach();
		}
		if(distance) *distance = d;
	} COMCATCH(;)
}


HRESULT FCO::get_ParentChain(IMgaFCOs **parents) {    // returns the real parents only (not self)
	COMTRY {
		CComPtr<IMgaFCO> cur;
		getinterface(&cur);
		CREATEEXCOLLECTION_FOR(MgaFCO, q);
		while(true) {
			CComPtr<IMgaContainer> f;
			objtype_enum t;
			COMTHROW(cur->GetParent(&f, &t));
			if(t != OBJTYPE_MODEL) break;
			f.QueryInterface(&cur);
			q->Add(cur);
		}
		*parents = q.Detach();
	} COMCATCH(;)
}


// check if object is a child or any grandchild of parentobject (also true if itself)
bool FCO::IsDescendantOf(IMgaFCO *object, long *distance) {
		int d;
		if(IsContained(self, CoreObj(object), &d)) {
			if(distance) *distance = d;
			return true;
		}
		else return false;

}
 
put_NameTask::put_NameTask(BSTR oname, BSTR nname) : oldname(oname), newname(nname) { ; }

bool put_NameTask::Do(CoreObj self, std::vector<CoreObj> *peers ) {
		if(!(oldname == CComBSTR(self[ATTRID_NAME]))) return false;
		if(ObjForCore(self)->mgaproject->preferences & MGAPREF_NODUPLICATENAMES) {
			CoreObj parent = self[ATTRID_PARENT];
			if(parent.GetMetaID() != DTID_ROOT) {  // Make sure it is not rootfolder (=parent is not rootobj)
				CoreObjs children = parent[ATTRID_PARENT+ATTRID_COLLECTION];
				ITERATE_THROUGH(children) {
					CComBSTR n = ITER[ATTRID_NAME];
					if(n == newname && !COM_EQUAL(self, ITER)) COMTHROW(E_MGA_NAME_DUPLICATE);
				}
			}		
		}
		self[ATTRID_NAME] = newname;
		ObjForCore(self)->SelfMark(OBJEVENT_PROPERTIES);
		if(self.IsFCO() && self.IsRootOfDeriv()) return false;   // Type renaming does not affect instances
		return true;
	}


void  FCO::initialname() { 
	if(mgaproject->preferences & MGAPREF_NAMENEWOBJECTS) {
		metaref_type mr;
		if(self.IsFCO() && !self.IsRootFCO()) mr = self[ATTRID_ROLEMETA];
		else mr = self[ATTRID_META];
		CComBSTR nname;
		COMTHROW(mgaproject->FindMetaRef(mr)->get_Name(&nname));
		if(mgaproject->preferences & MGAPREF_NODUPLICATENAMES) {
			size_t len = nname.Length();
			unsigned int freenum = 0;
			CoreObj parent = self[ATTRID_PARENT];
			if(parent.GetMetaID() != DTID_ROOT) {  // Make sure it is not rootfolder (=parent is not rootobj)
				CoreObjs children = parent[ATTRID_PARENT+ATTRID_COLLECTION];
				ITERATE_THROUGH(children) {
					CComBSTR n = ITER[ATTRID_NAME];
					if(!wcsncmp(nname, n, len)) {
						unsigned int f = 0;
						if(n.Length() == len) f = 1;
						else {
							swscanf(n+len,L"-%d", &f);
							f++;
							if(f > NEWNAME_MAXNUM) COMTHROW(E_MGA_GEN_OUT_OF_SPACE);
						}
						if(f > freenum) freenum = f;
					}
				}
			}
			if(freenum) {
				OLECHAR p[10];
				swprintf(p, 10, L"-%d",freenum);
				nname.Append(p);
			}
		}
		self[ATTRID_NAME] = nname;
	}
}


void giveme( CMgaProject *mgaproject, CoreObj par, CoreObj cur, CComBSTR cur_kind, int *relpos)
{
	int count_lower_ids = 0;
	CComBSTR cur_name = cur[ATTRID_NAME];
	objid_type cur_id = cur.GetObjID();
	
	CoreObjs children = par[ATTRID_FCOPARENT+ATTRID_COLLECTION];
	ITERATE_THROUGH(children) {
		CComBSTR n = ITER[ATTRID_NAME];

		CComBSTR kind;
		COMTHROW( mgaproject->FindMetaRef( ITER[ATTRID_META])->get_Name( &kind));

		bool similar = n == cur_name;
		//similar = similar || ITER[ATTRID_PERMISSIONS] == LIBROOT_FLAG && libraryNameEqual(n, name_b);
		similar = similar && kind == cur_kind;
		if( similar) // similar name 
		{
			objid_type id = ITER.GetObjID();
			if( cur_id > id)
				++count_lower_ids;
		}
	}
	*relpos = count_lower_ids;
}

// ----------------------------------------
// Add FCO to a collection (create new coll if null)
// ----------------------------------------
HRESULT FCO::CreateCollection(IMgaFCOs **pVal) {
	COMTRY {
// check type of collection
		CComPtr<IMgaFCO> tthis;
		getinterface(&tthis);
		if(*pVal) {
			CComPtr< COLLECTIONTYPE_FOR(IMgaFCO)> q;
			q.Attach(static_cast< COLLECTIONTYPE_FOR(IMgaFCO) *>(*pVal));
			q->Add(tthis);
			q.Detach(); // modified by ZolMol
		}
		else {
			CREATEEXCOLLECTION_FOR(MgaFCO,q2)
			COMTHROW(q2->Append(tthis));
			*pVal = q2.Detach(); // added by ZolMol
		}
	} COMCATCH(;);
}


// ----------------------------------------
// Parts access
// ----------------------------------------
HRESULT FCO::get_Parts(struct IMgaParts ** pVal) {
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPAR(pVal);

			CComPtr<IMgaMetaRole> metaro;					
			COMTHROW(get_MetaRole(&metaro));
			if(!metaro) COMTHROW( E_MGA_ROOTFCO);

			CComPtr<IMgaMetaParts> metaps;					
			COMTHROW(metaro->get_Parts(&metaps));

			CREATECOLLECTION_FOR(IMgaPart,q);
			MGACOLL_ITERATE(IMgaMetaPart, metaps) {
					metaref_type r;	            				
					COMTHROW(MGACOLL_ITER->get_MetaRef(&r));
					q->Add(ppool.getpoolobj(r, this, mgaproject));
			}
			MGACOLL_ITERATE_END;
			*pVal = q.Detach();

		} COMCATCH(;);
}


HRESULT FCO::get_Part(IMgaMetaAspect * asp, IMgaPart **pVal) {
		COMTRY {
			CheckRead();
			CHECK_INPTRPAR(asp);
			CHECK_OUTPTRPAR(pVal);

			CComPtr<IMgaMetaRole> metaro;					
			COMTHROW(get_MetaRole(&metaro));
			if(!metaro) COMTHROW( E_MGA_ROOTFCO);

			CComPtr<IMgaMetaParts> metaps;					
			COMTHROW(metaro->get_Parts(&metaps));
			MGACOLL_ITERATE(IMgaMetaPart, metaps) {
				CComPtr<IMgaMetaAspect> metaa;					
				COMTHROW(MGACOLL_ITER->get_ParentAspect(&metaa));
				if(COM_EQUAL(metaa, asp)) {
					metaref_type r;	            				
					COMTHROW(MGACOLL_ITER->get_MetaRef(&r));
					*pVal = ppool.getpoolobj(r, this, mgaproject).Detach();
				}
			}

			MGACOLL_ITERATE_END;
		} COMCATCH(;);
}


HRESULT FCO::get_PartByMP(IMgaMetaPart *part, IMgaPart **pVal) {
		COMTRY {
			CheckRead();
			CHECK_INPTRPAR(part);
			CHECK_OUTPTRPAR(pVal);

			CComPtr<IMgaMetaRole> metaro, metaro2;					
			COMTHROW(get_MetaRole(&metaro));
			if(!metaro) COMTHROW( E_MGA_ROOTFCO);

			COMTHROW(part->get_Role(&metaro2));
			if(!COM_EQUAL(metaro, metaro2)) COMTHROW( E_MGA_INVALID_ROLE);

			metaref_type r;	            				
			COMTHROW(part->get_MetaRef(&r));
			*pVal = ppool.getpoolobj(r, this, mgaproject).Detach();
		} COMCATCH(;);
}
*/

const Result_t MgaFCOBase::GetAttributes(std::list<MgaAttribute*> &attribList) throw()
{
	// Get the list of all metaAttributes for this MgaFCO
	MetaFCO* metaFCO;
	ASSERT( this->GetMeta(metaFCO) == S_OK );
	std::list<MetaAttribute*> metaAttribList;
	ASSERT( metaFCO->GetAttributes(metaAttribList) == S_OK );
	// Iterate over the metaAttribute list
	std::list<MetaAttribute*>::iterator metaAttribIter = metaAttribList.begin();
	while (metaAttribIter != metaAttribList.end())
	{
		MetaRef_t metaRef;
		(*metaAttribIter)->GetMetaRef(metaRef);
		// Create the MgaAttribute object
		MgaAttribute* mgaAttribute = NULL;
		// TODO: Figure out how to create an MgaAttribute
//		q->Add(apool.getpoolobj(r, this, mgaproject));
		ASSERT( mgaAttribute != NULL );
		attribList.push_back(mgaAttribute);
		// Move on to the next metaAttribute
		++metaAttribIter;
	}
	return S_OK;
}

/*
HRESULT FCO::get_Attribute(IMgaMetaAttribute *metaa,  IMgaAttribute **pVal) { 
		COMTRY {
			CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CHECK_INPTRPAR(metaa);
			CComPtr<IMgaMetaFCO> metao;					
			COMTHROW(get_Meta(&metao));
			metaref_type r;	            				
			COMTHROW(metaa->get_MetaRef(&r));
#ifdef OWN_META
			CComPtr<IMgaMetaFCO> meta;				    
			COMTHROW(metaa->get_Owner(&meta));
			if(!meta.IsEqualObject(metao)) return E_META_INVALIDATTR;
#else
			{
			CComPtr<IMgaMetaFCOs> metas;				    
			COMTHROW(metaa->get_UsedIn(&metas));
			MGACOLL_ITERATE(IMgaMetaFCO,metas) {
				if(MGACOLL_ITER.IsEqualObject(metao)) break;
			}
			if(MGACOLL_AT_END) return E_META_INVALIDATTR;
			MGACOLL_ITERATE_END;
			}
#endif

			*pVal = apool.getpoolobj(r, this, mgaproject).Detach();
		} COMCATCH(;);
	};

// THROWS!!, always returns a valid attr.
CComPtr<IMgaAttribute> FCO::AbyN(BSTR name) {
		CHECK_INSTRPAR(name);
		CComPtr<IMgaMetaAttribute> metaattr;
		CComPtr<IMgaMetaFCO> meta;
		metaref_type r;
		COMTHROW(get_Meta(&meta));
		COMTHROW(meta->get_AttributeByName(name, &metaattr));
		COMTHROW(metaattr->get_MetaRef(&r));
		return apool.getpoolobj(r, this, mgaproject);
}


HRESULT FCO::get_AttributeByName(BSTR name, VARIANT *pVal) { 
		COMTRY {
			CheckRead();
			// parameter will be checked by attr. fun
			COMTHROW(AbyN(name)->get_Value(pVal));
		} COMCATCH(;);
};
HRESULT FCO::put_AttributeByName(BSTR name, VARIANT newVal) { 
		COMTRY {
			CheckRead();
			// parameter will be checked by attr. fun
			COMTHROW(AbyN(name)->put_Value(newVal));
		} COMCATCH(;);
}
HRESULT FCO::get_StrAttrByName( BSTR name,  BSTR *pVal) {  
		COMTRY {
			CheckRead();
			// parameter will be checked by attr. fun
			COMTHROW(AbyN(name)->get_StringValue(pVal));
		} COMCATCH(;);
}
HRESULT FCO::put_StrAttrByName( BSTR name,  BSTR newVal) {
		COMTRY {
			CheckRead();
			// parameter will be checked by attr. fun
			COMTHROW(AbyN(name)->put_StringValue(newVal));
		} COMCATCH(;);
}
HRESULT FCO::get_IntAttrByName( BSTR name,  long *pVal) { 
		COMTRY {
			CheckRead();
			// parameter will be checked by attr. fun
			COMTHROW(AbyN(name)->get_IntValue(pVal));
		} COMCATCH(;);
}
HRESULT FCO::put_IntAttrByName( BSTR name,  long newVal) {
		COMTRY {
			CheckRead();
			// parameter will be checked by attr. fun
			COMTHROW(AbyN(name)->put_IntValue(newVal));
		} COMCATCH(;);
}
HRESULT FCO::get_FloatAttrByName( BSTR name,  double *pVal) { 
		COMTRY {
			CheckRead();
			// parameter will be checked by attr. fun
			COMTHROW(AbyN(name)->get_FloatValue(pVal));
		} COMCATCH(;);
}
HRESULT FCO::put_FloatAttrByName( BSTR name,  double newVal) { 
		COMTRY {
			CheckRead();
			// parameter will be checked by attr. fun
			COMTHROW(AbyN(name)->put_FloatValue(newVal));
		} COMCATCH(;);
}
HRESULT FCO::get_BoolAttrByName( BSTR name,  VARIANT_BOOL *pVal) { 
		COMTRY {
			CheckRead();
			// parameter will be checked by attr. fun
			COMTHROW(AbyN(name)->get_BoolValue(pVal));
		} COMCATCH(;);
}
HRESULT FCO::put_BoolAttrByName( BSTR name,  VARIANT_BOOL newVal) {
		COMTRY {
			CheckRead();
			// parameter will be checked by attr. fun
			COMTHROW(AbyN(name)->put_BoolValue(newVal));
		} COMCATCH(;);
}
HRESULT FCO::get_RefAttrByName( BSTR name,  IMgaFCO **pVal) {
		COMTRY {
			CheckRead();
			// parameter will be checked by attr. fun
			COMTHROW(AbyN(name)->get_FCOValue(pVal));
		} COMCATCH(;);
}
HRESULT FCO::put_RefAttrByName( BSTR name,  IMgaFCO * newVal) {
		COMTRY {
			CheckRead();
			if(newVal) CHECK_MYINPTRPAR(newVal);
			// parameter will be checked by attr. fun
			COMTHROW(AbyN(name)->put_FCOValue(newVal));
		} COMCATCH(;);
}

HRESULT FCO::ClearAttrByName( BSTR name) { 
		COMTRY_IN_TRANSACTION {
			CheckWrite();
			COMTHROW(AbyN(name)->Clear());
		} COMCATCH_IN_TRANSACTION(;);
}

HRESULT FCO::get_RegistryNode( BSTR path,  IMgaRegNode **pVal) {  
	COMTRY {
			CheckRead();
			CHECK_INSTRPAR(path);
			CHECK_OUTPTRPAR(pVal);
			*pVal = rpool.getpoolobj(path, this, mgaproject).Detach();
	} COMCATCH(;)
}


// ----------------------------------------
// Registry access
// ----------------------------------------
HRESULT FCO::get_Registry(VARIANT_BOOL virtuals, IMgaRegNodes **pVal) {  
	COMTRY {  
		CheckRead();
		CHECK_OUTPTRPAR(pVal);
		CREATEEXCOLLECTION_FOR(MgaRegNode,q);
		CoreObj s = self;
		if(!s.IsFCO()) virtuals = VARIANT_FALSE;
		stdext::hash_set<CComBSTRNoAt, CComBSTR_hashfunc> match;
		do {
			CoreObjs children = s[ATTRID_REGNOWNER+ATTRID_COLLECTION];
			ITERATE_THROUGH(children) {
				CComBSTR path;
				path	= ITER[ATTRID_NAME];
				if(virtuals) {
					if(match.find(path) != match.end()) continue;
					match.insert(path);
				}
				q->Add(rpool.getpoolobj(path, this, mgaproject));
			}
		} while(virtuals && (s = s[ATTRID_DERIVED]));
		if(virtuals) {
			CComQIPtr<IMgaMetaBase> m;
			COMTHROW(get_MetaBase(&m));
			CComPtr<IMgaMetaRegNodes> rns;
			COMTHROW(m->get_RegistryNodes(&rns));
			MGACOLL_ITERATE(IMgaMetaRegNode, rns) {
				CComBSTR path;
				COMTHROW(MGACOLL_ITER->get_Name(&path));
				if(match.find(path) != match.end()) continue;
				q->Add(rpool.getpoolobj(path, this, mgaproject));
			} MGACOLL_ITERATE_END;
		}

		*pVal = q.Detach();
	} COMCATCH(;);
}


HRESULT FCO::get_RegistryValue( BSTR path,  BSTR *pVal) {  
	COMTRY {  // no arg check, called methods will do that
		CheckRead();
		CComPtr<IMgaRegNode> node;
		COMTHROW(get_RegistryNode(path, &node));
		COMTHROW(node->get_Value(pVal));
	} COMCATCH(;)
}


HRESULT FCO::put_RegistryValue( BSTR path,  BSTR newval) {  
	COMTRY {  // no arg check, called methods will do that
		CheckRead();  // put_Value will check write
		CComPtr<IMgaRegNode> node;
		COMTHROW(get_RegistryNode(path, &node));
		COMTHROW(node->put_Value(newval));
	} COMCATCH(;)
}

//-------------------------------------------------------------------------------------
// lph: Change description for ATTR, REGISTRY and PROPERTIES notifications

typedef std::vector<CComVariant> ModificationsVector;

void getRegistryModifications(CoreObj &cobj, CComBSTR &path, ModificationsVector &mv) {
	CComVariant current  = cobj[ATTRID_REGNODEVALUE];
	CComVariant previous;
	COMTHROW(cobj->get_PreviousAttrValue(ATTRID_REGNODEVALUE, &previous));
	if (previous != current) {
		CComBSTR label = "REGISTRY:";
		label.Append(path);
		CComVariant ident = label;
		mv.push_back(ident);
		mv.push_back(previous);
	}
	ITERATE_THROUGH(cobj[ATTRID_REGNOWNER+ATTRID_COLLECTION]) {
		CComBSTR cname = ITER[ATTRID_NAME];
		CComBSTR cpath = path;
		cpath.Append("/");
		cpath.Append(cname);
		getRegistryModifications(ITER, cpath, mv);
	}
}

HRESULT get_Modifications(FCO *fco, unsigned long changemask, CComVariant *mods) {
	ModificationsVector modifications;
	if (changemask & OBJEVENT_REGISTRY) {
		ITERATE_THROUGH(fco->self[ATTRID_REGNOWNER+ATTRID_COLLECTION]) {
			CComBSTR path = ITER[ATTRID_NAME];
			getRegistryModifications(ITER, path, modifications);
		}
	}
	if (changemask & OBJEVENT_ATTR) {
		CComPtr<IMgaMetaFCO> mfco;
		COMTHROW(fco->get_Meta(&mfco));
		ITERATE_THROUGH(fco->self[ATTRID_ATTRPARENT+ATTRID_COLLECTION]) {
			CComPtr<IMgaMetaAttribute> ma;
			COMTHROW(mfco->get_AttributeByRef(ITER[ATTRID_META], &ma));
			attval_enum vt;
			COMTHROW(ma->get_ValueType(&vt));
			if (vt == ATTVAL_ENUM) vt = ATTVAL_STRING;
			attrid_type aid = ATTRID_ATTRTYPESBASE + vt;
			CComVariant current = ITER[aid];
			static const VARTYPE vartypes[] = { VT_NULL, VT_BSTR, VT_I4, VT_R8, VT_BOOL, VT_DISPATCH, VT_BSTR };
			if(vartypes[vt] != current.vt) {
				COMTHROW(current.ChangeType(vartypes[vt]));
			}
			CComVariant previous;
			COMTHROW(ITER->get_PreviousAttrValue(aid, &previous));
			if(vartypes[vt] != previous.vt) {
				COMTHROW(previous.ChangeType(vartypes[vt]));
			}
			if (previous != current) {
				CComBSTR name;
				COMTHROW(ma->get_Name(&name));
				CComBSTR label = "ATTR:";
				label.Append(name);
				CComVariant ident = label;
				modifications.push_back(ident);
				modifications.push_back(previous);
			}
		}
	}
	if (changemask & OBJEVENT_PROPERTIES) {
		CComVariant name = fco->self[ATTRID_NAME];
		CComVariant pname;
		fco->self->get_PreviousAttrValue(ATTRID_NAME, &pname);
		if (pname != name) {
			CComVariant ident = "PROPERTIES:Name";
			modifications.push_back(ident);
			modifications.push_back(pname);
		}
/* lph: possibly necessary, but not yet
		CComVariant perm = fco->self[ATTRID_PERMISSIONS];
		CComVariant pperm;
		COMTHROW(fco->self->get_PreviousAttrValue(ATTRID_PERMISSIONS, &pperm));
		if (pperm != perm) {
			CComVariant ident = "PROPERTIES:Permissions";
			modifications.push_back(ident);
			modifications.push_back(pperm);
		}
	}
	if (modifications.size() > 0) {
		SAFEARRAY *pVariantsArray = NULL;
		SAFEARRAYBOUND rgsabound[1];
		rgsabound[0].lLbound = 0;
		rgsabound[0].cElements = modifications.size();
		pVariantsArray = SafeArrayCreate(VT_VARIANT, 1, rgsabound);
		for (LONG i=0; i<LONG(modifications.size()); i++) {
			SafeArrayPutElement(pVariantsArray, &i, &modifications[i]);
		}
		CComVariant varOut;
		varOut.vt = VT_ARRAY | VT_VARIANT;
		varOut.parray = pVariantsArray;
		varOut.Detach(mods);
	}
	return S_OK;
}


//-------------------------------------------------------------------------------------

#define REQUIRE_NOTIFICATION_SUCCESS 1   // return with error if addons/rwhandlers fail

HRESULT FCO::objrwnotify() {
	COMTRY {
			unsigned long chmask = changemask;
			changemask = 0;
			// No other notification on deleted objects
			if(chmask & OBJEVENT_DESTROYED) chmask = OBJEVENT_DESTROYED;
			CMgaProject::addoncoll::iterator ai, abeg = mgaproject->alladdons.begin(), aend = mgaproject->alladdons.end();
			if(abeg != aend) {
				CComVariant mods;
				if(!(chmask & OBJEVENT_CREATED) && (chmask & (OBJEVENT_REGISTRY+OBJEVENT_ATTR+OBJEVENT_PROPERTIES)))
					COMTHROW(get_Modifications(this, chmask, &mods));
				COMTHROW(mgaproject->pushterr(*mgaproject->reserveterr));
				for(ai = abeg; ai != aend; ) {
					CComPtr<CMgaAddOn> t = *ai++;	// it is important to incr ii here, while obj 
												    // still holds a reference to the (*ai) object
					unsigned long mmask;
					if((mmask = (t->eventmask & chmask)) != 0) {
						CComPtr<IMgaObject> tt;
						getinterface(&tt);

#if(REQUIRE_NOTIFICATION_SUCCESS)
						COMTHROW(t->handler->ObjectEvent(tt, mmask, mods));
#else
						if((s = t->handler->ObjectEvent(tt, mmask, mods)) != S_OK) {
							ASSERT(("Notification failed", false));
						}
#endif
					    t->notified = true;
					}
				}
				COMTHROW(mgaproject->popterr());
			}
			if(chmask & OBJEVENT_CREATED) {
				// send message to all territories that contain parent, 
				CoreObj parent = self[ATTRID_PARENT];
				if(parent.IsContainer()) {
					FCO &p = *ObjForCore(parent);
					
					// if parent is also new, notify it first
					if(p.notifymask & OBJEVENT_CREATED) COMTHROW(p.objnotify());

					pubfcohash::iterator ii, pbeg = p.pubfcos.begin(), pend = p.pubfcos.end();
					for(ii = pbeg; ii != pend;) {  
						CMgaTerritory *t = (*ii).second->territory;
						CComPtr<IMgaObject> obj = (*ii).second;
						++ii;  // it is important to incr ii here, while obj 
							   // still holds a reference to the (*ii) object
						CComVariant dummy;
						if(t->rwhandler && (t->eventmask & OBJEVENT_CREATED) != 0) {
							COMTHROW(mgaproject->pushterr(*t));
							{
								CComPtr<IMgaObject> newoo;
								getinterface(&newoo, t);
#if(REQUIRE_NOTIFICATION_SUCCESS)
								COMTHROW(t->rwhandler->ObjectEvent(newoo, (unsigned long)OBJEVENT_CREATED, dummy));
#else
								if(t->rwhandler->ObjectEvent(newoo, (unsigned long)OBJEVENT_CREATED, dummy) != S_OK) {
									ASSERT(("Notification failed", false));
								}
#endif
							}
							COMTHROW(mgaproject->popterr());
							t->notified = true;
						}
					}
				}
				chmask &= ~OBJEVENT_CREATED;
			}
			pubfcohash::iterator ii, beg = pubfcos.begin(), end = pubfcos.end();
			for(ii = beg; ii != end;) {  
				CMgaTerritory *t = (*ii).second->territory;
				CComVariant &ud = (*ii).second->userdata;
				CComPtr<IMgaObject> obj = (*ii).second;
				++ii;  // it is important to incr ii here, while obj 
					   // still holds a reference to the (*ii) object
				long mmask;
				if(t->rwhandler && (mmask = (t->rweventmask & chmask)) != 0) {
					COMTHROW(mgaproject->pushterr(*t));
#if(REQUIRE_NOTIFICATION_SUCCESS)
					COMTHROW(t->rwhandler->ObjectEvent(obj, mmask,ud));
#else
					if(t->rwhandler->ObjectEvent(obj, mmask,ud) != S_OK) {
						ASSERT(("Notification failed", false));
					}
#endif
					t->notified = true;
					COMTHROW(mgaproject->popterr());  // this may release the territory!!!
				}
			}
	} COMCATCH(;)
}


HRESULT FCO::objnotify() {
	if(notifymask == 0) return S_OK;
	COMTRY {
			unsigned long chmask = notifymask;
			notifymask = 0;
			// No other notification on deleted objects
			if(chmask & OBJEVENT_DESTROYED) chmask = OBJEVENT_DESTROYED;
			if(chmask & OBJEVENT_CREATED) {
				// send message to all territories that contain parent, 
				CoreObj parent = self[ATTRID_PARENT];
				if(parent.IsContainer()) {
					FCO &p = *ObjForCore(parent);
					
					// if parent is also new, notify it first
					if(p.notifymask & OBJEVENT_CREATED) COMTHROW(p.objnotify());

					pubfcohash::iterator ii, pbeg = p.pubfcos.begin(), pend = p.pubfcos.end();
					for(ii = pbeg; ii != pend;) {  
						CMgaTerritory *t = (*ii).second->territory;
						CComPtr<IMgaObject> obj = (*ii).second;
						++ii;  // it is important to incr ii here, while obj 
							   // still holds a reference to the (*ii) object
						CComVariant dummy;
						if(t->handler && (t->eventmask & OBJEVENT_CREATED) != 0) {
							COMTHROW(mgaproject->pushterr(*t));
							{
								CComPtr<IMgaObject> newoo;
								getinterface(&newoo, t);
								if(t->handler->ObjectEvent(newoo, (unsigned long)OBJEVENT_CREATED, dummy) != S_OK) {
									ASSERT(("Notification failed", false));
								}
							}
							COMTHROW(mgaproject->popterr());
							t->notified = true;
						}
					}
				}
				chmask &= ~OBJEVENT_CREATED;
			}
			pubfcohash::iterator ii, beg = pubfcos.begin(), end = pubfcos.end();
			for(ii = beg; ii != end;) {  
				CMgaTerritory *t = (*ii).second->territory;
				CComVariant &ud = (*ii).second->userdata;
				CComPtr<IMgaObject> obj = (*ii).second;
				++ii;  // it is important to incr ii here, while obj 
					   // still holds a reference to the (*ii) object
				unsigned long mmask;
				if(t->handler && (mmask = (t->eventmask & chmask)) != 0) {
					COMTHROW(mgaproject->pushterr(*t));
					if(t->handler->ObjectEvent(obj, mmask, ud) != S_OK) {
						ASSERT(("Notification failed", false));
					}
					COMTHROW(mgaproject->popterr());
					t->notified = true;
				}
			}
			ASSERT(notifymask == 0);
	} COMCATCH(;)
}


void FCO::objforgetchange() {
	temporalmask = 0;
}


void FCO::objrecordchange() {
	ASSERT(temporalmask != 0);
	if(!changemask) {
		mgaproject->changedobjs.push(this);
	}	
	changemask |= temporalmask;
// 
	if(!notifymask) {
		mgaproject->notifyobjs.push(this);
	}	
	notifymask |= temporalmask;
	temporalmask = 0;
}


HRESULT FCO::SendEvent(long mask) {
	COMTRY_IN_TRANSACTION {
		CheckRead();
		if((mask | OBJEVENT_USERBITS) != OBJEVENT_USERBITS) COMTHROW(E_MGA_BAD_MASKVALUE);
		SelfMark(mask);
	} COMCATCH_IN_TRANSACTION(;)
}


void FCO::SelfMark(long newmask) {
	ASSERT(("Error: event generated in read-only transaction",!mgaproject->read_only));
	ASSERT(newmask);
	if(!temporalmask) {
		mgaproject->temporalobjs.push(this);
	}	
	temporalmask |= (unsigned long)newmask;
}

void FCO::objsetuserdata(CMgaTerritory *t, VARIANT udata) {
		pubfcohash::iterator i = pubfcos.find(t); 
		if(i == pubfcos.end()) COMTHROW(E_MGA_NOT_IN_TERRITORY);
		(*i).second->userdata = udata;
}

/*
HRESULT FCO::Associate(VARIANT userdata) {
  COMTRY {
	CheckDeletedRead();
	objsetuserdata(mgaproject->activeterr, userdata);
  } COMCATCH(;);
}

HRESULT FCO::get_CurrentAssociation(VARIANT *userdata) {
  COMTRY {
	CheckDeletedRead();
		pubfcohash::iterator i = pubfcos.find(mgaproject->activeterr);
		if(i == pubfcos.end()) COMTHROW(E_MGA_NOT_IN_TERRITORY);
		VariantCopy(userdata, &((*i).second->userdata)); 
   } COMCATCH(;);
}


HRESULT FCO::Open( openmode mode)  { 
	COMTRY {	
		ASSERT(mode == LOCKING_READ || mode == LOCKING_EXCLUSIVE);

		CComVariant lock = self[ATTRID_LOCK];
		ASSERT (lock.vt == VT_UI1);
		int  m = lock.bVal;

		switch(m) {
		case LOCKING_READ:
				if(mode == LOCKING_READ) break;
		case LOCKING_WRITE:
				mode = static_cast<openmode>(LOCKING_EXCLUSIVE);
		case LOCKING_NONE:
				self[ATTRID_LOCK] = mode;
		case LOCKING_EXCLUSIVE:
				break;
		default: 
				ASSERT(("Invalid locking value detected",false));
		}
	} COMCATCH(;);
};


// results in a single object with all local subobjects removed
void SingleObjTreeUnLock(CoreObj &self) {
	CComPtr<ICoreAttributes> atts;
	COMTHROW(self->get_Attributes(&atts));
	MGACOLL_ITERATE(ICoreAttribute, atts) {
			attrid_type ai;
			CComPtr<ICoreMetaAttribute> mattr;
			COMTHROW(MGACOLL_ITER->get_MetaAttribute(&mattr));
			COMTHROW(mattr->get_AttrID(&ai));
			if(ai >= ATTRID_COLLECTION) {
				ai -= ATTRID_COLLECTION;
				if(LINKREF_ATTR(ai) && ai != ATTRID_PARENT) {
					CoreObjs collmembers = self[ai + ATTRID_COLLECTION].CachedColl();
					ITERATE_THROUGH(collmembers) {
						SingleObjTreeUnLock(ITER);
					}
				}
			}
	} MGACOLL_ITERATE_END;
	self[ATTRID_LOCK] = LOCKING_NONE;
}


FCOPtr::FCOPtr(FCOPtr const &o) {
	p = o.p;
	p->AddRef();
}

FCOPtr::FCOPtr(FCO *f) {
	p = f;
	p->AddRef();
}

FCOPtr::FCOPtr() {
	p = NULL;
}

FCOPtr::~FCOPtr() {
	if(p) p->Release();
}

void FCOPtr::operator= (FCO *f) {	
	if(p == f) return;
	if(p) p->Release();
	p = f;
	p->AddRef();
}

bool FCOPtr::operator< (const FCOPtr &o) const {
	return p < o.p;
}

void CoreObjMark(CoreObj const &ob, long mask) {
	ObjForCore(ob)->SelfMark(mask);
}
	
HRESULT FCO::GetGuid(long* pl1, long* pl2, long* pl3, long* pl4)
{
	COMTRY {
		CheckRead();
		CHECK_OUTPAR( pl1);
		CHECK_OUTPAR( pl2);
		CHECK_OUTPAR( pl3);
		CHECK_OUTPAR( pl4);

		*pl1 = self[ATTRID_GUID1];
		*pl2 = self[ATTRID_GUID2];
		*pl3 = self[ATTRID_GUID3];
		*pl4 = self[ATTRID_GUID4];

	} COMCATCH(;)
}

HRESULT FCO::PutGuid( long l1, long l2, long l3, long l4)
{
	COMTRY_IN_TRANSACTION {
		CheckWrite();
		//ASSERT( self.IsFCO());
		//if( self.IsFCO()) {
		self[ATTRID_GUID1] = l1;
		self[ATTRID_GUID2] = l2;
		self[ATTRID_GUID3] = l3;
		self[ATTRID_GUID4] = l4;
	}  COMCATCH_IN_TRANSACTION(;)
}

HRESULT FCO::GetGuidDisp( BSTR *p_pGuidStr)
{
	// keep logic in sync with LibImgHelper::GetItsGuid() in MgaLibOps.cpp
	COMTRY {
		CheckRead();
		CHECK_OUTPAR( p_pGuidStr);
		long v1(0), v2(0), v3(0), v4(0);

		COMTHROW( this->GetGuid( &v1, &v2, &v3, &v4));

		GUID t_guid;
		t_guid.Data1 = v1;
		t_guid.Data2 = (v2 >> 16);
		t_guid.Data3 = (v2 << 16) >> 16;
		t_guid.Data4[0] = (v3 >> 24);
		t_guid.Data4[1] = (v3 << 8) >> 24;
		t_guid.Data4[2] = (v3 << 16) >> 24;
		t_guid.Data4[3] = (v3 << 24) >> 24;

		t_guid.Data4[4] = (v4 >> 24);
		t_guid.Data4[5] = (v4 << 8) >> 24;
		t_guid.Data4[6] = (v4 << 16) >> 24;
		t_guid.Data4[7] = (v4 << 24) >> 24;

		char buff[39];
		sprintf( buff, "{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
			t_guid.Data1, t_guid.Data2, t_guid.Data3,
			t_guid.Data4[0], t_guid.Data4[1], t_guid.Data4[2], t_guid.Data4[3],
			t_guid.Data4[4], t_guid.Data4[5], t_guid.Data4[6], t_guid.Data4[7]);

		*p_pGuidStr = CComBSTR( buff).Detach();

	} COMCATCH(;)
}

HRESULT FCO::PutGuidDisp( BSTR p_guidStr)
{
	COMTRY_IN_TRANSACTION {
		CheckWrite();
		CHECK_INSTRPAR( p_guidStr);
		//self[ATTRID_GUID1] = newVal;
		GUID t_guid;
		CopyTo( p_guidStr, t_guid);

		long v1 = t_guid.Data1; // Data1: 32 b, Data2, Data 3: 16 b, Data4: 64 bit
		long v2 = (t_guid.Data2 << 16) + t_guid.Data3;
		long v3 = (((((t_guid.Data4[0] << 8) + t_guid.Data4[1]) << 8) + t_guid.Data4[2]) << 8) + t_guid.Data4[3];
		long v4 = (((((t_guid.Data4[4] << 8) + t_guid.Data4[5]) << 8) + t_guid.Data4[6]) << 8) + t_guid.Data4[7];

		PutGuid( v1, v2, v3, v4);
	}  COMCATCH_IN_TRANSACTION(;)
}
*/

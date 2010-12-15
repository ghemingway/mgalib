#ifndef __MGA_FCO_H__
#define __MGA_FCO_H__


/*** Included Header Files ***/
#include "MgaTypes.h"
//#include "MgaAttribute.h"


/*** Class Predefinitions ***/
class MgaProject;
class CoreObject;
class MgaTerritory;
class MetaFCO;
class MetaBase;
class MgaModel;
class MgaFolder;
class MgaFilter;
class MgaFCO;
class MgaAttribute;
//class MgaConnPoint;
//class MgaConstraint;


//#define IMgaContainer IMgaObject
//#define RELIDSPACE (RELID_BASE_MAX+1L)
//#define RELIDSPACE_OVERFLOWED (RELID_VALUE_MAX+1UL)

//typedef stdext::hash_map<MgaTerritory*, MgaAtom*, terr_hashfunc> pubfcohash;
//typedef stdext::hash_map<ObjId_t, MgaConnPoint *, cp_hashfunc> cphash;
//typedef stdext::hash_map<ObjId_t, MgaConstraint *, cp_hashfunc> cshash;


// --------------------------- MgaFCOBase --------------------------- //


class MgaFCOBase
{
private:
	MgaProject					*_mgaProject;
	CoreObject					*_coreObject;
/*
	 static const CComBSTR SrcName, DstName;
	 unsigned long changemask, notifymask, temporalmask;			//Notification
	 pubfcohash pubfcos;
	 partpool ppool;
	 attrpool apool;
	 regnpool rpool;
	 cphash connpointhash;
	 cshash constrainthash;
	 MetaAttribute* metaattr;
	 
	 void PreDeleteNotify();										// PreDelete Notification by Tihamer for the PAMS SynchTool
*/
	MgaFCOBase();

public:
	MgaFCOBase(MgaProject* &project, CoreObject* &coreObject);
	~MgaFCOBase();
/*
	// these THROW
	bool simpleconn();
	long getstatus();
	void CheckRead();
	void CheckDeletedRead();
	void CheckWrite();
	void initialname();  // add inital name to object
	void assignnewchild(CoreObject &ss);
	void newrelidforchildren(CoreObject &ss);
	void inDeleteObject();
	bool findroleobj(std::string rolename, CoreObject &tt);
	void prepareDepInfo2Coll( std::string& pInfo, IMgaFolders** p_resultColl);
	void prepareColl2DepInfo( IMgaFolders* pDep, std::string& pResInfo);
	void objsetuserdata(MgaTerritory *t, VARIANT udata);
	void ObjMark(MgaObject *s, long mask) { mgaproject->ObjMark(s, mask); }
	void objforgetrwnotify() { changemask = 0; };
	void objforgetnotify() { notifymask = 0; };
	void SelfMark(long newmask);  // not unsigned long, to avoid warnings
	void objrecordchange();
	void objforgetchange();
	void removeterrfromlist(MgaTerritory *t);
	MgaAttribute* AbyN(std::string name);

	const Result_t FCO::GetAllBaseTypes(IMgaFCOs **bases);			// Get a list of basetypes this object is derived from
	const Result_t FCO::GetRootParent(IMgaFCO **parent, long *distance = NULL);	// Get root parent of object
	const Result_t FCO::GetParentChain(IMgaFCOs **parents);		// Get all parents starting from root (does not contain the object itself)
	bool FCO::IsDescendantOf(IMgaFCO *rootobject, long *distance = NULL);// check if object is a child or any grandchild of parentobject (also true if itself)
*/
	inline const Result_t GetProject(MgaProject* &project) const throw()	{ project = this->_mgaProject; return S_OK; }
	inline const Result_t CheckProject(MgaProject* &project) const throw()	{ return (project == this->_mgaProject ? S_OK : E_MGA_FOREIGN_PROJECT); }

	const Result_t GetID(std::string &id) const throw();
	const Result_t GetRelID(int32_t &relID) const throw();
	const Result_t SetRelID(const int32_t &relID) throw();
	const Result_t GetName(std::string &name) const throw();
	const Result_t SetName(const std::string &name) throw();
	const Result_t GetAbsolutePath(std::string &path) const throw();
	const Result_t GetMeta(MetaFCO* &meta) const throw();
	const Result_t GetParentModel(MgaModel* &model) const throw();
	const Result_t GetParentFolder(MgaFolder* &folder) const throw();
	const Result_t GetMetaBase(MetaBase* &metaBase) const throw();
	const Result_t GetObjType(ObjType_t &type) const throw();
	const Result_t GetAbsMetaPath(std::string &absPath) const throw();
	const Result_t GetRelMetaPath(const MgaFCO &begfco, std::string &relPath, std::list<MgaFCO*> &refs) const throw();
/*
	const Result_t GetIsLibObject(bool *pVal);
	const Result_t GetIsEqual(MgaObject *o, bool *pVal);
	const Result_t GetStatus(long *p);
	const Result_t GetIsWritable(bool *p);
	const Result_t GetParent(MgaContainer **pVal, objtype_enum *l = NULL);
	const Result_t GetMetaRole(MgaMetaRole **pVal);
	const Result_t Check(void);
	const Result_t CheckTree(void);
	const Result_t CheckRCS(void);										// check the relations in refs, conns and sets
	const Result_t SetExempt(bool newVal);
	const Result_t GetExempt(bool *pVal);
	const Result_t PutReadOnlyAccessWithPropagate( bool pReadOnly);
	const Result_t PutReadOnlyAccess( bool pReadOnly);
	const Result_t HasReadOnlyAccess( bool *pReadOnly);
	const Result_t GetGuid( long* l1, long* l2, long* l3, long* l4);
	const Result_t PutGuid( long l1, long l2, long l3, long l4);
	const Result_t GetChildObjects(IMgaObjects **pVal);
	const Result_t GetChildObjectByRelID(long relid, MgaObject **pVal); 
	const Result_t GetObjectByPath(std::string path, MgaObject **pVal);
	const Result_t GetNthObjectByPath(long n_th, std::string path, MgaObject **pVal);
	const Result_t DeleteObject();
	const Result_t Open( openmode mode = OPEN_READ);
	const Result_t SendEvent(long mask);
*/
	// FOLDER
	const Result_t GetChildFolders(std::list<MgaFolder*> &folderList) throw();
/*
	const Result_t CreateFolder(MetaFolder *meta, MgaFolder **nf);
	const Result_t CreateRootObject(MetaFCO *meta, MgaFCO **nobj);
	const Result_t GetMetaFolder(IMgaMetaFolder **pVal);
	const Result_t CopyFolders(MgaFolders *copylist, MgaFolders **objs);
	const Result_t MoveFolders(MgaFolders *copylist, MgaFolders **objs);
	const Result_t RefreshParent( MgaFolder * f);
	// MODEL
	const Result_t CreateChildObject( IMgaMetaRole *role, IMgaFCO **newobj);
	const Result_t CreateReference( IMgaMetaRole *meta,  IMgaFCO *target,  IMgaFCO **newobj);
	const Result_t GetAspectParts( IMgaMetaAspect * asp, unsigned int filter, IMgaParts **pVal);
	const Result_t CreateSimpleConn( IMgaMetaRole *metar,  IMgaFCO *src,  IMgaFCO *dst, IMgaFCOs *srcrefs, IMgaFCOs *dstrefs, IMgaFCO **newobj);
	// FOLDER & MODEL (CONTAINER)
	const Result_t ContainerCreateFCO(IMgaMetaFCO *meta, CoreObj &FCO);
*/
	const Result_t GetChildrenOfKind(const std::string &kind, std::list<MgaFCO*> &fcoList) throw();
	const Result_t GetDescendantFCOs(MgaFilter* &filter, std::list<MgaFCO*> &fcoList) throw(); 
	const Result_t GetChildFCOs(std::list<MgaFCO*> &fcoList) throw();
	const Result_t GetChildFCO(const std::string &name, MgaFCO* &fco) throw();
/*
	 const Result_t CopyFCOs(IMgaFCOs *copylist, IMgaMetaRoles *rlist, IMgaFCOs **objs);
	 const Result_t MoveFCOs(IMgaFCOs *copylist, IMgaMetaRoles *rlist, IMgaFCOs **objs);
	 const Result_t DeriveFCO(IMgaFCO *base, IMgaMetaRole *role, VARIANT_BOOL instance, IMgaFCO** newobj);
	 const Result_t AddInternalConnections(IMgaFCOs *inobjs, IMgaFCOs **objs);
	 // CONNECTION
	 const Result_t GetNamedRoleTarget(BSTR rolename, IMgaFCO **pVal);
	 const Result_t GetNamedRoleReferences(BSTR rolename, IMgaFCOs **pVal);
	 const Result_t put_NamedRole(BSTR rolename, IMgaFCOs *rs, IMgaFCO * newVal);
	 const Result_t AddConnPoint(BSTR rolename, long maxinrole, IMgaFCO *target, IMgaFCOs *refs, IMgaConnPoint **pVal);
	 const Result_t GetConnPoints(IMgaConnPoints **pVal);
	 // REFERENCE
	 const Result_t GetReferred(IMgaFCO **pVal);
	 const Result_t put_Referred(IMgaFCO * newVal);
	 const Result_t GetRefAspect(IMgaMetaAspect **pVal);
	 const Result_t put_RefAspect(IMgaMetaAspect * newVal);
	 const Result_t GetUsedByConns(IMgaConnPoints **pVal);
	 // SET
	 const Result_t RemoveMember(IMgaFCO *member);
	 const Result_t SetRemoveAll();
	 const Result_t AddMember(IMgaFCO *member);
	 const Result_t GetMembers(IMgaFCOs * *pVal);
	 const Result_t GetIsMember(IMgaFCO *obj, VARIANT_BOOL *pVal);
	 // ALL
	 const Result_t CreateCollection(IMgaFCOs **pVal);
	 const Result_t GetConstraints(VARIANT_BOOL includemeta, IMgaConstraints **pVal);
	 const Result_t CheckConstraints(long,int *,unsigned short ** );
	 const Result_t AddConstraint(struct IMgaConstraint *p);
	 const Result_t DefineConstraint(BSTR name, long eventmask, BSTR expression, IMgaConstraint **nconstraint);
	 const Result_t GetBaseType(IMgaFCO **basetype, IMgaFCO ** immbase, VARIANT_BOOL *isinst);
	 const Result_t GetDerivedFrom( IMgaFCO **pVal);
	 const Result_t GetType( IMgaFCO **pVal);
	 const Result_t GetBaseType( IMgaFCO **pVal);
	 const Result_t GetArcheType( IMgaFCO **pVal);
	 const Result_t GetIsInstance( VARIANT_BOOL *pVal);
	 const Result_t GetIsPrimaryDerived( VARIANT_BOOL *pVal);
	 const Result_t GetDerivedObjects( IMgaFCOs **pVal);
	 const Result_t GetChildDerivedFrom(IMgaFCO *baseobj, IMgaFCO **pVal);
	 const Result_t DetachFromArcheType();
	 const Result_t AttachToArcheType( IMgaFCO *newtype,  VARIANT_BOOL instance);
	 const Result_t RefCompareToBase(short *status);				
	 const Result_t RefRevertToBase();								
	 const Result_t SetCompareToBase(IMgaFCO *p, short *status);				
	 const Result_t SetRevertToBase(IMgaFCO *p);								
	 const Result_t ConnCompareToBase(IMgaConnPoint *p, short *status);				
	 const Result_t ConnRevertToBase(IMgaConnPoint *p);								
	 const Result_t GetParts(struct IMgaParts ** pVal);
	 const Result_t GetPart(IMgaMetaAspect * asp, IMgaPart **pVal);
	 const Result_t GetPartByMP(IMgaMetaPart *part, IMgaPart **pVal);
*/
	const Result_t GetAttributes(std::list<MgaAttribute*> &attribList) throw();
/*
	 const Result_t GetAttribute(IMgaMetaAttribute *metaa,  IMgaAttribute **pVal);
	 const Result_t GetPartOfConns(IMgaConnPoints **pVal);
	 const Result_t GetMemberOfSets(IMgaFCOs **pVal);
	 const Result_t GetReferencedBy(IMgaFCOs **pVal);
	 // THROWS!!
	template<class T>
	const Result_t GetAttributeByName(std::string name, T &value);
	template<class T>
	const Result_t SetAttributeByName(std::string name, const T &newValue);
	 const Result_t GetStrAttrByName( std::string name, std::string *pVal);
	 const Result_t SetStrAttrByName( std::string name, std::string newVal);
	 const Result_t GetIntAttrByName( std::string name, long *pVal);
	 const Result_t SetIntAttrByName( std::string name, long newVal);
	 const Result_t GetFloatAttrByName( std::string name, double *pVal);
	 const Result_t SetFloatAttrByName( std::string name, double newVal);
	 const Result_t GetBoolAttrByName( std::string name, VARIANT_BOOL *pVal);
	 const Result_t SetBoolAttrByName( std::string name, VARIANT_BOOL newVal);
	 const Result_t GetRefAttrByName( std::string name, IMgaFCO **pVal);
	 const Result_t SetRefAttrByName( std::string name, IMgaFCO * newVal);
	 const Result_t ClearAttrByName( std::string name);
	 const Result_t GetRegistryNode( std::string path,  IMgaRegNode **pVal);
	 const Result_t GetRegistry( VARIANT_BOOL virtuals, IMgaRegNodes **pVal);
	 const Result_t GetRegistryValue( std::string path,  std::string *pVal);
	 const Result_t SetRegistryValue( std::string path,  std::string newval);
	 const Result_t ChangeObject(IMgaMetaRole *r, IMgaMetaFCO *kind);
	 // Library
	 const Result_t doAttach(  std::string libname, VARIANT_BOOL ungroup, IMgaFolder **nf);
	 const Result_t doRefresh( std::string libname, VARIANT_BOOL ungroup, long *numOfErrors);
	 const Result_t AttachLibraryV3(std::string libname, VARIANT_BOOL ungrouped, IMgaFolder **nf);
	 const Result_t RefreshLibraryV3(std::string libname, VARIANT_BOOL ungrouped, long *numOfErrors);
	 const Result_t AttachLibrary(std::string libname, IMgaFolder **nf);
	 const Result_t RefreshLibrary(std::string libname);
	 const Result_t GetLibraryName(std::string *libname);
	 const Result_t put_LibraryName(std::string libname);
	 const Result_t GetVirtuallyIncludedBy( IMgaFolders** pDep);
	 const Result_t SetVirtuallyIncludedBy( IMgaFolders*  pDep);
	 const Result_t GetVirtuallyIncludes  ( IMgaFolders** pDep);
	 const Result_t SetVirtuallyIncludes  ( IMgaFolders*  pDep);
	 // Source control
	 const Result_t GetSourceControlInfo( long* scInfo);
	 const Result_t GetChildRelIDCounter(long *pVal);
	 const Result_t put_ChildRelIDCounter(long pVal);
	 const Result_t objrwnotify();
	 const Result_t objnotify();
	 const Result_t PreNotify(unsigned long changemask, CComVariant param);
*/
};


// --------------------------- MgaFCO --------------------------- //


// ----------------------------------------
// Simple base class used in MgaModel, MgaAtom, MgaFolder etc.
// most methods just dispatch to the corresponding method in the _fcoBase object
// ----------------------------------------
class MgaFCO
{
protected:
	MgaFCOBase					*_fcoBase;
	MgaTerritory				*_territory;
//	CComVariant					_userdata;

private:
	MgaFCO();

public:
	MgaFCO(MgaTerritory *territory, MgaProject* &project, CoreObject* &coreObject) : _fcoBase(NULL), _territory(territory)
	{
		ASSERT( this->_territory != NULL );
		// Try to create an FCOBase with the given project and coreObject
		this->_fcoBase = new MgaFCOBase(project, coreObject);
		ASSERT( this->_fcoBase != NULL );
	}
	virtual ~MgaFCO() { 
// TODO: if an object is released outside a transaction, 
//	 every object to be deleted opens and closes a transaction.
//   this is very slow. 
//   In an updated version 
//	 I recommend opening  a 'implicit transaction' in MgaProject,
//	 at the first time such an out-of-transaction destruct happens, 
//   and not closing that transaction until an explicit transaction or a MgaProject::Close()
//	 is initiated
/*		if(this->_territory->coreterr) {
			if(!this->_fcoBase->mgaproject->activeterr) {
				if(S_OK == this->_fcoBase->mgaproject->BeginTransaction(territory, TRANSACTION_READ_ONLY)) {
					this->_fcoBase->Close();
					if(S_OK != this->_fcoBase->mgaproject->CommitTransaction()) {
						this->_fcoBase->mgaproject->AbortTransaction();
					}
				}
			}
			else this->_FCO->Close();
		}
		FCO::pubfcohash::iterator ii = this->_fcoBase->pubfcos.find(territory);
		ASSERT(ii != this->_fcoBase->pubfcos.end());
		this->_fcoBase->pubfcos.erase(ii);
*/
		if (this->_fcoBase != NULL) delete this->_fcoBase;
	}
	// Methods Dispatched to the FCO  	 
	inline const Result_t GetID(std::string &id) const throw()				{ return this->_fcoBase->GetID(id); }
	inline const Result_t GetRelID(int32_t &relID)	const throw()			{ return this->_fcoBase->GetRelID(relID); }
	inline const Result_t SetRelID(const int32_t &relID) const throw()		{ return this->_fcoBase->SetRelID(relID); }
	inline const Result_t GetName(std::string &name) const throw()			{ return this->_fcoBase->GetName(name); }
	inline const Result_t SetName(const std::string &name) throw()			{ return this->_fcoBase->SetName(name); }
	inline const Result_t GetAbsolutePath(std::string &absPath) throw()		{ return this->_fcoBase->GetAbsolutePath(absPath); }
	inline const Result_t GetMeta(MetaFCO* &metaFCO) const throw()			{ return this->_fcoBase->GetMeta(metaFCO); }
	inline const Result_t GetParentModel(MgaModel* &model) const throw()	{ return this->_fcoBase->GetParentModel(model); }
	inline const Result_t GetParentFolder(MgaFolder* &folder) const throw()	{ return this->_fcoBase->GetParentFolder(folder); }
	inline const Result_t GetMetaBase(MetaBase* &metaBase) const throw()	{ return this->_fcoBase->GetMetaBase(metaBase); }
	inline const Result_t GetObjType(ObjType_t &type) const throw()			{ return this->_fcoBase->GetObjType(type); }
	inline const Result_t GetAbsMetaPath(std::string &absPath) const throw(){ return this->_fcoBase->GetAbsMetaPath(absPath); }
	inline const Result_t GetRelMetaPath(const MgaFCO &begFCO, std::string &relPath, std::list<MgaFCO*> &refs) const throw()
																			{ return this->_fcoBase->GetRelMetaPath(begFCO, relPath, refs); }
	inline const Result_t GetProject(MgaProject* &project) const throw()	{ return this->_fcoBase->GetProject(project); }
	inline const Result_t CheckProject(MgaProject* &project)const throw()	{ return this->_fcoBase->CheckProject(project); }
/*
	const Result_t SendEvent(long mask)						{ return this->_fcoBase->SendEvent(mask); }
	const Result_t GetIsLibObject( VARIANT_BOOL *pVal)		{ return this->_fcoBase->GetIsLibObject(pVal); }
	const Result_t GetTerritory(MgaTerritory* &territory)	{ territory = this->_territory; return S_OK; }
	const Result_t GetIsEqual(IMgaObject *o, VARIANT_BOOL *pVal) { return this->_FCO->GetIsEqual(o, pVal); }
	const Result_t DestroyObject(void)							{ return this->_FCO->DeleteObject(); }
	const Result_t GetStatus(long *p)						{ return this->_FCO->GetStatus(p); }
	const Result_t GetIsWritable(VARIANT_BOOL *p)			{ return this->_FCO->GetIsWritable(p); }
//	const Result_t Open( openmode mode)						{ return this->_FCO->Open(mode); }
//	const Result_t Close()									{ return this->_FCO->Close(); }
	const Result_t Associate( VARIANT ud)					{ userdata= ud; return S_OK; }
	const Result_t GetCurrentAssociation( VARIANT *udp)	{ VariantCopy(udp, &userdata); }
	const Result_t GetParent( IMgaContainer **pVal,  objtype_enum *l)  { return this->_FCO->GetParent(pVal,l); }
	const Result_t Check()										{ return this->_FCO->Check(); }
	const Result_t CheckTree()									{ return this->_FCO->CheckTree(); }
	const Result_t SetExempt)(VARIANT_BOOL newVal)				{ return this->_FCO->put_Exempt(newVal); }
	const Result_t GetExempt)(VARIANT_BOOL *pVal)				{ return this->_FCO->GetExempt(pVal); }
	const Result_t PutReadOnlyAccessWithPropagate)( VARIANT_BOOL pReadOnly) { return this->_FCO->PutReadOnlyAccessWithPropagate( pReadOnly); }
	const Result_t PutReadOnlyAccess)( VARIANT_BOOL pReadOnly)	{ return this->_FCO->PutReadOnlyAccess( pReadOnly); }
	const Result_t HasReadOnlyAccess)( VARIANT_BOOL *pReadOnly)	{ return this->_FCO->HasReadOnlyAccess( pReadOnly); }
	const Result_t GetGuid)( long* l1, long* l2, long* l3, long* l4)	{ return this->_FCO->GetGuid( l1, l2, l3, l4); }
	const Result_t PutGuid)( long l1, long l2, long l3, long l4){ return this->_FCO->PutGuid( l1, l2, l3, l4); }
	const Result_t GetChildObjects)(IMgaObjects **pVal)		{ return this->_FCO->GetChildObjects(pVal); }
	const Result_t GetChildObjectByRelID)(long relid, IMgaObject **pVal)	{ return this->_FCO->GetChildObjectByRelID(relid ,pVal); }
	const Result_t GetObjectByPath)(BSTR path, IMgaObject **pVal)	{ return this->_FCO->GetObjectByPath(path, pVal); }
	const Result_t GetNthObjectByPath)(long n_th, BSTR path, IMgaObject **pVal)	{ return this->_FCO->GetNthObjectByPath(n_th, path, pVal); }
	const Result_t GetMetaRole)( IMgaMetaRole **pVal)			{ return this->_FCO->GetMetaRole(pVal); }
	const Result_t GetParts)( IMgaParts **pVal)				{ return this->_FCO->GetParts(pVal); }
	const Result_t GetPart)( IMgaMetaAspect * asp,  IMgaPart **pVal)  { return this->_FCO->GetPart(asp, pVal); }
	const Result_t GetPartByMetaPart)( IMgaMetaPart *p,  IMgaPart **pVal)  { return this->_FCO->GetPartByMP(p, pVal); }
*/
	inline const Result_t GetAttributes(std::list<MgaAttribute*> &attribList) throw()	{ return this->_fcoBase->GetAttributes(attribList); }
/*
	const Result_t GetAttribute(MetaAttribute* &meta,  MgaAttribute* &attrib) const throw() { return this->_FCO->GetAttribute(meta,pVal); }
	const Result_t GetAttributeByName)( BSTR name,  VARIANT *pVal) { return this->_FCO->GetAttributeByName(name, pVal); }
	const Result_t SetAttributeByName)( BSTR name,  VARIANT newval) { return this->_FCO->put_AttributeByName( name, newval); }
	const Result_t GetStrAttrByName)( BSTR name,  BSTR *pVal)  { return this->_FCO->GetStrAttrByName(name, pVal); }
	const Result_t SetStrAttrByName)( BSTR name,  BSTR newval)  { return this->_FCO->put_StrAttrByName(name, newval); }
	const Result_t GetIntAttrByName)( BSTR name,  long *pVal)  { return this->_FCO->GetIntAttrByName(name, pVal); }
	const Result_t SetIntAttrByName)( BSTR name,  long newval)  { return this->_FCO->put_IntAttrByName(name, newval); }
	const Result_t GetFloatAttrByName)( BSTR name,  double *pVal)  { return this->_FCO->GetFloatAttrByName(name, pVal); }
	const Result_t SetFloatAttrByName)( BSTR name,  double newval)  { return this->_FCO->put_FloatAttrByName(name, newval); }
	const Result_t GetBoolAttrByName)( BSTR name,  VARIANT_BOOL *pVal)  { return this->_FCO->GetBoolAttrByName(name, pVal); }
	const Result_t SetBoolAttrByName)( BSTR name,  VARIANT_BOOL newval)  { return this->_FCO->put_BoolAttrByName(name, newval); }
	const Result_t GetRefAttrByName)( BSTR name,  IMgaFCO **pVal)  { return this->_FCO->GetRefAttrByName(name, pVal); }
	const Result_t put_RefAttrByName)( BSTR name,  IMgaFCO * newval)  { return this->_FCO->put_RefAttrByName(name, newval); }
	const Result_t ClearAttrByName)( BSTR name)			{ return this->_FCO->ClearAttrByName(name); }
	const Result_t GetRegistryNode)( BSTR path,  IMgaRegNode **pVal)  { return this->_FCO->GetRegistryNode(path, pVal); }
	const Result_t GetRegistry)(VARIANT_BOOL virtuals, IMgaRegNodes **pVal)		{ return this->_FCO->GetRegistry (virtuals, pVal); }
	const Result_t GetRegistryValue)( BSTR path,  BSTR *pVal)  { return this->_FCO->GetRegistryValue(path, pVal); }
	const Result_t SetRegistryValue)( BSTR path,  BSTR newval)  { return this->_FCO->put_RegistryValue(path, newval); }
	const Result_t GetConstraints)( VARIANT_BOOL includemeta, IMgaConstraints **pVal) { return this->_FCO->GetConstraints (includemeta, pVal); }
	const Result_t AddConstraint)( IMgaConstraint *pVal)  { return this->_FCO->AddConstraint (pVal); }
	const Result_t DefineConstraint)(BSTR name, long mask, BSTR expr, IMgaConstraint **nc) { return this->_FCO->DefineConstraint (name, mask, expr, nc); }
	const Result_t CreateCollection)( IMgaFCOs **pVal) { return this->_FCO->CreateCollection(pVal); } 
	const Result_t GetBaseType)(IMgaFCO ** basetype, IMgaFCO ** immbase, VARIANT_BOOL *isinst) { return this->_FCO->GetBaseType(basetype, immbase, isinst); }
	const Result_t GetDerivedFrom)( IMgaFCO **pVal)		{ return this->_FCO->GetDerivedFrom( pVal); }
	const Result_t GetType)( IMgaFCO **pVal)			{ return this->_FCO->GetType( pVal); }
	const Result_t GetBaseType)( IMgaFCO **pVal) 		{ return this->_FCO->GetBaseType( pVal); }
	const Result_t GetArcheType)( IMgaFCO **pVal)		{ return this->_FCO->GetArcheType( pVal); }
	const Result_t GetIsInstance)( VARIANT_BOOL *pVal)	{ return this->_FCO->GetIsInstance( pVal); }
	const Result_t GetIsPrimaryDerived)( VARIANT_BOOL *pVal) 	{ return this->_FCO->GetIsPrimaryDerived( pVal); }
	const Result_t GetDerivedObjects)( IMgaFCOs **pVal) { return this->_FCO->GetDerivedObjects( pVal); }
	const Result_t AttachToArcheType)( IMgaFCO *newtype,  VARIANT_BOOL instance) { return this->_FCO->AttachToArcheType(newtype,instance); }
	const Result_t DetachFromArcheType)()				{ return this->_FCO->DetachFromArcheType (); }
	const Result_t GetRootFCO)(IMgaFCO **pVal) 		{ return this->_FCO->GetRootParent(pVal); }
	const Result_t GetPartOfConns)(IMgaConnPoints **pVal)	{ return this->_FCO->GetPartOfConns(pVal); }
	const Result_t GetMemberOfSets)(IMgaFCOs **pVal)	{ return this->_FCO->GetMemberOfSets(pVal); }
	const Result_t GetReferencedBy)(IMgaFCOs **pVal)	{ return this->_FCO->GetReferencedBy(pVal); }
*/
	// ReleaseList provides an ease way to dispose of lists of allocated MgaFCO objects
	template<class T>
	static const Result_t ReleaseList(std::list<T> fcoList) throw()
	{
		typename std::list<T>::iterator listIter = fcoList.begin();
		while (listIter != fcoList.end())
		{
			if (*listIter != NULL) delete *listIter;
			++listIter;
		}
		return S_OK;
	}
};


// --------------------------- MgaAtom --------------------------- //


class MgaAtom : public MgaFCO
{
private:
	MgaAtom();
public:
	MgaAtom(MgaTerritory *territory, MgaProject* &project, CoreObject* &coreObject) : ::MgaFCO(territory, project, coreObject)	{ }
};


#endif //__MGA_FCO_H__


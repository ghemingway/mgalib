
#ifndef MGA_MGAMETACONNJOINT_H
#define MGA_MGAMETACONNJOINT_H

// --------------------------- CMgaMetaConnJoint

class ATL_NO_VTABLE CMgaMetaConnJoint : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaMetaConnJoint, &CLSID_MgaMetaConnJoint>,
	public IDispatchImpl<IMgaMetaConnJoint, &IID_IMgaMetaConnJoint, &LIBID_MGAMetaLib>
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaConnJoint*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETACONNJOINT)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaConnJoint)

BEGIN_COM_MAP(CMgaMetaConnJoint)
	COM_INTERFACE_ENTRY(IMgaMetaConnJoint)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaConnJoint)
END_COM_MAP()

public:
	STDMETHOD(get_PointerSpecs)(IMgaMetaPointerSpecs **p)
	{ return ComGetCollectionValue<IMgaMetaPointerSpec>(
		GetUnknown(), ATTRID_PTRSPECS_COLL, p); }

	STDMETHOD(get_Parent)(IMgaMetaConnection **p)
	{ return ComGetPointerValue(GetUnknown(), ATTRID_CONNJOINTS_COLL,p); }

	STDMETHOD(get_PointerSpecByName)(BSTR name, IMgaMetaPointerSpec **p)
	{ return ComGetCollValueByName(name, GetUnknown(), ATTRID_PTRSPECS_COLL, ATTRID_PTRSPECNAME, p); }

	STDMETHOD(GetPointerSpecByNameDisp)(BSTR name, IMgaMetaPointerSpec **p)
	{ return get_PointerSpecByName( name, p); }
// ------- Edit

public:
	STDMETHOD(CreatePointerSpec)(IMgaMetaPointerSpec **p)
	{ return ComCreateMetaObj(GetUnknown(), METAID_METAPOINTERSPEC, ATTRID_PTRSPECS_COLL, p); }

// ------- Methods

public:
	static void Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me);
	static bool CheckPaths(CCoreObjectPtr &self, jointpaths_type &jointpaths);
};

#endif//MGA_MGAMETACONNJOINT_H

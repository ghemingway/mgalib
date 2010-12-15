
#ifndef MGA_MGAMETAPOINTERITEM_H
#define MGA_MGAMETAPOINTERITEM_H

// --------------------------- CMgaMetaPointerItem

class ATL_NO_VTABLE CMgaMetaPointerItem : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMgaMetaPointerItem, &CLSID_MgaMetaPointerItem>,
	public IDispatchImpl<IMgaMetaPointerItem, &IID_IMgaMetaPointerItem, &LIBID_MGAMetaLib>
{
public:
	IUnknown *GetUnknown() const { return (IMgaMetaPointerItem*)this; }

DECLARE_REGISTRY_RESOURCEID(IDR_MGAMETAPOINTERITEM)
DECLARE_ONLY_AGGREGATABLE(CMgaMetaPointerItem)

BEGIN_COM_MAP(CMgaMetaPointerItem)
	COM_INTERFACE_ENTRY(IMgaMetaPointerItem)
	COM_INTERFACE_ENTRY2(IDispatch, IMgaMetaPointerItem)
END_COM_MAP()

public:
	STDMETHOD(get_Desc)(BSTR *p);

	STDMETHOD(get_Parent)(IMgaMetaPointerSpec **p)
	{ return ComGetPointerValue(GetUnknown(), ATTRID_PTRITEMS_COLL, p); }

// ------- Edit

public:
	STDMETHOD(put_Desc)(BSTR p)
	{ return ComPutAttrValue(GetUnknown(), ATTRID_PTRITEMDESC, p); }

// ------- Methods

public:
	static bool CheckToken(CComBstrObj &token, bstr_const_iterator i, bstr_const_iterator &e);
	static bool CheckPath(CCoreObjectPtr &self, pathitems_type &pathitems, bool global);

};

#endif//MGA_MGAMETAPOINTERITEM_H

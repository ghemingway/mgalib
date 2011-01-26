/*** Included Header Files ***/
#include "MgaAttribute.h"
#include "MgaFCO.h"
#include "MgaProject.h"
#include "../Meta/MetaAttribute.h"


// --------------------------- MgaAttribute --------------------------- //


MgaAttribute::MgaAttribute(MetaRef_t &metaRef, MgaFCOBase* &fcoBase, MgaProject* &mgaProject)
		: _metaRef(metaRef), _fcoBase(fcoBase), _mgaProject(mgaProject), _regPrefix("AttrRegs/"), _metaAttribute(NULL),
		  _prevPtr(NULL), _next(NULL), _loadStatus(/*ATTSTATUS_INVALID*/) 
{
//	this->_metaAttribute = this->_mgaProject->FindMetaRef(metaRef);
	Result_t result = this->_metaAttribute->GetValueType(this->_attribType);
	ASSERT( result == S_OK );
	this->_inputType = this->_attribType;
	if(this->_attribType == ATTVAL_ENUM) this->_attribType = ATTVAL_STRING;
	std::string metaName;
	result = this->_metaAttribute->GetName(metaName);
	ASSERT( result == S_OK );
	this->_regPrefix.append(metaName);
}

/*
STDMETHODIMP CMgaAttribute::get_Owner( IMgaFCO **pVal) {  
	COMTRY { 
		CHECK_OUTPTRPAR(pVal);
		fco->getinterface(pVal); 
	} COMCATCH(;) 
};


STDMETHODIMP CMgaAttribute::get_Status( long *status) {
		COMTRY	{
			CHECK_OUTPAR(status);
			fco->CheckRead();
			if(load_status == ATTSTATUS_INVALID) {
				CComVariant v;
				get_Value(&v);
			}
			*status = load_status;
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_Value(VARIANT *pVal) {
		COMTRY {
			CHECK_OUTVARIANTPAR(pVal);
			fco->CheckRead();
			CComVariant var;
			if(load_status == ATTSTATUS_INVALID) {
				long ls = ATTSTATUS_HERE;
				CoreObj cur = fco->self;
				valueobj = NULL;
				while(cur) {
					CoreObjs attrs = cur[ATTRID_ATTRPARENT+ATTRID_COLLECTION];
					ITERATE_THROUGH(attrs) {
						if(mref == ITER[ATTRID_META]) {
							valueobj = ITER;
							break;
						}
					}
					if(valueobj != NULL) {
						load_status = ls;
						break;				// breaks here with >= HERE 
					}
					cur = cur[ATTRID_DERIVED];
					ls++;
				}
			}
			if(load_status >= ATTSTATUS_HERE) {
				COMTHROW(valueobj->get_AttributeValue((short)(ATTRID_ATTRTYPESBASE + attrtyp), &var));
			}
			else {
				CComQIPtr<IMgaMetaAttribute> m(mgaproject->FindMetaRef(mref));
				COMTHROW(m->get_DefaultValue(&var));
				load_status = ATTSTATUS_METADEFAULT;
			}
			{
				static const VARTYPE vartypes[] = { VT_NULL, VT_BSTR, VT_I4, VT_R8, VT_BOOL, VT_DISPATCH, VT_BSTR };
				if(vartypes[attrtyp] != var.vt) {
					COMTHROW(var.ChangeType(vartypes[attrtyp]));
				}
			}
			var.Detach(pVal);
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_OrigValue(VARIANT *pVal) {
		COMTRY {
			CHECK_OUTVARIANTPAR(pVal);
			fco->CheckRead();
			CComVariant var;
			if(load_status == ATTSTATUS_INVALID) {
				long ls = ATTSTATUS_HERE;
				CoreObj cur = fco->self;
				valueobj = NULL;
				while(cur) {
					CoreObjs attrs = cur[ATTRID_ATTRPARENT+ATTRID_COLLECTION];
					ITERATE_THROUGH(attrs) {
						if(mref == ITER[ATTRID_META]) {
							valueobj = ITER;
							break;
						}
					}
					if(valueobj != NULL) {
						load_status = ls;
						break;				// breaks here with >= HERE 
					}
					cur = cur[ATTRID_DERIVED];
					ls++;
				}
			}
			if(load_status >= ATTSTATUS_HERE) {
				COMTHROW(valueobj->get_AttributeValue((short)(ATTRID_ATTRTYPESBASE + attrtyp), &var));
			}
			else {
				CComQIPtr<IMgaMetaAttribute> m(mgaproject->FindMetaRef(mref));
				COMTHROW(m->get_DefaultValue(&var));
				load_status = ATTSTATUS_METADEFAULT;
			}
			{
				static const VARTYPE vartypes[] = { VT_NULL, VT_BSTR, VT_I4, VT_R8, VT_BOOL, VT_DISPATCH, VT_BSTR };
				if(vartypes[attrtyp] != var.vt) {
					COMTHROW(var.ChangeType(vartypes[attrtyp]));
				}
			}
			var.Detach(pVal);
		} COMCATCH(;)
}


STDMETHODIMP CMgaAttribute::get_HasChanged(VARIANT_BOOL *pVal) {
		COMTRY {
			CHECK_OUTPAR(pVal);
			*pVal = VARIANT_TRUE;
		} COMCATCH(;)
}


class attrnotifytask : public DeriveTreeTask {
	bool Do(CoreObj self, std::vector<CoreObj> *peers = NULL) {
		ObjForCore(self)->SelfMark(OBJEVENT_ATTR);
		return true;
	}
};


STDMETHODIMP CMgaAttribute::Clear() {
	COMTRY_IN_TRANSACTION {
		fco->CheckWrite();
		CoreObjs attrs = fco->self[ATTRID_ATTRPARENT+ATTRID_COLLECTION];
		ITERATE_THROUGH(attrs) {
			if(mref == ITER[ATTRID_META]) {
				// lph: Pre-Notification PRE_STATUS (the attribute is being changed to its default value)
				CComQIPtr<IMgaMetaAttribute> ma(mgaproject->FindMetaRef(mref));
				CComBSTR name;
				COMTHROW(ma->get_Name(&name));
				CComBSTR desc = "ATTR,";
				desc.Append(name);
				desc.Append(",Cleared");
				fco->PreNotify(OBJEVENT_PRE_STATUS, CComVariant(desc));
				//---------------------------------------------------------------------------------------
				COMTHROW(ITER->Delete());
				load_status = ATTSTATUS_INVALID;
				// lph: added notification of attribute mod
				attrnotifytask().DoWithDeriveds(fco->self);
				//-----------------------------------------
				break;
			}
		}
	} COMCATCH_IN_TRANSACTION(;);
}


STDMETHODIMP CMgaAttribute::put_Value(VARIANT newVal) {
		COMTRY_IN_TRANSACTION {
			CHECK_INPAR(newVal);
			fco->CheckWrite();
			if(load_status == ATTSTATUS_INVALID) {
				valueobj = NULL;
				CoreObjs attrs = fco->self[ATTRID_ATTRPARENT+ATTRID_COLLECTION];
				ITERATE_THROUGH(attrs) {
					if(mref == ITER[ATTRID_META]) {
						valueobj = ITER;
						load_status = ATTSTATUS_HERE;
						break;
					}
				}
			}
			if(load_status != ATTSTATUS_HERE) {
					load_status = ATTSTATUS_INVALID;
					valueobj = NULL;
					COMTHROW(mgaproject->dataproject->CreateObject(DTID_ATTRTYPESBASE+attrtyp, &valueobj.ComPtr()));
					// lph: Pre-Notification PRE_STATUS (the attribute is being changed from its default value)
					CComQIPtr<IMgaMetaAttribute> ma(mgaproject->FindMetaRef(mref));
					CComBSTR name;
					COMTHROW(ma->get_Name(&name));
					CComBSTR desc = "ATTR,";
					desc.Append(name);
					desc.Append(",Defined");
					fco->PreNotify(OBJEVENT_PRE_STATUS, CComVariant(desc));
					//-----------------------------------------------------------------------------------------
					valueobj[ATTRID_META]=mref;
					valueobj[ATTRID_ATTRPARENT]=fco->self;
					load_status = ATTSTATUS_HERE;
			}

			if(inputtyp == ATTVAL_ENUM) {
				CComVariant v = newVal;
				if(v.vt != VT_BSTR) v.ChangeType(VT_BSTR);
				if(v.vt != VT_BSTR) COMTHROW(E_MGA_BAD_ENUMVALUE);
				CComQIPtr<IMgaMetaAttribute> m(mgaproject->FindMetaRef(mref));
				CComPtr<IMgaMetaEnumItems> enms;
				COMTHROW(m->get_EnumItems(&enms));
				MGACOLL_ITERATE(IMgaMetaEnumItem, enms) {
					CComBSTR b;
					COMTHROW(MGACOLL_ITER->get_Value(&b));
					if(b == v.bstrVal) break;
				} 
				if(MGACOLL_AT_END) COMTHROW(E_MGA_BAD_ENUMVALUE);
				MGACOLL_ITERATE_END;

				valueobj[ATTRID_ATTRTYPESBASE + ATTVAL_STRING] = v;
			}
			else valueobj[ATTRID_ATTRTYPESBASE + attrtyp] = newVal;
			{
				CComPtr<IMgaFCO> p;
				COMTHROW(get_Owner(&p));
				attrnotifytask().DoWithDeriveds(fco->self);
			}
		} COMCATCH_IN_TRANSACTION(;)
}


STDMETHODIMP CMgaAttribute::get_StringValue(BSTR *pVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_RDATTRTYPES) &&
			attrtyp != ATTVAL_STRING && inputtyp != ATTVAL_ENUM) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_OUTSTRPAR(pVal);
			CComVariant v;
			COMTHROW( get_Value(&v));
			CopyTo(v, pVal);
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::put_StringValue(BSTR newVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_WRATTRTYPES) &&
			attrtyp != ATTVAL_STRING) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_INSTRPAR(newVal);
			COMTHROW( put_Value(CComVariant(newVal)));
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_IntValue(long *pVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_RDATTRTYPES) &&
			attrtyp != ATTVAL_INTEGER && inputtyp != ATTVAL_ENUM) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_OUTPAR(pVal);
			CComVariant v;
			COMTHROW( get_Value(&v));
			if(inputtyp == ATTVAL_ENUM) {
				long ord = 0;
				if(v.vt != VT_BSTR) v.ChangeType(VT_BSTR);
				if(v.vt != VT_BSTR) COMTHROW(E_MGA_BAD_ENUMVALUE);
				CComQIPtr<IMgaMetaAttribute> m(mgaproject->FindMetaRef(mref));
				CComPtr<IMgaMetaEnumItems> enms;
				COMTHROW(m->get_EnumItems(&enms));
				MGACOLL_ITERATE(IMgaMetaEnumItem, enms) {
					CComBSTR b;
					COMTHROW(MGACOLL_ITER->get_Value(&b));
					if(b == v.bstrVal) break;
					ord++;
				} 
				if(MGACOLL_AT_END) COMTHROW(E_MGA_BAD_ENUMVALUE);
				MGACOLL_ITERATE_END;
				*pVal = ord;
			}
			else CopyTo(v, *pVal);  // throws if type mismatch
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::put_IntValue(long newVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_WRATTRTYPES) &&
		   attrtyp != ATTVAL_INTEGER && inputtyp != ATTVAL_ENUM) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_INPAR(newVal);
			if(inputtyp == ATTVAL_ENUM) {
				CComQIPtr<IMgaMetaAttribute> m(mgaproject->FindMetaRef(mref));
				CComPtr<IMgaMetaEnumItems> enms;
				CComPtr<IMgaMetaEnumItem> enm;
				COMTHROW(m->get_EnumItems(&enms));
				long count;
				COMTHROW(enms->get_Count(&count));
				if(newVal < 0 || newVal > count) COMTHROW(E_MGA_BAD_ENUMVALUE);
				COMTHROW(enms->get_Item(1+newVal, &enm));
				CComBSTR estr;
				COMTHROW(enm->get_Value(&estr));
				COMTHROW( put_Value(CComVariant(estr)));
			}
			COMTHROW( put_Value(CComVariant(newVal)));
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_BoolValue(VARIANT_BOOL *pVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_RDATTRTYPES) &&
			attrtyp != ATTVAL_BOOLEAN) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_OUTPAR(pVal);
			CComVariant v;
			COMTHROW( get_Value(&v));
			CopyTo(v, *pVal);
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::put_BoolValue(VARIANT_BOOL newVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_WRATTRTYPES) &&
		   attrtyp != ATTVAL_BOOLEAN) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_INPAR(newVal);
			COMTHROW( put_Value(CComVariant(newVal)));
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_FloatValue(double *pVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_RDATTRTYPES) &&
			attrtyp != ATTVAL_DOUBLE) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_OUTPAR(pVal);
			CComVariant v;
			COMTHROW( get_Value(&v));
			CopyTo(v, *pVal);
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::put_FloatValue(double newVal) {
		if(!(mgaproject->preferences & MGAPREF_RELAXED_WRATTRTYPES) &&
		   attrtyp != ATTVAL_DOUBLE) return E_MGA_ACCESS_TYPE;
		COMTRY {
			CHECK_INPAR(newVal);
			COMTHROW( put_Value(CComVariant(newVal)));
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_FCOValue(IMgaFCO **pVal)
{
	if(attrtyp != ATTVAL_REFERENCE) return E_MGA_ACCESS_TYPE;
	COMTRY {
		CHECK_OUTPTRPAR(pVal);
		CComVariant v;
		COMTHROW( get_Value(&v));
		CoreObj h;
		if(v.vt != VT_DISPATCH) COMTHROW(E_MGA_MODULE_INCOMPATIBILITY);
		if(v.pdispVal) COMTHROW(v.pdispVal->QueryInterface(_uuidof(ICoreObject), (void **)&h));
		if(!h) COMTHROW(E_MGA_MODULE_INCOMPATIBILITY);
		ObjForCore(h)->getinterface(pVal);
	} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::put_FCOValue(IMgaFCO *newVal)
{
	if(attrtyp != ATTVAL_REFERENCE) return E_MGA_ACCESS_TYPE;
	COMTRY {
		CHECK_MYINPTRPAR(newVal);
		COMTHROW( put_Value(CComVariant(CoreObj(newVal))));
	} COMCATCH(;)

}


STDMETHODIMP CMgaAttribute::get_RegistryNode( BSTR path,  IMgaRegNode **pVal) {  
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CHECK_INSTRPAR(path);
      
			CComBSTR xpath(regprefix);
			xpath.Append("/");
			COMTHROW(xpath.AppendBSTR(path));
			COMTHROW(fco->get_RegistryNode(xpath, pVal));
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::get_Registry( VARIANT_BOOL virtuals, IMgaRegNodes **pVal) {  
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CComPtr<IMgaRegNode> p;
			COMTHROW(fco->get_RegistryNode(regprefix, &p));
			COMTHROW(p->get_SubNodes(virtuals, pVal));
		} COMCATCH(;);
}

STDMETHODIMP CMgaAttribute::get_RegistryValue( BSTR path,  BSTR *pVal) {  
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPAR(pVal);
			CHECK_INSTRPAR(path);
			CComPtr<IMgaRegNode> p;
			COMTHROW(get_RegistryNode(path, &p));
			COMTHROW(p->get_Value(pVal));
		} COMCATCH(;)
}

STDMETHODIMP CMgaAttribute::put_RegistryValue( BSTR path,  BSTR newval) {  
		COMTRY_IN_TRANSACTION {
			fco->CheckWrite();
			CHECK_INSTRPAR(path);
			CHECK_INSTRPAR(newval);
			CComPtr<IMgaRegNode> p;
			COMTHROW(get_RegistryNode(path, &p));
			COMTHROW(p->put_Value(newval));
		} COMCATCH_IN_TRANSACTION(;)
}



void MergeAttrs(const CoreObj &src, CoreObj &dst) {
	CoreObjs dstcoll = dst[ATTRID_ATTRPARENT+ATTRID_COLLECTION];
	ITERATE_THROUGH(src[ATTRID_ATTRPARENT+ATTRID_COLLECTION]) {
		CoreObj &srcn = ITER;
		CoreObj dstn;
		metaref_type srcmeta = srcn[ATTRID_META];
		ITERATE_THROUGH(dstcoll) {
			if(srcmeta == ITER[ATTRID_META]) {
				break;
			}
		}
		if(!ITER_BROKEN) {
			CComPtr<ICoreProject> p;
			COMTHROW(srcn->get_Project(&p));
			metaid_type mi = srcn.GetMetaID();
			COMTHROW(p->CreateObject(mi, &dstn.ComPtr()));
			dstn[ATTRID_META] = srcmeta;
			dstn[ATTRID_ATTRPARENT] = dst;
// MAGIC with attrtype numbers   to set the value attribute
			dstn[ATTRID_ATTRTYPESBASE + (mi - DTID_ATTRTYPESBASE) ] = srcn[ATTRID_ATTRTYPESBASE + (mi - DTID_ATTRTYPESBASE)];
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/// Registry   
/////////////////////////////////////////////////////////////////////////////////////////////////////


#define RFLAG_HASVALUE 1
#define RFLAG_OPAQUE 2


// THROWS!!!
// RECURSIVE!!!
void RegistryChildrenRemove(CoreObj &t) {
		CoreObjs children = CoreObj(t)[ATTRID_REGNOWNER + ATTRID_COLLECTION];
		ITERATE_THROUGH(children) {
			RegistryChildrenRemove(ITER);
			COMTHROW(ITER->Delete());
		}
}


// THROWS!!!
// RECURSIVE!!!
// Used when an FCO is detached from its basetype. 
// All nodes present in src node will be added to dst node, unless dst already has 
// a node with the same name and a value.
// If dst node is opaque, src is not copied and recursion also stops.

void MergeRegs(const CoreObj &src, CoreObj &dst) {
	CoreObjs dstcoll = dst[ATTRID_REGNOWNER+ATTRID_COLLECTION];
	ITERATE_THROUGH(src[ATTRID_REGNOWNER+ATTRID_COLLECTION]) {
		CoreObj &srcn = ITER;
		CoreObj dstn;
		CComBSTR srcname = srcn[ATTRID_NAME];
		long dstflags, srcflags = ITER[ATTRID_REGFLAGS];
		ITERATE_THROUGH(dstcoll) {
			if(srcname == CComBSTR(ITER[ATTRID_NAME])) {
				dstn = ITER;
				break;
			}
		}
		if(!dstn) {
			CComPtr<ICoreProject> p;
			COMTHROW(dst->get_Project(&p));
			COMTHROW(p->CreateObject(DTID_REGNODE, &dstn.ComPtr()));
			dstn[ATTRID_NAME] = srcn[ATTRID_NAME];
			dstn[ATTRID_REGNOWNER] = dst;
			dstflags = 0;
		}
		else dstflags = ITER[ATTRID_REGFLAGS];
		if(!(dstflags & RFLAG_OPAQUE)) {
			if(!(dstflags & RFLAG_HASVALUE) && (srcflags & RFLAG_HASVALUE)) {
				dstn[ATTRID_REGNODEVALUE] = srcn[ATTRID_REGNODEVALUE];
				dstn[ATTRID_XREF] = srcn[ATTRID_XREF];
			}
			dstn[ATTRID_REGFLAGS] = dstflags | srcflags;
			MergeRegs(srcn, dstn);
		}
	}
}


// throws
CoreObj findregvalueobj(CoreObj &cur, LPOLESTR obname, long &opacity, bool create) {  //returns NULL if not found
	ASSERT(*obname != '/');
	LPOLESTR endstr = wcschr(obname,'/');
	size_t len = endstr ? endstr - obname : wcslen(obname);
	CoreObjs snodes = cur[ATTRID_REGNOWNER+ATTRID_COLLECTION];
	ITERATE_THROUGH(snodes) {
		CComBSTR nm = ITER[ATTRID_NAME];
		if(nm.Length()==len && wcsncmp(nm,obname, len) == 0) {
			opacity |= (long)ITER[ATTRID_REGFLAGS];
			if(!endstr) {
				return ITER;
			}
			return findregvalueobj(ITER, endstr +1, opacity, create);
		}
	}
	if(create) {
		CComPtr<ICoreProject> p;
		COMTHROW(cur->get_Project(&p));
		CoreObj nob;
		COMTHROW(p->CreateObject(DTID_REGNODE, &nob.ComPtr()));
		{
			LPOLESTR z = new OLECHAR[len+1];
			wcsncpy(z, obname, len);
			z[len] = '\0';
			nob[ATTRID_NAME] = z;
			delete z;
		}
		nob[ATTRID_REGFLAGS] = 0; 
		nob[ATTRID_REGNOWNER] =  cur;
		if(!endstr) return nob;
		return findregvalueobj(nob, endstr +1, opacity, create);
	}
	return NULLCOREOBJ;
}



#if(0)
// throws
CoreObj findregvalueobj(CoreObj &cur, LPOLESTR obname, long &opacity, bool create) {  //returns NULL if not found
	ASSERT(*obname != '/');
	LPOLESTR endstr = wcschr(obname,'/');
	size_t len = endstr ? endstr - obname : wcslen(obname);

	CComBSTR namesegment(len, obname);
	VARIANT v;
	v.vt = VT_BSTR;
	v.bstrVal = namesegment;
	if(CoreObj node = cur[ATTRID_REGNOWNER+ATTRID_COLLECTION].Search(ATTRID_NAME, v)) {
		opacity |= (long)node[ATTRID_REGFLAGS];
		if(!endstr) {
			return node;
		}
		return findregvalueobj(node, endstr +1, opacity, create);
	}

	if(create) {
		CComPtr<ICoreProject> p;
		COMTHROW(cur->get_Project(&p));
		CoreObj nob;
		COMTHROW(p->CreateObject(DTID_REGNODE, &nob.ComPtr()));
		{
			LPOLESTR z = new OLECHAR[len+1];
			wcsncpy(z, obname, len);
			z[len] = '\0';
			nob[ATTRID_NAME] = z;
			delete z;
		}
		nob[ATTRID_REGFLAGS] = 0; 
		nob[ATTRID_REGNOWNER] =  cur;
		if(!endstr) return nob;
		return findregvalueobj(nob, endstr +1, opacity, create);
	}
	return NULLCOREOBJ;
}

#endif


class regnotifytask : public DeriveTreeTask {
	bool Do(CoreObj self, std::vector<CoreObj> *peers = NULL) {
		ObjForCore(self)->SelfMark(OBJEVENT_REGISTRY);
		return true;
	}
};



void CMgaRegNode::markchg() {
	regnotifytask().DoWithDeriveds(fco->self);
}





STDMETHODIMP CMgaRegNode::get_Object( IMgaObject **pVal) {  
	COMTRY { 
		CHECK_OUTPTRPAR(pVal);
		IMgaFCO *p;
		fco->getinterface(&p); 
		*pVal = p;
	} COMCATCH(;) 
};

STDMETHODIMP CMgaRegNode::get_Status( long *status) {
		COMTRY	{
			fco->CheckRead();
			CHECK_OUTPAR(status);

			if(load_status == ATTSTATUS_INVALID) {
				CComBSTR v;
				COMTHROW(get_Value(&v));
			}
			*status = load_status;
		} COMCATCH(;)
}


STDMETHODIMP CMgaRegNode::get_Value(BSTR *pVal) {
		COMTRY {
			CHECK_OUTVARIANTPAR(pVal);
			fco->CheckRead();
			CComVariant var;
			long opacity = 0;
			if(load_status == ATTSTATUS_INVALID) {
				long ls = ATTSTATUS_HERE;
				CoreObj cur = fco->self;
				valueobj = NULL;
				while(cur) {
					valueobj <<= findregvalueobj(cur, mypath, opacity, false);
					if(valueobj != NULL) {
					   long flags = valueobj[ATTRID_REGFLAGS];
					   if(flags & RFLAG_HASVALUE) {
  							load_status = ls;
							break;				// breaks here with >= HERE 
					   }
					}
					if((opacity & RFLAG_OPAQUE) != 0) {  // opaque node hit.
						load_status = ATTSTATUS_UNDEFINED;
						break;
					}
					CComPtr<ICoreMetaObject> mo;
					if(GetMetaID(cur) == DTID_FOLDER) break;  // folders do not inherit
					cur = cur[ATTRID_DERIVED];
					ls++;
				}
			}

			if(load_status >= ATTSTATUS_HERE) {
				*pVal = CComBSTR(valueobj[ATTRID_REGNODEVALUE]).Detach();
			}
			else if(load_status != ATTSTATUS_UNDEFINED) {    // INVALID || INMETA
				load_status = ATTSTATUS_UNDEFINED;
				metaref_type mref = fco->self[ATTRID_META];
				if(mref) {
					CComQIPtr<IMgaMetaBase> m(mgaproject->FindMetaRef(mref));
					CComPtr<IMgaMetaRegNode> rn;
					HRESULT hr = m->get_RegistryNode(mypath, &rn);
					if(hr == S_OK) {
						load_status = ATTSTATUS_METADEFAULT;
						COMTHROW(rn->get_Value( pVal));
					}
					else if(hr != E_NOTFOUND) COMTHROW(hr);
				}
			}
			if(load_status == ATTSTATUS_UNDEFINED) {
				// n.b. this is a fancy way of saying *pVal = NULL;
				// FIXME: *pVal is NULL here and we return S_OK
				CComBSTR x;   *pVal = x.Detach();
			}

		} COMCATCH(;)
}

STDMETHODIMP CMgaRegNode::get_FCOValue(IMgaFCO **pVal) {
		COMTRY	{
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);

			if(load_status == ATTSTATUS_INVALID) {
				CComBSTR v;
				COMTHROW(get_Value(&v));
			}
			if(load_status >= ATTSTATUS_HERE) {
				CoreObj v = valueobj[ATTRID_XREF];
				if(v) {
					ObjForCore(v)->getinterface(pVal);
				}
			}
		} COMCATCH(;)
}

STDMETHODIMP CMgaRegNode::get_Opacity( VARIANT_BOOL *pVal) {
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPAR(pVal)
			*pVal = VARIANT_FALSE;
			long opa;
			CoreObj vobj;
			vobj <<= findregvalueobj(fco->self, mypath, opa, false);
			if(vobj) {
				long flags = vobj[ATTRID_REGFLAGS];
				if(flags & ~RFLAG_OPAQUE) 
					*pVal = VARIANT_TRUE;
			}
		} COMCATCH(;);
}

STDMETHODIMP CMgaRegNode::put_Opacity( VARIANT_BOOL newVal) {
		COMTRY_IN_TRANSACTION {
			fco->CheckWrite();
			CHECK_INPAR(newVal);
			long opa;
			CoreObj vobj;
			vobj <<= findregvalueobj(fco->self, mypath, opa, true);
			long newmask = newVal ? RFLAG_OPAQUE : 0;
			long flags = vobj[ATTRID_REGFLAGS];
			if((flags & RFLAG_OPAQUE) != newmask) {
				vobj[ATTRID_REGFLAGS] = (flags & ~RFLAG_OPAQUE) | newmask;
				if(load_status != ATTSTATUS_HERE) load_status = ATTSTATUS_INVALID;
				markchg();
			}
		} COMCATCH_IN_TRANSACTION(;);
}


STDMETHODIMP CMgaRegNode::put_Value(BSTR newVal) {
		COMTRY_IN_TRANSACTION {
			CHECK_INSTRPAR(newVal);
			fco->CheckWrite();
			long dummy;
			valueobj <<= findregvalueobj(fco->self, mypath, dummy, true);
			load_status = ATTSTATUS_HERE;
			valueobj[ATTRID_REGNODEVALUE] = newVal;
			valueobj[ATTRID_XREF] = NULLCOREOBJ;
			long flags = valueobj[ATTRID_REGFLAGS];
			if(!(flags & RFLAG_HASVALUE)) valueobj[ATTRID_REGFLAGS] = flags | RFLAG_HASVALUE;
			markchg();
		} COMCATCH_IN_TRANSACTION(;)
}

STDMETHODIMP CMgaRegNode::put_FCOValue(IMgaFCO *newVal) {
		COMTRY_IN_TRANSACTION {
			CHECK_MYINPTRPAR(newVal);
			fco->CheckWrite();
			long dummy;
			valueobj <<= findregvalueobj(fco->self, mypath, dummy, true);
			load_status = ATTSTATUS_HERE;
			valueobj[ATTRID_REGNODEVALUE] = NULLSTR;
			valueobj[ATTRID_XREF] = CoreObj(newVal);
			long flags = valueobj[ATTRID_REGFLAGS];
			if(!(flags & RFLAG_HASVALUE)) valueobj[ATTRID_REGFLAGS] = flags | RFLAG_HASVALUE;
			markchg();
		} COMCATCH_IN_TRANSACTION(;)
}

STDMETHODIMP CMgaRegNode::get_SubNodes( VARIANT_BOOL virtuals, IMgaRegNodes **pVal) {
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			long dummy;
			stdext::hash_set<CComBSTRNoAt, CComBSTR_hashfunc> match;
			CoreObj s = fco->self;
			if(!s.IsFCO()) virtuals = VARIANT_FALSE;
			CREATEEXCOLLECTION_FOR(MgaRegNode,q);
			do {
				CoreObj vobj;
				vobj <<= findregvalueobj(s, mypath, dummy, false);
				if(!vobj)  continue;
				CoreObjs children = vobj[ATTRID_REGNOWNER+ATTRID_COLLECTION];

				ITERATE_THROUGH(children) {
					CComBSTR subpath(mypath);
					subpath.Append("/");
					CComBSTR path = ITER[ATTRID_NAME];
					COMTHROW(subpath.Append(path));
					if(virtuals) {
						if(match.find(path) != match.end()) continue;
						match.insert(path);
					}
					q->Add(fco->rpool.getpoolobj(subpath, fco, mgaproject));
				}
			} while(virtuals && (s = s[ATTRID_DERIVED]));
			if(virtuals) {
				metaref_type mref = fco->self[ATTRID_META];
				if(mref) {
					CComQIPtr<IMgaMetaBase> m(mgaproject->FindMetaRef(mref));
					CComPtr<IMgaMetaRegNode> rn;
					HRESULT hr = m->get_RegistryNode(mypath, &rn);
					CComPtr<IMgaMetaRegNodes> rns;
					if(hr == S_OK) {
						COMTHROW(hr = rn->get_RegistryNodes(&rns));
						MGACOLL_ITERATE(IMgaMetaRegNode, rns) {
							CComBSTR path;
							COMTHROW(MGACOLL_ITER->get_Name(&path));
							if(match.find(path) != match.end()) continue;
							CComBSTR subpath(mypath);
							subpath.Append("/");
							COMTHROW(subpath.Append(path));
							q->Add(fco->rpool.getpoolobj(subpath, fco, mgaproject));
						} MGACOLL_ITERATE_END;
					}
				}
			}
			*pVal = q.Detach();
		} COMCATCH(;)
}

STDMETHODIMP CMgaRegNode::get_SubNodeByName(BSTR name, IMgaRegNode **pVal) {
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CHECK_INSTRPAR(name);
      
			CComBSTR xpath(mypath);
			xpath.Append("/");
			COMTHROW(xpath.AppendBSTR(name));
			COMTHROW(fco->get_RegistryNode(xpath, pVal));
		} COMCATCH(;)
}

STDMETHODIMP CMgaRegNode::get_ParentNode(IMgaRegNode **pVal) {
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
      
			BSTR p = wcsrchr(mypath, '/');
			if(p) {
				CComBSTR xpath;
				xpath.Attach(::SysAllocStringLen(mypath,p-mypath));
				COMTHROW(fco->get_RegistryNode(xpath, pVal));
			}
		} COMCATCH(;);
}


STDMETHODIMP CMgaRegNode::Clear() {
		COMTRY_IN_TRANSACTION {
			fco->CheckWrite();
			long dummy;
			valueobj <<= findregvalueobj(fco->self, mypath, dummy, true);
			valueobj[ATTRID_REGNODEVALUE] = NULLSTR;
			long flags = valueobj[ATTRID_REGFLAGS];
			valueobj[ATTRID_REGFLAGS] = flags & ~RFLAG_HASVALUE;
			load_status = ATTSTATUS_INVALID;
			markchg();
		} COMCATCH_IN_TRANSACTION(;);
}

STDMETHODIMP CMgaRegNode::RemoveTree() {
	COMTRY_IN_TRANSACTION {
			fco->CheckWrite();
			long dummy;
			valueobj <<= findregvalueobj(fco->self, mypath, dummy, false);
			if(valueobj) {
				// lph: Pre-Notification PRE_STATUS (the registry node is being destroyed)
				CComBSTR desc = "REGISTRY,";
				desc.Append(mypath);
				desc.Append(",Removed");
				fco->PreNotify(OBJEVENT_PRE_STATUS, CComVariant(desc));
				//--------------------------------------------------------------------------
				RegistryChildrenRemove(valueobj);
				COMTHROW(valueobj->Delete());			
				load_status = ATTSTATUS_INVALID;
				markchg();
			}
	} COMCATCH_IN_TRANSACTION(;);
}


STDMETHODIMP CMgaPart::get_Model(struct IMgaModel **pVal ) {
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CComPtr<IMgaContainer> parent;
			COMTHROW(fco->GetParent(&parent));
			COMTHROW(parent.QueryInterface(pVal));
		} COMCATCH(;);
};

STDMETHODIMP CMgaPart::get_MetaAspect(struct IMgaMetaAspect **pVal ) { 
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CComPtr<IMgaMetaPart> part;
			part = CComQIPtr<IMgaMetaPart>(mgaproject->FindMetaRef(mref));
			COMTHROW(part->get_ParentAspect(pVal));
		} COMCATCH(;);
};

STDMETHODIMP CMgaPart::get_Meta(struct IMgaMetaPart **pVal ) { 
		COMTRY {
			fco->CheckRead();
		CHECK_OUTPTRPAR(pVal);
		*pVal = CComQIPtr<IMgaMetaPart>(mgaproject->FindMetaRef(mref)).Detach();
		} COMCATCH(;);
};

STDMETHODIMP CMgaPart::get_MetaRole(struct IMgaMetaRole **pVal ) { 
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			COMTHROW(fco->get_MetaRole(pVal));
		} COMCATCH(;);
}

 
STDMETHODIMP CMgaPart::get_FCO(struct IMgaFCO **pVal) { 
		COMTRY {
			CHECK_OUTPTRPAR(pVal);
			fco->getinterface(pVal); 
		} COMCATCH(;);
};

STDMETHODIMP CMgaPart::get_AccessMask(long *) { return E_MGA_NOT_IMPLEMENTED; }

STDMETHODIMP CMgaPart::GetGmeAttrs(BSTR *icon,long *x,long *y) { 
		COMTRY {
			fco->CheckRead();
			CComBSTR vval;
			if(icon) COMTHROW(get_RegistryValue(CComBSTR("Icon"), icon));
			if(x || y) {
				CComBSTR bb;
				COMTHROW(get_RegistryValue(CComBSTR("Position"), &bb));
				long ld, *lx = x?x:&ld, *ly = y?y:&ld;
				if(!bb) { *lx = -1; *ly = -1;  }
				else if(swscanf(bb,OLESTR("%ld,%ld"), lx, ly) != 2) COMTHROW(E_MGA_BAD_POSITIONVALUE);
			}
		} COMCATCH(;)
}

STDMETHODIMP CMgaPart::SetGmeAttrs(BSTR icon,long x,long y) { 
		COMTRY_IN_TRANSACTION {
			fco->CheckWrite();
			CComBSTR vval;
			if(icon != NULL) COMTHROW(put_RegistryValue(CComBSTR("Icon"), icon));
			if(x >= 0 && y >= 0) {
				OLECHAR bbc[40];
				swprintf(bbc, 40, OLESTR("%ld,%ld"), x, y);
				CComBSTR bb(bbc);
				COMTHROW(put_RegistryValue(CComBSTR("Position"), bb));
			}
		} COMCATCH_IN_TRANSACTION(;)
};


STDMETHODIMP CMgaPart::get_RegistryMode( VARIANT_BOOL *own) {  return E_MGA_NOT_IMPLEMENTED; }
STDMETHODIMP CMgaPart::put_RegistryMode( VARIANT_BOOL own) {  return E_MGA_NOT_IMPLEMENTED; }

STDMETHODIMP CMgaPart::get_RegistryNode( BSTR path,  IMgaRegNode **pVal) {  
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CHECK_INSTRPAR(path);
      
			CComBSTR xpath(regprefix);
			xpath.Append("/");
			COMTHROW(xpath.AppendBSTR(path));
			COMTHROW(fco->get_RegistryNode(xpath, pVal));
		} COMCATCH(;)
}

STDMETHODIMP CMgaPart::get_Registry(VARIANT_BOOL virtuals, IMgaRegNodes **pVal) {  
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPTRPAR(pVal);
			CComPtr<IMgaRegNode> p;
			COMTHROW(fco->get_RegistryNode(regprefix, &p));
			COMTHROW(p->get_SubNodes(virtuals, pVal));
		} COMCATCH(;);
}

STDMETHODIMP CMgaPart::get_RegistryValue( BSTR path,  BSTR *pVal) {  
		COMTRY {
			fco->CheckRead();
			CHECK_OUTPAR(pVal);
			CHECK_INSTRPAR(path);
			CComPtr<IMgaRegNode> p;
			COMTHROW(get_RegistryNode(path, &p));
			COMTHROW(p->get_Value(pVal));
		} COMCATCH(;)
}

STDMETHODIMP CMgaPart::put_RegistryValue( BSTR path,  BSTR newval) {  
		COMTRY_IN_TRANSACTION {
			fco->CheckWrite();
			CHECK_INSTRPAR(path);
			CHECK_INSTRPAR(newval);
			CComPtr<IMgaRegNode> p;
			COMTHROW(get_RegistryNode(path, &p));
			COMTHROW(p->put_Value(newval));
		} COMCATCH_IN_TRANSACTION(;)
}
	

CMgaPart::CMgaPart()	: prevptr(NULL), next(NULL), load_status(ATTSTATUS_INVALID) {	}
CMgaPart::~CMgaPart() {						// remove object from hash
		if(next) next->prevptr = prevptr;
		*prevptr = next;
}
void CMgaPart::Initialize(metaref_type mr, FCO *o, CMgaProject *p) {   // Throws!!!
		mref = mr;		
		fco = o;
		mgaproject = p;
		CComPtr<IMgaMetaAspect> meta;
		COMTHROW(get_MetaAspect(&meta));
		COMTHROW(meta->get_Name(&aspname));
		regprefix = "PartRegs/";
		COMTHROW(regprefix.Append(aspname));
}
*/

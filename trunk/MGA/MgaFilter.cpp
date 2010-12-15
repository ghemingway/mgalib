/*** Included Header Files ***/
#include "MgaFilter.h"
#include "MgaGeneric.h"
#include "../Core/CoreObject.h"


// --------------------------- MgaFilter --------------------------- //


#define MAXLEV 30

void MgaFilter::Init()
{
/*
	if (this->newLevel)
	{
		this->_levels = 0;
		for (OLECHAR *p = wcstok(level, L" "); p; p = wcstok(NULL,L" "))
		{
			int low, high, highincr = 0;
			OLECHAR c;
			switch(swscanf(p,L"%u%c%u", &low, &c, &high)) {
			case 0: if(swscanf(p,L"-%u", &high) != 1) COMTHROW(E_MGA_FILTERFORMAT);
					low = 0;
					break;
			case 1: high=low;
					break;
			case 2: if(c != L'-') COMTHROW(E_MGA_FILTERFORMAT);
					high= MAXLEV; 
					highincr =1;
			case 3: if(low > high) COMTHROW(E_MGA_FILTERFORMAT);
			}
			if(high > MAXLEV) COMTHROW(E_MGA_FILTERFORMAT);
			high += highincr;
			unsigned long mask = 1 << low;
			for(int i = low; i <= high; i++) {
				levels |= mask;
				mask <<= 1;
			}
		}
		this->_newLevel = false;
	}
	if(newobjtype) {
		objtypes = 0;
		for(OLECHAR *p = wcstok(objtype, L" "); p; p = wcstok(NULL,L" ")) {
			unsigned int code;
			if(swscanf(p,L"%u", &code) != 1) {
				const OLECHAR *c[] = {L"OBJTYPE_MODEL", L"OBJTYPE_ATOM", L"OBJTYPE_REFERENCE", L"OBJTYPE_CONNECTION", L"OBJTYPE_SET"};
				for(code = 0; code < DIM(c); code++) {
					if(!wcscmp(p,c[code])) break;
				}
				code += OBJTYPE_MODEL;  // offset of objtype
			}
			if(code < OBJTYPE_MODEL || code > OBJTYPE_SET) COMTHROW(E_MGA_FILTERFORMAT);
			objtypes |= 1 << code;
		}	
		newobjtype = false;
	}
	if(newkind) {
		kindnums.clear();
		kindnames.clear();
		for(OLECHAR *p = wcstok(kind, L" "); p; p = wcstok(NULL,L" ")) {
			unsigned int code;
			if(swscanf(p,L"#%u", &code) == 1) kindnums.insert(code);
			//else kindnames.insert(CComBSTR(p));// NAMESPACE logic inserted here:
			else {
				kindnames.insert( CComBSTR(p));
				kindnames.insert( mgaproject->prefixWNmspc( CComBSTR(p)));
			}
		}	
		newkind = false;
	}
	if(newrole) {
		rolenums.clear();
		rolenames.clear();
		for(OLECHAR *p = wcstok(role, L" "); p; p = wcstok(NULL,L" ")) {
			unsigned int code;
			if(swscanf(p,L"#%u", &code) == 1) rolenums.insert(code);
			else rolenames.insert(CComBSTR(p));
		}	
		newrole = false;
	}
	if(newname) {
		names.clear();
		MGA_TRACE("Names at init1");
		for(BSTRhash::iterator i = names.begin(); i!=names.end(); ++i) {
			MGA_TRACE("Names: %ls\n",*i);
		}

		wchar_t *finish = name + wcslen(name);
		bool inquotes = false;
		wchar_t *begin = name;
		wchar_t *end = wcschr(begin, L'\"');
		if (!end) {
			end = finish;
		}
		while (begin < finish) {
			*end = L'\0';
			if (inquotes) {
				names.insert(CComBSTR(begin));
				inquotes = false;
			}
			else {
				for(OLECHAR *p = wcstok(begin, L" "); p; p = wcstok(NULL,L" ")) {
					names.insert(CComBSTR(p));
				}	
				inquotes = true;
			}
			begin = end + 1;
			end = wcschr(begin, L'\"');
			if (!end) {
				end = finish;
			}
		}

		MGA_TRACE("Names at init");
		for(BSTRhash::iterator i = names.begin(); i!=names.end(); ++i) {
			MGA_TRACE("Names: %ls\n",*i);
		}
		newname = false;
	}
*/
}


const Result_t MgaFilter::SearchFCOs(CoreObject* &coreObject, std::list<MgaFCO*> &fcoList, const int &level)
{
	bool add = true;
	// Initialize the search
	this->Init();
	MetaObjIDPair idPair;
	coreObject->IDPair(idPair);

	if (idPair.metaID == DTID_FOLDER) add = false;
	if (add && this->_levels)
	{
		if ( (this->_levels & 1 << (level > MAXLEV+1 ? MAXLEV+1: level)) == 0) add = false;
	}
	if (add && this->_objTypes)
	{
		if ( (this->_objTypes & 1 << (idPair.metaID - DTID_BASE)) == 0) add = false;
	}
/*
	if(add && !names.empty()) {
		MGA_TRACE("Names at testbeg");
		for(BSTRhash::iterator i = names.begin(); i!=names.end(); ++i) {
			MGA_TRACE("Names: %ls\n",*i);
		}
		if(names.find(CComBSTR(coreObject[ATTRID_NAME])) == names.end()) add =false;
		MGA_TRACE("Names at testend");
		for(BSTRhash::iterator i = names.begin(); i!=names.end(); ++i) {
			MGA_TRACE("Names: %ls\n",*i);
		}
	}
	if(add && !(rolenums.empty() && rolenames.empty())) {
		if(coreObject.IsRootFCO()) add = false;
		else {
			long roleref = coreObject[ATTRID_ROLEMETA];
			if(rolenums.find(roleref) == rolenums.end()) {
				CComBSTR(b);
				COMTHROW(mgaproject->FindMetaRef(roleref)->get_Name(&b));
				if(rolenames.find(b) == rolenames.end()) add = false;
			}
		}
	}
	if(add && !(kindnums.empty() && kindnames.empty())) {
		long kindref = coreObject[ATTRID_META];
		if(kindnums.find(kindref) == kindnums.end()) {
			CComBSTR(b);
			COMTHROW(mgaproject->FindMetaRef(kindref)->get_Name(&b));
			if(kindnames.find(b) == kindnames.end()) add = false;
		}
	}
	if(add) {
		CComPtr<IMgaFCO> bb;
		ObjForCore(coreObject)->getinterface(&bb);
		retcoll->Add(bb);
	}
	level += 1;
	if((idPair.metaID == DTID_MODEL || idPair.metaID == DTID_FOLDER) && 
		(!levels || levels & 0xFFFFFFFF << ((level > MAXLEV+1 ? MAXLEV+1: level)))) {
		ITERATE_THROUGH(coreObject[ATTRID_FCOPARENT+ATTRID_COLLECTION]) {
			this->SearchFCOs(ITER, retcoll, level);
		}
	}
*/
	return S_OK;
}


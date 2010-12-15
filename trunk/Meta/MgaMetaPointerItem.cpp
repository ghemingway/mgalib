
#include "stdafx.h"
#include "MgaMetaPointerItem.h"
#include "MgaMetaBase.h"

// --------------------------- CMgaMetaPointerItem
STDMETHODIMP CMgaMetaPointerItem::get_Desc(BSTR *p)
{
	COMTRY {
		CComBSTR pn;
		COMTHROW( ComGetAttrValue(GetUnknown(), ATTRID_PTRITEMDESC, &pn));
		CComBSTR tn = truncateName( GetUnknown(), pn);
		*p = tn.Detach();
	} COMCATCH(;)
}

bool CMgaMetaPointerItem::CheckToken(CComBstrObj &token, bstr_const_iterator i, bstr_const_iterator &e)
{
	// the last character cannot be a space
	ASSERT( i != e && *(e-1) != L' ' );

	int len = token.Length();

	// return if token is longer or no token
	if( (e - i) < len || len <= 0 )
		return false;

	// return if not equal
	if( !std::equal(e - len, e, begin(token)) )
		return false;

	// skip those
	e -= len;

	// no more characters
	if( i == e )
		return true;

	// the previous character must be a space
	if( *(--e) != L' ' )
	{
		// set the orginal value
		e += len + 1;

		return false;
	}

	// eat the white spaces
	while( i != e && *(e-1) == L' ' )
		--e;

	return true;
}

bool CMgaMetaPointerItem::CheckPath(CCoreObjectPtr &self, pathitems_type &pathitems, bool global)
{
	CComBstrObj bstr;
	self.GetStringValue(ATTRID_PTRITEMDESC, PutOut(bstr));

#ifdef _DEBUG
	std::string s;
	CopyTo(bstr, s);

	PATH_TRACE("PointerItem \"%s\"", s.c_str());

	pathitems_type::iterator i = pathitems.begin();
	pathitems_type::iterator e = pathitems.end();
	while( i != e )
	{
		CopyTo( (*i).terminal_name, s );
		PATH_TRACE(" (term \"%s\" ", s.c_str());

		CopyTo( (*i).continual_name, s );
		PATH_TRACE("cont \"%s\") ", s.c_str());

		++i;
	}

	if( global )
		PATH_TRACE("(global) ");

	PATH_TRACE("returns ");
#endif

	// we will check the lists and path from the back

	// bstr iterators
	bstr_const_iterator bi = begin(bstr);
	bstr_const_iterator be = end(bstr);

	// eat the white spaces
	while( bi != be && *(be-1) == L' ' )
		--be;

	// pathitem iterators
	pathitems_type::iterator pi = pathitems.begin();
	pathitems_type::iterator pe = pathitems.end();

	while( bi != be && pi != pe )
	{
		// get the next pathitem
		--pe;

		bstr_const_iterator bf = be;
		if( global && CheckToken(pe->terminal_name, bi, be) )
		{
			if( bi == be )
			{
				PATH_TRACE("true\n");
				return true;
			}

			be = bf;
		}
		ASSERT( be == bf );

		if( !CheckToken(pe->continual_name, bi, be) )
		{
			PATH_TRACE("false\n");
			return false;
		}

		ASSERT( be < bf );
	}

	PATH_TRACE( (!global && bi == be && pi == pe) ? "true\n" : "false\n");
	return !global && bi == be && pi == pe;
}

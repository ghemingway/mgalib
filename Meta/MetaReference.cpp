/*** Included Header Files ***/
#include "MetaReference.h"
#include "MetaPointerSpec.h"


// --------------------------- MetaReference --------------------------- //


const Result_t MetaReference::GetRefSpec(MetaPointerSpec* &pointerSpec) const throw()
{
	ASSERT(false);
	return S_OK;
}


const Result_t MetaReference::CheckPath(const std::string &path, bool &valid) const throw()
{
//	pathitems_type pathitems;
//	metaproject->CreatePathItems(begin(path), end(path), pathitems);
//	CCoreObjectPtr self(GetUnknown());
//	bool valid = CMgaMetaPointerSpec::CheckPath(self, pathitems, true);
//	*p = valid ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}


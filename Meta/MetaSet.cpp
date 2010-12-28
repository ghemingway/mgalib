/*** Included Header Files ***/
#include "MetaSet.h"
#include "MetaPointerSpec.h"


// --------------------------- MetaSet --------------------------- //

/*
void MetaSet::Traverse(MetaProject *metaProject, CoreObject* &coreObject)
{
	ASSERT( metaProject != NULL );
	ASSERT( coreObject != NULL );
	MetaFCO::Traverse(metaProject, coreObject);
	MetaPointerSpec::Traverse(metaProject, coreObject);
}
*/

const Result_t MetaSet::CheckPath(const std::string &path, bool &flag) const throw()
{
	ASSERT( this->_metaProject != NULL );
//	pathitems_type pathitems;
//	metaproject->CreatePathItems(begin(path), end(path), pathitems);
//	CCoreObjectPtr self(GetUnknown());
//	bool valid = CMgaMetaPointerSpec::CheckPath(self, pathitems, false);
//	*p = valid ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}


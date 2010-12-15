
#include "stdafx.h"
#include "MgaMetaConnJoint.h"
#include "MgaMetaPointerSpec.h"

// --------------------------- CMgaMetaConnJoint

// ------- Methods

void CMgaMetaConnJoint::Traverse(CMgaMetaProject *metaproject, CCoreObjectPtr &me)
{
	ASSERT( metaproject != NULL );
	ASSERT( me != NULL );

	TraverseCollection(metaproject, me, ATTRID_PTRSPECS_COLL);
}

bool CMgaMetaConnJoint::CheckPaths(CCoreObjectPtr &self, jointpaths_type &jointpaths)
{
	ASSERT( self != NULL );

	jointpaths_type::iterator i = jointpaths.begin();
	jointpaths_type::iterator e = jointpaths.end();
	while( i != e )
	{
		CCoreObjectPtr spec;

		if( FAILED(ComGetCollValueByName(i->ptrspec_name,
				self, ATTRID_PTRSPECS_COLL, ATTRID_PTRSPECNAME, PutOut(spec))) )
			return false;

		ASSERT( spec != NULL );

		if( !CMgaMetaPointerSpec::CheckPath(spec, i->pathitems, false) )
			return false;

		++i;
	}

	return true;
}

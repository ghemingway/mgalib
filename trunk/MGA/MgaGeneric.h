#ifndef __MGA_GENERIC_H__
#define __MGA_GENERIC_H__


/*** Included Header Files ***/
#include "Core/CoreMetaProject.h"
#include "Core/CoreMetaObject.h"



/*** Public Method Declarations ***/
const Result_t CreateMGACoreMetaProject(const bool &v2, CoreMetaProject* &metaProject);


// DATA
#define ATTRID_REF_MIN					500
#define ATTRID_REF_MAX					699
#define ATTRID_LINKREF_MIN				600

#define ATTRID_PARADIGM					400
#define ATTRID_META						401
#define ATTRID_PARENT					602
#define ATTRID_FPARENT					ATTRID_PARENT
#define ATTRID_FCOPARENT				ATTRID_PARENT
#define ATTRID_ROLEMETA					404
#define ATTRID_REFERENCE				505
#define ATTRID_XREF						ATTRID_FCOREFATTR
#define ATTRID_GUID						406
#define ATTRID_PARGUID					407
#define ATTRID_RELID					408
#define ATTRID_LASTRELID				409
#define ATTRID_FILESTATUS				410
#define ATTRID_GUID1					411
#define ATTRID_GUID2					412
#define ATTRID_GUID3					413
#define ATTRID_GUID4					414
#define ATTRID_DERIVED					510
#define ATTRID_SEGREF					511
#define ATTRID_SETMEMBER				612 
#define ATTRID_CONNSEG					613
#define ATTRID_CONNROLE					614
#define ATTRID_PERMISSIONS				415
#define ATTRID_SEGORDNUM				416
#define ATTRID_MASTEROBJ				517
#define ATTRID_REFASPECT				418
#define ATTRID_MGAVERSION				419
#define ATTRID_ATTRPARENT				620
// NUMBER MAGIC!!!! keep these as a contigious block of numbers:
#define ATTRID_ATTRTYPESBASE			420		// Not used in MetaAttrs
#define ATTRID_STRATTR					421
#define ATTRID_INTATTR					422
#define ATTRID_FLOATATTR				423
#define ATTRID_BOOLATTR					424
#define ATTRID_FCOREFATTR				525
// contigious block ends
#define	ATTRID_CDATE					430
#define	ATTRID_MDATE					431
#define	ATTRID_CREATOR					432
#define	ATTRID_EXTDATA					433
#define	ATTRID_VERSION					434
#define	ATTRID_PARVERSION				435
#define	ATTRID_CONSTROCL				450
#define	ATTRID_CONSTRPRIORITY			451
#define	ATTRID_CONSTROWNER				652
#define	ATTRID_REGNOWNER				655
#define	ATTRID_REGNODENAME				456
#define	ATTRID_REGNODEVALUE				457
#define	ATTRID_REGFLAGS					458


// OBJECT ID'S
#define DTID_ROOT						1
// NUMBER MAGIC!!!! keep these as a contigious block of numbers:
#define DTID_BASE						100				
#define DTID_MODEL						101
#define DTID_ATOM						102
#define DTID_REFERENCE					103	
#define DTID_CONNECTION					104
#define DTID_SET						105
#define DTID_FOLDER						106
// contigious block ends
#define DTID_CONNROLE					107	
#define DTID_CONNROLESEG				108		
// NUMBER MAGIC!!!! keep these as a contigious block of numbers:
#define DTID_ATTRTYPESBASE				110   // never created
#define DTID_STRATTR					111	
#define DTID_INTATTR					112	
#define DTID_FLOATATTR					113	
#define DTID_BOOLATTR					114	
#define DTID_REFATTR					115
// contigious block ends
#define DTID_CONSTRAINT					120	
#define DTID_REGNODE					121	
#define DTID_SETNODE					122	

#define VALTYPE_METAREF					VALTYPE_LONG
#define VALTYPE_DATE					VALTYPE_STRING
#define VALTYPE_BOOL					VALTYPE_LONG

#define PRIM_PARTTYP					1
#define SEC_PARTTYP						2
#define LINK_PARTTYP					4
#define METAREF_NULL					0
#define INSTANCE_FLAG					1
#define LIBRARY_FLAG					2 
#define LIBROOT_FLAG					4 
#define EXEMPT_FLAG						8
#define READONLY_FLAG					16

// the maximum numeric extension assigned to new objects;
#define NEWNAME_MAXNUM					10000


#endif // __MGA_GENERIC_H__



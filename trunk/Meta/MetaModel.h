#ifndef __META_MODEL_H__
#define __META_MODEL_H__


/*** Included Header Files ***/
#include "MetaFCO.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Included Header Files ***/
class MetaRole;
class MetaAspect;


// --------------------------- MetaModel --------------------------- //


class MetaModel : public MetaFCO
{
private:
	MetaModel();											//!< Deny access to default constructor
	MetaModel(const MetaModel &);							//!< Deny access to copy constructor
	MetaModel& operator=(const MetaModel &);				//!< Deny access to equals operator

public:
	static void Traverse(MetaProject *metaProject, CoreObject* &coreObject);

	const Result_t GetDefinedFCOs(std::list<MetaFCO*> &fcoList) const throw();
	const Result_t GetRoles(std::list<MetaRole*> &roleList) const throw();
	const Result_t GetAspects(std::list<MetaAspect*> &aspectList) const throw();
	const Result_t GetDefinedFCOByName(const std::string &name, bool &inScope, MetaFCO* &metaFCO) const throw();
	const Result_t GetRoleByName(const std::string &name, MetaRole* &metaRole) const throw();
	const Result_t GetAspectByName(const std::string &name, MetaAspect* &metaAspect) const throw();
	const Result_t LegalConnectionRoles(const std::string &paths, std::list<MetaRole*> &roleList) const throw();
	const Result_t LegalReferenceRoles(const std::string &path, std::list<MetaRole*> &roleList) const throw();
	const Result_t LegalSetRoles(const std::string &path, std::list<MetaRole*> &roleList) const throw();
	const Result_t LegalRoles(MetaFCO* &kind, std::list<MetaRole*> &roleList) const throw();
//	const Result_t DefineModel)(IMgaMetaModel **p)					{ return ComDefineBase(this, METAID_METAMODEL, ATTRID_DEFFCO_PTR, p); }
//	const Result_t DefineAtom)(IMgaMetaAtom **p)					{ return ComDefineBase(this, METAID_METAATOM, ATTRID_DEFFCO_PTR, p); }
//	const Result_t DefineReference)(IMgaMetaReference **p)			{ return ComDefineBase(this, METAID_METAREFERENCE, ATTRID_DEFFCO_PTR, p); }
//	const Result_t DefineSet)(IMgaMetaSet **p)						{ return ComDefineBase(this, METAID_METASET, ATTRID_DEFFCO_PTR, p); }
//	const Result_t DefineConnection)(IMgaMetaConnection **p)		{ return ComDefineBase(this, METAID_METACONNECTION, ATTRID_DEFFCO_PTR, p); }
//	const Result_t CreateAspect)(IMgaMetaAspect **p)				{ return ComDefineBase(this, METAID_METAASPECT, ATTRID_ASPECTS_COLL, p); }
//	const Result_t CreateRole)(IMgaMetaFCO *kind, IMgaMetaRole **p);
};


/*** End of MGA Namespace ***/
}


#endif // __META_MODEL_H__


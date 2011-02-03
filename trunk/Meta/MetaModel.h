#ifndef __META_MODEL_H__
#define __META_MODEL_H__


/*** Included Header Files ***/
#include "MetaFCO.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Included Header Files ***/
class MetaRole;
class MetaAspect;
class MetaReference;
class MetaSet;
class MetaConnection;


// --------------------------- MetaModel --------------------------- //


class MetaModel : public MetaFCO
{
private:
	MetaModel();											//!< Deny access to default constructor
	MetaModel(const MetaModel &);							//!< Deny access to copy constructor
	MetaModel& operator=(const MetaModel &);				//!< Deny access to equals operator

	friend class MetaBase;
	MetaModel(CoreObject &coreObject, MetaProject* const &metaProject) : ::MetaFCO(coreObject, metaProject) { }

public:
	virtual ~MetaModel() { }

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
	const Result_t CreateModel(MetaModel* &metaModel) throw();
	const Result_t CreateAtom(MetaAtom* &metaAtom) throw();
	const Result_t CreateReference(MetaReference* &metaRef) throw();
	const Result_t CreateSet(MetaSet* &metaSet) throw();
	const Result_t CreateConnection(MetaConnection* &metaConn) throw();
	const Result_t CreateAspect(MetaAspect* &aspect) throw();
	const Result_t CreateRole(MetaFCO* &kind, MetaRole* &metaRole) throw();
};


/*** End of MGA Namespace ***/
}


#endif // __META_MODEL_H__


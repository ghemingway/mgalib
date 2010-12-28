#ifndef __META_REGNODE_H__
#define __META_REGNODE_H__


/*** Included Header Files ***/
#include "../MGA/MgaTypes.h"


/*** Namespace Declaration ***/
namespace MGA {


/*** Class Predefinitions ***/
class MetaRegNode;
class MetaProject;


// --------------------------- MetaRegNodes --------------------------- //


class MetaRegNodes
{
public:
	static const Result_t GetRegistryNode(CoreObject &coreObject, const std::string &path, MetaRegNode* &regNode);
	static const Result_t GetRegistryValue(CoreObject &coreObject, const std::string & path, std::string &value);
	static const Result_t SetRegistryValue(CoreObject &coreObject, const std::string & path, const std::string &value);
};


// --------------------------- MetaRegNode --------------------------- //


class MetaRegNode : public MetaRegNodes
{
public:
	inline const Result_t GetRegistryNode(const std::string &path, MetaRegNode* &regNode) const throw();
//		{ return MetaRegNodes::GetRegistryNode(this, path, regNode); }
	inline const Result_t GetRegistryValue(const std::string &path, std::string value) const throw();
//		{ return MetaRegNodes::GetRegistryValue(this, path, value); }
	const Result_t GetRegistryNodes(std::list<MetaRegNode*> &nodeList) const throw();
	const Result_t GetName(std::string &name) const throw();
	const Result_t GetValue(std::string &value) const throw();
//	const Result_t SetRegistryValue(const std::string &path, BSTR p){ return MetaRegNodes::SetRegistryValue(this, path, p); }
//	const Result_t SetValue(const std::string &value)				{ return ComPutAttrValue(this, ATTRID_VALUE, value); }
//	const Result_t Delete(void);

};


/*** End of MGA Namespace ***/
}


#endif // __META_REGNODE_H__


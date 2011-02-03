#ifndef __PARSER_XMPPARSER_H__
#define __PARSER_XMPPARSER_H__


/*** Included Header Files ***/
#include "../MGA/MgaTypes.h"


/*** Xerces Included Header Files ***/
#ifdef _WIN32
#include <xercesc/util/Xerces_autoconf_config.msvc.hpp>
#define XERCES_AUTOCONFIG_CONFIG_HPP
#endif
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <sys/stat.h>
#include <stdexcept>
using namespace xercesc;


/*** Namespace Declaration ***/
namespace MGA {
	
	
/*** Class Predefinitions ***/
class MetaProject;
class MetaBase;
class MetaFCO;
class MetaFolder;
class MetaConstraint;
class MetaEnumItem;
class MetaAtom;
class MetaAttribute;
class MetaSet;
class MetaPointerItem;
class MetaPointerSpec;
class IMetaPointerSpec;
class MetaConnection;
class MetaConnJoint;
class MetaReference;
class MetaModel;
class MetaRole;
class MetaAspect;
class MetaPart;


/*** Type Definitions ***/
//None
	
	
// --------------------------- XmpParser --------------------------- //


class XmpParser
{
public:
	// Get XML strings ready
	static XMLCh* TAG_Folder;
	static XMLCh* TAG_Comment;
	static XMLCh* TAG_Author;
	static XMLCh* TAG_Constraint;
	static XMLCh* TAG_DispName;
	static XMLCh* TAG_AttrDef;
	static XMLCh* TAG_RegNode;
	static XMLCh* TAG_EnumItem;
	static XMLCh* TAG_Atom;
	static XMLCh* TAG_Set;
	static XMLCh* TAG_PointerSpec;
	static XMLCh* TAG_PointerItem;
	static XMLCh* TAG_Connection;
	static XMLCh* TAG_ConnJoint;
	static XMLCh* TAG_Reference;
	static XMLCh* TAG_Model;
	static XMLCh* TAG_Role;
	static XMLCh* TAG_Aspect;
	static XMLCh* TAG_Part;
	static XMLCh* ATTR_Name;
	static XMLCh* ATTR_Uuid;
	static XMLCh* ATTR_Version;
	static XMLCh* ATTR_CDate;
	static XMLCh* ATTR_MDate;
	static XMLCh* ATTR_Metaref;
	static XMLCh* ATTR_Subfolders;
	static XMLCh* ATTR_RootObjects;
	static XMLCh* ATTR_EventMask;
	static XMLCh* ATTR_Depth;
	static XMLCh* ATTR_Priority;
	static XMLCh* ATTR_Type;
	static XMLCh* ATTR_ValueType;
	static XMLCh* ATTR_DefValue;
	static XMLCh* ATTR_Value;
	static XMLCh* ATTR_Aliasenabled;
	static XMLCh* ATTR_Attributes;
	static XMLCh* ATTR_Desc;
	static XMLCh* ATTR_Kind;
	static XMLCh* ATTR_Primary;
	static XMLCh* ATTR_Linked;
	static XMLCh* ATTR_Min;
	static XMLCh* ATTR_Max;
	static XMLCh* ATTR_Kindaspect;
	static XMLCh* ATTR_Viewable;

private:
	static std::map<std::string,std::string> metaRefMap;
	static MetaFolder* ParseFolder(DOMElement* element, MetaFolder* parentFolder);
	static std::string ParseComment(DOMElement* element);
	static std::string ParseAuthor(DOMElement* element);
	static MetaConstraint* ParseConstraint(DOMElement* element, MetaBase* metaBase);
	static std::string ParseDisplayName(DOMElement* element);
	static MetaAttribute* ParseAttribute(DOMElement* element, MetaFolder* metaFolder, MetaFCO* metaFCO);
	static void ParseRegNode(DOMElement* element, std::string &key, std::string &value);
	static MetaEnumItem* ParseEnumItem(DOMElement* element, MetaAttribute* metaAttribute);
	static MetaAtom* ParseAtom(DOMElement* element, MetaFolder* metaFolder, MetaFCO* metaFCO);
	static MetaSet* ParseSet(DOMElement* element, MetaFolder* metaFolder, MetaFCO* metaFCO);
	static MetaPointerSpec* ParsePointerSpec(DOMElement* element, MetaConnJoint* metaConnJoint);
	static IMetaPointerSpec* ParseIPointerSpec(DOMElement* element, IMetaPointerSpec* iMetaPointerSpec);
	static MetaPointerItem* ParsePointerItem(DOMElement* element, IMetaPointerSpec* metaPointerSpec);
	static MetaConnection* ParseConnection(DOMElement* element, MetaFolder* metaFolder, MetaModel* metaModel);
	static MetaConnJoint* ParseConnJoint(DOMElement* element, MetaConnection* metaConnection);
	static MetaReference* ParseReference(DOMElement* element, MetaFolder* metaFolder, MetaModel* metaModel);
	static MetaModel* ParseModel(DOMElement* element, MetaFolder* metaFolder, MetaModel* metaModel);
	static MetaRole* ParseRole(DOMElement* element, MetaModel* metaModel);
	static MetaAspect* ParseAspect(DOMElement* element, MetaModel* metaModel);
	static MetaPart* ParsePart(DOMElement* element, MetaAspect* metaAspect);
	static void CleanUp(void);
	
public:
	static const Result_t Parse(const std::string &xmpFile, const std::string &mtaFile) throw();
};


/*** End of MGA Namespace ***/
}


#endif // __PARSER_XMPPARSER_H__


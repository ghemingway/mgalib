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
class MetaFolder;
class MetaConstraint;
class MetaRegNode;
class MetaEnumItem;
class MetaAtom;
class MetaSet;
class MetaPointerSpec;
class MetaConnection;
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
private:
	// Get XML strings ready
	static XMLCh* TAG_Folder;
	static XMLCh* TAG_Comment;
	static XMLCh* TAG_Author;
	static XMLCh* TAG_Constraint;
	static XMLCh* TAG_Dispname;
	static XMLCh* TAG_Attrdef;
	static XMLCh* TAG_Regnode;
	static XMLCh* TAG_Enumitem;
	static XMLCh* TAG_Atom;
	static XMLCh* TAG_Set;
	static XMLCh* TAG_Pointerspec;
	static XMLCh* TAG_Pointeritem;
	static XMLCh* TAG_Connection;
	static XMLCh* TAG_Connjoint;
	static XMLCh* TAG_Reference;
	static XMLCh* TAG_Model;
	static XMLCh* TAG_Role;
	static XMLCh* TAG_Aspect;
	static XMLCh* TAG_Part;
	static XMLCh* ATTR_Name;
	static XMLCh* ATTR_Uuid;
	static XMLCh* ATTR_CDate;
	static XMLCh* ATTR_MDate;
	static XMLCh* ATTR_Metaref;
	static XMLCh* ATTR_Subfolders;
	static XMLCh* ATTR_Rootobjects;
	static XMLCh* ATTR_Eventmask;
	static XMLCh* ATTR_Depth;
	static XMLCh* ATTR_Priority;
	static XMLCh* ATTR_Type;
	static XMLCh* ATTR_Valuetype;
	static XMLCh* ATTR_Defvalue;
	static XMLCh* ATTR_Value;
	static XMLCh* ATTR_Attributes;
	static XMLCh* ATTR_Desc;
	static XMLCh* ATTR_Kind;
	static XMLCh* ATTR_Primary;
	static XMLCh* ATTR_Linked;

	static MetaFolder* ParseFolder(DOMElement* element);
	static std::string ParseComment(DOMElement* element);
	static std::string ParseAuthor(DOMElement* element);
	static MetaConstraint* ParseConstraint(DOMElement* element);
	static std::string ParseDispname(DOMElement* element);
	static void ParseAttrdef(DOMElement* element);
	static MetaRegNode* ParseRegnode(DOMElement* element);
	static MetaEnumItem* ParseEnumitem(DOMElement* element);
	static MetaAtom* ParseAtom(DOMElement* element);
	static MetaSet* ParseSet(DOMElement* element);
	static MetaPointerSpec* ParsePointerspec(DOMElement* element);
	static void ParsePointeritem(DOMElement* element);
	static MetaConnection* ParseConnection(DOMElement* element);
	static void ParseConnjoint(DOMElement* element);
	static MetaReference* ParseReference(DOMElement* element);
	static MetaModel* ParseModel(DOMElement* element);
	static MetaRole* ParseRole(DOMElement* element);
	static MetaAspect* ParseAspect(DOMElement* element);
	static MetaPart* ParsePart(DOMElement* element);
	static void CleanUp(void);
	
public:
	static const Result_t Parse(const std::string &xmpFile, const std::string &mtaFile) throw();
};


/*** End of MGA Namespace ***/
}


#endif // __PARSER_XMPPARSER_H__


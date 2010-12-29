/*** Included Header Files ***/
#include "XmpParser.h"
#include "../Meta/MetaProject.h"


XMLCh* XmpParser::TAG_Folder = NULL;
XMLCh* XmpParser::TAG_Comment = NULL;
XMLCh* XmpParser::TAG_Author = NULL;
XMLCh* XmpParser::TAG_Constraint = NULL;
XMLCh* XmpParser::TAG_Dispname = NULL;
XMLCh* XmpParser::TAG_Attrdef = NULL;
XMLCh* XmpParser::TAG_Regnode = NULL;
XMLCh* XmpParser::TAG_Enumitem = NULL;
XMLCh* XmpParser::TAG_Atom = NULL;
XMLCh* XmpParser::TAG_Set = NULL;
XMLCh* XmpParser::TAG_Pointerspec = NULL;
XMLCh* XmpParser::TAG_Pointeritem = NULL;
XMLCh* XmpParser::TAG_Connection = NULL;
XMLCh* XmpParser::TAG_Connjoint = NULL;
XMLCh* XmpParser::TAG_Reference = NULL;
XMLCh* XmpParser::TAG_Model = NULL;
XMLCh* XmpParser::TAG_Role = NULL;
XMLCh* XmpParser::TAG_Aspect = NULL;
XMLCh* XmpParser::TAG_Part = NULL;
XMLCh* XmpParser::ATTR_Name = NULL;
XMLCh* XmpParser::ATTR_Uuid = NULL;
XMLCh* XmpParser::ATTR_CDate = NULL;
XMLCh* XmpParser::ATTR_MDate = NULL;
XMLCh* XmpParser::ATTR_Metaref = NULL;
XMLCh* XmpParser::ATTR_Subfolders = NULL;
XMLCh* XmpParser::ATTR_Rootobjects = NULL;
XMLCh* XmpParser::ATTR_Eventmask = NULL;
XMLCh* XmpParser::ATTR_Depth = NULL;
XMLCh* XmpParser::ATTR_Priority = NULL;
XMLCh* XmpParser::ATTR_Type = NULL;
XMLCh* XmpParser::ATTR_Valuetype = NULL;
XMLCh* XmpParser::ATTR_Defvalue = NULL;
XMLCh* XmpParser::ATTR_Value = NULL;
XMLCh* XmpParser::ATTR_Attributes = NULL;
XMLCh* XmpParser::ATTR_Desc = NULL;
XMLCh* XmpParser::ATTR_Kind = NULL;
XMLCh* XmpParser::ATTR_Primary = NULL;
XMLCh* XmpParser::ATTR_Linked = NULL;


// --------------------------- Public XmpParser Methods --------------------------- //


const Result_t XmpParser::Parse(const std::string &xmpFile, const std::string &mtaFile) throw()
{
	MetaProject* metaProject = NULL;

	// Initialize Xerces infrastructure
	XMLPlatformUtils::Initialize();

	// Get XML strings ready
	XmpParser::TAG_Folder		= XMLString::transcode("folder");
	XmpParser::TAG_Comment		= XMLString::transcode("comment");
	XmpParser::TAG_Author		= XMLString::transcode("author");
	XmpParser::TAG_Constraint	= XMLString::transcode("constraint");
	XmpParser::TAG_Dispname		= XMLString::transcode("dispname");
	XmpParser::TAG_Attrdef		= XMLString::transcode("attrdef");
	XmpParser::TAG_Regnode		= XMLString::transcode("regnode");
	XmpParser::TAG_Enumitem		= XMLString::transcode("enumitem");
	XmpParser::TAG_Atom			= XMLString::transcode("atom");
	XmpParser::TAG_Set			= XMLString::transcode("set");
	XmpParser::TAG_Pointerspec	= XMLString::transcode("pointerspec");
	XmpParser::TAG_Pointeritem	= XMLString::transcode("pointeritem");
	XmpParser::TAG_Connection	= XMLString::transcode("connection");
	XmpParser::TAG_Connjoint	= XMLString::transcode("connjoint");
	XmpParser::TAG_Reference	= XMLString::transcode("reference");
	XmpParser::TAG_Model		= XMLString::transcode("model");
	XmpParser::TAG_Role			= XMLString::transcode("role");
	XmpParser::TAG_Aspect		= XMLString::transcode("aspect");
	XmpParser::TAG_Part			= XMLString::transcode("part");
	XmpParser::ATTR_Name		= XMLString::transcode("name");
	XmpParser::ATTR_Uuid		= XMLString::transcode("guid");
	XmpParser::ATTR_CDate		= XMLString::transcode("cdate");
	XmpParser::ATTR_MDate		= XMLString::transcode("mdate");
	XmpParser::ATTR_Metaref		= XMLString::transcode("metaref");
	XmpParser::ATTR_Subfolders	= XMLString::transcode("subfolders");
	XmpParser::ATTR_Rootobjects	= XMLString::transcode("rootobjects");
	XmpParser::ATTR_Eventmask	= XMLString::transcode("eventmask");
	XmpParser::ATTR_Depth		= XMLString::transcode("depth");
	XmpParser::ATTR_Priority	= XMLString::transcode("priority");
	XmpParser::ATTR_Type		= XMLString::transcode("type");
	XmpParser::ATTR_Valuetype	= XMLString::transcode("valuetype");
	XmpParser::ATTR_Defvalue	= XMLString::transcode("defvalue");
	XmpParser::ATTR_Value		= XMLString::transcode("value");
	XmpParser::ATTR_Attributes	= XMLString::transcode("attributes");
	XmpParser::ATTR_Desc		= XMLString::transcode("desc");
	XmpParser::ATTR_Kind		= XMLString::transcode("kind");
	XmpParser::ATTR_Primary		= XMLString::transcode("primary");
	XmpParser::ATTR_Linked		= XMLString::transcode("linked");

	// Configure DOM parser.
	XercesDOMParser* xmpParser = new XercesDOMParser();
	ASSERT( xmpParser != NULL );
	xmpParser->setValidationScheme(XercesDOMParser::Val_Never);
	xmpParser->setDoNamespaces(false);
	xmpParser->setDoSchema(false);
	xmpParser->setLoadExternalDTD(false);

	// Try parsing the file
	try
	{
		xmpParser->parse( xmpFile.c_str() );
		// no need to free this pointer - owned by the parent parser object
		xercesc::DOMDocument* xmlDoc = xmpParser->getDocument();
		// Get the top-level element: Name is "paradigm".
		DOMElement* elementRoot = xmlDoc->getDocumentElement();
		if( !elementRoot )
		{
			// Clean up all of the XML strings
			XmpParser::CleanUp();
			return E_XMLPARSER;
		}

		// Get the paradigm name
		const XMLCh* xmlch = elementRoot->getAttribute(XmpParser::ATTR_Name);
		char* tmpChar = XMLString::transcode(xmlch);
		std::string paradigmName = tmpChar;
		XMLString::release(&tmpChar);

		// Get the Uuid
		xmlch = elementRoot->getAttribute(XmpParser::ATTR_Uuid);
		tmpChar = XMLString::transcode(xmlch);
		std::string uuidString = tmpChar;
		XMLString::release(&tmpChar);

		// Get the cdate
		xmlch = elementRoot->getAttribute(XmpParser::ATTR_CDate);
		tmpChar = XMLString::transcode(xmlch);
		std::string cdate = tmpChar;
		XMLString::release(&tmpChar);

		// Get the mdate
		xmlch = elementRoot->getAttribute(XmpParser::ATTR_MDate);
		tmpChar = XMLString::transcode(xmlch);
		std::string mdate = tmpChar;
		XMLString::release(&tmpChar);		

		// Create the MetaProject
		// ...

		// Look one level nested within "paradigm"
		DOMNodeList* children = elementRoot->getChildNodes();
		const XMLSize_t nodeCount = children->getLength();
		// For all nodes, children of "paradigm" in the XML tree.
		for(XMLSize_t i = 0; i < nodeCount; ++i)
		{
			DOMNode* currentNode = children->item(i);
			if( currentNode->getNodeType() &&  // true is not NULL
			   currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
			{
				// Found node which is an Element. Re-cast node as element
				DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
				if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Folder) )
				{
					MetaFolder* metaFolder = XmpParser::ParseFolder(currentElement);
					// Look for possible errors
					if (metaFolder == NULL)
					{
						// Clean up all of the XML strings
						XmpParser::CleanUp();
						return E_XMLPARSER;
					}
				}
				else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Comment) )
				{
					std::string comment = XmpParser::ParseComment(currentElement);
				}
				else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Author) )
				{
					std::string author = XmpParser::ParseAuthor(currentElement);
				}
				else
				{
					ASSERT(false);
				}

			}
		}
	}
	catch( xercesc::XMLException& e )
	{
		char* message = xercesc::XMLString::transcode( e.getMessage() );
		std::cout << "Error parsing file: " << message << std::endl;
		XMLString::release( &message );
		// Delete the metaProject (if it exists)
		if (metaProject != NULL) delete metaProject;
		// Clean up all of the XML strings
		XmpParser::CleanUp();
		return E_INVALID_USAGE;
	}
	// All must be OK
	XmpParser::CleanUp();
	return S_OK;
}


MetaFolder* XmpParser::ParseFolder(DOMElement* element)
{
	// Get the folder name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string folderName = tmpChar;
	XMLString::release(&tmpChar);

	// Get the metaRef
	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
	tmpChar = XMLString::transcode(xmlch);
	std::string metaRefStr = tmpChar;
	XMLString::release(&tmpChar);

	// Get the subfolders
	xmlch = element->getAttribute(XmpParser::ATTR_Subfolders);
	tmpChar = XMLString::transcode(xmlch);
	std::string subfoldersStr = tmpChar;
	XMLString::release(&tmpChar);

	// Get the rootobjects
	xmlch = element->getAttribute(XmpParser::ATTR_Rootobjects);
	tmpChar = XMLString::transcode(xmlch);
	std::string rootObjectsStr = tmpChar;
	XMLString::release(&tmpChar);

	// Now create the MetaFolder
	MetaFolder* metaFolder = NULL;
	// ...

	// Look one level nested within "folder"
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children of folder in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Constraint) )
			{
				MetaConstraint* metaConstraint = XmpParser::ParseConstraint(currentElement);
				// Look for possible errors
				if (metaConstraint == NULL)
				{
					// Clean up all of the XML strings
					XmpParser::CleanUp();
					return NULL;
				}
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Attrdef) )
			{
				XmpParser::ParseAttrdef(currentElement);
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Atom) )
			{
				MetaAtom* metaAtom = XmpParser::ParseAtom(currentElement);
				// Look for possible errors
				if (metaAtom == NULL)
				{
					// Clean up all of the XML strings
					XmpParser::CleanUp();
					return NULL;
				}
			}
			else
			{
				ASSERT(false);
			}
			
		}
	}
	// All done, return the folder
	return metaFolder;
}


std::string XmpParser::ParseComment(DOMElement* element)
{
	return "";
}


std::string XmpParser::ParseAuthor(DOMElement* element)
{
	return "";
}


MetaConstraint* XmpParser::ParseConstraint(DOMElement* element)
{
	// Get the name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string constraintName = tmpChar;
	XMLString::release(&tmpChar);

	// Get the metaRef
	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
	tmpChar = XMLString::transcode(xmlch);
	std::string metaRefStr = tmpChar;
	XMLString::release(&tmpChar);

	// Get the attributes
	xmlch = element->getAttribute(XmpParser::ATTR_Attributes);
	tmpChar = XMLString::transcode(xmlch);
	std::string attributesStr = tmpChar;
	XMLString::release(&tmpChar);

	// Get the eventmask
	xmlch = element->getAttribute(XmpParser::ATTR_Eventmask);
	tmpChar = XMLString::transcode(xmlch);
	std::string eventMaskStr = tmpChar;
	XMLString::release(&tmpChar);

	// Get the depth
	xmlch = element->getAttribute(XmpParser::ATTR_Depth);
	tmpChar = XMLString::transcode(xmlch);
	std::string depthStr = tmpChar;
	XMLString::release(&tmpChar);

	// Get the priority
	xmlch = element->getAttribute(XmpParser::ATTR_Priority);
	tmpChar = XMLString::transcode(xmlch);
	std::string priorityStr = tmpChar;
	XMLString::release(&tmpChar);

	// Get the type
	xmlch = element->getAttribute(XmpParser::ATTR_Type);
	tmpChar = XMLString::transcode(xmlch);
	std::string typeStr = tmpChar;
	XMLString::release(&tmpChar);

	// Look one level nested within "constraint"
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children of folder in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Dispname) )
			{
				std::string dispName = XmpParser::ParseDispname(currentElement);
			}
		}
	}

	return NULL;
}


std::string XmpParser::ParseDispname(DOMElement* element)
{
	return "";
}


void XmpParser::ParseAttrdef(DOMElement* element)
{
	// Get the name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string constraintName = tmpChar;
	XMLString::release(&tmpChar);
	
	// Get the metaRef
	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
	tmpChar = XMLString::transcode(xmlch);
	std::string metaRefStr = tmpChar;
	XMLString::release(&tmpChar);
	
	// Get the ValueType
	xmlch = element->getAttribute(XmpParser::ATTR_Valuetype);
	tmpChar = XMLString::transcode(xmlch);
	std::string valueTypeStr = tmpChar;
	XMLString::release(&tmpChar);
	
	// Get the Default Value
	xmlch = element->getAttribute(XmpParser::ATTR_Defvalue);
	tmpChar = XMLString::transcode(xmlch);
	std::string defaultValueStr = tmpChar;
	XMLString::release(&tmpChar);

	// Look one level nested within "constraint"
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children of folder in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Dispname) )
			{
				std::string dispName = XmpParser::ParseDispname(currentElement);
			}
		}
	}
}


MetaRegNode* XmpParser::ParseRegnode(DOMElement* element)
{
	return NULL;
}


MetaEnumItem* XmpParser::ParseEnumitem(DOMElement* element)
{
	return NULL;
}


MetaAtom* XmpParser::ParseAtom(DOMElement* element)
{
	return NULL;
}


MetaSet* XmpParser::ParseSet(DOMElement* element)
{
	return NULL;
}


MetaPointerSpec* XmpParser::ParsePointerspec(DOMElement* element)
{
	return NULL;
}


void XmpParser::ParsePointeritem(DOMElement* element)
{
}


MetaConnection* XmpParser::ParseConnection(DOMElement* element)
{
	return NULL;
}


void XmpParser::ParseConnjoint(DOMElement* element)
{
}


MetaReference* XmpParser::ParseReference(DOMElement* element)
{
	return NULL;
}


MetaModel* XmpParser::ParseModel(DOMElement* element)
{
	return NULL;
}


MetaRole* XmpParser::ParseRole(DOMElement* element)
{
	return NULL;
}


MetaAspect* XmpParser::ParseAspect(DOMElement* element)
{
	return NULL;
}


MetaPart* XmpParser::ParsePart(DOMElement* element)
{
	return NULL;
}


void XmpParser::CleanUp(void)
{
	XMLString::release(&XmpParser::TAG_Folder);
	XMLString::release(&XmpParser::TAG_Comment);
	XMLString::release(&XmpParser::TAG_Author);
	XMLString::release(&XmpParser::TAG_Constraint);
	XMLString::release(&XmpParser::TAG_Dispname);
	XMLString::release(&XmpParser::TAG_Attrdef);
	XMLString::release(&XmpParser::TAG_Regnode);
	XMLString::release(&XmpParser::TAG_Enumitem);
	XMLString::release(&XmpParser::TAG_Atom);
	XMLString::release(&XmpParser::TAG_Set);
	XMLString::release(&XmpParser::TAG_Pointerspec);
	XMLString::release(&XmpParser::TAG_Pointeritem);
	XMLString::release(&XmpParser::TAG_Connection);
	XMLString::release(&XmpParser::TAG_Connjoint);
	XMLString::release(&XmpParser::TAG_Reference);
	XMLString::release(&XmpParser::TAG_Model);
	XMLString::release(&XmpParser::TAG_Role);
	XMLString::release(&XmpParser::TAG_Aspect);
	XMLString::release(&XmpParser::TAG_Part);
	XMLString::release(&XmpParser::ATTR_Name);
	XMLString::release(&XmpParser::ATTR_Uuid);
	XMLString::release(&XmpParser::ATTR_CDate);
	XMLString::release(&XmpParser::ATTR_MDate);
	XMLString::release(&XmpParser::ATTR_Metaref);
	XMLString::release(&XmpParser::ATTR_Subfolders);
	XMLString::release(&XmpParser::ATTR_Rootobjects);
	XMLString::release(&XmpParser::ATTR_Eventmask);
	XMLString::release(&XmpParser::ATTR_Depth);
	XMLString::release(&XmpParser::ATTR_Priority);
	XMLString::release(&XmpParser::ATTR_Type);
	XMLString::release(&XmpParser::ATTR_Valuetype);
	XMLString::release(&XmpParser::ATTR_Defvalue);
	XMLString::release(&XmpParser::ATTR_Value);
	XMLString::release(&XmpParser::ATTR_Attributes);
	XMLString::release(&XmpParser::ATTR_Desc);
	XMLString::release(&XmpParser::ATTR_Kind);
	XMLString::release(&XmpParser::ATTR_Primary);
	XMLString::release(&XmpParser::ATTR_Linked);
}


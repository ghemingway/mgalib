/*** Included Header Files ***/
#include "XmpParser.h"
#include "../Meta/MetaProject.h"
#include "../Meta/MetaFolder.h"
#include "../Meta/MetaConstraint.h"
#include "../Meta/MetaAttribute.h"
#include "../Meta/MetaEnumItem.h"
#include "../Meta/MetaFCO.h"
#include "../Meta/MetaSet.h"
#include "../Meta/MetaPointerSpec.h"
#include "../Meta/MetaPointerItem.h"
#include "../Meta/MetaConnection.h"
#include "../Meta/MetaConnJoint.h"
#include "../Meta/MetaReference.h"
#include "../Meta/MetaModel.h"
#include "../Meta/MetaRole.h"
#include "../Meta/MetaAspect.h"
#include "../Meta/MetaPart.h"


XMLCh* XmpParser::TAG_Folder = NULL;
XMLCh* XmpParser::TAG_Comment = NULL;
XMLCh* XmpParser::TAG_Author = NULL;
XMLCh* XmpParser::TAG_Constraint = NULL;
XMLCh* XmpParser::TAG_DispName = NULL;
XMLCh* XmpParser::TAG_AttrDef = NULL;
XMLCh* XmpParser::TAG_RegNode = NULL;
XMLCh* XmpParser::TAG_EnumItem = NULL;
XMLCh* XmpParser::TAG_Atom = NULL;
XMLCh* XmpParser::TAG_Set = NULL;
XMLCh* XmpParser::TAG_PointerSpec = NULL;
XMLCh* XmpParser::TAG_PointerItem = NULL;
XMLCh* XmpParser::TAG_Connection = NULL;
XMLCh* XmpParser::TAG_ConnJoint = NULL;
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
XMLCh* XmpParser::ATTR_RootObjects = NULL;
XMLCh* XmpParser::ATTR_EventMask = NULL;
XMLCh* XmpParser::ATTR_Depth = NULL;
XMLCh* XmpParser::ATTR_Priority = NULL;
XMLCh* XmpParser::ATTR_Type = NULL;
XMLCh* XmpParser::ATTR_ValueType = NULL;
XMLCh* XmpParser::ATTR_DefValue = NULL;
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
	XmpParser::TAG_DispName		= XMLString::transcode("dispname");
	XmpParser::TAG_AttrDef		= XMLString::transcode("attrdef");
	XmpParser::TAG_RegNode		= XMLString::transcode("regnode");
	XmpParser::TAG_EnumItem		= XMLString::transcode("enumitem");
	XmpParser::TAG_Atom			= XMLString::transcode("atom");
	XmpParser::TAG_Set			= XMLString::transcode("set");
	XmpParser::TAG_PointerSpec	= XMLString::transcode("pointerspec");
	XmpParser::TAG_PointerItem	= XMLString::transcode("pointeritem");
	XmpParser::TAG_Connection	= XMLString::transcode("connection");
	XmpParser::TAG_ConnJoint	= XMLString::transcode("connjoint");
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
	XmpParser::ATTR_RootObjects	= XMLString::transcode("rootobjects");
	XmpParser::ATTR_EventMask	= XMLString::transcode("eventmask");
	XmpParser::ATTR_Depth		= XMLString::transcode("depth");
	XmpParser::ATTR_Priority	= XMLString::transcode("priority");
	XmpParser::ATTR_Type		= XMLString::transcode("type");
	XmpParser::ATTR_ValueType	= XMLString::transcode("valuetype");
	XmpParser::ATTR_DefValue	= XMLString::transcode("defvalue");
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

		// Create the MetaProject
		MetaProject* metaProject = NULL;
		std::string connString = "MGA=" + mtaFile;
		Result_t result = MetaProject::Create(connString, metaProject);
		if (result != S_OK)
		{
			XmpParser::CleanUp();
			return result;
		}
		ASSERT( metaProject != NULL );
		// Get the root folder
		MetaFolder* rootFolder = NULL;
		result = metaProject->RootFolder(rootFolder);
		ASSERT( result == S_OK && rootFolder != NULL );

		// Get the paradigm name
		const XMLCh* xmlch = elementRoot->getAttribute(XmpParser::ATTR_Name);
		char* tmpChar = XMLString::transcode(xmlch);
		std::string paradigmName = tmpChar;
		result = metaProject->SetName(paradigmName);
		ASSERT( result == S_OK );
		XMLString::release(&tmpChar);

		// Get the Uuid
		xmlch = elementRoot->getAttribute(XmpParser::ATTR_Uuid);
		tmpChar = XMLString::transcode(xmlch);
		std::string paradigmUuidStr = tmpChar;
		if (paradigmUuidStr != "" )
		{
			Uuid paradigmUuid = paradigmUuidStr;
			result = metaProject->SetUuid(paradigmUuid);
			ASSERT( result == S_OK );
		}
		XMLString::release(&tmpChar);

		// Get the cdate
		xmlch = elementRoot->getAttribute(XmpParser::ATTR_CDate);
		tmpChar = XMLString::transcode(xmlch);
		std::string cdate = tmpChar;
		result = metaProject->SetCreatedAt(cdate);
		ASSERT( result == S_OK );
		XMLString::release(&tmpChar);

		// Get the mdate
		xmlch = elementRoot->getAttribute(XmpParser::ATTR_MDate);
		tmpChar = XMLString::transcode(xmlch);
		std::string mdate = tmpChar;
		result = metaProject->SetModifiedAt(mdate);
		ASSERT( result == S_OK );
		XMLString::release(&tmpChar);

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
					MetaFolder* metaFolder = XmpParser::ParseFolder(currentElement, rootFolder);
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
					// Extract the comment string and place it into the metaProject
					std::string comment = XmpParser::ParseComment(currentElement);
					result = metaProject->SetComment(comment);
					ASSERT( result == S_OK );
				}
				else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Author) )
				{
					// Extract the author string and place it into the metaProject
					std::string author = XmpParser::ParseAuthor(currentElement);
					result = metaProject->SetAuthor(author);
					ASSERT( result == S_OK );
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


MetaFolder* XmpParser::ParseFolder(DOMElement* element, MetaFolder* parentFolder)
{
	// Now create the MetaFolder
	MetaFolder* metaFolder = NULL;
	Result_t result = parentFolder->CreateFolder(metaFolder);
	ASSERT( result == S_OK && metaFolder != NULL );
	
	// Get the folder name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string folderName = tmpChar;
	metaFolder->SetName(folderName);
	XMLString::release(&tmpChar);

	// Get the metaRef
//	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
//	tmpChar = XMLString::transcode(xmlch);
//	std::string metaRefStr = tmpChar;
//	XMLString::release(&tmpChar);

	// Get the subfolders
	xmlch = element->getAttribute(XmpParser::ATTR_Subfolders);
	tmpChar = XMLString::transcode(xmlch);
	std::string subfoldersStr = tmpChar;
	XMLString::release(&tmpChar);

	// Get the rootobjects
	xmlch = element->getAttribute(XmpParser::ATTR_RootObjects);
	tmpChar = XMLString::transcode(xmlch);
	std::string rootObjectsStr = tmpChar;
	XMLString::release(&tmpChar);

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
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Folder) )
			{
				MetaFolder* metaFolder = XmpParser::ParseFolder(currentElement, metaFolder);
				// Look for possible errors
				if (metaFolder == NULL) return NULL;
			}
			else if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Constraint) )
			{
				MetaConstraint* metaConstraint = XmpParser::ParseConstraint(currentElement, metaFolder);
				// Look for possible errors
				if (metaConstraint == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_AttrDef) )
			{
				MetaAttribute* metaAttribute = XmpParser::ParseAttrdef(currentElement, metaFolder, NULL);
				// Look for possible errors
				if (metaAttribute == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Atom) )
			{
				MetaAtom* metaAtom = XmpParser::ParseAtom(currentElement, metaFolder);
				// Look for possible errors
				if (metaAtom == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Set) )
			{
				MetaSet* metaSet = XmpParser::ParseSet(currentElement, metaFolder);
				// Look for possible errors
				if (metaSet == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Connection) )
			{
				MetaConnection* metaConnection = XmpParser::ParseConnection(currentElement, metaFolder);
				// Look for possible errors
				if (metaConnection == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Reference) )
			{
				MetaReference* metaReference = XmpParser::ParseReference(currentElement, metaFolder);
				// Look for possible errors
				if (metaReference == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Model) )
			{
				MetaModel* metaModel = XmpParser::ParseModel(currentElement, metaFolder);
				// Look for possible errors
				if (metaModel == NULL) return NULL;
			}
		}
	}
	// Handle all rootObjects
	// ...

	// Handle all subfolders
	// ...

	// All done, return the folder
	return metaFolder;
}


std::string XmpParser::ParseComment(DOMElement* element)
{
	std::string comment = "";
	// Look one level nested within the element
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::TEXT_NODE ) // is text element
		{
			// Cast to a test element
			DOMText* textElement = dynamic_cast< xercesc::DOMText* >( currentNode );
			// Just get the elements value
			const XMLCh* xmlch = textElement->getNodeValue();
			char* tmpChar = XMLString::transcode(xmlch);
			comment = tmpChar;
			XMLString::release(&tmpChar);
		}
	}
	// Return the value
	return comment;
}


std::string XmpParser::ParseAuthor(DOMElement* element)
{
	std::string author = "";
	// Look one level nested within the element
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::TEXT_NODE ) // is text element
		{
			// Cast to a test element
			DOMText* textElement = dynamic_cast< xercesc::DOMText* >( currentNode );
			// Just get the elements value
			const XMLCh* xmlch = textElement->getNodeValue();
			char* tmpChar = XMLString::transcode(xmlch);
			author = tmpChar;
			XMLString::release(&tmpChar);
		}
	}
	// Return the value
	return author;
}


std::string XmpParser::ParseDispname(DOMElement* element)
{
	std::string displayName = "";
	// Look one level nested within "constraint"
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children of displayname in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::TEXT_NODE ) // is text element
		{
			// Cast to a test element
			DOMText* textElement = dynamic_cast< xercesc::DOMText* >( currentNode );
			// Just get the elements value
			const XMLCh* xmlch = textElement->getNodeValue();
			char* tmpChar = XMLString::transcode(xmlch);
			displayName = tmpChar;
			XMLString::release(&tmpChar);
		}
	}
	// Return the displayName
	return displayName;
}


MetaConstraint* XmpParser::ParseConstraint(DOMElement* element, MetaBase* metaBase)
{
	// Create the constraint
	MetaConstraint* metaConstraint = NULL;
	Result_t result = metaBase->CreateConstraint(metaConstraint);
	ASSERT (result == S_OK && metaConstraint != NULL );

	// Get the name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string constraintName = tmpChar;
	result = metaConstraint->SetName(constraintName);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);

	// Get the metaRef
//	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
//	tmpChar = XMLString::transcode(xmlch);
//	std::string metaRefStr = tmpChar;
//	XMLString::release(&tmpChar);

	// Get the eventmask
	xmlch = element->getAttribute(XmpParser::ATTR_EventMask);
	tmpChar = XMLString::transcode(xmlch);
	std::string eventMaskStr = tmpChar;
	if (eventMaskStr != "")
	{
		std::stringstream ss;
		ss << eventMaskStr;
		uint32_t eventMask;
		ss >> eventMask ;
		result = metaConstraint->SetEventMask(eventMask);
		ASSERT( result == S_OK );
	}
	XMLString::release(&tmpChar);

	// Get the depth
	xmlch = element->getAttribute(XmpParser::ATTR_Depth);
	tmpChar = XMLString::transcode(xmlch);
	std::string depthStr = tmpChar;
	if (depthStr != "")
	{
		ConstraintDepth depth = CONSTRAINT_DEPTH_ZERO;
		if (depthStr == "one") depth = CONSTRAINT_DEPTH_ONE;
		else if (depthStr == "any") depth = CONSTRAINT_DEPTH_ANY;
		result = metaConstraint->SetDepth(depth);
		ASSERT( result == S_OK );
	}
	XMLString::release(&tmpChar);

	// Get the priority
	xmlch = element->getAttribute(XmpParser::ATTR_Priority);
	tmpChar = XMLString::transcode(xmlch);
	std::string priorityStr = tmpChar;
	if (priorityStr != "")
	{
		std::stringstream ss;
		ss << priorityStr;
		int32_t priority;
		ss >> priority;
		result = metaConstraint->SetPriority(priority);
		ASSERT( result == S_OK );
	}
	XMLString::release(&tmpChar);

	// Get the type
	xmlch = element->getAttribute(XmpParser::ATTR_Type);
	tmpChar = XMLString::transcode(xmlch);
	std::string typeStr = tmpChar;
	if (typeStr != "")
	{
		ConstraintType type = CONSTRAINT_TYPE_FUNCTION;
		if (typeStr == "ondemand") type = CONSTRAINT_TYPE_ONDEMAND;
		else if (typeStr == "eventbased") type = CONSTRAINT_TYPE_EVENTBASED;
		result = metaConstraint->SetType(type);
		ASSERT( result == S_OK );
	}
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
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_DispName) )
			{
				// Get the displayName and set it in the constraint
				std::string dispName = XmpParser::ParseDispname(currentElement);
				result = metaConstraint->SetDisplayedName(dispName);
				ASSERT( result == S_OK );
			}
		}
		else if( currentNode->getNodeType() &&  // true is not NULL
				currentNode->getNodeType() == DOMNode::CDATA_SECTION_NODE )
		{
			// Try to get the CDATA
			DOMText* textElement = dynamic_cast< xercesc::DOMText* >( currentNode );
			xmlch = textElement->getData();
			tmpChar = XMLString::transcode(xmlch);
			std::string expressionStr = tmpChar;
			result = metaConstraint->SetExpression(expressionStr);
			ASSERT( result == S_OK );
			XMLString::release(&tmpChar);
		}
	}
	// Return the new constraint
	return metaConstraint;
}


MetaAttribute* XmpParser::ParseAttrdef(DOMElement* element, MetaFolder* metaFolder, MetaFCO* metaFCO)
{
	// Create the attribute
	MetaAttribute* metaAttribute = NULL;
	Result_t result;
	if (metaFolder != NULL) result= metaFolder->CreateAttribute(metaAttribute);
	else if (metaFCO != NULL) result = metaFCO->CreateAttribute(metaAttribute);
	ASSERT( result == S_OK && metaAttribute != NULL );

	// Get the name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string name = tmpChar;
	result = metaAttribute->SetName(name);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);
	
	// Get the metaRef
//	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
//	tmpChar = XMLString::transcode(xmlch);
//	std::string metaRefStr = tmpChar;
//	XMLString::release(&tmpChar);
	
	// Get the ValueType
	xmlch = element->getAttribute(XmpParser::ATTR_ValueType);
	tmpChar = XMLString::transcode(xmlch);
	std::string valueTypeStr = tmpChar;
	if (valueTypeStr != "" )
	{
		AttVal_t valueType = ATTVAL_NULL;
		if (valueTypeStr == "boolean") valueType = ATTVAL_BOOLEAN;
		else if (valueTypeStr == "string") valueType = ATTVAL_STRING;
		else if (valueTypeStr == "enum") valueType = ATTVAL_ENUM;
		else if (valueTypeStr == "integer") valueType = ATTVAL_INTEGER;
//		ATTVAL_DOUBLE = 3,
//		ATTVAL_REFERENCE = 5,
//		ATTVAL_DYNAMIC
		result = metaAttribute->SetValueType(valueType);
		ASSERT( result == S_OK );
	}
	XMLString::release(&tmpChar);
	
	// Get the Default Value
	xmlch = element->getAttribute(XmpParser::ATTR_DefValue);
	tmpChar = XMLString::transcode(xmlch);
	std::string defaultValueStr = tmpChar;
	result = metaAttribute->SetDefaultValue(defaultValueStr);
	ASSERT( result == S_OK );
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
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_DispName) )
			{
				std::string dispName = XmpParser::ParseDispname(currentElement);
				result = metaAttribute->SetDisplayedName(dispName);
				ASSERT( result == S_OK );
			}
			else if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_RegNode) )
			{
				std::string key, value;
				XmpParser::ParseRegNode(currentElement, key, value);
				// Set the K-V pair in the attribute
				result = metaAttribute->SetRegistryValue(key, value);
				ASSERT( result == S_OK );
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_EnumItem) )
			{
				MetaEnumItem* metaEnumItem = XmpParser::ParseEnumitem(currentElement, metaAttribute);
				// Look for possible errors
				if (metaEnumItem == NULL) return NULL;
			}
		}
	}
	// Return the happy attribute
	return metaAttribute;
}


void XmpParser::ParseRegNode(DOMElement* element, std::string &key, std::string &value)
{
	// Get the key(aka name)
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	key = tmpChar;
	XMLString::release(&tmpChar);
	
	// Get the value
	xmlch = element->getAttribute(XmpParser::ATTR_Value);
	tmpChar = XMLString::transcode(xmlch);
	value = tmpChar;
	XMLString::release(&tmpChar);
}


MetaEnumItem* XmpParser::ParseEnumitem(DOMElement* element, MetaAttribute* metaAttribute)
{
	// Create the enumItem
	MetaEnumItem* metaEnumItem = NULL;
	Result_t result = metaAttribute->CreateEnumItem(metaEnumItem);
	ASSERT( result == S_OK && metaEnumItem != NULL );
	
	// Get the displayname
	const XMLCh* xmlch = element->getAttribute(XmpParser::TAG_DispName);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string dispname = tmpChar;
	result = metaEnumItem->SetDisplayedName(dispname);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);
	
	// Get the value
	xmlch = element->getAttribute(XmpParser::ATTR_Value);
	tmpChar = XMLString::transcode(xmlch);
	std::string value = tmpChar;
	result = metaEnumItem->SetValue(value);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);

	// All done
	return metaEnumItem;
}


MetaAtom* XmpParser::ParseAtom(DOMElement* element, MetaFolder* metaFolder)
{
	// Create the atom
	MetaAtom* metaAtom = NULL;
	Result_t result = metaFolder->CreateAtom(metaAtom);
	ASSERT( result == S_OK && metaAtom != NULL );
	
	// Get the name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string name = tmpChar;
	result = metaAtom->SetName(name);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);

	// Get the metaRef
//	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
//	tmpChar = XMLString::transcode(xmlch);
//	std::string metaRefStr = tmpChar;
//	XMLString::release(&tmpChar);

	// Get the Attributes string
	xmlch = element->getAttribute(XmpParser::ATTR_Attributes);
	tmpChar = XMLString::transcode(xmlch);
	std::string attributesStr = tmpChar;
	XMLString::release(&tmpChar);
	
	// Look one level down
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_RegNode) )
			{
				std::string key, value;
				XmpParser::ParseRegNode(currentElement, key, value);
				// Set the K-V pair in the attribute
				result = metaAtom->SetRegistryValue(key, value);
				ASSERT( result == S_OK );
			}
			else if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Constraint) )
			{
				MetaConstraint* metaConstraint = XmpParser::ParseConstraint(currentElement, metaAtom);
				// Look for possible errors
				if (metaConstraint == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_AttrDef) )
			{
				MetaAttribute* metaAttribute = XmpParser::ParseAttrdef(currentElement, NULL, metaAtom);
				// Look for possible errors
				if (metaAttribute == NULL) return NULL;
			}
		}
	}
	// Set the attributes values
	// ...
	return metaAtom;
}


MetaSet* XmpParser::ParseSet(DOMElement* element, MetaFolder* metaFolder)
{
	// Create the set
	MetaSet* metaSet = NULL;
	Result_t result = metaFolder->CreateSet(metaSet);
	ASSERT( result == S_OK && metaSet != NULL );
	
	// Get the name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string name = tmpChar;
	result = metaSet->SetName(name);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);
	
	// Get the metaRef
//	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
//	tmpChar = XMLString::transcode(xmlch);
//	std::string metaRefStr = tmpChar;
//	XMLString::release(&tmpChar);
	
	// Get the Attributes
	xmlch = element->getAttribute(XmpParser::ATTR_Attributes);
	tmpChar = XMLString::transcode(xmlch);
	std::string attributes = tmpChar;
	XMLString::release(&tmpChar);
	
	// Look one level nested
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_RegNode) )
			{
				std::string key, value;
				XmpParser::ParseRegNode(currentElement, key, value);
				// Set the K-V pair in the object
				result = metaSet->SetRegistryValue(key, value);
				ASSERT( result == S_OK );
			}
			else if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Constraint) )
			{
				MetaConstraint* metaConstraint = XmpParser::ParseConstraint(currentElement, metaSet);
				// Look for possible errors
				if (metaConstraint == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_AttrDef) )
			{
				MetaAttribute* metaAttribute = XmpParser::ParseAttrdef(currentElement, NULL, metaSet);
				// Look for possible errors
				if (metaAttribute == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_PointerSpec) )
			{
				MetaPointerSpec* metaPointerSpec = XmpParser::ParsePointerspec(currentElement, metaSet);
				// Look for possible errors
				if (metaPointerSpec == NULL) return NULL;
			}
		}
	}
	// Do something with the attributes
	// ...
	// Return the happy set
	return metaSet;
}


MetaPointerSpec* XmpParser::ParsePointerspec(DOMElement* element, MetaSet* metaSet)
{
	// Create the pointerSpec
	MetaPointerSpec* metaPointerSpec = NULL;
	Result_t result = metaSet->CreateMemberSpec(metaPointerSpec);
	ASSERT( result == S_OK && metaPointerSpec != NULL );
	
	// Get the name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string name = tmpChar;
	result = metaPointerSpec->SetName(name);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);

	// Look one level nested
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_PointerItem) )
			{
				MetaPointerItem* metaPointerItem = XmpParser::ParsePointeritem(currentElement, metaPointerSpec);
				// Look for possible errors
				if (metaPointerItem == NULL) return NULL;
			}
		}
	}
	// Return the happy metaPointerSpec
	return metaPointerSpec;
}


MetaPointerItem* XmpParser::ParsePointeritem(DOMElement* element, MetaPointerSpec* metaPointerSpec)
{
	// Create the pointerItem
	MetaPointerItem* metaPointerItem = NULL;
	Result_t result = metaPointerSpec->CreateItem(metaPointerItem);
	ASSERT( result == S_OK && metaPointerItem != NULL );
	
	// Get the desc
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Desc);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string desc = tmpChar;
	result = metaPointerItem->SetDescription(desc);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);

	// Return the happy metaPointerItem
	return metaPointerItem;
}


MetaConnection* XmpParser::ParseConnection(DOMElement* element, MetaFolder* metaFolder)
{
	// Create the MetaConnection
	MetaConnection* metaConnection = NULL;
	Result_t result = metaFolder->CreateConnection(metaConnection);
	ASSERT( result == S_OK && metaConnection != NULL );
	
	// Get the name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string name = tmpChar;
	result = metaConnection->SetName(name);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);

	// Get the metaRef
//	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
//	tmpChar = XMLString::transcode(xmlch);
//	std::string metaRefStr = tmpChar;
//	XMLString::release(&tmpChar);

	// Get the Attributes
	xmlch = element->getAttribute(XmpParser::ATTR_Attributes);
	tmpChar = XMLString::transcode(xmlch);
	std::string attributes = tmpChar;
	XMLString::release(&tmpChar);

	// Look one level nested
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_RegNode) )
			{
				std::string key, value;
				XmpParser::ParseRegNode(currentElement, key, value);
				// Set the K-V pair in the object
				result = metaConnection->SetRegistryValue(key, value);
				ASSERT( result == S_OK );
			}
			else if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_ConnJoint) )
			{
				MetaConnJoint* metaConnJoint = XmpParser::ParseConnjoint(currentElement, metaConnection);
				// Look for possible errors
				if (metaConnJoint == NULL) return NULL;
			}
			else if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Constraint) )
			{
				MetaConstraint* metaConstraint = XmpParser::ParseConstraint(currentElement, metaConnection);
				// Look for possible errors
				if (metaConstraint == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_AttrDef) )
			{
				MetaAttribute* metaAttribute = XmpParser::ParseAttrdef(currentElement, NULL, metaConnection);
				// Look for possible errors
				if (metaAttribute == NULL) return NULL;
			}
		}
	}
	// Do something with the attributes
	// ...
	// Return the happy metaConnection
	return metaConnection;
}


MetaConnJoint* XmpParser::ParseConnjoint(DOMElement* element, MetaConnection* metaConnection)
{
	// Create the connJoint
	MetaConnJoint* metaConnJoint = NULL;
	Result_t result = metaConnection->CreateJoint(metaConnJoint);
	ASSERT( result == S_OK && metaConnJoint != NULL );
	
	// Look one level nested
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_PointerSpec) )
			{
				ASSERT(false);
				MetaPointerSpec* metaPointerSpec = NULL;//XmpParser::ParsePointerspec(currentElement, metaConnJoint);
				// Look for possible errors
				if (metaPointerSpec == NULL) return NULL;
			}
		}
	}
	// Return the happy object
	return metaConnJoint;
}


MetaReference* XmpParser::ParseReference(DOMElement* element, MetaFolder* metaFolder)
{
	// Create the MetaReference
	MetaReference* metaReference = NULL;
	Result_t result = metaFolder->CreateReference(metaReference);
	ASSERT( result == S_OK && metaReference != NULL );
	
	// Get the name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string name = tmpChar;
	result = metaReference->SetName(name);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);
	
	// Get the metaRef
//	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
//	tmpChar = XMLString::transcode(xmlch);
//	std::string metaRefStr = tmpChar;
//	XMLString::release(&tmpChar);
	
	// Get the Attributes
	xmlch = element->getAttribute(XmpParser::ATTR_Attributes);
	tmpChar = XMLString::transcode(xmlch);
	std::string attributes = tmpChar;
	XMLString::release(&tmpChar);
	
	// Look one level nested
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_RegNode) )
			{
				std::string key, value;
				XmpParser::ParseRegNode(currentElement, key, value);
				// Set the K-V pair in the object
				result = metaReference->SetRegistryValue(key, value);
				ASSERT( result == S_OK );
			}
			else if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Constraint) )
			{
				MetaConstraint* metaConstraint = XmpParser::ParseConstraint(currentElement, metaReference);
				// Look for possible errors
				if (metaConstraint == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_PointerSpec) )
			{
				ASSERT(false);
				MetaPointerSpec* metaPointerSpec = NULL;//XmpParser::ParsePointerspec(currentElement, metaReference);
				// Look for possible errors
				if (metaPointerSpec == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_AttrDef) )
			{
				MetaAttribute* metaAttribute = XmpParser::ParseAttrdef(currentElement, NULL, metaReference);
				// Look for possible errors
				if (metaAttribute == NULL) return NULL;
			}
		}
	}
	// Do something with the attributes
	// ...
	// Return the happy metaConnection
	return metaReference;
}


MetaModel* XmpParser::ParseModel(DOMElement* element, MetaFolder* metaFolder)
{
	// Create the MetaModel
	MetaModel* metaModel = NULL;
	Result_t result = metaFolder->CreateModel(metaModel);
	ASSERT( result == S_OK && metaModel != NULL );
	
	// Get the name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string name = tmpChar;
	result = metaModel->SetName(name);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);
	
	// Get the metaRef
//	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
//	tmpChar = XMLString::transcode(xmlch);
//	std::string metaRefStr = tmpChar;
//	XMLString::release(&tmpChar);
	
	// Look one level nested
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_RegNode) )
			{
				std::string key, value;
				XmpParser::ParseRegNode(currentElement, key, value);
				// Set the K-V pair in the object
				result = metaModel->SetRegistryValue(key, value);
				ASSERT( result == S_OK );
			}
			else if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Role) )
			{
				MetaRole* metaRole = XmpParser::ParseRole(currentElement, metaModel);
				// Look for possible errors
				if (metaRole == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_PointerSpec) )
			{
				ASSERT(false);
				MetaPointerSpec* metaPointerSpec = NULL;//XmpParser::ParsePointerspec(currentElement, metaReference);
				// Look for possible errors
				if (metaPointerSpec == NULL) return NULL;
			}
			else if ( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Aspect) )
			{
				MetaAspect* metaAspect = XmpParser::ParseAspect(currentElement, metaModel);
				// Look for possible errors
				if (metaAspect == NULL) return NULL;
			}
		}
	}
	// Return the happy object
	return metaModel;
}


MetaRole* XmpParser::ParseRole(DOMElement* element, MetaModel* metaModel)
{
	// Create the MetaRole
	MetaRole* metaRole = NULL;
	Result_t result = S_OK;//metaModel->CreateRole(metaRole);
	ASSERT( result == S_OK && metaRole != NULL );
	
	// Get the name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string name = tmpChar;
	result = metaRole->SetName(name);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);
	
	// Get the metaRef
//	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
//	tmpChar = XMLString::transcode(xmlch);
//	std::string metaRefStr = tmpChar;
//	XMLString::release(&tmpChar);

	// Get the kind
	xmlch = element->getAttribute(XmpParser::ATTR_Kind);
	tmpChar = XMLString::transcode(xmlch);
	std::string kind = tmpChar;
	XMLString::release(&tmpChar);

	// Return the happy object
	return metaRole;
}


MetaAspect* XmpParser::ParseAspect(DOMElement* element, MetaModel* metaModel)
{
	// Create the MetaAspect
	MetaAspect* metaAspect = NULL;
	Result_t result = metaModel->CreateAspect(metaAspect);
	ASSERT( result == S_OK && metaAspect != NULL );
	
	// Get the name
	const XMLCh* xmlch = element->getAttribute(XmpParser::ATTR_Name);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string name = tmpChar;
	result = metaAspect->SetName(name);
	ASSERT( result == S_OK );
	XMLString::release(&tmpChar);
	
	// Get the metaRef
//	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
//	tmpChar = XMLString::transcode(xmlch);
//	std::string metaRefStr = tmpChar;
//	XMLString::release(&tmpChar);
	
	// Look one level nested
	DOMNodeList* children = element->getChildNodes();
	const XMLSize_t nodeCount = children->getLength();
	// For all nodes, children in the XML tree.
	for(XMLSize_t i = 0; i < nodeCount; ++i)
	{
		DOMNode* currentNode = children->item(i);
		if( currentNode->getNodeType() &&  // true is not NULL
		   currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
		{
			// Found node which is an Element. Re-cast node as element
			DOMElement* currentElement = dynamic_cast< xercesc::DOMElement* >( currentNode );
			if( XMLString::equals(currentElement->getTagName(), XmpParser::TAG_Part) )
			{
				MetaPart* metaPart = XmpParser::ParsePart(currentElement, metaAspect);
				// Look for possible errors
				if (metaPart == NULL) return NULL;
			}
		}
	}
	// Return the happy object
	return metaAspect;
}


MetaPart* XmpParser::ParsePart(DOMElement* element, MetaAspect* metaAspect)
{
	// Create the MetaPart
	MetaPart* metaPart = NULL;
	ASSERT(false);
	Result_t result = NULL;//metaAspect->CreatePart(metaPart);
	ASSERT( result == S_OK && metaPart != NULL );
	
	// Get the metaRef
//	xmlch = element->getAttribute(XmpParser::ATTR_Metaref);
//	tmpChar = XMLString::transcode(xmlch);
//	std::string metaRefStr = tmpChar;
//	XMLString::release(&tmpChar);

	// Get the role
	const XMLCh* xmlch = element->getAttribute(XmpParser::TAG_Role);
	char* tmpChar = XMLString::transcode(xmlch);
	std::string role = tmpChar;
	XMLString::release(&tmpChar);

	// Get the role
	xmlch = element->getAttribute(XmpParser::ATTR_Primary);
	tmpChar = XMLString::transcode(xmlch);
	std::string primary = tmpChar;
	XMLString::release(&tmpChar);

	// Get the role
	xmlch = element->getAttribute(XmpParser::ATTR_Linked);
	tmpChar = XMLString::transcode(xmlch);
	std::string linked = tmpChar;
	XMLString::release(&tmpChar);
}


void XmpParser::CleanUp(void)
{
	XMLString::release(&XmpParser::TAG_Folder);
	XMLString::release(&XmpParser::TAG_Comment);
	XMLString::release(&XmpParser::TAG_Author);
	XMLString::release(&XmpParser::TAG_Constraint);
	XMLString::release(&XmpParser::TAG_DispName);
	XMLString::release(&XmpParser::TAG_AttrDef);
	XMLString::release(&XmpParser::TAG_RegNode);
	XMLString::release(&XmpParser::TAG_EnumItem);
	XMLString::release(&XmpParser::TAG_Atom);
	XMLString::release(&XmpParser::TAG_Set);
	XMLString::release(&XmpParser::TAG_PointerSpec);
	XMLString::release(&XmpParser::TAG_PointerItem);
	XMLString::release(&XmpParser::TAG_Connection);
	XMLString::release(&XmpParser::TAG_ConnJoint);
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
	XMLString::release(&XmpParser::ATTR_RootObjects);
	XMLString::release(&XmpParser::ATTR_EventMask);
	XMLString::release(&XmpParser::ATTR_Depth);
	XMLString::release(&XmpParser::ATTR_Priority);
	XMLString::release(&XmpParser::ATTR_Type);
	XMLString::release(&XmpParser::ATTR_ValueType);
	XMLString::release(&XmpParser::ATTR_DefValue);
	XMLString::release(&XmpParser::ATTR_Value);
	XMLString::release(&XmpParser::ATTR_Attributes);
	XMLString::release(&XmpParser::ATTR_Desc);
	XMLString::release(&XmpParser::ATTR_Kind);
	XMLString::release(&XmpParser::ATTR_Primary);
	XMLString::release(&XmpParser::ATTR_Linked);
}


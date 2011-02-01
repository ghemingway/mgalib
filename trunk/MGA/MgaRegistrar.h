#ifndef __MGA_REGISTRAR_H__
#define __MGA_REGISTRAR_H__


/*** Included Header Files ***/
#include "../Core/CoreTypes.h"
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


// --------------------------- MgaRegistrar --------------------------- //


class MgaRegistrar;


class MgaRegistryWriter
{
public:
	virtual bool Write(MgaRegistrar* registry)=0;
};


// Paradigm information structure
typedef struct ParadigmStruct
{
	std::string name;
	std::map<std::string,std::string>::iterator currentVersion;		// Iterator into the version map
	std::map<std::string,std::string> versions;						// Paradigm GUID, connection string
} ParadigmStruct;

// Component information structure
typedef struct ComponentStruct	{ } ComponentStruct;


class MgaRegistrar
{
protected:
	std::string					_iconPath;
	bool						_showMultipleView;
	bool						_eventLoggingEnabled;
	bool						_autosaveEnabled;
	long						_autosaveFrequency;
	bool						_autosaveUseDirectory;
	std::string					_autosaveDirectory;
	bool						_externalEditorEnabled;
	std::string					_externalEditor;
	bool						_useAutoRouting;
	bool						_labelAvoidance;
	std::string					_scriptEngine;
	std::string					_zoomLevel;
	bool						_mouseOverNotify;
	std::string					_realNumberFormatString;
	bool						_timeStamping;
	bool						_useNavigation;
	std::string					_undoQueueSize;
	long						_edgeSmoothMode;
	long						_fontSmoothMode;

	std::list<ParadigmStruct>	_paradigms;
	std::list<ComponentStruct>	_components;
	
	MgaRegistrar();	
public:
	virtual ~MgaRegistrar() { }

	inline const Result_t GetIconPath(std::string &iconPath) const throw()	{ iconPath = this->_iconPath; return S_OK; }
	inline const Result_t SetIconPath(const std::string &iconPath) throw()	{ this->_iconPath = iconPath; return S_OK; }
	inline const Result_t GetShowMultipleView(bool &flag) const throw()		{ flag = this->_showMultipleView; return S_OK; }
	inline const Result_t SetShowMultipleView(const bool &flag) throw()		{ this->_showMultipleView = flag; return S_OK; }
/*
	STDMETHOD(get_EventLoggingEnabled)(regaccessmode_enum mode, VARIANT_BOOL *enabled);
	STDMETHOD(put_EventLoggingEnabled)(regaccessmode_enum mode, VARIANT_BOOL enabled);
	STDMETHOD(get_AutosaveEnabled)(regaccessmode_enum mode, VARIANT_BOOL *enabled);
	STDMETHOD(put_AutosaveEnabled)(regaccessmode_enum mode, VARIANT_BOOL enabled);
	STDMETHOD(get_AutosaveFreq)(regaccessmode_enum mode, long *secs);
	STDMETHOD(put_AutosaveFreq)(regaccessmode_enum mode, long secs);
	STDMETHOD(get_AutosaveUseDir)(regaccessmode_enum mode, VARIANT_BOOL *use);
	STDMETHOD(put_AutosaveUseDir)(regaccessmode_enum mode, VARIANT_BOOL use);
	STDMETHOD(get_AutosaveDir)(regaccessmode_enum mode, BSTR *dir);
	STDMETHOD(put_AutosaveDir)(regaccessmode_enum mode, BSTR dir);
	STDMETHOD(get_ExternalEditorEnabled)(regaccessmode_enum mode, VARIANT_BOOL *enabled);
	STDMETHOD(put_ExternalEditorEnabled)(regaccessmode_enum mode, VARIANT_BOOL enabled);
	STDMETHOD(get_ExternalEditor)(regaccessmode_enum mode, BSTR *path);
	STDMETHOD(put_ExternalEditor)(regaccessmode_enum mode, BSTR path);
	STDMETHOD(get_UseAutoRouting)(regaccessmode_enum mode, VARIANT_BOOL *enabled);
	STDMETHOD(put_UseAutoRouting)(regaccessmode_enum mode, VARIANT_BOOL enabled);
	STDMETHOD(get_LabelAvoidance)(regaccessmode_enum mode, VARIANT_BOOL *enabled);
	STDMETHOD(put_LabelAvoidance)(regaccessmode_enum mode, VARIANT_BOOL enabled);
	STDMETHOD(get_ScriptEngine)(regaccessmode_enum mode, BSTR *path);
	STDMETHOD(put_ScriptEngine)(regaccessmode_enum mode, BSTR path);
	STDMETHOD(GetDefZoomLevel)(regaccessmode_enum mode, BSTR *zlevel);
	STDMETHOD(SetDefZoomLevel)(regaccessmode_enum mode, BSTR zlevel);
	STDMETHOD(GetMouseOverNotify)(regaccessmode_enum mode, VARIANT_BOOL *enabled);
	STDMETHOD(SetMouseOverNotify)(regaccessmode_enum mode, VARIANT_BOOL enabled);
	STDMETHOD(GetRealNmbFmtStr)(regaccessmode_enum mode, BSTR *fmtStr);
	STDMETHOD(SetRealNmbFmtStr)(regaccessmode_enum mode, BSTR fmtStr);
	STDMETHOD(GetTimeStamping)(regaccessmode_enum mode, VARIANT_BOOL *enabled);
	STDMETHOD(SetTimeStamping)(regaccessmode_enum mode, VARIANT_BOOL enabled);
	STDMETHOD(GetNavigation)(regaccessmode_enum mode, VARIANT_BOOL *enabled);
	STDMETHOD(SetNavigation)(regaccessmode_enum mode, VARIANT_BOOL enabled);
	STDMETHOD(GetUndoQueueSize)(regaccessmode_enum mode, BSTR *queuesz);
	STDMETHOD(SetUndoQueueSize)(regaccessmode_enum mode, BSTR queuesz);
	STDMETHOD(get_EdgeSmoothMode)(regaccessmode_enum mode, edgesmoothmode_enum* smoothMode);
	STDMETHOD(put_EdgeSmoothMode)(regaccessmode_enum mode, edgesmoothmode_enum smoothMode);
	STDMETHOD(get_FontSmoothMode)(regaccessmode_enum mode, fontsmoothmode_enum* smoothMode);
	STDMETHOD(put_FontSmoothMode)(regaccessmode_enum mode, fontsmoothmode_enum smoothMode);
	STDMETHOD(get_Paradigms)(regaccessmode_enum mode, VARIANT *names);
	STDMETHOD(RegisterParadigmFromData)(BSTR connstr, BSTR *newname, regaccessmode_enum mode);
	STDMETHOD(RegisterParadigm)(BSTR name, BSTR connstr, BSTR version, VARIANT guid, regaccessmode_enum mode);
	STDMETHOD(QueryParadigmAllGUIDs)(BSTR name, VARIANT *names, regaccessmode_enum mode);
*/
	const Result_t QueryParadigm(const std::string &name, const std::string &guid, std::string &connString) const throw();
/*
	STDMETHOD(get_ParadigmGUIDString)(regaccessmode_enum mode, BSTR name, BSTR *guidstr);
	STDMETHOD(UnregisterParadigmGUID)(BSTR name, VARIANT v, regaccessmode_enum mode);
	STDMETHOD(UnregisterParadigm)(BSTR name, regaccessmode_enum mode);
	STDMETHOD(VersionFromGUID)(BSTR name, VARIANT guid, BSTR *ver, regaccessmode_enum mode);
*/
	const Result_t UuidFromVersion(const std::string &name, const std::string &version, std::string &uuidStr) const throw();
/*
	STDMETHOD(get_Components)(regaccessmode_enum mode, VARIANT *progids);
	STDMETHOD(RegisterComponent)(BSTR progid, componenttype_enum type, BSTR desc, regaccessmode_enum mode);
	STDMETHOD(QueryComponent)(BSTR progid, componenttype_enum *type, BSTR *desc, regaccessmode_enum mode);
	STDMETHOD(UnregisterComponent)(BSTR progid, regaccessmode_enum mode);
	STDMETHOD(put_ComponentExtraInfo)(regaccessmode_enum mode, BSTR progid, BSTR name, BSTR newVal);
	STDMETHOD(get_ComponentExtraInfo)(regaccessmode_enum mode, BSTR progid, BSTR name, BSTR* pVal);
	STDMETHOD(get_LocalDllPath)(BSTR progid, BSTR* pVal);
*/
	const Result_t AssociatedComponents(const std::string &paradigm, const ComponentType_t &type, std::vector<std::string> &programIDs) throw();
/*
	STDMETHOD(get_AssociatedParadigms)(BSTR progid, regaccessmode_enum mode, VARIANT *paradigms);
	STDMETHOD(Associate)(BSTR progid, BSTR paradigm, regaccessmode_enum mode);
	STDMETHOD(Disassociate)(BSTR progid, BSTR paradigm, regaccessmode_enum mode);
	STDMETHOD(IsAssociated)(BSTR progid, BSTR paradigm, VARIANT_BOOL *is_ass, VARIANT_BOOL *can_ass, regaccessmode_enum mode);
	STDMETHOD(RegisterComponentLibrary)(BSTR path, regaccessmode_enum mode);
	STDMETHOD(UnregisterComponentLibrary)(BSTR path, regaccessmode_enum mode);
*/
};


class XMLRegistrar :
	public MgaRegistrar,
	public MgaRegistryWriter
{
private:
	std::string					_filename;
	// Get XML tags ready
	XMLCh*						_TAG_MGARegistry;
	XMLCh*						_TAG_Components;
	XMLCh*						_TAG_GUI;
	XMLCh*						_TAG_Paradigms;
	XMLCh*						_TAG_Paradigm;
	XMLCh*						_TAG_Version;
	XMLCh*						_ATTR_RegVersion;
	XMLCh*						_ATTR_IconPath;
	XMLCh*						_ATTR_JavaClassPath;
	XMLCh*						_ATTR_JavaMemory;
	XMLCh*						_ATTR_Name;
	XMLCh*						_ATTR_CurrentVersion;
	XMLCh*						_ATTR_Uuid;
	XMLCh*						_ATTR_Connection;
	XercesDOMParser*			_configFileParser;

	XMLRegistrar();													//!< Deny access to default constructor
	XMLRegistrar(const XMLRegistrar &);								//!< Deny access to copy constructor
	XMLRegistrar& operator=(const XMLRegistrar &);					//!< Deny access to equals operator
	XMLRegistrar(const std::string &filename);						//!< Hidden primary constructor

	const Result_t Parse(void);										//!< Primary entry point for static open method
	const Result_t ParseComponents(DOMElement* element);			//!<
	const Result_t ParseGUI(DOMElement* currentElement);			//!<
	const Result_t ParseParadigms(DOMElement* currentElement);		//!<
	const Result_t ParseParadigm(DOMElement* currentElement);		//!<
	
	virtual bool Write(MgaRegistrar* registry);
public:
	static const Result_t OpenRegistry(const std::string &filename, MgaRegistrar* &registrar) throw();

	virtual ~XMLRegistrar();
};


/*** End of MGA Namespace ***/
}


#endif // __MGA_REGISTRAR_H__


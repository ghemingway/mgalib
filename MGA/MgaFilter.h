#ifndef __MGA_FILTER_H__
#define __MGA_FILTER_H__


/*** Included Header Files ***/
#include "MgaFCO.h"


/*** Class Predefinitions ***/
class MgaProject;
class CoreObject;


// --------------------------- MgaFilter --------------------------- //


//typedef stdext::hash_set<CComBSTRNoAt, CComBSTR_hashfunc> BSTRhash;
//typedef stdext::hash_set<metaref_type, metaref_hashfunc> mrefhash;


class MgaFilter
{
private:
	std::string				_level, _objType, _role, _kind, _name;
	bool					_newLevel, _newObjType, _newRole, _newKind, _newName;
	unsigned long			_levels;
	unsigned long			_objTypes;
//	mrefhash				_roleNums, _kindNums;
//	BSTRhash				_roleNames, _kindNames, _names;
	MgaProject				*_mgaProject;

	void Init();
	inline void SetProject(MgaProject* &project)	{ this->_mgaProject = project; }

public:
	MgaFilter() : _level(""), _objType(""), _role(""), _kind(""), _name(""),
		_newLevel(false), _newObjType(false), _newRole(false), _newKind(false), _newName(false),
		_levels(0), _objTypes(0), _mgaProject(NULL) { }

	const Result_t SearchFCOs(CoreObject* &coreObject, std::list<MgaFCO*> &fcoList, const int &level=0);
	const Result_t GetLevel(std::string &level) const throw()		{ level = this->_level; return S_OK; }
	const Result_t SetLevel(const std::string &level) throw()		{ this->_level = level; this->_newLevel = true; return S_OK; }
	const Result_t GetObjType(std::string &objType) const throw()	{ objType = this->_objType; return S_OK; }
	const Result_t SetObjType(const std::string &objType) throw()	{ this->_objType = objType; this->_newObjType = true; return S_OK; }
	const Result_t GetRole(std::string &role) const throw()			{ role = this->_role; return S_OK; }
	const Result_t SetRole(const std::string &role) throw()			{ this->_role = role; this->_newRole = true; return S_OK; }
	const Result_t GetKind(std::string &kind) const throw()			{ kind = this->_kind; return S_OK; }
	const Result_t SetKind(const std::string &kind) throw()			{ this->_kind = kind; this->_newKind = true; return S_OK; }
	const Result_t GetName(std::string &name) const throw()			{ name = this->_name; return S_OK; }
	const Result_t SetName(const std::string &name) throw()			{ this->_name = name; this->_newName = true; return S_OK; }
	const Result_t GetProject(MgaProject* &project) const throw()	{ project = this->_mgaProject; return S_OK; }
};

#endif //__MGAFILTER_H_

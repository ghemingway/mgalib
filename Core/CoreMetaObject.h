#ifndef __CORE_METAOBJECT_H__
#define __CORE_METAOBJECT_H__


/*** Included Header Files ***/
#include "CoreTypes.h"


/*** Namespace Declaration ***/
namespace MGA {
	

/*** Class Predefinitions ***/
class CoreMetaProject;
class CoreMetaAttribute;


// --------------------------- CoreMetaObject --------------------------- //


class CoreMetaObject
{
private:
	friend class CoreMetaProject;									//!< Only class allowed to create metaObjects
	CoreMetaObject();												//!< Deny access to default constructor
	CoreMetaObject(const CoreMetaObject &);							//!< Deny access to copy constructor
	CoreMetaObject& operator=(const CoreMetaObject &);				//!< Deny access to equals operator
	CoreMetaObject(CoreMetaProject *project, const MetaID_t &metaID, const std::string &token, const std::string &name) :
		_project(project), _attributes(), _metaID(metaID), _token(token), _name(name) { }

protected:
	CoreMetaProject								*_project;			//!< MetaProject this MetaObject belongs to
	std::list<CoreMetaAttribute*>				_attributes;		//!< Child attributes
	MetaID_t									_metaID;			//!< MetaID of this object
	std::string									_token;				//!< Short description of this object
	std::string									_name;				//!< Full description of this object

public:
	~CoreMetaObject();

	inline const Result_t GetProject(CoreMetaProject* &project) const throw()	{ project = this->_project; return S_OK; }
	inline const Result_t GetMetaID(MetaID_t &metaID) const throw()				{ metaID = this->_metaID; return S_OK; }
	inline const Result_t GetToken(std::string &token) const throw()			{ token = this->_token; return S_OK; }
	inline const Result_t GetName(std::string &name) const throw()				{ name = this->_name; return S_OK; }
	inline const Result_t GetAttributes(std::list<CoreMetaAttribute*> &attrList) const	throw()	{ attrList = this->_attributes; return S_OK; }
	const Result_t GetAttribute(const AttrID_t &attrID, CoreMetaAttribute* &attribute) const throw();
	const Result_t AddAttribute(const AttrID_t &attrID, const std::string &token, const std::string &name,
								const ValueType &valueType, CoreMetaAttribute* &attribute) throw();
	friend std::ostream& operator<<(std::ostream& out, const CoreMetaObject *object);
};


/*** End of MGA Namespace ***/
}


#endif	//__CORE_METAOBJECT_H__


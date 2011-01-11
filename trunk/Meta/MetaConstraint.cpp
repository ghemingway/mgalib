/*** Included Header Files ***/
#include "MetaConstraint.h"


// --------------------------- MetaConstraint --------------------------- //


MetaConstraint::MetaConstraint(CoreObject &coreObject, MetaProject* &metaProject) :
	_coreObject(coreObject), _metaProject(metaProject)
{
	ASSERT(coreObject != NULL);
	ASSERT(metaProject != NULL);
}


const Result_t MetaConstraint::GetName(std::string &name) const throw()
{
	ASSERT(false);
//	return ComGetAttrValue(GetUnknown(), ATTRID_NAME, p);
	return S_OK;
}


const Result_t MetaConstraint::SetName(const std::string &name) throw()
{ 
	ASSERT(false);
//	return ComPutAttrValue(GetUnknown(), ATTRID_NAME, p);
	return S_OK;
}


const Result_t MetaConstraint::GetDisplayedName(std::string &name) const throw()
{
	ASSERT(false);
//	return ComGetDisplayedName(GetUnknown(), ATTRID_DISPNAME, ATTRID_NAME, p);
	return S_OK;
}


const Result_t MetaConstraint::SetDisplayedName(const std::string &name) throw()
{
	ASSERT(false);
	//	return ComPutAttrValue(GetUnknown(), ATTRID_DISPNAME, p);
	return S_OK;
}


const Result_t MetaConstraint::GetExpression(std::string &exp) const throw()
{
	ASSERT(false);
//	return ComGetAttrValue(GetUnknown(), ATTRID_CONSEXPR, p);
	return S_OK;
}


const Result_t MetaConstraint::SetExpression(const std::string &expression) throw()
{
	ASSERT(false);
//	return ComPutAttrValue(GetUnknown(), ATTRID_CONSEXPR, p);
	return S_OK;
}


const Result_t MetaConstraint::GetEventMask(unsigned long &mask) const throw()
{
	ASSERT(false);
//	return ComGetAttrValue(GetUnknown(), ATTRID_CONSMASK, (long *)p);
	return S_OK;
}


const Result_t MetaConstraint::SetEventMask(const unsigned long &mask) throw()
{
	ASSERT(false);
//	return ComPutAttrValue(GetUnknown(), ATTRID_CONSMASK, (long)p);
	return S_OK;
}


const Result_t MetaConstraint::GetDepth(ConstraintDepth &depth) const throw()
{
	ASSERT(false);
//	CCoreObjectPtr me(GetUnknown());
//		*p = static_cast<constraint_depth_enum>(me.GetLongValue(ATTRID_CONSDEPTH));
//		if( *p != CONSTRAINT_DEPTH_ZERO &&
//		   *p != CONSTRAINT_DEPTH_ANY )
//			*p = CONSTRAINT_DEPTH_ONE;
	return S_OK;
}


const Result_t MetaConstraint::SetDepth(const ConstraintDepth &depth) throw()
{
	ASSERT(false);
//	return ComPutAttrValue(GetUnknown(), ATTRID_CONSDEPTH, static_cast<long>(p));
	return S_OK;
}


const Result_t MetaConstraint::GetPriority(long &priority) const throw()
{
	ASSERT(false);
//	return ComGetAttrValue(GetUnknown(), ATTRID_CONSPRIORITY, p);
	return S_OK;
}


const Result_t MetaConstraint::SetPriority(const long &priority) throw()
{
	ASSERT(false);
//	return ComPutAttrValue(GetUnknown(), ATTRID_CONSPRIORITY, p);
	return S_OK;
}


const Result_t MetaConstraint::GetType(ConstraintType &type) const throw()
{
	ASSERT(false);
//	CCoreObjectPtr me(GetUnknown());
//	*p = static_cast<constraint_type_enum>(me.GetLongValue(ATTRID_CONSTYPE));
//	if( *p != CONSTRAINT_TYPE_EVENTBASED &&
//		*p != CONSTRAINT_TYPE_FUNCTION )
//		*p = CONSTRAINT_TYPE_ONDEMAND;
	return S_OK;
}


const Result_t MetaConstraint::SetType(const ConstraintType &type) throw()
{
	ASSERT(false);
//	return ComPutAttrValue(GetUnknown(), ATTRID_CONSTYPE, static_cast<long>(p));
	return S_OK;
}


const Result_t MetaConstraint::GetDefinedForNamespace(std::string &value) const throw()
{
	ASSERT(false);
//	return ComGetAttrValue(GetUnknown(), ATTRID_CONSNAMESPC, pStr);
	return S_OK;
}


const Result_t MetaConstraint::SetDefinedForNamespace(const std::string &value) throw()
{
	ASSERT(false);
//	return ComPutAttrValue(GetUnknown(), ATTRID_CONSNAMESPC, pStr);
	return S_OK;
}


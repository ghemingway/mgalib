/*** Included Header Files ***/
#include "MetaConstraint.h"
#include "MetaGeneric.h"


// --------------------------- Public MetaConstraint Methods --------------------------- //


MetaConstraint::MetaConstraint(CoreObject &coreObject, MetaProject* const &metaProject) :
	_coreObject(coreObject), _metaProject(metaProject)
{
	ASSERT(coreObject != NULL);
	ASSERT(metaProject != NULL);
}


const Result_t MetaConstraint::GetParent(MetaBase* &metaBase) throw()
{
	// Use the MetaBase helper function to get this pointer object
	return MetaBase::ObjectFromAttribute(this->_coreObject, this->_metaProject, ATTRID_CONSTRAINT_PTR, metaBase);
}


const Result_t MetaConstraint::GetName(std::string &name) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_NAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::SetName(const std::string &name) throw()
{ 
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_NAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::GetDisplayedName(std::string &name) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_DISPNAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::SetDisplayedName(const std::string &name) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_DISPNAME, name);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::GetExpression(std::string &expression) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_CONSEXPR, expression);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::SetExpression(const std::string &expression) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_CONSEXPR, expression);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;	
}


const Result_t MetaConstraint::GetEventMask(uint32_t &mask) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longMask;
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_CONSMASK, longMask);
	mask = longMask;
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::SetEventMask(const uint32_t &mask) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longMask = (int32_t)mask;
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_CONSMASK, longMask);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::GetDepth(ConstraintDepth &depth) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longDepth;
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_CONSDEPTH, longDepth);
	depth = static_cast<ConstraintDepth>(longDepth);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::SetDepth(const ConstraintDepth &depth) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longDepth = static_cast<int32_t>(depth);
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_CONSDEPTH, longDepth);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::GetPriority(int32_t &priority) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_CONSPRIORITY, priority);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::SetPriority(const int32_t &priority) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_CONSPRIORITY, priority);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::GetType(ConstraintType &type) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longType;
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_CONSTYPE, longType);
	type = static_cast<ConstraintType>(longType);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::SetType(const ConstraintType &type) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	int32_t longType = static_cast<int32_t>(type);
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_CONSTYPE, longType);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


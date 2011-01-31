/*** Included Header Files ***/
#include "MetaConstraint.h"
#include "MetaGeneric.h"


// --------------------------- MetaConstraint --------------------------- //


MetaConstraint::MetaConstraint(CoreObject &coreObject, MetaProject* &metaProject) :
	_coreObject(coreObject), _metaProject(metaProject)
{
	ASSERT(coreObject != NULL);
	ASSERT(metaProject != NULL);
}


const Result_t MetaConstraint::GetParent(MetaBase* &metaBase) throw()
{
	Result_t result = this->_metaProject->BeginTransaction();
	ASSERT( result == S_OK );
	Uuid parentUuid = Uuid::Null();
	result = this->_coreObject->GetAttributeValue(ATTRID_CONSTRAINT_PTR, parentUuid);
	ASSERT( result == S_OK );
	result = this->_metaProject->CommitTransaction();
	ASSERT( result == S_OK );
	
	// Is the Uuid valid
	if (parentUuid == Uuid::Null()) metaBase = NULL;
	// Convert this Uuid into a MetaBase
	else
	{
		// Get the coreProject and coreObject for the parent metaAttribute
		CoreProject *coreProject = NULL;
		result = this->_coreObject->Project(coreProject);
		ASSERT( result == S_OK );
		ASSERT( coreProject != NULL );
		CoreObject parentObject;
		result = coreProject->Object(parentUuid, parentObject);
		ASSERT( result == S_OK );
		ASSERT( parentObject != NULL );
		// Create the MetaBase with the coreObject and metaProject
		metaBase = new MetaBase(parentObject, this->_metaProject);
		ASSERT( metaBase != NULL );
	}
	return S_OK;
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


const Result_t MetaConstraint::GetDefinedForNamespace(std::string &value) const throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->GetAttributeValue(ATTRID_CONSNAMESPC, value);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


const Result_t MetaConstraint::SetDefinedForNamespace(const std::string &value) throw()
{
	Result_t txResult = this->_metaProject->BeginTransaction();
	ASSERT( txResult == S_OK );
	Result_t result = this->_coreObject->SetAttributeValue(ATTRID_CONSNAMESPC, value);
	txResult = this->_metaProject->CommitTransaction();
	ASSERT( txResult == S_OK );
	return result;
}


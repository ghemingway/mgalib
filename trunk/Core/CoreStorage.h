#ifndef __CORE_STORAGE_H__
#define __CORE_STORAGE_H__


/*** Included Header Files ***/
#include "CoreTypes.h"


/*** Class Predefinitions ***/
class CoreMetaProject;
class CoreMetaObject;
class ICoreStorage;


// --------------------------- CoreStorageFactory ---------------------------


class CoreStorageFactory {
public:
	virtual const Result_t Create(const std::string &filename, CoreMetaProject *metaProject, ICoreStorage* &coreStorage)=0;
	virtual const Result_t Open(const std::string &filename, CoreMetaProject *metaProject, ICoreStorage* &coreStorage)=0;
};


// --------------------------- CoreStorage ---------------------------


class ICoreStorage
{
private:
	ICoreStorage();																			//!< Hide the default constructor
	ICoreStorage(const ICoreStorage &);														//!< Hide copy constructor
	ICoreStorage& operator=(const ICoreStorage &);											//!< Hide equals operator
	static std::map<std::string,CoreStorageFactory*>		_storageFactories;				//!< Static map of factories

protected:
	CoreMetaProject											*_metaProject;					//!< Related metaProject
	bool													_isDirty;						//!< Dirty flag for the storage
	bool													_inTransaction;					//!< Transaction flag for the storage
	ICoreStorage(CoreMetaProject* metaProject) : _metaProject(metaProject), _isDirty(false), _inTransaction(false)
		{ ASSERT( this->_metaProject != NULL); }

	inline virtual CoreMetaProject* MetaProject(void) const	{ return this->_metaProject; }	//!< Get associated CoreMetaProject
	inline virtual bool InTransaction(void) const			{ return this->_inTransaction; }//!< Is transaction open
	inline virtual const bool IsDirty(void) const			{ return this->_isDirty; }		//!< Is project dirty
	inline virtual void MarkDirty(void)						{ this->_isDirty = true; }		//!< Mark project as dirty

public:
	virtual ~ICoreStorage()									{ }								//!< Default destructor

// --------------------------- Public Interface Methods --------------------------- //

	// --- Static Methods
	
	static const Result_t RegisterStorage(const std::string &tag, CoreStorageFactory* factory) throw();	//!< Register a factory
	static const Result_t ClearStorageRegistry(void) throw();								//!< Clear factory registry
	static const Result_t Create(const std::string &tag, const std::string &filename,		//!< Create new storage using factory
								 CoreMetaProject* metaProject, ICoreStorage* &storage) throw();
	static const Result_t Open(const std::string &tag, const std::string &filename,			//!< Open storage from file using factory
							   CoreMetaProject* metaProject, ICoreStorage* &storage) throw();

	// --- CoreMetaObject

	virtual const Result_t MetaObject(CoreMetaObject* &coreMetaObject) const throw()=0;		//!< Get open object's metaObject
	virtual const Result_t MetaID(MetaID_t &metaID) const throw()=0;						//!< Get open object's metaID

	// --- Project

	virtual const Result_t ObjectVector(std::vector<MetaObjIDPair> &objectVector) throw()=0;//!< Get idPair vector for all objects in project
	virtual const Result_t Save(const std::string &filename, const bool &v3=true) throw()=0;//!< Save project to file
	virtual const Result_t BeginTransaction(void) throw()=0;								//!< Begin transaction on project
	virtual const Result_t CommitTransaction(void) throw()=0;								//!< Commit transaction on project
	virtual const Result_t AbortTransaction(void) throw()=0;								//!< Abort transaction on project

	// --- CoreObject

	virtual const Result_t OpenObject(const MetaObjIDPair &idPair) throw()=0;				//!< Open an object in project
	virtual const Result_t CloseObject(void) throw()=0;										//!< Close current object
	virtual const Result_t CreateObject(const MetaID_t &metaID, ObjID_t &newObjID) throw()=0;//!< Create a new object
	virtual const Result_t DeleteObject(void) throw()=0;									//!< Delete current object

	// --- CoreAttribute

	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, int32_t &value) throw()=0;						//!< Get long
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, double &value) throw()=0;						//!< Get real
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, std::string &value) throw()=0;					//!< Get string
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, std::vector<unsigned char> &value) throw()=0;	//!< Get binary
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, std::list<MetaObjIDPair> &value) throw()=0;	//!< Get collection
	virtual const Result_t GetAttributeValue(const AttrID_t &attrID, MetaObjIDPair &value) throw()=0;				//!< Get pointer

	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const int32_t &value) throw()=0;				//!< Set long
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const double &value) throw()=0;				//!< Set real
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const std::string &value) throw()=0;			//!< Set string
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const std::vector<unsigned char> &value) throw()=0;//!< Set binary
	virtual const Result_t SetAttributeValue(const AttrID_t &attrID, const MetaObjIDPair &value) throw()=0;			//!< Set pointer
};


#endif	//__CORE_STORAGE_H__




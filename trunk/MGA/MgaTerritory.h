#ifndef __MGA_TERRITORY_H__
#define __MGA_TERRITORY_H__


/*** Included Header Files ***/
#include "MgaTypes.h"
#include "MgaEventSink.h"


/*** Class Predefinitions ***/
class MgaProject;
class MgaObject;
class MgaFCO;
class MgaFCOBase;
typedef std::list<MgaFCO*> MgaFCOs;


// --------------------------- MgaTerritory --------------------------- //


class MgaTerritory
{
protected:
	MgaProject					*_mgaProject;
	bool						_notified;
	unsigned long				_eventmask, _rwEventmask;
	IMgaEventSink				*_handler, *_rwHandler;
	int							_refCount;
	std::string					_namespace;
	std::set<MgaFCOBase*>		_objects;

	MgaTerritory();													//!< Deny access to default constructor
	MgaTerritory(MgaProject* project, IMgaEventSink* handler, IMgaEventSink *rwHandler) :
		_mgaProject(project), _notified(false), _eventmask(0xFFFFFFFF), _rwEventmask(0), _handler(handler),
		_rwHandler(rwHandler), _refCount(0), _namespace(""), _objects() { ASSERT( project != NULL ); }
public:
	static const Result_t CreateTerritory(MgaProject* project, IMgaEventSink* handler,
										  IMgaEventSink *rwHandler, MgaTerritory* &territory) throw();
	~MgaTerritory()	{ }

	inline const Result_t EventMask(const unsigned long &mask) throw()		{ this->_eventmask = mask; return S_OK; }
	inline const Result_t RWEventMask(const unsigned long &mask) throw()	{ this->_rwEventmask = mask; return S_OK; }
	inline const Result_t Project(MgaProject* &project) throw()				{ project = this->_mgaProject; return S_OK; } 
	inline const Result_t GetNamespace(std::string &name)const throw()		{ name = this->_namespace; return S_OK; }
	const Result_t SetNamespace(const std::string &name) throw();
	inline const Result_t CheckProject(MgaProject *project) const throw()
	{ 
		return (project == this->_mgaProject? S_OK : E_MGA_FOREIGN_PROJECT);
	}

	const Result_t Flush(void) throw();
	const Result_t Destroy(void) throw();
//	const Result_t Associate(MgaObject *obj, VARIANT userdata) throw();
	const Result_t OpenObj(MgaObject *oldptr, MgaObject **newptr) throw();
	const Result_t OpenFCO(MgaFCO *oldptr, MgaFCO **newptr) throw();
	const Result_t OpenFCOs(MgaFCOs &oldptr, MgaFCOs* &newptr) throw();
	const Result_t CloseObj(MgaObject *obj) throw();
};


// --------------------------- MgaAddOn --------------------------- //


class MgaAddOn
{
private:
	bool				_active;				//!<
	bool				_notified;				//!<
	bool				_automatic;				//!< This addon was created automatically
	unsigned long		_eventmask;				//!<
	MgaProject			*_mgaProject;			//!<
	IMgaEventSink*		_handler;				//!< non-null if object active

	MgaAddOn();									//!< Deny access to default constructor

public:
	MgaAddOn(MgaProject *project, IMgaEventSink *handler) :
		_active(true), _notified(false), _automatic(false), _eventmask(), _mgaProject(project), _handler(handler)
	{
		ASSERT(project != NULL);
		ASSERT(handler != NULL);
	}
	~MgaAddOn()	{
		if(this->_active) this->Destroy();	
	}

	inline void SetAutomatic(void)				{ this->_automatic = true; }
	inline bool IsAutomatic(void) const			{ return this->_automatic; }
	inline bool Notified(void) const			{ return this->_notified; }
	inline IMgaEventSink* Handler(void) const	{ return this->_handler; }
	const Result_t GetPriority(long &pVal);
	const Result_t SetPriority(const long &priority);
	const Result_t SetEventMask(const unsigned long &mask) { 
			if(!this->_handler) return E_MGA_TARGET_DESTROYED;
			this->_eventmask = mask;
	};
	const Result_t Project(MgaProject* &project) { 
			if(!this->_handler) return E_MGA_TARGET_DESTROYED;
			project = this->_mgaProject;
//			project.AddRef(); 
	};
	const Result_t CheckProject(MgaProject *project);  
	const Result_t Destroy(void);
};


#endif // __MGA_TERRITORY_H__


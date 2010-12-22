#ifndef __MGA_EVENTSINK_H__
#define __MGA_EVENTSINK_H__


/*** Included Header Files ***/
#include "MgaTypes.h"


/*** Namespace Declaration ***/
namespace MGA {
	

/*** Class Predefinitions ***/
class MgaObject;


// --------------------------- IMgaEventSink --------------------------- //


class IMgaEventSink
{
public:
	virtual const Result_t GlobalEvent(const GlobalEvent_t &event)=0;
	virtual const Result_t ObjectEvent(MgaObject* &obj, const unsigned long &eventmask, void* v)=0;
};


/*** End of MGA Namespace ***/
}


#endif // __MGA_EVENTSINK_H__


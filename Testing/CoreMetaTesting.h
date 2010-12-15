#ifndef __COREMETA_TESTING_H__
#define __COREMETA_TESTING_H__


/*** Included Header Files ***/
#include "Core/CoreMetaAttribute.h"
#include "Core/CoreMetaObject.h"
#include "Core/CoreMetaProject.h"
#include "MGA/MgaGeneric.h"
#include <gtest/gtest.h>



/*** Externally Defined Functions ***/
extern std::string testFileName;


class CoreMetaTest : public ::testing::Test {
protected:
	CoreMetaTest() { }
	virtual ~CoreMetaTest() { }
	
	virtual void SetUp() { }
	virtual void TearDown() { }
};



#endif // __COREMETA_TESTING_H__


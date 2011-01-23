#ifndef __COREMETA_TESTING_H__
#define __COREMETA_TESTING_H__


/*** Included Header Files ***/
#include <gtest/gtest.h>
#include "Core/CoreMetaProject.h"


// --------------------------- CoreMetaTest Class  --------------------------- //


class CoreMetaTest : public ::testing::Test {
protected:
	static 	CoreMetaProject		*metaCoreMetaProject;

	static void SetUpTestCase()
	{
		// Make sure project is NULL
		CoreMetaTest::metaCoreMetaProject = NULL;
	}

	static void TearDownTestCase()
	{
		// Close the project and delete the object
		if (CoreMetaTest::metaCoreMetaProject != NULL)delete CoreMetaTest::metaCoreMetaProject;
	}
};


#endif // __COREMETA_TESTING_H__


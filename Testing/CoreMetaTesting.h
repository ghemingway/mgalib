#ifndef __COREMETA_TESTING_H__
#define __COREMETA_TESTING_H__


/*** Included Header Files ***/
#include <gtest/gtest.h>
#include "Core/CoreMetaProject.h"


// --------------------------- CoreMetaTest Class  --------------------------- //


class CoreMetaTest : public ::testing::Test {
protected:
	static 	CoreMetaProject		*mgaCoreMetaProject;
	static 	CoreMetaProject		*metaCoreMetaProject;

	static void SetUpTestCase()
	{
		// Make sure projects are NULL
		CoreMetaTest::mgaCoreMetaProject = NULL;
		CoreMetaTest::metaCoreMetaProject = NULL;
	}

	static void TearDownTestCase()
	{
		// Close the projects and delete the objects
		if (CoreMetaTest::mgaCoreMetaProject != NULL) delete CoreMetaTest::mgaCoreMetaProject;
		if (CoreMetaTest::metaCoreMetaProject != NULL)delete CoreMetaTest::metaCoreMetaProject;
	}
};


#endif // __COREMETA_TESTING_H__


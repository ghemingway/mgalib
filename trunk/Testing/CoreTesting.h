#ifndef __CORE_TESTING_H__
#define __CORE_TESTING_H__


/*** Included Header Files ***/
#include "Core/CoreProject.h"
#include "Core/CoreStorage.h"
#include "Core/CoreMetaProject.h"
#include "Core/BinFile.h"
#include "MGA/MgaGeneric.h"
#include <gtest/gtest.h>


/*** Externally Defined Functions ***/
extern std::string testFileName;


// The fixture for testing class BinStorage.
class CoreProjectTest : public ::testing::Test {
protected:
	// You can remove any or all of the following functions if its body
	// is empty.
	CoreProjectTest() {
		// You can do set-up work for each test here.
	}
	virtual ~CoreProjectTest() {
		// You can do clean-up work that doesn't throw exceptions here.
	}
	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	virtual void SetUp() {
		// Code here will be called immediately after the constructor (right
		// before each test).
	}
	virtual void TearDown() {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Register this storage class
//	ICoreStorage::RegisterStorage("MGA", new BinFileFactory());
//	CoreMetaProject* metaMGA = CreateCoreMetaProject(true);
};


#endif // __CORE_TESTING_H__


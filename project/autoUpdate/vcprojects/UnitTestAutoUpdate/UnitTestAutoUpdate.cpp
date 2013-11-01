#include "gtest/gtest.h"

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	//testing::GTEST_FLAG(repeat) = 5;
	//testing::GTEST_FLAG(filter) = "UserClientSDK.reentrant*";

	RUN_ALL_TESTS();

	for (;;)
	{
	}
	return 0;
}


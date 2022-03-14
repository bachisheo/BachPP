#include "pch.h"
#include "CppUnitTest.h"
#include "Scanner.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(UnitTests)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			Scanner sc = Scanner("input.txt");
		}
	};
}

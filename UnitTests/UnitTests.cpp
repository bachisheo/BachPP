#include "pch.h"

#include <codecvt>
#include <fstream>

#include "CppUnitTest.h"
#include "SyntacticalAnalyzer.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
	TEST_CLASS(UnitTests)
	{
	public:
		void TestSA(std::string code, ErrorCode expected)
		{
			setlocale(LC_ALL, "rus");
			Scanner sc = Scanner(code.c_str(), false);
			SyntacticalAnalyzer sa = SyntacticalAnalyzer(&sc);
			try
			{
				sa.Program();
				Assert::Fail( L"Исключение не было брошено");
			}
			catch (CompilException e) {
				auto a =e.what();
				const int max_msg = 300;
				size_t length = strlen(a) > max_msg ? max_msg : strlen(a);
				wchar_t text_wchar[max_msg + 1];
				mbstowcs_s(&length, text_wchar, a, length);
				Assert::IsTrue(e.code == expected, text_wchar);
			}

		}
		TEST_METHOD(ScannerTest)
		{
			Scanner sc = Scanner("int a = 3;", false);
			sc.ScanAll();
		}
		void Scan(Scanner * a)
		{
			a->ScanAll();
		}
		TEST_METHOD(ExceptionTest)
		{
			Scanner sc = Scanner("int a = 3 / 0;", false);
			SyntacticalAnalyzer sa = SyntacticalAnalyzer(&sc);
			auto func = [&sa]() { sa.Program(); };
			Assert::ExpectException<CompilException>(func);
		}

		TEST_METHOD(DivByZeroTest)
		{
			std::string code = "int a = 3 / 0;";
			TestSA(code, ErrorCode::DivisionByZero);
			code = "int a = 3 % 0;";
			TestSA(code, ErrorCode::DivisionByZero);
			code = "float b = 3 / 0;";
			TestSA(code, ErrorCode::DivisionByZero);
			code = "float b = 3 % 0;";
			TestSA(code, ErrorCode::DivisionByZero);
		}

		TEST_METHOD(TooLargeConstant)
		{
			char* large_const = new char[MAX_LEX + 1];
			memset(large_const, '1', MAX_LEX);
			large_const[MAX_LEX] = '\0';
			std::string code = "int a =" + std::string(large_const) + ";";
			delete[] large_const;
			TestSA(code, ErrorCode::TooLargeConstSize);
		}

		TEST_METHOD(WrongOperations)
		{
			std::string code = "float b = 4; int a = b % 3; ";
			TestSA(code, ErrorCode::WaitForOtherType);
			code = "float b = 4; int a = b >> 3; ";
			TestSA(code, ErrorCode::WaitForOtherType);

		}


	};
}

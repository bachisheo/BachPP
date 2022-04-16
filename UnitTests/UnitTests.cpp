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
		TEST_METHOD(WrongReturnedType)
		{
			std::string code = "asdas func(){ return 12;}; ";
			TestSA(code, ErrorCode::Undefined);
		}
		TEST_METHOD(WrongVariableType)
		{
			std::string code = "asdas a = 12; ";
			TestSA(code, ErrorCode::Undefined);
			code = "intb a = 12; ";
			TestSA(code, ErrorCode::Undefined);
		}
		///
	
	};
	TEST_CLASS(SimpleExamples)
	{
		void testScan(std::string code)
		{
			setlocale(LC_ALL, "rus");
			Scanner sc = Scanner(code.c_str(), false);
			SyntacticalAnalyzer sa = SyntacticalAnalyzer(&sc);
			sa.Program();
		}
	public:
		TEST_METHOD(ScannerTest)
		{
			Scanner sc = Scanner("int a = 3;", false);
			sc.ScanAll();
		}
		TEST_METHOD(SimpleMain)
		{
			std::vector<std::string> codes = { "int a;",
			"int main(){return 12;}",
			"float foo(){ int a = 3; return a;}"
			};
			for (auto code : codes)
			{
				testScan(code);
			}
		}
		TEST_METHOD(FuncUse)
		{
			std::vector<std::string> codes = { 
			"float foo(){ int a = 3; return a;} int main(){return foo();}",
			"int main() { return foo(); }"
			};
			for (auto code : codes)
			{
				testScan(code);
			}
		}
		TEST_METHOD(Expressions)
		{
			std::vector<std::string> codes = {" int a = 2 + 3;",
				"int a = 1, b = -1; int c = a * b + (a + b); "
			};
			for (auto code : codes)
			{
				testScan(code);
			}
		}
	};
}

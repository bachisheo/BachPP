// Главный класс проекта
#include <fstream>
#include <string>

#include "SyntacticalAnalyzer.h"
#include  <Windows.h>
int a = 3;
int main()
{
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	for(int i = 1; i < 7; i++)
	{
		std::string fname = "tests\\input" + std::to_string(i) + ".txt";
		std::cout << "\n-------------------Code\n";
		std::ifstream fin;
		std::string tmp;
		fin.open(fname);
		
		while(!fin.eof())
		{
			std::getline(fin, tmp);
			std::cout << tmp << std::endl;;
		}
		fin.close();
		std::cout << "\n-------------------Program output\n";
	
		Scanner* sc = new Scanner(fname.data());
		SyntacticalAnalyzer sa = SyntacticalAnalyzer(sc);
		try {
			sa.Program();
		}catch(CompilException ce)
		{
			std::cout << std::endl << ce.what();
			return 0;
		}
		std::cout << std::endl;
		sa.PrintSemanticTree(std::cout);
	}
}


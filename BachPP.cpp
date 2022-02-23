// Главный класс проекта
#include <fstream>
#include <string>

#include "SyntacticalAnalyzer.h"
#include  <Windows.h>
int a = 3;

int main()
{
	int v = 2 * (1 + (1 > 0.01));
	int a = 1, b = 3;
	int c = a * b;
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	for(int i = 1; i < 4; i++)
	{
		std::string fname = "input" + std::to_string(i) + ".txt";
		std::cout << "\n-------------------Исходный код\n";
		std::ifstream fin;
		std::string tmp;
		fin.open(fname);
		
		while(!fin.eof())
		{
			std::getline(fin, tmp);
			std::cout << tmp << std::endl;;
		}
		fin.close();
		std::cout << "\n-------------------Реакция программы\n";

		Scanner* sc = new Scanner(fname.data());
		SyntacticalAnalyzer sa = SyntacticalAnalyzer(sc);
		sa.Program();
		std::cout << std::endl;
		sa.PrintSemanticTree(std::cout);
	}
	

}


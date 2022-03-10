// Р“Р»Р°РІРЅС‹Р№ РєР»Р°СЃСЃ РїСЂРѕРµРєС‚Р°
#include <fstream>
#include <string>

#include "SyntacticalAnalyzer.h"
#include  <Windows.h>
int a = 3;

int main()
{
	
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	char* text = new char[MAX_TEXT];

	for(int i = 1; i < 4; i++)

	{
		std::string fname = "input" + std::to_string(i) + ".txt";
		std::cout << "\n-------------------Code\n";
		std::ifstream in(fname);
		in >> text;
		in.close();
		std::cout << text << std::endl << "\n-------------------Program output\n";

		Scanner* sc = new Scanner(text, false);
		SyntacticalAnalyzer sa = SyntacticalAnalyzer(sc);
		try {
			sa.Program();
		}
		catch (std::exception e) {
			std::cerr << e.what();
		}
		std::cout << std::endl;
		sa.PrintSemanticTree(std::cout);
	}
	

}


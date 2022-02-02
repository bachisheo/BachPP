п»ї// Р“Р»Р°РІРЅС‹Р№ РєР»Р°СЃСЃ РїСЂРѕРµРєС‚Р°
#include "SyntacticalAnalyzer.h"
#include  <Windows.h>
int a = 3;

int main()
{
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	Scanner *sc = new Scanner("input.txt");
	SyntacticalAnalyzer sa =  SyntacticalAnalyzer(sc);
	sa.Program();
	std::cout << std::endl;
	sa.PrintSemanticTree(std::cout);
}


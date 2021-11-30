п»ї// Р“Р»Р°РІРЅС‹Р№ РєР»Р°СЃСЃ РїСЂРѕРµРєС‚Р°
#include "SyntacticalAnalyzer.h"
int main()
{
	setlocale(LC_ALL, "rus");

	Scanner *sc = new Scanner("input.txt");
	SyntacticalAnalyzer sa =  SyntacticalAnalyzer(sc);
	sa.Program();
	std::cout << "РћС€РёР±РѕРє РЅРµ РѕР±РЅР°СЂСѓР¶РµРЅРѕ!";



}


// Р“Р»Р°РІРЅС‹Р№ РєР»Р°СЃСЃ РїСЂРѕРµРєС‚Р°
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include "SyntacticalAnalyzer.h"
#include  <Windows.h>
int a = 3;
using namespace std;
int main()
{
	
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);

	string text;
	stringstream ss;

	for(int i = 2; i < 3; i++)

	{
		cout << "\n-------------------Code\n"; 
		ifstream in("input" + to_string(i) + ".txt");
		ss << in.rdbuf();
		ss.putback('\0');
		in.close();
		text = ss.str();
		cout << text << endl << "\n-------------------Program output\n";
		Scanner* sc = new Scanner(text.c_str(), false);
		SyntacticalAnalyzer sa = SyntacticalAnalyzer(sc);
			sa.Program();
	
		cout << endl;
		sa.PrintSemanticTree(cout);
	}
	

}


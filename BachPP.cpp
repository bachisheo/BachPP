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
		auto fname = "input" + to_string(i) + ".txt";
		cout << "\n-------------------Code\n"; 
		ifstream in(fname);
		ss << in.rdbuf();
		in.close();
		text = ss.str();
		text.push_back('\0');
		cout << text << endl << "\n-------------------Program output\n";
		auto sc = Scanner(text.c_str(), false);
		SyntacticalAnalyzer sa = SyntacticalAnalyzer(&sc);
		try {
			sa.Program();
			cout << endl;
			sa.PrintSemanticTree(cout);
		}catch(exception e)
		{
			std::cout << e.what();
		}
		
	}
}


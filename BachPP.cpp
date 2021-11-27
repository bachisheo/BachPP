// Главный класс проекта
#include "Scanner.h"
#include "SyntacticalAnalyzer.h"
int main()
{
	setlocale(LC_ALL, "rus");
	Scanner *sc = new Scanner("input.txt");
	SyntacticalAnalyzer sa =  SyntacticalAnalyzer(sc);
	sa.Programm();



}


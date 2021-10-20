// BachPP.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "Scanner.h"

int main()
{
	setlocale(LC_ALL, "rus");
	Scanner sc = Scanner("input.txt");
	sc.ScanAll();
}


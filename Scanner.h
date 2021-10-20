#include "defs.h"
#include <stdio.h>

#pragma once
typedef char Lexema[MAX_LEX + 1];
typedef int b;

class Scanner
{
public:
	lt Scan(Lexema lex);
	void SkipIgnored();
	void ScanAll();
	Scanner(const char* name);
	void ErrorMsg(MSG_ID id, int str, int col, std::vector<std::string> params);
	int GetPtr() { return ptr; }
	void SetPtr(int ptr) { this->ptr = ptr; }
private:
	lt ScanNumber(Lexema lex, int &len);
	FILE *fin;
	int line;
	int col;
	int ptr;
	char* t;
	int size;
};


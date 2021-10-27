#include "defs.h"
#include <stdio.h>

#pragma once
typedef char LexemaView[MAX_LEX + 1];
typedef int b;

class Scanner
{
public:
	lt Scan(LexemaView lex);
	lt Scan() {
		LexemaView lex;
		return Scan(lex);
	}
	void SkipIgnored();
	void ScanAll();
	Scanner(const char* name);
	static void ErrorMsg(MSG_ID id, int str, int col, std::vector<std::string> params);
	void ErrorMsg(MSG_ID id, std::vector<std::string> params, bool isBegin = false);
	int GetPtr() { return ptr; }
	void SetPtr(int ptr) { this->ptr = ptr; }
	void GetPtrs(int& ptr, int& line, int& col) {
		ptr = this->ptr;
		line = this->line;
		col = this->col;
	}
	void SetPtrs(int ptr, int line, int col) {
		this->ptr = ptr;
		this->line = line;
		this->col = col;
	}
	const char* GetText() {
		return t;
	}
private:
	lt ScanNumber(LexemaView lex, int &len);
	FILE *fin;
	int line;
	int col;
	int ptr;
	char* t;
	int size;
	int lexBeginCol;
	int lexBeginLine;
};


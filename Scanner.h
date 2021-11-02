/*
* Лексический сканер, основанный на программной модели детерминированного конечного автомата лексики языка.
* Группирует теpминальные символы, составляющие входную пpогpамму, в отдельные лексические элементы (лексемами).
* За одно обращение выделяет только одну очередную лексему.
* Никакая синтаксическая информация сканеру не доступна, 
* синтаксис языка не должен учитываться при программировании сканера
*/
#include "defs.h"
#include <stdio.h>

#pragma once
class Scanner
{
public:
	LexType Scan(LexemaView lex);

	LexType Scan() {
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
	LexType ScanNumber(LexemaView lex, int &len);
	FILE *fin;
	int line;
	int col;
	int ptr;
	char* t;
	int size;
	int lexBeginCol;
	int lexBeginLine;
};


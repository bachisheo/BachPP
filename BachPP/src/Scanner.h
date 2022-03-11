/*
* Лексический сканер, основанный на программной модели детерминированного конечного автомата лексики языка.
* Группирует теpминальные символы, составляющие входную пpогpамму, в отдельные лексические элементы (лексемами).
* За одно обращение выделяет только одну очередную лексему.
* Никакая синтаксическая информация сканеру не доступна, 
* синтаксис языка не должен учитываться при программировании сканера
*/
#include "defs.h"

#ifndef scan

class Scanner
{
	LexType ScanCode(LexemaView& lex);
public:
	LexType Scan(LexemaView & lex);
	LexType Scan();
	void SkipIgnored();
	void ScanAll();
	Scanner(const char* name);
	static void ErrorMsg(MSG_ID id, int str, int col, const std::vector<std::string>& params);
	void ErrorMsg(MSG_ID id, const std::vector<std::string>& params, bool isBegin = false);
	int GetPtr();
	void SetPtr(int Ptr);
	void GetPtrs(int& Ptr, int& Line, int& Col);
	void SetPtrs(int Ptr, int Line, int Col);
	const char* GetText();
private:
	LexType ScanNumber(LexemaView & lex, int &len);
	FILE *fin{};
	int line;
	int col;
	int ptr;
	char* t;
	int size;
	int lexBeginCol;
	int lexBeginLine;
};

#endif
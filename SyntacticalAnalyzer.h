#pragma once
#include "Scanner.h"
#define first3Func lt::LRoundBracket
#define first3Data lt::Id
class SyntacticalAnalyzer
{
public :
	SyntacticalAnalyzer(Scanner sc) : sc(sc) {}
	//аксиома S
	void Programm(lt endLex = lt::End);
private:
	Scanner sc;
	lt LookForward(int k = 1);
	void SavePtrs();
	void RestorePtrs();
	int colPtr = 0, linePtr = 0, ptr = 0;
	bool saveCheckScan(lt lex, bool needMsg = true);
	//bool IsFirst(lt lex, st Syn);
	void LexExit(std::vector<std::string> waiting);
	void LexExit(lt waitingLex);
	void DeclarateInFunction();
	void Function();
	void Data();//
	void Variable();
	void Name();//
	bool Type();
	void ClassSynt();//
	void Expression();
	void LogicalExpression();
	void ShiftExpression();
	void AdditionalExpression();
	void MultExpression();
	void ElementaryExpression();
	void Operator();
	void Block();
};


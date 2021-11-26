/*
* Синтаксический анализатор:
* Правила лексики языка представляются в виде синтаксических диаграмм.
* Каждой синтаксической диаграмме соответствует отдельный метод программы синтаксического анализатора.
* Он не содержит параметров, если требуется только синтаксический контроль исходной цепочки.
* В главной пpогpамме тpанслятоpа вызов блока синтаксического анализатоpа pеализуется вызовом той
* функции, котоpая соответствует аксиоме
*/
#pragma once
#include "SemanticTree.h"
#define first3Func LexType::LRoundBracket
#define first3Data LexType::Id
class SyntacticalAnalyzer
{
public :
	SyntacticalAnalyzer(Scanner* sc);
	//аксиома S
	void Programm(LexType endLex = LexType::End);
	std::vector<Node*> userTypes;
private:
	SemanticTree* _tree;
	Scanner* _sc;
	LexType LookForward(int k = 1);
	void SavePtrs();
	void RestorePtrs();
	int colPtr = 0, linePtr = 0, ptr = 0;
	bool saveCheckScan(LexType lex, bool needMsg = true);
	void LexExit(std::vector<std::string> waiting);
	void LexExit(LexType waitingLex);
	void DeclarateInFunction();
	void Function();
	void Data();
	void Variable();
	void Name();
	bool Type();
	void ClassSynt();
	void Expression();
	void LogicalExpression();
	void ShiftExpression();
	void AdditionalExpression();
	void MultExpression();
	void ElementaryExpression();
	void Operator();
	void Block();
};


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
	LexType LookForward(size_t k = 1);
	void SavePtrs();
	void RestorePtrs();
	int colPtr = 0, linePtr = 0, ptr = 0;
	bool CheckScan(LexType lex, LexemaView lv,  bool needMsg = true);
	bool CheckScan(LexType lex,  bool needMsg = true);
	void LexExit(std::vector<std::string> waiting);
	void LexExit(LexType waitingLex);
	void DeclarateInFunction();
	void FunctionDeclarate();
	void Data();
	void Variable();
	std::vector<char*> GetFullName();
	SemanticType Type();
	void ClassDeclarate();
	SemanticType Expression();
	SemanticType LogicalExpression();
	SemanticType ShiftExpression();
	SemanticType AdditionalExpression();
	SemanticType MultExpression();
	SemanticType ElementaryExpression();
	void Operator();
	void Block();
};


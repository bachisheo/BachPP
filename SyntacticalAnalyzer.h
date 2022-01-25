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
class SyntacticalAnalyzer
{
public :
	SyntacticalAnalyzer(Scanner* sc);
	//аксиома S
	void Program(LexType endLex = LexType::End);
	std::vector<Node*> userTypes;
	void PrintSemanticTree(std::ostream& out);
private:
	SemanticTree* _tree;
	Scanner* _sc;
	LexType LookForward(size_t k = 1);
	LexType LookForward(size_t k, LexemaView& lv);
	bool CheckScan(LexType lex, LexemaView & lv,  bool needMsg = true);
	bool CheckScan(LexType lex,  bool needMsg = true);
	void LexExit(const std::vector<std::string>& waiting) const;
	void LexExit(LexType waitingLex) const;
	void DeclarateInFunction();
	void FunctionDeclarate();
	void Data();
	std::vector<LexemaView> GetFullName();
	SemanticType Type(LexemaView& lv) const;
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


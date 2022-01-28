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
	bool isInterpret = false;
	SyntacticalAnalyzer(Scanner* sc);
	//аксиома S
	void Program(LexType endLex = LexType::End);
	void PrintSemanticTree(std::ostream& out) const;
	std::vector<Node*> userTypes;

private:
	SemanticTree* _tree;
	Scanner* _sc;
	LexType LookForward(size_t k = 1);
	LexType LookForward(size_t k, LexemaView& lv);
	bool ScanAndCheck(LexType lex, LexemaView & lv,  bool needMsg = true);
	bool ScanAndCheck(LexType lex,  bool needMsg = true);
	void LexExit(const std::vector<std::string>& waiting) const;
	void LexExit(LexType waitingLex) const;
	void DeclareInFunction();
	void FunctionDeclare();
	void DataDeclare();
	std::vector<LexemaView> GetFullName();
	SemanticType ScanType(LexemaView& lv) const;
	void ClassDeclare();
	SemanticType Expression();
	SemanticType LogicalExpression();
	SemanticType ShiftExpression();
	SemanticType AdditionalExpression();
	SemanticType MultExpression();
	SemanticType ElementaryExpression();
	void Operator();
	void CompoundBlock();
};


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
#include "TriadGenerator.h"
class SyntacticalAnalyzer
{
public:
	SyntacticalAnalyzer(Scanner* sc);
	//аксиома S
	void Program(LexType endLex = LexType::End);
	void PrintSemanticTree(std::ostream& out) const;
	std::vector<Node*> userTypes;
	TriadGenerator tg = TriadGenerator();

private:
	SemanticTree* _tree;
	Scanner* _sc;
	LexType LookForward(size_t k = 1);
	LexType LookForward(size_t k, LexemaView& lv);
	bool ScanAndCheck(LexType lex, LexemaView& lv, bool needMsg = true);
	bool ScanAndCheck(LexType lex, bool needMsg = true);
	void LexExit(const std::vector<std::string>& waiting) const;
	void LexExit(LexType waitingLex) const;
	void DeclareInFunction();
	void FunctionDeclare();
	void ReturnDeclare(Operand *returned);
	Operand * FunctionExecute(std::vector<LexemaView> name);
	void DataDeclare();
	std::vector<LexemaView> GetFullName();
	SemanticType ScanType(LexemaView& lv) const;
	void TypeConvToLarge(Operand* a, Operand* b);
	Operand * TypeConv(Operand* a, SemanticType neededType);
	void ClassDeclare();
	Operand* Expression();
	Operand* LogicalExpression();
	Operand* ShiftExpression();
	Operand* AdditionalExpression();
	Operand* MultExpression();
	Operand* ElementaryExpression();
	Operand* BinaryOperation(Operand* o1, Operand* o2, LexType sign);
	Operand* UnaryOperation(Operand* o1, LexType sign);
	void SetValue(Operand* dest, Operand *src);
	void SetValue(Node* dest, Operand *src);
	void Operator();
	void CompoundBlock();
	Node* Block();
	void WhileExecute();
};


#pragma once
#include "defs.h"
struct Operand {
	bool isLink;     		// флаг: непосредственный операнд или ссылка
	int triNumber;     		// номер триады-ссылки
	LexemaView lexema;        	// непосредственный операнд (лексема)
	SemanticType type;
	Operand();
	Operand(int index, SemanticType typ);
	Operand(int index);
};

struct Triada {
	std::string op;     			//  операция
	Operand operand1, operand2;
	Triada(LexemaView lv);
	Triada(LexemaView op, Operand o1, Operand o2);
	Triada();
};
class TriadGenerator
{
	std::vector<int> _current_rets = std::vector<int>();
public:
	std::vector<Triada> triads = std::vector<Triada>();
	Operand* BinaryOperation(Operand* o1, Operand* o2, LexType sign);
	Operand* UnaryOperation(Operand* o1, LexType sign);
	Operand * TypeConv(Operand* o, SemanticType neededType);
	void DeclFunctionBegin(LexemaView id);
	void DeclFunctionEnd();
	void DeclFunctionRet();
	size_t WhileIfTriada();
	void WhileGotoTriada(size_t if_triada_place, size_t start_expression_place);
	void SetValue(Operand* src, Operand* dest);
	void printTriads() const;
	Operand* FunctionCall(LexemaView func_name, SemanticType returned);
};

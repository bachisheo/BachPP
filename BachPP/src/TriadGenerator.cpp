#include "TriadGenerator.h"
#include "../Utils.h"

Operand::Operand()
{
}

Operand::Operand(int index, SemanticType typ) : type(typ)
{
	isLink = true;
	this->triNumber = index;
	this->lexema = "(" + std::to_string(index) + ")";
}

Operand::Operand(int index) : Operand(index, SemanticType::Undefined)
{
}

Triada::Triada(LexemaView lv)
{
	op = lv;
}

Triada::Triada(LexemaView op, Operand o1, Operand o2) : op(op), operand1(o1), operand2(o2)
{
}

Triada::Triada()
{
}

Operand* TriadGenerator::BinaryOperation(Operand* o1, Operand* o2, LexType sign)
{
	auto t = Triada();
	t.operand1 = *o1;
	t.operand2 = *o2;
	t.op = Utils::LexTypeToString(sign);
	triads.push_back(t);
	return new Operand(triads.size() - 1, t.operand1.type);
}

Operand* TriadGenerator::UnaryOperation(Operand* o1, LexType sign)
{
	auto o = new Operand();
	o->type = o1->type;
	o->lexema = "0";
	return BinaryOperation(o, o1, sign);
}

Operand* TriadGenerator::TypeConv(Operand* o, SemanticType neededType)
{
	auto t = new Triada();
	std::string a, b;
	a = o->type == SemanticType::Float ? "f" : "i";
	b = neededType == SemanticType::Float ? "f" : "i";
	t->op = a + " -> " + b;
	t->operand1 = *o;
	triads.push_back(*t);
	return new Operand(triads.size() - 1, neededType);
}

void TriadGenerator::DeclFunctionBegin(LexemaView id)
{
	auto t = Triada();
	t.op = "proc";
	t.operand1 = Operand();
	t.operand1.lexema = id;
	triads.push_back(t);
	t = Triada();
	t.op = "prolog";
	t.operand1 = Operand();
	triads.push_back(t);
}

void TriadGenerator::DeclFunctionEnd()
{
	triads.push_back(Triada("epilog"));
	int epilog_id = triads.size() - 1;
	triads.push_back(Triada("ret"));
	triads.push_back(Triada("endp"));
	for (auto rets : _current_rets)
	{
		triads[rets].operand1 = Operand(epilog_id);
	}
	int i = _current_rets.size();
	if (i) {
		triads[_current_rets[i-1]] = Triada("nop");
		_current_rets.clear();
	}
}

void TriadGenerator::DeclFunctionRet()
{
	//jump to epilog
	triads.push_back(Triada("jump"));
	_current_rets.push_back(triads.size() - 1);
}


size_t TriadGenerator::WhileIfTriada()
{
	//for one-digit statement
	//ex: while(1)
	auto statement = Operand(triads.size() - 1, SemanticType::ShortInt);
	auto zero = Operand();
	zero.type = SemanticType::ShortInt;
	zero.lexema = "0";
	triads.push_back(Triada("!=", statement, zero));
	auto t = Triada("if");
	//link on next added triada 
	t.operand1 = Operand(triads.size() + 1);
	triads.push_back(t);
	//return "if" triada index
	return triads.size() - 1;
}

void TriadGenerator::WhileGotoTriada(size_t if_triada_place, size_t start_expression_place)
{
	auto jump_t = Triada("goto");
	jump_t.operand1 = Operand(start_expression_place);
	triads.push_back(jump_t);
	triads.push_back(Triada("nop"));
	//set place for jump on "false" statement
	triads[if_triada_place].operand2 = Operand(triads.size() - 1);
}

void TriadGenerator::SetValue(Operand* src, Operand* dest)
{
	auto t = Triada();
	t.operand1 = *dest;
	t.operand2 = *src;
	t.op = "=";
	triads.push_back(t);
}

void TriadGenerator::printTriads() const
{
	for (int i = 0; i < triads.size(); i++)
	{
		std::cout << std::endl << i << "\t|" << triads[i].op << "\t|" << triads[i].operand1.lexema << "\t|" << triads[i].operand2.lexema;
	}
}

Operand* TriadGenerator::FunctionCall(LexemaView func_name, SemanticType returned)
{
	auto t = Triada("call");
	t.operand1 = Operand();
	t.operand1.type = returned;
	t.operand1.lexema = func_name;
	triads.push_back(t);
	return new Operand(triads.size() - 1, returned);
}

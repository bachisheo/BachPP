#include "SyntacticalAnalyzer.h"

#include <functional>
#include <utility>
#include "../Utils.h"

SyntacticalAnalyzer::SyntacticalAnalyzer(Scanner* sc) : _sc(sc)
{
	_tree = new SemanticTree(_sc);
}

void SyntacticalAnalyzer::Operator()
{
	auto nextLex = LookForward();
	switch (nextLex) {
	case LexType::DotComma: _sc->Scan(); break;
		//блок
	case LexType::LFigBracket:
	{
		CompoundBlock();
		break;
	}
	//while с предусловием
	case LexType::While: {
		_sc->Scan();
		WhileExecute();
		break;
	}
	case LexType::Return: {
		_sc->Scan();
		ReturnDeclare(Expression());
		ScanAndCheck(LexType::DotComma);
		break;
	}
	case LexType::Id:
	{
		int a, b, c;
		bool isAssigment = false;
		_sc->GetPtrs(a, b, c);
		auto full_name = GetFullName();
		Node* destination = _tree->GetNodeByView(full_name);
		//присваивание
		if (LookForward() == LexType::Equal)
		{
			ScanAndCheck(LexType::Equal);
			isAssigment = true;
		}
		else
		{
			//если это просто вызов операндов, то имя считается повторно
			//в элементарном выражении, поэтому возвращаем указатель
			_sc->SetPtrs(a, b, c);
		}
		auto exprResult = Expression();
		if (isAssigment)
		{
			
			SetValue(destination, exprResult);
		}
		ScanAndCheck(LexType::DotComma);
		break;
	}
	default:_tree->SemanticExit({ "оператор" }, ErrorCode::WrongSyntax);
	}
}
int i = 0;
void SyntacticalAnalyzer::WhileExecute()
{
	size_t expression_start_triad = tg.triads.size(), if_triad;
	ScanAndCheck(LexType::LRoundBracket);
	const auto data = Expression();
	Utils::CheckWhileExp(data->type, _tree);
	ScanAndCheck(LexType::RRoundBracket);
	if_triad = tg.WhileIfTriada();
	Operator();
	tg.WhileGotoTriada(if_triad, expression_start_triad);
}
// --{ declInFunc } --
void SyntacticalAnalyzer::CompoundBlock()
{
	auto comp_block = _tree->AddCompoundBlock();
	Block();
	_tree->SetTreePtr(comp_block);
}
Node* SyntacticalAnalyzer::Block()
{
	auto comp_block = _tree->AddEmptyNodeAsChild();
	ScanAndCheck(LexType::LFigBracket);
	DeclareInFunction();
	ScanAndCheck(LexType::RFigBracket);
	_tree->SetTreePtr(comp_block);
	return comp_block;
}


//просканировать на k символов вперед без 
//изменения указателей сканера
LexType SyntacticalAnalyzer::LookForward(size_t k)
{
	LexemaView lv;
	return LookForward(k, lv);
}
LexType SyntacticalAnalyzer::LookForward(size_t k, LexemaView& lv)
{
	int _ptr, _col, _line;
	_sc->GetPtrs(_ptr, _line, _col);
	LexType lex = LexType::End;
	for (size_t i = 0; i < k; i++)
	{
		lex = _sc->Scan(lv);
		if (lex == LexType::End)
			break;
	}
	_sc->SetPtrs(_ptr, _line, _col);
	return lex;
}

//считывает лексему и возвращает указатель на прежнее место,
//если лексема не равна заданной
bool SyntacticalAnalyzer::ScanAndCheck(LexType neededLex, LexemaView& lv, bool needMsg)
{
	int _ptr, _col, _line;
	_sc->GetPtrs(_ptr, _line, _col);
	auto lex = _sc->Scan(lv);
	if (lex != neededLex) {
		if (needMsg)
			LexExit(neededLex);
		_sc->SetPtrs(_ptr, _line, _col);
		return false;
	}
	return true;
}
bool SyntacticalAnalyzer::ScanAndCheck(LexType neededLex, bool needMsg)
{
	LexemaView lv;
	return ScanAndCheck(neededLex, lv, needMsg);
}

//Получить полное имя при обращении вида a.b.c...
//изменяет указатель
std::vector<LexemaView> SyntacticalAnalyzer::GetFullName()
{
	auto ids = std::vector<LexemaView>();
	LexType next = LookForward();
	while (next == LexType::Id) {
		LexemaView lv;
		_sc->Scan(lv);
		ids.push_back(lv);
		ScanAndCheck(LexType::Dot, false);
		next = LookForward();
	}
	return ids;
}

SemanticType SyntacticalAnalyzer::ScanType(LexemaView& lv) const
{
	auto lex = _sc->Scan(lv);
	if (lex == LexType::Short || lex == LexType::Long) {
		auto nextLex = _sc->Scan();
		return Utils::GetType(lex, nextLex);
	}
	return Utils::GetType(lex, lv, _tree);
}

void SyntacticalAnalyzer::TypeConvToLarge(Operand** a, Operand** b)
{
	if ((*a)->type != (*b)->type)
	{
		if ((*a)->type == SemanticType::Float) {
			(*b) = TypeConv(*b, SemanticType::Float);
		}
		else {
			(*a) = TypeConv(*a, SemanticType::Float);
		}
	}
}
Operand* SyntacticalAnalyzer::TypeConv(Operand* a, SemanticType neededType)
{
	if (a->type == neededType) return a;
	if (!Utils::IsComparableType(a->type, neededType))
		_tree->SemanticExit({ a->type.id, neededType.id }, ErrorCode::WrongTypeConversion);
	else return tg.TypeConv(a, neededType);
}

//---- class id { Program } ;----
void SyntacticalAnalyzer::ClassDeclare()
{
	LexemaView className;
	std::vector<LexType> ltsBeforeProgramm = { LexType::Class, LexType::Id, LexType::LFigBracket };
	std::vector<LexType> ltsAfterProgramm = { LexType::RFigBracket, LexType::DotComma };
	ScanAndCheck(LexType::Class);
	ScanAndCheck(LexType::Id, className);
	ScanAndCheck(LexType::LFigBracket);
	const auto class_ptr = _tree->AddClass(className);
	Program(LexType::RFigBracket);
	_tree->SetTreePtr(class_ptr);
	ScanAndCheck(LexType::RFigBracket);
	ScanAndCheck(LexType::DotComma);
}
Operand* SyntacticalAnalyzer::Expression()
{
	auto o1 = LogicalExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::LogEqual || nextLex == LexType::LogNotEqual) {
		auto sign = _sc->Scan();
		o1 = BinaryOperation(o1, LogicalExpression(), sign);
		nextLex = LookForward();
	}
	return o1;
}

Operand* SyntacticalAnalyzer::LogicalExpression()
{
	auto o1 = ShiftExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::Less || nextLex == LexType::LessOrEqual ||
		nextLex == LexType::More || nextLex == LexType::MoreOrEqual) {
		auto sign = _sc->Scan();
		o1 = BinaryOperation(o1, ShiftExpression(), sign);
		nextLex = LookForward();
	}
	return o1;
}

Operand* SyntacticalAnalyzer::ShiftExpression()
{

	auto o1 = AdditionalExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::ShiftLeft || nextLex == LexType::ShiftRight) {
		auto sign = _sc->Scan();
		auto o2 = AdditionalExpression();
		o1 = BinaryOperation(o1, o2, sign);
		nextLex = LookForward();
	}
	return o1;
}

Operand* SyntacticalAnalyzer::AdditionalExpression()
{
	auto nextLex = LookForward();
	bool unaryOperation = false;
	LexType sign;
	if (nextLex == LexType::Plus || nextLex == LexType::Minus) {
		sign = _sc->Scan();
		unaryOperation = true;
	}
	auto o1 = MultExpression();
	if (unaryOperation)
	{
		o1 = UnaryOperation(o1, sign);
	}
	nextLex = LookForward();
	while (nextLex == LexType::Plus || nextLex == LexType::Minus) {
		sign = _sc->Scan();
		o1 = BinaryOperation(o1, MultExpression(), sign);
		nextLex = LookForward();
	}
	return o1;
}

Operand* SyntacticalAnalyzer::MultExpression()
{
	auto o1 = ElementaryExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::DivSign || nextLex == LexType::ModSign || nextLex == LexType::MultSign) {
		auto sign = _sc->Scan();
		auto o2 = ElementaryExpression();
		o1 = BinaryOperation(o1, o2, sign);
		nextLex = LookForward();
	}
	return o1;
}

Operand* SyntacticalAnalyzer::ElementaryExpression()
{
	LexType lexType = LookForward();
	LexemaView lexemaView;
	Operand* result = new Operand();
	result->type = SemanticType::Undefined;
	switch (lexType) {
	case LexType::ConstExp:
	case LexType::ConstInt:
		_sc->Scan(lexemaView);
		result = Utils::GetConstOperand(lexemaView, lexType, _tree);
		break;

	case LexType::Id: {
		auto ids = GetFullName();
		lexType = LookForward();
		//если происходит вызов функции
		if (lexType == LexType::LRoundBracket) {
			_sc->Scan();
			ScanAndCheck(LexType::RRoundBracket);
			result = FunctionExecute(ids);
		}
		else
		{
			result = Utils::GetDataOperand(_tree->GetNodeValue(ids));
		}
		break;
	}
	case LexType::LRoundBracket:
		_sc->Scan();
		result = Expression();
		ScanAndCheck(LexType::RRoundBracket);
		break;
	default:
		_tree->SemanticExit({ "Ожидается операнд, но встречен \"" , LexTypesName.find(lexType)->second, "\"" }, ErrorCode::WrongSyntax);
	}
	return result;
}

Operand* SyntacticalAnalyzer::BinaryOperation(Operand* o1, Operand* o2, LexType sign)
{
	TypeConvToLarge(&o1, &o2);
	return tg.BinaryOperation(o1, o2, sign);
}

Operand* SyntacticalAnalyzer::UnaryOperation(Operand* o1, LexType sign)
{
	return tg.UnaryOperation(o1, sign);
}

void SyntacticalAnalyzer::SetValue(Operand* dest, Operand* src)
{
	if(dest->type != src->type)
	{
		src = TypeConv(src, dest->type);
	}
	tg.SetValue(src, dest);
}

void SyntacticalAnalyzer::SetValue(Node* destination, Operand* src)
{
	auto dest = new Operand();
	dest->type = destination->_data->type;
	dest->lexema = destination->_data->id;
	SetValue(dest, src);
}

//     ---|     CLASS		|---
//     ---|     FUNCTION	|---
//     ---|     DATA		|---
//  ------------------------------
void SyntacticalAnalyzer::Program(LexType endLex)
{
	auto nextLex = LookForward();
	while (nextLex != endLex) {
		if (nextLex == LexType::Class)
			ClassDeclare();
		else {
			int _ptr, _col, _line;
			_sc->GetPtrs(_ptr, _line, _col);
			LexemaView type_view;
			if (ScanType(type_view) == SemanticType::Undefined)
				_tree->SemanticExit({ "Тип \'" , type_view, "\' не определен" }, ErrorCode::Undefined);
			//получить следующую за id лексему
			nextLex = LookForward(2);
			_sc->SetPtrs(_ptr, _line, _col);
			if (nextLex == LexType::LRoundBracket)
				FunctionDeclare();
			else
				DataDeclare();
		}
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::PrintSemanticTree(std::ostream& out) const
{
	//_tree->Print(out);
	tg.printTriads();
}

void SyntacticalAnalyzer::LexExit(const std::vector<std::string>& waiting) const
{
	_sc->ErrorMsg(MSG_ID::SYNT_ERR, waiting);
	exit(1);
}


void SyntacticalAnalyzer::LexExit(LexType waitingLex) const
{
	LexExit({ LexTypesName.find(waitingLex)->second });
}
//  
//     ---|     CLASS		|---
//     ---|     DATA		|---
//     ---|     OPERATOR	|---
//  ------------------------------
void SyntacticalAnalyzer::DeclareInFunction()
{
	LexType endLex = LexType::RFigBracket;
	LexemaView lv;
	LexType nextLex = LookForward(1, lv);
	while (nextLex != endLex) {
		if (nextLex == LexType::Class) {
			ClassDeclare();
		}
		else {
			int a, b, c;
			LexemaView tmp;
			_sc->GetPtrs(a, b, c);
			auto type = ScanType(tmp);
			_sc->SetPtrs(a, b, c);
			if (type != SemanticType::Undefined)
			{
				DataDeclare();
			}
			else
			{
				Operator();
			}
		}
		nextLex = LookForward();
	}
}


//---type   |--id    --|() block
//          |--main  --|
void SyntacticalAnalyzer::FunctionDeclare()
{
	LexemaView func_name, type_view;
	Node* func_body = nullptr;
	auto returned_type = ScanType(type_view);

	if (returned_type == SemanticType::Undefined) {
		_tree->SemanticExit({ "Тип \'" , type_view, "\' не определен" }, ErrorCode::Undefined);
	}

	returned_type.id = type_view;
	if (!ScanAndCheck(LexType::main, func_name, false)) {
		ScanAndCheck(LexType::Id, func_name);
	}
	ScanAndCheck(LexType::LRoundBracket);
	ScanAndCheck(LexType::RRoundBracket);
	auto func_node = _tree->AddFunctionDeclare(returned_type, func_name, func_body);
	tg.DeclFunctionBegin(func_name);
	Block();
	tg.DeclFunctionEnd();
	_tree->SetTreePtr(func_node);
}

void SyntacticalAnalyzer::ReturnDeclare(Operand* returned)
{
	auto func = dynamic_cast<FunctionData*>(_tree->FindCurrentFunc()->_data);
	TypeConv(returned, func->returned_type);
	tg.DeclFunctionRet();
}

Operand * SyntacticalAnalyzer::FunctionExecute(std::vector<LexemaView> name)
{
	auto func = dynamic_cast<FunctionData*>(_tree->FindUp(name[name.size() - 1])->_data);
	return tg.FunctionCall(func->id, func->returned_type);
}


//type var |;
//         |, var...;
void PrintUpdate(std::string _event)
{
	std::cout << "\n\n" << _event;
}
void SyntacticalAnalyzer::DataDeclare()
{
	LexemaView type_view, var_name, value_view;
	const auto current_type = ScanType(type_view);
	if (current_type == SemanticType::Undefined)
		_tree->SemanticExit({ "Тип \'" , type_view, "\' не определен" }, ErrorCode::Undefined);
	//read first variable name
	ScanAndCheck(LexType::Id, var_name);
	Node* variable;
	if (current_type == SemanticType::ClassObj)
	{
		variable = _tree->AddClassObject(var_name, type_view);
	}
	else {
		variable = _tree->AddVariableObject(new Data(current_type, var_name));
	}
	//initiate, if needed
	if (LookForward() == LexType::Equal) {
		_sc->Scan();
		auto exprResult = Expression();
		SetValue(variable, exprResult);
	}

	while (LookForward() == LexType::Comma) {
		_sc->Scan();
		ScanAndCheck(LexType::Id, var_name);
		if (current_type == SemanticType::ClassObj)
		{
			variable = _tree->AddClassObject(var_name, type_view);
		}
		else {
			variable = _tree->AddVariableObject(new Data(current_type, var_name));
		}
		//initiate, if needed
		if (LookForward() == LexType::Equal) {
			_sc->Scan();
			auto exprResult = Expression();
			SetValue(variable, exprResult);
		}
	}
	ScanAndCheck(LexType::DotComma);
}





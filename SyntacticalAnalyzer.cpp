#include "SyntacticalAnalyzer.h"

#include <functional>
#include <utility>

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
		ScanAndCheck(LexType::LRoundBracket);
		//условие
		auto data = Expression();
		if (!_tree->IsComparableType(data->type, SemanticType::ShortInt))
		{
			_tree->SemanticExit({ "Недопустимое условие для 'while'" });
		}
		ScanAndCheck(LexType::RRoundBracket);
		Operator();
		break;
	}
	case LexType::Return: {
		// todo поднимать флаг ретурна только если он вызван не в операторе 
		_sc->Scan();
		auto returnedData = Expression();
		auto func = _tree->FindCurrentFunc();
		auto func_data = dynamic_cast<FunctionData*>(func->_data);
		func_data->is_return_operator_declarated = true;
		if (!_tree->IsComparableType(returnedData->type, func_data->returned_type))
		{
			_tree->SemanticExit({ "Тип возвращаемого значения не соответсвует объявленному" });
		}
		func_data->returned_data = returnedData;
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
			_tree->SetData(destination, exprResult);
		}
		ScanAndCheck(LexType::DotComma);
		break;
	}
	default:_tree->SemanticExit({ "Ошибочная операция" });
	}
}
// --{ declInFunc } --
void SyntacticalAnalyzer::CompoundBlock()
{
	auto block_ptr = _tree->AddCompoundBlock();
	ScanAndCheck(LexType::LFigBracket);
	DeclareInFunction();
	ScanAndCheck(LexType::RFigBracket);
	_tree->SetTreePtr(block_ptr);
	_tree->RemoveObject(block_ptr);
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

//Получить полное при обращении вида a.b.c...
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
		return _tree->GetType(lex, nextLex);
	}
	return _tree->GetType(lex, lv);
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
	_tree->AddCompoundBlock();
	Program(LexType::RFigBracket);
	_tree->SetTreePtr(class_ptr);
	ScanAndCheck(LexType::RFigBracket);
	ScanAndCheck(LexType::DotComma);
}
Data* SyntacticalAnalyzer::Expression()
{
	auto o1 = LogicalExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::LogEqual || nextLex == LexType::LogNotEqual) {
		auto sign = _sc->Scan();
		o1 = _tree->LogicalOperation(o1, LogicalExpression(), sign);
		nextLex = LookForward();
	}
	return o1;
}

Data* SyntacticalAnalyzer::LogicalExpression()
{
	auto o1 = ShiftExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::Less || nextLex == LexType::LessOrEqual ||
		nextLex == LexType::More || nextLex == LexType::MoreOrEqual) {
		auto sign = _sc->Scan();
		o1 = _tree->LogicalOperation(o1, ShiftExpression(), sign);
		nextLex = LookForward();
	}
	return o1;
}

Data* SyntacticalAnalyzer::ShiftExpression()
{
	auto o1 = AdditionalExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::ShiftLeft || nextLex == LexType::ShiftRight) {
		auto sign = _sc->Scan();
		auto o2 = AdditionalExpression();
		o1 = _tree->BinaryOperation(o1, o2, sign);
		nextLex = LookForward();
	}
	return o1;
}

Data* SyntacticalAnalyzer::AdditionalExpression()
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
		o1 = _tree->UnaryOperation(o1, sign);
	}
	nextLex = LookForward();
	while (nextLex == LexType::Plus || nextLex == LexType::Minus) {
		sign = _sc->Scan();
		o1 = _tree->BinaryOperation(o1, MultExpression(), sign);
		nextLex = LookForward();
	}
	return o1;
}

Data* SyntacticalAnalyzer::MultExpression()
{
	auto o1 = ElementaryExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::DivSign || nextLex == LexType::ModSign || nextLex == LexType::MultSign) {
		auto sign = _sc->Scan();
		auto o2 = ElementaryExpression();
		o1 = _tree->BinaryOperation(o1, o2, sign);
		nextLex = LookForward();
	}
	return o1;
}


Data* SyntacticalAnalyzer::ElementaryExpression()
{
	LexType lexType = LookForward();
	LexemaView lexemaView;
	Data* result = new Data(SemanticType::Undefined, "");
	switch (lexType) {
	case LexType::ConstExp:
	case LexType::ConstInt:
		_sc->Scan(lexemaView);
		result = _tree->GetConstData(lexemaView, lexType);
		break;

	case LexType::Id: {
		auto ids = GetFullName();
		lexType = LookForward();
		//если происходит вызов функции
		if (lexType == LexType::LRoundBracket) {
			result = FunctionCall(ids);
		}
		//при обращении к переменной
		else
		{
			//проверить, есть ли такая переменная и вернуть ее тип и значение
			result = _tree->GetNodeByView(ids)->_data->Clone();
		}
		break;
	}
	case LexType::LRoundBracket:
		_sc->Scan();
		result = Expression();
		ScanAndCheck(LexType::RRoundBracket);
		break;
	default:
		_tree->SemanticExit({ "Ожидается операнд, но встречен \"" , LexTypesName.find(lexType)->second, "\""});
	}
	return result;
}

Data* SyntacticalAnalyzer::FunctionCall(std::vector<LexemaView>& ids)
{
	_sc->Scan();
	//в нашей программе методы без параметров
	ScanAndCheck(LexType::RRoundBracket);
	auto funcDeclare = _tree->GetNodeByView(ids, true);
	//скопировать заголовок функции
	auto func = _tree->FunctionCall(ids);
	auto data = dynamic_cast<FunctionData*>(func->_data);
	//сохранить контекст
	int ptr, line, col;
	_sc->GetPtrs(ptr, line, col);
	//установить контекст описания фукнции
	_sc->SetPtrs(data->ptr, data->line, data->col);
	_tree->SetTreePtr()	

	CompoundBlock();
	_sc->SetPtrs(ptr, line, col);
	 _tree->isInterpreting = saved_interpret;
	return data->returned_data;

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
				_tree->SemanticExit({ "Тип \'" , type_view, "\' не определен" });
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
	_tree->Print(out);
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
	bool is_main = false, saved_interpret;

	auto returned_type = ScanType(type_view);
	if (returned_type == SemanticType::Undefined) {
		_tree->SemanticExit({ "Тип \'" , type_view, "\' не определен" });
	}
	returned_type.id = type_view;
	if (ScanAndCheck(LexType::main, func_name, false))
	{
		is_main = true;
		_tree->isInterpreting = true;
	}
	else {
		ScanAndCheck(LexType::Id, func_name);
		saved_interpret = _tree->isInterpreting;
		_tree->isInterpreting = false;
	}
	//create node in tree 
	auto func_node = _tree->AddFunctionDeclare(returned_type, func_name);
	auto func_data = dynamic_cast<FunctionData*>(func_node->_data);

	ScanAndCheck(LexType::LRoundBracket);
	ScanAndCheck(LexType::RRoundBracket);

	//save ptr's to function body
	_sc->GetPtrs(func_data->ptr, func_data->line, func_data->col);
	CompoundBlock();
	if (!func_data->is_return_operator_declarated && func_data->returned_type != SemanticType::Void)
	{
		_tree->SemanticExit({ func_name,  " должна возвращать значение" });
	}
	_tree->SetTreePtr(func_node);
	if(!is_main)
	{
		_tree->isInterpreting = saved_interpret;
	}
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
		_tree->SemanticExit({ "Тип \'" , type_view, "\' не определен" });
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
		_tree->SetData(variable, Expression());
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
			_tree->SetData(variable, Expression());
		}
	}
	ScanAndCheck(LexType::DotComma);
}





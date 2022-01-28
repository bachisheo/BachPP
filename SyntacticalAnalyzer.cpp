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
		auto type = Expression();
		if (!_tree->IsComparableType(type, SemanticType::ShortInt))
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
		auto returnedType = Expression();
		auto function_data = dynamic_cast<FunctionData*>(_tree->FindCurrentFunc()->_data);
		function_data->is_return_operator_declarated = true;
		if (!_tree->IsComparableType(returnedType, function_data->returned_type))
		{
			_tree->SemanticExit({ "Тип возвращаемого значения не соответсвует объявленному" });
		}
		ScanAndCheck(LexType::DotComma);
		break;
	}
	case LexType::Id:
	{
		SemanticType needed = SemanticType::Undefined;
		int a, b, c;
		_sc->GetPtrs(a, b, c);
		auto full_name = GetFullName();
		if (LookForward() == LexType::Equal)
		{
			ScanAndCheck(LexType::Equal);
			needed = _tree->GetTypeByView(full_name);
		}
		else
		{
			_sc->SetPtrs(a, b, c);
		} 
		//присваивание 
		auto valueType = Expression();
		if (needed != SemanticType::Undefined && 
			!_tree->IsComparableType(valueType, needed))
		{
			_tree->SemanticExit({ "Тип присваемого значения не соответсвует типу переменной" });
		};
		ScanAndCheck(LexType::DotComma);
		break;
		
	}
	default:_tree->SemanticExit({ "Ошибочная операция" });;
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
	//_tree->RemoveObject(block_ptr);
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
SemanticType SyntacticalAnalyzer::Expression()
{
	auto o1 = LogicalExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::LogEqual || nextLex == LexType::LogNotEqual) {
		auto sign = _sc->Scan();
		o1 = _tree->GetResultType(o1, LogicalExpression(), sign);
		nextLex = LookForward();
	}
	return o1;
}

SemanticType SyntacticalAnalyzer::LogicalExpression()
{
	auto o1 = ShiftExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::Less || nextLex == LexType::LessOrEqual ||
		nextLex == LexType::More || nextLex == LexType::MoreOrEqual) {
		auto sign = _sc->Scan();
		auto o2 = ShiftExpression();
		o1 = _tree->GetResultType(o1, o2, sign);
		nextLex = LookForward();
	}
	return o1;
}

SemanticType SyntacticalAnalyzer::ShiftExpression()
{
	auto o1 = AdditionalExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::ShiftLeft || nextLex == LexType::ShiftRight) {
		auto sign = _sc->Scan();
		auto o2 = AdditionalExpression();
		o1 = _tree->GetResultType(o1, o2, sign);
		nextLex = LookForward();
	}
	return o1;
}

SemanticType SyntacticalAnalyzer::AdditionalExpression()
{
	auto nextLex = LookForward();
	bool unaryOperation = false;
	if (nextLex == LexType::Plus || nextLex == LexType::Minus) {
		_sc->Scan();
		unaryOperation = true;
	}
	auto o1 = MultExpression();
	if (unaryOperation)
	{
		_tree->IsEnableUnarySign(o1);
	}
	nextLex = LookForward();
	while (nextLex == LexType::Plus || nextLex == LexType::Minus) {
		auto sign = _sc->Scan();
		auto o2 = MultExpression();
		o1 = _tree->GetResultType(o1, o2, sign);
		nextLex = LookForward();
	}
	return o1;
}

SemanticType SyntacticalAnalyzer::MultExpression()
{
	auto o1 = ElementaryExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::DivSign || nextLex == LexType::ModSign || nextLex == LexType::MultSign) {
		auto sign = _sc->Scan();
		auto o2 = ElementaryExpression();
		o1 = _tree->GetResultType(o1, o2, sign);
		nextLex = LookForward();
	}
	return o1;
}


SemanticType SyntacticalAnalyzer::ElementaryExpression()
{
	LexType lexType = LookForward();
	LexemaView lv;
	SemanticType result;
	switch (lexType) {
	case LexType::ConstExp:
	case LexType::ConstInt:
		_sc->Scan(lv);
		result = _tree->GetConstType(lv, lexType);
		break;

	case LexType::Id: {
		auto ids = GetFullName();
		lexType = LookForward();
		//если происходит вызов функции
		if (lexType == LexType::LRoundBracket) {
			_sc->Scan();
			//проверить, есть ли такая функция и вернуть ее тип
			result = _tree->GetTypeByView(ids, true);
			ScanAndCheck(LexType::RRoundBracket);
		}
		else
			//при обращении к переменной
		{
			//проверить, есть ли такая переменная и вернуть ее тип
			result = _tree->GetTypeByView(ids);
		}
		break;
	}
	case LexType::LRoundBracket:
		_sc->Scan();
		result = Expression();
		ScanAndCheck(LexType::RRoundBracket);
		break;
	default:
		LexExit({ "No empty expression" });
		result = SemanticType::Undefined;
	}
	return result;
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
	auto returned_type = ScanType(type_view);
	if (returned_type == SemanticType::Undefined) {
		_tree->SemanticExit({ "Тип \'" , type_view, "\' не определен" });
	}
	returned_type.id = type_view;
	if (!ScanAndCheck(LexType::main, func_name, false))
		ScanAndCheck(LexType::Id, func_name);
	//create node in tree, save ptr
	auto func_node = _tree->AddFunctionDeclare(returned_type, func_name);
	auto func_data = dynamic_cast<FunctionData*>(func_node->_data);

	ScanAndCheck(LexType::LRoundBracket);
	ScanAndCheck(LexType::RRoundBracket);
	CompoundBlock();
	if (!func_data->is_return_operator_declarated && func_data->returned_type != SemanticType::Void)
	{
		_tree->SemanticExit({ func_name,  " должна возвращать значение" });
	}
	_tree->SetTreePtr(func_node);
}
//type var |;
//         |, var...;
void PrintUpdate(std::string _event, SemanticTree * tree)
{
	std::cout << "\n\n" << _event;
	tree->Print(std::cout);
}
void SyntacticalAnalyzer::DataDeclare()
{
	LexemaView type_view, var_name;
	const auto current_type = ScanType(type_view);
	if (current_type == SemanticType::Undefined)
		_tree->SemanticExit({ "Тип \'" , type_view, "\' не определен" });
	//read first variable name
	ScanAndCheck(LexType::Id, var_name);
	//initiate, if needed
	if (LookForward() == LexType::Equal) {
		_sc->Scan();
		if (!_tree->IsComparableType(current_type, Expression()))
		{
			_tree->SemanticExit({ " Ошибка приведения типов" });
		}
	}
	if (current_type == SemanticType::ClassObj)
	{
		_tree->AddClassObject(var_name, type_view);
	}
	else {
		_tree->CheckUnique(var_name);
		_tree->AddObject(new Data(current_type, var_name));
	}


	while (LookForward() == LexType::Comma) {
		_sc->Scan();
		ScanAndCheck(LexType::Id, var_name);
		if (LookForward() == LexType::Equal) {
			_sc->Scan();
			if (!_tree->IsComparableType(current_type, Expression()))
			{
				_tree->SemanticExit({ "Ошибка приведения типов" });
			}
		}
		if (current_type == SemanticType::ClassObj)
		{
			_tree->AddClassObject(var_name, type_view);
		}
		else {
			_tree->CheckUnique(var_name);
			_tree->AddObject(new Data(current_type, var_name));
		}
	}
	ScanAndCheck(LexType::DotComma);
}





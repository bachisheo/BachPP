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
		//блок
	case LexType::LFigBracket:
	{
		auto ptr = _tree->AddCompoundBlock();
		Block();
		_tree->SetTreePtr(ptr);
		break;
	}
	//while с предусловием
	case LexType::While: {
		_sc->Scan();
		CheckScan(LexType::LRoundBracket);
		//условие
		auto type = Expression();
		if (!_tree->IsComparableType(type, SemanticType::ShortInt))
		{
			_tree->SemanticExit({ "Недопустимое условие для 'while'"  });

		};
		CheckScan(LexType::RRoundBracket);
		Operator();
		break;
	}
		//присваивание 
	case LexType::Id:
	{
		auto name = GetFullName();
		auto varType = _tree->GetTypeByView(name);
		CheckScan(LexType::Equal);
		auto valueType = Expression();
		if(!_tree->IsComparableType(valueType, varType))
		{
			_tree->SemanticExit({ "Тип присваемого значения не соответсвует типу переменной" });

		};
		CheckScan(LexType::DotComma);
		break;
	}
	case LexType::Return: {
		_sc->Scan();
		auto returnedType = Expression();
		auto function_data = dynamic_cast<FunctionData*>(_tree->FindCurrentFunc()->_data);
		function_data->is_return_operator_declarated = true;
		if (!_tree->IsComparableType(returnedType, function_data->returned_type))
		{
			_tree->SemanticExit({ "Тип возвращаемого значения не соответсвует объявленному" });
		}
		CheckScan(LexType::DotComma);
		break;
	}
	default:_tree->SemanticExit({ "Ошибочная операция" });;
	}
}
// --{ declInFunc } --
void SyntacticalAnalyzer::Block()
{
	CheckScan(LexType::LFigBracket);
	DeclarateInFunction();
	CheckScan(LexType::RFigBracket);
}

//просканировать на k символов вперед без 
//изменения указателей сканера
LexType SyntacticalAnalyzer::LookForward(size_t k)
{
	LexemaView lv;
	return LookForward(k, lv);
}
LexType SyntacticalAnalyzer::LookForward(size_t k, LexemaView & lv)
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
bool SyntacticalAnalyzer::CheckScan(LexType neededLex, LexemaView& lv, bool needMsg)
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
bool SyntacticalAnalyzer::CheckScan(LexType neededLex, bool needMsg)
{
	LexemaView lv;
	return CheckScan(neededLex, lv, needMsg);
}

//Получить полное при обращении вида a.b.c...
std::vector<LexemaView> SyntacticalAnalyzer::GetFullName()
{
	auto ids = std::vector<LexemaView>();
	LexType next = LookForward();
	while (next == LexType::Id) {
		LexemaView lv;
		_sc->Scan(lv);
		ids.push_back(lv);
		CheckScan(LexType::Dot, false);
		next = LookForward();
	}
	return ids;
}

SemanticType SyntacticalAnalyzer::Type(LexemaView& lv) const
{

	auto lex = _sc->Scan(lv);
	if (lex == LexType::Short || lex == LexType::Long) {
		auto nextLex = _sc->Scan();
		return _tree->GetType(lex, nextLex);
	}
	return _tree->GetType(lex, lv);
}

//---- class id { Program } ;----
void SyntacticalAnalyzer::ClassDeclarate()
{
	LexemaView className;
	std::vector<LexType> ltsBeforeProgramm = { LexType::Class, LexType::Id, LexType::LFigBracket };
	std::vector<LexType> ltsAfterProgramm = { LexType::RFigBracket, LexType::DotComma };
	CheckScan(LexType::Class);
	CheckScan(LexType::Id, className);
	CheckScan(LexType::LFigBracket);

	const auto ptr = _tree->AddClass(className);
	Program(LexType::RFigBracket);
	_tree->SetTreePtr(ptr);
	CheckScan(LexType::RFigBracket);
	CheckScan(LexType::DotComma);
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
			CheckScan(LexType::RRoundBracket);
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
		CheckScan(LexType::RRoundBracket);
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
			ClassDeclarate();
		else {
			int _ptr, _col, _line;
			_sc->GetPtrs(_ptr, _line, _col);
			LexemaView typeView;
			Type(typeView);
			//получить следующую за id лексему
			nextLex = LookForward(2);
			_sc->SetPtrs(_ptr, _line, _col);
			if (nextLex == LexType::LRoundBracket)
				FunctionDeclarate();
			else
				Data();
		}
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::PrintSemanticTree(std::ostream& out)
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
void SyntacticalAnalyzer::DeclarateInFunction()
{
	LexType endLex = LexType::RFigBracket;
	LexemaView lv;
	LexType nextLex = LookForward(1, lv);
	while (nextLex != endLex) {
		if (nextLex == LexType::Class) {
			ClassDeclarate();
		}
		else {
			
			auto it = std::find(TypeWords.begin(), TypeWords.end(), nextLex);
			if (it == TypeWords.end() || 
				(*it == LexType::Id && LookForward(2) != LexType::Id))
			{
				Operator();
			}
			else Data();
		}
		nextLex = LookForward();
	}
}


//---type   |--id    --|() block
//          |--main  --|
void SyntacticalAnalyzer::FunctionDeclarate()
{
	LexemaView funcView, typeView;
	auto returned_type = Type(typeView);
	returned_type.id = typeView;
	if (!CheckScan(LexType::main, funcView, false))
		CheckScan(LexType::Id, funcView);
	auto func_node = _tree->AddFunc(returned_type, funcView);
	auto func_data = dynamic_cast<FunctionData*>(func_node->_data);
	CheckScan(LexType::LRoundBracket);
	CheckScan(LexType::RRoundBracket);
	Block();
	if(func_data->is_return_operator_declarated == false && func_data->returned_type != SemanticType::Void)
	{
		_tree->SemanticExit({ funcView,  " должна возвращать значение" });
	}
	_tree->SetTreePtr(func_node);
}
//type var |;
//         |, var...;
void SyntacticalAnalyzer::Data()
{
	LexemaView typeView;
	const auto varType = Type(typeView);
	LexemaView objName;
	CheckScan(LexType::Id, objName);
	if (LookForward() == LexType::Equal) {
		_sc->Scan();
		auto valueType = Expression();
		if(!_tree->IsComparableType(varType, valueType))
		{
			_tree->SemanticExit({ " Ошибка приведения типов" });
		}
	}
	if (varType == SemanticType::ClassObj)
	{
		_tree->AddClassObject(objName, typeView);
	}
	else {
		_tree->IsUnique(objName);
		_tree->AddObject(objName, varType);
	}
	while (LookForward() == LexType::Comma) {
		_sc->Scan();
		CheckScan(LexType::Id, objName);
		if (LookForward() == LexType::Equal) {
			_sc->Scan();
			auto valueType = Expression();
			if (!_tree->IsComparableType(varType, valueType))
			{
				_tree->SemanticExit({ "Несоответсвие типов" });
			}
		}
		if (varType == SemanticType::ClassObj)
		{
			_tree->AddClassObject(objName, typeView);
		}
		else {
			_tree->IsUnique(objName);

			_tree->AddObject(objName, varType);
		}
	}
	CheckScan(LexType::DotComma);
}





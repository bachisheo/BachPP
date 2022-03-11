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
		//����
	case LexType::LFigBracket:
	{
		CompoundBlock();
		break;
	}

	//while � ������������
	case LexType::While: {
		_sc->Scan();
		WhileExecute();
		break;
	}
	case LexType::Return: {
		// todo ��������� ���� ������� ������ ���� �� ������ �� � ��������� 
		_sc->Scan();
		auto returnedData = Expression();
		_tree->SetReturnedData(returnedData);
		ScanAndCheck(LexType::DotComma);
		if (_tree->isInterpreting)
			_tree->isReturned = true;
		break;
	}
	case LexType::Id:
	{
		int a, b, c;
		bool isAssigment = false;
		_sc->GetPtrs(a, b, c);
		auto full_name = GetFullName();
		Node* destination = _tree->GetNodeByView(full_name);
		//������������
		if (LookForward() == LexType::Equal)
		{
			ScanAndCheck(LexType::Equal);
			isAssigment = true;
		}
		else
		{
			//���� ��� ������ ����� ���������, �� ��� ��������� ��������
			//� ������������ ���������, ������� ���������� ���������
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
	default:_tree->SemanticExit({ "��������� ��������" });
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

void SyntacticalAnalyzer::WhileExecute()
{
	int ptr, col, line;
	_sc->GetPtrs(ptr, line, col);
	bool savedInt = _tree->isInterpreting;
	while_body:
	_sc->SetPtrs(ptr, line, col);
	ScanAndCheck(LexType::LRoundBracket);
	//�������
	auto data = Expression();
	_tree->CheckWhileExp(data);
	_tree->isInterpreting = data->value.short_int_value && _tree->isWork();
	ScanAndCheck(LexType::RRoundBracket);
	Operator();
	if(_tree->isInterpreting) 
		goto while_body;
	_tree->isInterpreting = savedInt;
}

//�������������� �� k �������� ������ ��� 
//��������� ���������� �������
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

//��������� ������� � ���������� ��������� �� ������� �����,
//���� ������� �� ����� ��������
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

//�������� ������ ��� ��������� ���� a.b.c...
//�������� ���������
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
		//���� ���������� ����� �������
		if (lexType == LexType::LRoundBracket) {
			_sc->Scan();
			ScanAndCheck(LexType::RRoundBracket);
			result = FunctionExecute(ids);
		}
		else
		{
			result = _tree->GetNodeValue(ids);
		}
		break;
	}
	case LexType::LRoundBracket:
		_sc->Scan();
		result = Expression();
		ScanAndCheck(LexType::RRoundBracket);
		break;
	default:
		_tree->SemanticExit({ "��������� �������, �� �������� \"" , LexTypesName.find(lexType)->second, "\"" });
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
				_tree->SemanticExit({ "��� \'" , type_view, "\' �� ���������" });
			//�������� ��������� �� id �������
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
	bool isIntSaved = _tree->isInterpreting, isMain = true;
	LexemaView func_name, type_view;
	auto returned_type = ScanType(type_view);
	if (returned_type == SemanticType::Undefined) {
		_tree->SemanticExit({ "��� \'" , type_view, "\' �� ���������" });
	}
	returned_type.id = type_view;
	if (!ScanAndCheck(LexType::main, func_name, false)) {
		ScanAndCheck(LexType::Id, func_name);
		isMain = false;
	}
	//create node in tree, save ptr
	auto func_node = _tree->AddFunctionDeclare(returned_type, func_name);
	ScanAndCheck(LexType::LRoundBracket);
	ScanAndCheck(LexType::RRoundBracket);
	if (func_node)
	{
		auto data = dynamic_cast<FunctionData*>(func_node->_data);
		_sc->GetPtrs(data->ptr, data->line, data->col);
	}
	_tree->isInterpreting = isMain;
	CompoundBlock();
	_tree->SetTreePtr(func_node);
	_tree->isInterpreting = isIntSaved;
}

Data* SyntacticalAnalyzer::FunctionExecute(std::vector<LexemaView> ids)
{
	if (!_tree->isWork())
		return nullptr;
	_tree->isReturned = false;
	//�������� ���� � ��������� �������
	auto func = _tree->GetNodeByView(ids, true);
	auto fd = dynamic_cast<FunctionData*>(func->_data);
	//��������� ������� ��������
	int ptr, line, col;
	auto cur = _tree->GetTreePtr();
	_sc->GetPtrs(ptr, line, col);
	//������������ �������� �������
	auto func_call = _tree->AddFunctionCall(func);
	_sc->SetPtrs(fd->ptr, fd->line, fd->col);
	//��������� �������� �������
	CompoundBlock();
	auto result = dynamic_cast<FunctionData*>(func_call->_data->Clone());
	if (!result->is_return_operator_declarated && result->returned_type != SemanticType::Empty)
	{
		_tree->SemanticExit({ "������� ������ ���������� ��������" });
	}
	//������������ ��������� ��������� �� ������
	_tree->isReturned = false;
	_tree->RemoveFunctionCall(func_call);
	_tree->SetTreePtr(cur);
	_sc->SetPtrs(ptr, line, col);
	return result->returned_data;
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
		_tree->SemanticExit({ "��� \'" , type_view, "\' �� ���������" });
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





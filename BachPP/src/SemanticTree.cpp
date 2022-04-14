#include "SemanticTree.h"

#include <string>
bool SemanticTree::IsDataType(SemanticType type) const
{
	return (type == SemanticType::LongInt || type == SemanticType::ShortInt || type == SemanticType::Float);
}

SemanticTree::SemanticTree(Scanner* sc) :_sc(sc)
{
	_current = nullptr;
}

Node* SemanticTree::AddClassObject(const LexemaView& objName, const LexemaView& className)
{
	const auto classDeclaration = FindUp(className);
	if (classDeclaration == nullptr)
	{
		SemanticExit({ className },ErrorCode::ObjectNotFound);
	}
	SemanticType type = SemanticType::ClassObj;
	type.id = className;
	//add object root
	auto obj = AddVariableObject(new Data(type, objName));
	obj->SetChild(CopySubtree(classDeclaration->GetChild()));
	return obj;
}

void SemanticTree::CheckWhileExp(Data* data) const
{
	if (!IsComparableType(data->type, SemanticType::ShortInt))
	{
		SemanticExit({ "ShortInt" },ErrorCode::WaitForOtherType);
	}
}

bool SemanticTree::IsWhileExecute(Data* data) const
{
	return data->value.short_int_value;
}



//Получить значение переменной
//или результата выоплнения функции
Data* SemanticTree::GetNodeValue(std::vector<LexemaView> ids) const
{
	return GetNodeByView(ids)->_data->Clone();
}

Node* SemanticTree::AddFunctionCall(Node* func)
{
	Node* func_exec = new Node(func->_data->Clone());
	Node* neib = func->GetNeighbor();
	func_exec->AddNeighbor(neib);
	if (neib)
	{
		neib->SetParent(func_exec);
	}
	func->_neighbor = func_exec;
	func_exec->SetParent(func);
	_current = func_exec;
	return func_exec;
}

void SemanticTree::RemoveFunctionCall(Node* func_call)
{
	Node* func = func_call->GetParent();
	Node* neib = func_call->GetNeighbor();
	if (neib)
	{
		neib->SetParent(func);
	}
	func->_neighbor = neib;
	func_call->_neighbor = nullptr;
	delete func_call;
}

void SemanticTree::RemoveObject(Node* node)
{
	BaseTree::RemoveObject(node);
}

void SemanticTree::Print(std::ostream& out) const
{
	BaseTree::Print(out);
}


void SemanticTree::SetTreePtr(Node* current)
{
		BaseTree::SetTreePtr(current);
}
Node* SemanticTree::GetTreePtr()
{
	return _current;
}

std::string SemanticTree::GetFullName(Node* node)
{
	std::string result = node->_data->id;
	auto parent = node->GetParent();
	while (parent != nullptr)
	{
		if (parent->GetChild() == node && (parent->_data->type == SemanticType::ClassObj || parent->_data->type == SemanticType::Class ))
			result = parent->_data->id + "." + result;
		node = parent;
		parent = node->GetParent();
	}
	return result;
}

std::string SemanticTree::NameToString(std::vector<LexemaView> & ids) const
{
	std::string res = "";
	if(!ids.empty())
		res = ids[0];
	for (size_t i = 1; i < ids.size(); i++)
	{
		res += "." + ids[i];
	}
	return res;
}


Node* SemanticTree::AddVariableObject(Data* data)
{
	CheckUnique(data->id);
	if (_root == nullptr)
	{
		_root = new Node(data);
		_current = _root;
	}
	else
	{
		_current = _current->AddNeighbor(data);
	}
	return _current;
}
	//create node in tree, save ptr
Node* SemanticTree::AddFunctionDeclare(SemanticType returnedType, const LexemaView& funcName, Node * func_body)
{
	CheckUnique(funcName);
	auto func_node = AddVariableObject(new FunctionData(returnedType, funcName));
	auto func_data = dynamic_cast<FunctionData*>(func_node->_data);
	_sc->GetPtrs(func_data->ptr, func_data->line, func_data->col);
	return func_node;
}



Node* SemanticTree::AddClass(const LexemaView& className)
{
	CheckUnique(className);
	SemanticType type = SemanticType::Class;
	type.id = className;
	auto class_node = AddVariableObject(new Data(type, className));
	AddEmptyNodeAsChild();
	return class_node;
}

Node* SemanticTree::AddEmptyNodeAsChild()
{
	const auto block = new Node(new Data(SemanticType::Empty, "emptyNode"));
	_current->SetChild(block);
	_current = block;
	return block;
}

Node* SemanticTree::AddCompoundBlock()
{
	const auto comp_block = new Node(new Data(SemanticType::Empty, "emptyNode"));
	_current->SetNeighbor(comp_block);
	_current = comp_block;
	return comp_block;
}


void SemanticTree::SemanticExit(const std::vector<std::string>& errMsg, ErrorCode code) const
{
	int _col, _len, _ptr;
	_sc->GetPtrs(_ptr, _len, _col);
	throw CompilException(code, errMsg, _len, _col);

}


SemanticType SemanticTree::GetType(LexType type_type, LexType next_type)
{
	switch (type_type)
	{
	case LexType::Short:
		if (next_type != LexType::Int)
			return SemanticType::Undefined;
		return SemanticType::ShortInt;
	case LexType::Long:
		if (next_type != LexType::Int)
			return SemanticType::Undefined;
		return SemanticType::LongInt;
	default: return SemanticType::Undefined;
	}
}

SemanticType SemanticTree::GetType(LexType type_type, const LexemaView& type_view) const
{

	switch (type_type)
	{
	case LexType::Void:
		return SemanticType::Void;
	case LexType::Int:
		return SemanticType::ShortInt;
	case LexType::Float:
		return SemanticType::Float;
	case LexType::Id:
	{
		auto nod = FindUp(type_view);
		if (nod != nullptr)
		{
			if (nod->_data->type == SemanticType::Class) {
				SemanticType type = SemanticType::ClassObj;
				type.id = type_view;
				return type;
			}
		}

		return SemanticType::Undefined;
	}
	default:
		return SemanticType::Undefined;
	}
}

///
void SemanticTree::CheckUnique(const LexemaView& lv) const
{
	auto n = FindUpOnLevel(lv);
	if (n != nullptr)
		SemanticExit({ lv },ErrorCode::NotUniqueId);
}


Node* SemanticTree::GetNodeByView(std::vector<LexemaView>& ids, bool isFunc) const
{
	auto node = FindUp(ids[0]);
	//?
	if (node == nullptr)
	{
		SemanticExit({ NameToString(ids) },ErrorCode::ObjectNotFound);
	}
	if (node->_data->type == SemanticType::ClassObj)
	{
		for (size_t i = 1; i < ids.size() && node != nullptr; i++)
		{
			node = FindChild(node, ids[i]);
		}
		if (node == nullptr)
		{
			SemanticExit({ "Неверное описание класса: " , NameToString(ids) },ErrorCode::Undefined);
		}
	}
	if (isFunc)
	{
		if (node->_data->type != SemanticType::Function)
		{
			SemanticExit({ "Объект с именем \'", NameToString(ids), "\' не является функцией" },ErrorCode::Undefined);
		}
	}
	return node;
}

int numberStrCmp(std::string a, std::string b)
{
	size_t dif = a.size() - b.size();
	if (dif) return dif;
	return strcmp(a.c_str(), b.c_str());
}
Data* SemanticTree::GetConstData(const LexemaView& lv, LexType lt) const
{
	Data* result = new Data();
	switch (lt)
	{
	case LexType::ConstExp: {
		result->type = SemanticType::Float;
		result->value.float_value = atof(lv.c_str());
		break;
	}
	case LexType::ConstInt:
		if (numberStrCmp(lv, MaxShort) <= 0) {
			result->type = SemanticType::ShortInt;
			result->value.short_int_value = atoi(lv.data());
		}
		else
			if (numberStrCmp(lv, MaxLong) <= 0) {
				result->type = SemanticType::LongInt;
				result->value.short_int_value = atoi(lv.data());
			}
			else
				SemanticExit({ std::to_string(MAX_LEX) },ErrorCode::TooLargeConstSize);
		break;
	default:
		SemanticExit({ lv },ErrorCode::UndefinedConstType);

	}
	return result;
}

void SemanticTree::SetData(Node* dst, Data* src)
{/*
	if (!IsComparableType(dst->_data->type, src->type))
	{
		SemanticExit({ " Тип присваемого значения (", src->type.id,  ") не соответсвует типу переменной ", dst->_data->id, "(", dst->_data->type.id, ")" },ErrorCode::Undefined);
	}
	if (dst->_data->type == src->type)
	{
		dst->_data->value = src->value;
	}
	else {
		switch (dst->_data->type)
		{
		case SemanticType::Float: {
			switch (src->type)
			{
			case SemanticType::ShortInt: dst->_data->value.float_value = (float)src->value.short_int_value; break;
			}
			break;
		}
		case SemanticType::ShortInt: {
			switch (src->type)
			{
			case SemanticType::Float: dst->_data->value.short_int_value = (int16_t)src->value.float_value; break;
			}
			break;
		}
		}
	}
	std::cout << "\nПеременной " + GetFullName(dst) + " присвоено значение:\t " << *src;*/
}

/// <summary>
///Поиск типа результата выполнения операции
/// </summary>
/// <param name="a">Тип первого операнда</param>
/// <param name="b">Тип второго операнда</param>
/// <param name="sign">Тип операции</param>
/// <returns>Тип результата выполнения операции</returns>
SemanticType SemanticTree::GetResultType(SemanticType a, SemanticType b, LexType sign)
{
	if (a == SemanticType::Undefined || b == SemanticType::Undefined)
	{
		return SemanticType::Undefined;
	}
	if (!(IsDataType(a) && IsDataType(b)))
	{
		return SemanticType::Undefined;
	}
	if (a == b)
	{
		return a;
	}
	if (a == SemanticType::Float || b == SemanticType::Float)
	{
		return SemanticType::Float;
	}
	if (a == SemanticType::LongInt || b == SemanticType::LongInt)
	{
		return SemanticType::LongInt;
	}
	return SemanticType::ShortInt;
}



Data* SemanticTree::BinaryOperation(Data* a, Data* b, LexType sign)
{
	const auto type = GetResultType(a->type, b->type, sign);
	return new Data(type, "tmp");
}

Data* SemanticTree::UnaryOperation(Data* a, LexType sign)
{
	IsEnableUnaryOperation(a->type);
	Data* d = new Data(*a);
	return d;
}

Data* SemanticTree::LogicalOperation(Data* a, Data* b, LexType sign)
{
	const auto type = GetResultType(a->type, b->type, sign);
	return new Data(type, "");
}

bool SemanticTree::IsEnableUnaryOperation(SemanticType type) const
{
	if (type == SemanticType::LongInt || type == SemanticType::Float ||
		type == SemanticType::ShortInt)
		return true;
	SemanticExit({ "С опреандом типа \"",type.id, "\" не совместимы унарные операции " },ErrorCode::WrongOperation);
	return false;
}
/// <summary>
/// Проверить, можно ли привести тип выражения к типу используемой синтаксической конструкции
/// </summary>
bool SemanticTree::IsComparableType(SemanticType realType, SemanticType neededType) const
{
	if (realType == SemanticType::Void || neededType == SemanticType::Void)
		return false;
	if (realType == SemanticType::ClassObj)
	{
		return (neededType == SemanticType::ClassObj && realType.id == neededType.id);
	}
	return IsDataType(realType) && IsDataType(neededType);
}

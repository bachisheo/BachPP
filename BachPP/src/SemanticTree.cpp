#include "SemanticTree.h"

#include <string>


bool SemanticTree::IsDataType(SemanticType type) const
{
	return (type == SemanticType::LongInt || type == SemanticType::ShortInt || type == SemanticType::Float);
}


bool SemanticTree::isWork() const
{
	return isInterpreting && !isReturned;
}

SemanticTree::SemanticTree(Scanner* sc) :_sc(sc)
{
	_current = nullptr;
}

Node* SemanticTree::AddClassObject(const LexemaView& objName, const LexemaView& className)
{
	if (!isWork())
		return nullptr;
	const auto classDeclaration = FindUp(className);
	if (classDeclaration == nullptr)
	{
		SemanticExit({ "Класса с именем \'", className, "\' не существует" });
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
	if (!isWork())
		return;
	if (!IsComparableType(data->type, SemanticType::ShortInt))
	{
		SemanticExit({ "Недопустимое условие для 'while'" });
	}
}

bool SemanticTree::IsWhileExecute(Data* data) const
{
	if (!isWork())
		return false;
	return data->value.short_int_value;
}

void SemanticTree::SetReturnedData(Data* returnedData) const
{
	if (!isWork())
		return;
	auto func = FindCurrentFunc();
	auto func_data = dynamic_cast<FunctionData*>(func->_data);
	func_data->is_return_operator_declarated = true;
	if (!IsComparableType(returnedData->type, func_data->returned_type))
	{
		SemanticExit({ "Тип возвращаемого значения не соответсвует объявленному" });
	}
	func_data->returned_data = returnedData;
}

//Получить значение переменной
//или результата выоплнения функции
Data* SemanticTree::GetNodeValue(std::vector<LexemaView> ids) const
{
	if (!isWork())
		return nullptr;

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
	if (isWork())
		BaseTree::RemoveObject(node);
}

void SemanticTree::Print(std::ostream& out) const
{
	BaseTree::Print(out);
}


void SemanticTree::SetTreePtr(Node* current)
{
	if (isWork())
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


Node* SemanticTree::AddVariableObject(Data* data)
{
	if (!isWork())
		return nullptr;
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
	if (!isWork())
		return nullptr;
	CheckUnique(funcName);
	auto func_node = AddVariableObject(new FunctionData(returnedType, funcName));
	auto func_data = dynamic_cast<FunctionData*>(func_node->_data);
	_sc->GetPtrs(func_data->ptr, func_data->line, func_data->col);
	func_body = AddEmptyNodeAsChild();
	return func_node;
}



Node* SemanticTree::AddClass(const LexemaView& className)
{
	if (!isWork())
		return nullptr;
	CheckUnique(className);
	SemanticType type = SemanticType::Class;
	type.id = className;
	auto class_node = AddVariableObject(new Data(type, className));
	AddEmptyNodeAsChild();
	return class_node;
}

Node* SemanticTree::AddEmptyNodeAsChild()
{
	if (!isWork())
		return nullptr;
	const auto block = new Node(new Data(SemanticType::Empty, "emptyNode"));
	_current->SetChild(block);
	_current = block;
	return block;
}

Node* SemanticTree::AddCompoundBlock()
{
	if (!isWork())
		return nullptr;
	const auto comp_block = new Node(new Data(SemanticType::Empty, "emptyNode"));
	_current->SetNeighbor(comp_block);
	_current = comp_block;
	
	return comp_block;
}


void SemanticTree::SemanticExit(const std::vector<std::string>& errMsg) const
{
	Print(std::cout);
	_sc->ErrorMsg(MSG_ID::SEM_ERR, errMsg);
	exit(1);
}


Node* SemanticTree::FindCurrentFunc() const
{
	if (!isWork())
		return nullptr;
	auto func_node = _current;

	while (func_node->_data->type != SemanticType::Function) {
		func_node = func_node->GetParent();
	}
	return func_node;
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
		if (nod != nullptr && isInterpreting)
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
	if (!isWork())
		return;
	auto n = FindUpOnLevel(lv);
	if (n != nullptr)
	{
		std::string s = "Идентификатор \'";
		s += lv;
		s += "\' не уникален";
		SemanticExit({ s });
	}
}
std::string StringNameByView(std::vector<LexemaView> ids)
{
	std::string name = ids[0];
	for (size_t i = 1; i < ids.size(); i++)
	{
		name += ".";
		name += ids[i];
	}
	return name;
}

Node* SemanticTree::GetNodeByView(std::vector<LexemaView>& ids, bool isFunc) const
{
	if (!isWork())
		return nullptr;
	auto node = FindUp(ids[0]);
	//?
	if (node == nullptr)
	{
		SemanticExit({ "Объекта с именем \'", ids[0], "\' не существует" });
	}
	if (node->_data->type == SemanticType::ClassObj)
	{
		for (size_t i = 1; i < ids.size() && node != nullptr; i++)
		{
			node = FindChild(node, ids[i]);
		}
		if (node == nullptr)
		{
			SemanticExit({ "Неверное описание класса: " , StringNameByView(ids) });
		}
	}
	if (isFunc)
	{
		if (node->_data->type != SemanticType::Function)
		{
			SemanticExit({ "Объект с именем \'", StringNameByView(ids), "\' не является функцией" });
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
	if (!isWork())
		return nullptr;
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
				SemanticExit({ "Константа слишком велика" });
		break;
	default:
		SemanticExit({ "Тип константы \"", lv, "\" не определен" });

	}
	return result;
}

void SemanticTree::SetData(Node* dst, Data* src)
{
	if (!isWork())
		return;
	if (!IsComparableType(dst->_data->type, src->type))
	{
		SemanticExit({ " Тип присваемого значения (", src->type.id,  ") не соответсвует типу переменной ", dst->_data->id, "(", dst->_data->type.id, ")" });
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
	std::cout << "\nПеременной " + GetFullName(dst) + " присвоено значение:\t " << *src;
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
	if (!isWork())
		return SemanticType::Empty;
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

//float - тип результата операции, один из операндов может быть младше его
Data* SemanticTree::CalculateFloatValue(Data* _a, Data* _b, LexType sign) const
{
	Data* a = _a, * b = _b;
	Data* c = new Data();
	c->type = SemanticType::Float;
	float res = a->type == SemanticType::Float ? a->value.float_value : a->value.short_int_value;
	switch (sign)
	{
	case LexType::Plus:
		res += b->type == SemanticType::Float ? b->value.float_value : b->value.short_int_value;
		break;
	case LexType::Minus:
		res -= b->type == SemanticType::Float ? b->value.float_value : b->value.short_int_value;
		break;
	case LexType::DivSign:
		res /= b->type == SemanticType::Float ? b->value.float_value : b->value.short_int_value;
		break;
	case LexType::MultSign:
		res *= b->type == SemanticType::Float ? b->value.float_value : b->value.short_int_value;
		break;
	default:
		SemanticExit({ "С опреандом типа", TypesName.find(SemanticType::Float)->second,
			" несовместима бинарная операция ", LexTypesName.find(sign)->second });
	}
	c->value.float_value = res;
	return c;
}
Data* SemanticTree::CalculateShortIntValue(Data* a, Data* b, LexType sign) const
{
	if (a->type == SemanticType::Float || b->type == SemanticType::Float)
		SemanticExit({ "Ошибка приведения типов (один из операндов имеет тип ", TypesName.find(SemanticType::Float)->second,
			" а результат определен как  ", TypesName.find(SemanticType::ShortInt)->second });

	Data* c = new Data();
	c->type = SemanticType::ShortInt;
	int res = a->value.short_int_value;
	bool is_wrong_b = false;
	switch (sign)
	{
	case LexType::Plus:
		res += b->value.short_int_value;
		break;
	case LexType::Minus:
		res -= b->value.short_int_value;
		break;
	case LexType::DivSign:
		if (b->value.short_int_value == 0)
			SemanticExit({ "Ошибка: Деление на 0" });
		res /= b->value.short_int_value;
		break;
	case LexType::MultSign:
		res *= b->value.short_int_value;
		break;
	case LexType::ModSign:
		if (b->value.short_int_value == 0)
			SemanticExit({ "Ошибка: Деление с остатком на 0" });
		res %= b->value.short_int_value;
		break;
	case LexType::ShiftLeft:
		res <<= b->value.short_int_value;
		break;
	case LexType::ShiftRight:
		res >>= b->value.short_int_value;
		break;
	default:
		SemanticExit({ "С опреандом типа", TypesName.find(SemanticType::ShortInt)->second,
			" несовместимая бинарная операция ", LexTypesName.find(sign)->second });
		break;
	}
	c->value.short_int_value = res;
	return c;
}

Data* SemanticTree::CalculateFloatLogic(Data* a, Data* b, LexType sign) const
{
	bool res;
	float f_val = a->type == SemanticType::Float ? a->value.float_value : a->value.short_int_value;
	switch (sign)
	{
	case LexType::LogEqual:
		res = f_val == (b->type == SemanticType::Float ? b->value.float_value : b->value.short_int_value);
		break;
	case LexType::LogNotEqual:
		res = f_val != (b->type == SemanticType::Float ? b->value.float_value : b->value.short_int_value);
		break;
	case LexType::Less:
		res = f_val < (b->type == SemanticType::Float ? b->value.float_value : b->value.short_int_value);
		break;
	case LexType::More:
		res = f_val > (b->type == SemanticType::Float ? b->value.float_value : b->value.short_int_value);
		break;
	case LexType::LessOrEqual:
		res = f_val <= (b->type == SemanticType::Float ? b->value.float_value : b->value.short_int_value);
		break;
	case LexType::MoreOrEqual:
		res = f_val >= (b->type == SemanticType::Float ? b->value.float_value : b->value.short_int_value);
		break;
	default:
		SemanticExit({ "С опреандом типа", TypesName.find(SemanticType::Float)->second,
			" несовместима логическая операция ", LexTypesName.find(sign)->second });
	}
	Data* c = new Data();
	c->type = SemanticType::ShortInt;
	c->value.short_int_value = res;
	return c;
}

Data* SemanticTree::CalculateShortIntLogic(Data* a, Data* b, LexType sign) const
{

	if (a->type == SemanticType::Float || b->type == SemanticType::Float)
		SemanticExit({ "Ошибка приведения типов (один из операндов имеет тип ", TypesName.find(SemanticType::Float)->second,
			" а результат определен как  ", TypesName.find(SemanticType::ShortInt)->second, ")" });

	bool res;
	short int short_val = a->value.short_int_value;
	switch (sign)
	{
	case LexType::LogEqual:
		res = short_val == b->value.short_int_value;
		break;
	case LexType::LogNotEqual:
		res = short_val != b->value.short_int_value;
		break;
	case LexType::Less:
		res = short_val < b->value.short_int_value;
		break;
	case LexType::More:
		res = short_val > b->value.short_int_value;
		break;
	case LexType::LessOrEqual:
		res = short_val <= b->value.short_int_value;
		break;
	case LexType::MoreOrEqual:
		res = short_val >= b->value.short_int_value;
		break;
	default:
		SemanticExit({ "С опреандом типа", TypesName.find(SemanticType::ShortInt)->second,
			" несовместима логическая операция ", LexTypesName.find(sign)->second });
	}
	Data* c = new Data();
	c->type = SemanticType::ShortInt;
	c->value.short_int_value = res;
	return c;
}

Data* SemanticTree::BinaryOperation(Data* a, Data* b, LexType sign)
{
	if (!isWork())
		return nullptr;
	const auto type = GetResultType(a->type, b->type, sign);
	switch (type)
	{
	case SemanticType::Float:
		return CalculateFloatValue(a, b, sign);
	case SemanticType::ShortInt:
		return CalculateShortIntValue(a, b, sign);
	default:
		return nullptr;
	}
}

Data* SemanticTree::UnaryOperation(Data* a, LexType sign)
{
	if (!isWork())
		return nullptr;
	IsEnableUnaryOperation(a->type);
	Data* d = new Data(*a);
	switch (d->type)
	{
	case SemanticType::Float:
		if (sign == LexType::Plus)
		{
			d->value.float_value = abs(d->value.float_value);
		}
		else d->value.float_value *= -1.f;
		break;
	case SemanticType::ShortInt:
		if (sign == LexType::Plus)
		{
			d->value.short_int_value = abs(d->value.short_int_value);
		}
		else d->value.short_int_value *= -1;
		break;
	}
	return d;
}

Data* SemanticTree::LogicalOperation(Data* a, Data* b, LexType sign)
{
	if (!isWork())
		return nullptr;
	const auto type = GetResultType(a->type, b->type, sign);
	switch (type)
	{
	case SemanticType::Float:
		return CalculateFloatLogic(a, b, sign);
	case SemanticType::ShortInt:
		return CalculateShortIntLogic(a, b, sign);
	default:
		return nullptr;
	}
}

bool SemanticTree::IsEnableUnaryOperation(SemanticType type) const
{
	if (type == SemanticType::LongInt || type == SemanticType::Float ||
		type == SemanticType::ShortInt)
		return true;
	SemanticExit({ "С опреандом типа \"",TypesName.find(type)->second, "\" не совместимы унарные операции " });
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

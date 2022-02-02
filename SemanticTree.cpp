#include "SemanticTree.h"

#include <string>


bool SemanticTree::IsDataType(SemanticType type) const
{
	return (type == SemanticType::LongInt || type == SemanticType::ShortInt || type == SemanticType::Float);
}


void SemanticTree::Print(std::ostream& out) const
{
	out << "\n--------------------------Семантическое дерево";
	_root->Print(out, 0);
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
		SemanticExit({ "Класса с именем \'", className, "\' не существует" });
	}
	SemanticType type = SemanticType::ClassObj;
	type.id = className;
	//add object root
	auto obj = AddVariableObject(new Data(type, objName));
	obj->SetChild(CopySubtree(classDeclaration->GetChild()));
	return obj;
}

Node* SemanticTree::CopySubtree(Node* sub_root)
{
	if (sub_root == nullptr)
	{
		return nullptr;
	}
	Node* new_root = new Node(sub_root->_data->Clone());
	new_root->SetChild(CopySubtree(sub_root->GetChild()));
	new_root->SetNeighbor(CopySubtree(sub_root->GetNeighbor()));
	return new_root;
}

std::string SemanticTree::GetFullName(Node* node)
{
	std::string result = node->_data->id;
	auto parent = node->GetParent();
	while (parent != nullptr)
	{
		if(parent->GetChild() == node && (parent->_data->type == SemanticType::ClassObj || parent->_data->type == SemanticType::Class))
			result = parent->_data->id + "." + result;
		node = parent;
		parent = node->GetParent();
	}
	return result;
}

void SemanticTree::RemoveObject(Node* node)
{
	if (_current == node)
	{
		_current = node->GetParent();
	}
	//std::cout << "\n\nУДАЛЕН БЛОК: \n" << *node << std::endl;;
	delete node;
	//Print(std::cout);
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
	//std::cout << "\n\nДОБАВЛЕН УЗЕЛ: \n" << *data << std::endl;
	//Print(std::cout);
	return _current;
}

Node* SemanticTree::AddFunctionDeclare(SemanticType returnedType, const LexemaView& funcName)
{
	CheckUnique(funcName);
	auto func_node = AddVariableObject(new FunctionData(returnedType, funcName));
	return func_node;
}

void SemanticTree::SetTreePtr(Node* current)
{
	_current = current;
}

Node* SemanticTree::AddClass(const LexemaView& className)
{
	CheckUnique(className);
	SemanticType type = SemanticType::Class;
	type.id = className;
	auto class_node = AddVariableObject(new Data(type, className));
	return class_node;
}

Node* SemanticTree::AddCompoundBlock()
{
	const auto block = new Node(new Data(SemanticType::Empty, "emptyNode"));
	_current->SetChild(block);
	_current = block;
	return block;
}



void SemanticTree::SemanticExit(const std::vector<std::string>& errMsg) const
{
	_sc->ErrorMsg(MSG_ID::SEM_ERR, errMsg);
	exit(1);
}

void SemanticTree::SetParent(Node* parent) const
{
	_current->SetParent(parent);
}

Node* SemanticTree::GetParent() const
{
	return _current->GetParent();
}

Node* SemanticTree::AddNeighbor(Data* data)
{
	_current = _current->AddNeighbor(data);
	return _current;
}

Node* SemanticTree::GetNeighbor() const
{
	return _current->GetNeighbor();
}

Node* SemanticTree::FindUpOnLevel(const LexemaView& id) const
{
	if (_root == nullptr)
	{
		return nullptr;;
	}
	return FindUpOnLevel(_current, id);
}

Node* SemanticTree::FindUpOnLevel(Node* from, const LexemaView& id)
{
	auto par = from;
	while (true) {
		if (par->_data->id == id)
		{
			break;
		}
		auto parpar = par->GetParent();
		if (parpar == nullptr || parpar->GetNeighbor() != par)
			return nullptr;
		par = parpar;

	}
	return par;
}
Node* SemanticTree::FindUp(const LexemaView& id) const
{
	if (_root == nullptr)
	{
		return nullptr;
	}
	return FindUp(_current, id);
}

Node* SemanticTree::FindUp(Node* from, const LexemaView& id)
{
	auto par = from;
	while (par != nullptr && par->_data->id != id) {
		par = par->GetParent();
	}
	return par;
}

Node* SemanticTree::FindCurrentFunc() const
{
	auto func_node = _current;

	while (func_node->_data->type != SemanticType::Function) {
		func_node = func_node->GetParent();
	}
	return func_node;
}

bool SemanticTree::IsInOperator() const
{
	auto prev_node = _current;

	//if(prev_node->_data->data_type = )
	while (prev_node->_data->type != SemanticType::Function) {
		prev_node = prev_node->GetParent();
	}
	return prev_node;
}

Node* SemanticTree::FindChild(const LexemaView& id) const
{
	return FindChild(_current, id);
}

Node* SemanticTree::FindChild(const Node* from, const LexemaView& id)
{
	auto child = from->GetChild();
	while (!(child == nullptr || child->_data->id == id))
		child = child->GetNeighbor();
	return child;
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
	int dif = a.size() - b.size();
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
				SemanticExit({ "Константа слишком велика" });
	}
	return result;
}

void SemanticTree::SetData(Node* dst, Data* src)
{
	std::string value = "";
	if (!IsComparableType(dst->_data->type, src->type))
	{
		SemanticExit({ " Тип присваемого значения (", src->type.id,  ") не соответсвует типу переменной ", dst->_data->id, "(", dst->_data->type.id, ")" });
	}
	if (dst->_data->type == src->type)
	{
		dst->_data->value = src->value;
	}
	else{
		switch (dst->_data->type)
		{
		case SemanticType::Float: {
			switch (src->type)
			{
			case SemanticType::ShortInt: dst->_data->value.float_value = (float)src->value.short_int_value; break;
			}
			value = std::to_string(dst->_data->value.float_value);
			break;
		}
		case SemanticType::ShortInt: {
			switch (src->type)
			{
			case SemanticType::Float: dst->_data->value.short_int_value = (int16_t)src->value.float_value; break;
			}
			value = std::to_string(dst->_data->value.short_int_value);
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

Data* SemanticTree::GetResult(Data* a, Data* b, LexType sign)
{
	auto t = GetResultType(a->type, b->type, sign);
	return new Data(t, "");
}

bool SemanticTree::IsEnableUnarySign(SemanticType type) const
{
	if (type == SemanticType::LongInt || type == SemanticType::Float ||
		type == SemanticType::ShortInt)
		return true;
	SemanticExit({ "С опреандом данного типа не совместимы унарные операции " });
	return false;
}
/// <summary>
/// Проверить, можно ли привести тип выражения к типу используемой синтаксической конструкции
/// </summary>
bool SemanticTree::IsComparableType(SemanticType realType, SemanticType neededType)
{
	if (realType == SemanticType::Void || neededType == SemanticType::Void)
		return false;
	if (realType == SemanticType::ClassObj)
	{
		return (neededType == SemanticType::ClassObj && realType.id == neededType.id);
	}
	return IsDataType(realType) && IsDataType(neededType);
}

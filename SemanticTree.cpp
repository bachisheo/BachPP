#include "SemanticTree.h"


bool SemanticTree::IsDataType(SemanticType type) const
{
	return (type == SemanticType::LongInt || type == SemanticType::ShortInt || type == SemanticType::Float);

}

void SemanticTree::Print(std::ostream& out) const
{
	_root->Print(out, 0);
}

SemanticTree::SemanticTree(Scanner* sc) :_sc(sc)
{
	_current = nullptr;
}

Node* SemanticTree::AddObject(const LexemaView& lv, SemanticType type)
{
	return AddObject(new Data(type, lv));
}
Node* SemanticTree::AddClassObject(const LexemaView& objName, const LexemaView& className)
{
	auto classDeclaration = FindUp(className);
	if (classDeclaration == nullptr)
	{
		SemanticExit({ "Класса с именем \'", className, "\' не существует" });
	}
	IsUnique(objName);
	SemanticType type = SemanticType::ClassObj;
	type.id = className;
	auto obj = AddObject(new Data(type,  objName));
	obj->SetChild(classDeclaration->GetChild());
	return obj;
}
Node* SemanticTree::AddObject(Data * data)
{

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

Node* SemanticTree::AddFunc(SemanticType returnedType, const LexemaView& funcName)
{
	IsUnique(funcName);
	return AddBlock(new FunctionData(returnedType, funcName));
}

void SemanticTree::SetTreePtr(Node* current)
{
	_current = current;
}

Node* SemanticTree::AddClass(const LexemaView& className)
{
	IsUnique(className);
	SemanticType type = SemanticType::Class;
	type.id = className;
	return AddBlock(new Data(type, className ));
}

Node* SemanticTree::AddCompoundBlock()
{
	return AddBlock(new Data(SemanticType::Empty, "emptyNode"));
}

Node* SemanticTree::AddBlock(Data * parentData)
{
	Node* parent = AddObject(parentData);
	_current = parent->AddChild(new Data( SemanticType::Empty, "emptyNode"));
	return parent;
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

Node* SemanticTree::AddNeighbor(Data * data)
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

Node* SemanticTree::FindCurrentFunc()
{
	auto func_node = _current;

	while (func_node->_data->data_type != SemanticType::Function) {
		func_node = func_node->GetParent();
	}
	return func_node;
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
			if (nod->_data->data_type == SemanticType::Class) {
				SemanticType type = SemanticType::ClassObj;
				type.id = type_view;
				return type;
			}
		}
		SemanticExit({ "Тип \'" , type_view, "\' не определен" });
		return SemanticType::Undefined;
	}
	default:
		return SemanticType::Undefined;
	}
}

///
bool SemanticTree::IsUnique(const LexemaView& lv) const
{
	auto n = FindUpOnLevel(lv);
	if (n != nullptr)
	{
		std::string s = "Идентификатор \'";
		s += lv;
		s += "\' не уникален";
		SemanticExit({ s });
		return false;
	}
	return true;
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

SemanticType SemanticTree::GetTypeByView(std::vector<LexemaView>& ids, bool isFunc) const
{
	auto node = FindUp(ids[0]);
	//?
	if (node == nullptr)
	{
		SemanticExit({ "Объекта с именем \'", ids[0], "\' не существует" });
		return SemanticType::Undefined;
	}
	if (node->_data->data_type == SemanticType::ClassObj)
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
		if (node->_data->data_type != SemanticType::Function)
		{
			SemanticExit({ "Объект с именем \'", StringNameByView(ids), "\' не является функцией" });
			return SemanticType::Undefined;
		}
		return static_cast<FunctionData*>(node->_data)->returned_type;
	}
	return node->_data->data_type;
}
int numberStrCmp(std::string a, std::string b)
{
	int dif = a.size() - b.size();
	if (dif) return dif;
	return strcmp(a.c_str(), b.c_str());
}
SemanticType SemanticTree::GetConstType(const LexemaView& lv, LexType lt) const
{
	switch (lt)
	{
	case LexType::ConstExp: return SemanticType::Float;
	case LexType::ConstInt:
	{


		if (numberStrCmp(lv, MaxShort) <= 0)
			return SemanticType::ShortInt;
		if (numberStrCmp(lv, MaxLong) <= 0)
			return SemanticType::LongInt;
		SemanticExit({"Константа слишком велика"});

		return SemanticType::Undefined;
	}
	default:return SemanticType::Undefined;
	}
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

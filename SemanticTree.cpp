#include "SemanticTree.h"


bool SemanticTree::IsDataType(SemanticType type) const
{
	if (!(type == SemanticType::LongInt || type == SemanticType::ShortInt || type == SemanticType::Float))
	{
		SemanticExit({ "Тип переменной не является численным" });
		return false;
	}
	return true;
}

SemanticTree::SemanticTree(Scanner* sc) :_sc(sc)
{
	_current  = nullptr;
}

Node* SemanticTree::AddObject(const LexemaView& lv, SemanticType type)
{
	return AddObject({ lv,type });
}
Node* SemanticTree::AddClassObject(const LexemaView & objName, const LexemaView & className)
{
	auto classNode = FindUp(className);
	if(classNode == nullptr)
	{
		SemanticExit({ "Класса с именем \'", className, "\' не существует" });
	}
	auto obj = AddObject({ objName, SemanticType::ClassObj });
	obj->

}
Node* SemanticTree::AddObject(Data data)
{
	auto n = FindUp(data.id);
	if (n != nullptr)
	{
		std::string s = "Идентификатор ";
		s += data.id;
		s += " не уникален";
		SemanticExit({ s });
	}
	if (_root == nullptr)
	{
		_root = std::make_unique<Node>(data);
		_current = _root.get();
	}
	else
	{
		_current = _current->AddNeighbor(data);
	}
	return _current;
}

Node* SemanticTree::AddFunc(SemanticType returnedType, const LexemaView& funcName)
{
	if (IsUnique(funcName)) {
		Data funcData = { funcName, SemanticType::Function };
		funcData.returnedType = returnedType;
		return AddBlock(funcData);
	}
	return nullptr;
}

void SemanticTree::SetTreePtr(Node* current)
{
	_current = current;
}

Node* SemanticTree::AddClass(const LexemaView& className)
{
	if (IsUnique(className)) {
		return AddBlock({ className, SemanticType::Class });
	}
	return nullptr;
}

Node* SemanticTree::AddCompoundBlock()
{
	return AddBlock({ "emptyNode", SemanticType::Empty });
}

Node* SemanticTree::AddBlock(Data parentData)
{
	Node* parent = AddObject(parentData);
	_current = parent->AddChild({ "emptyNode", SemanticType::Empty });
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

Node* SemanticTree::AddNeighbor(Data data) 
{
	_current = _current->AddNeighbor(data);
	return _current;
}

Node* SemanticTree::GetNeighbor() const
{
	return _current->GetNeighbor();
}

Node* SemanticTree::FindUp(const LexemaView& id) const
{
	return FindUp(_current, id);
}

Node* SemanticTree::FindUp(Node* from, const LexemaView& id)
{
	auto par = from;
	while (par != nullptr) {
		if(par->_data->id == id)
		{
			break;
		}
		par = par->GetParent();
	}
	return par;
}

Node* SemanticTree::FindChild(const LexemaView& id) const
{
	return FindChild(_current, id);
}

Node* SemanticTree::FindChild(const Node* from, const LexemaView& id)
{
	auto child = from->GetChild();
	while (!(child == nullptr || child->_data->id != id))
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
				return SemanticType::ClassObj;
			}
		}
		return SemanticType::Undefined;
	}
	default:
		return SemanticType::Undefined;
	}
}

///
bool SemanticTree::IsUnique(const LexemaView& lv) const
{
	return FindUp(lv) == nullptr;
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
SemanticType SemanticTree::GetTypeByView(std::vector<LexemaView> & ids, bool isFunc) const
{
	const auto node = FindUp(ids[0]);
	//?
	if (node == nullptr)
	{
		SemanticExit({ "Объекта с именем \'", ids[0], "\' не существует" }); 
			return SemanticType::Undefined;
	}
	if(node->_data->type == SemanticType::ClassObj)
	{
		
	}
	if (isFunc)
	{
		if (node->_data->type != SemanticType::Function)
		{
			SemanticExit( { "Объект с именем \'", StringNameByView(ids), "\' не является функцией" });
			return SemanticType::Undefined;
		}
		return node->_data->returnedType;
	}
	if (node->_data->type == SemanticType::ClassObj)
	{
		//todo проверить сответсвие обращения сигнатуре класса
		//todo вернуть тип поля класса
	}
	return node->_data->type;
}

SemanticType SemanticTree::GetConstType(const LexemaView& lv, LexType lt) const
{
	switch (lt)
	{
	case LexType::ConstExp: return SemanticType::Float;
	case LexType::ConstInt:
	{
		//todo check on long or short
		return SemanticType::ShortInt;
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
	SemanticExit( { "С опреандом данного типа не совместимы унарные операции " });
	return false;
}
/// <summary>
/// Проверить, можно ли привести тип выражения к типу используемой синтаксической конструкции
/// </summary>
bool SemanticTree::IsComparableType(SemanticType realType, SemanticType neededType)
{
	if (!(IsDataType(realType) && IsDataType(neededType)))
	{
		return false;
	}
	return true;
}

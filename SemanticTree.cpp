#include "SemanticTree.h"

bool SemanticTree::IsDataType(SemanticType type)
{
	if (!(type == SemanticType::LongInt || type == SemanticType::ShortInt || type == SemanticType::Float))
	{
		_sc->ErrorMsg(MSG_ID::SEM_ERR, { "Тип переменной не является численным" });
		return false;
	}
	return true;
}

SemanticTree::SemanticTree(Scanner* sc) :_sc(sc)
{
	_current = _root = nullptr;
}

Node* SemanticTree::AddObject(LexemaView lv, SemanticType type)
{
	if (FindUp(lv) != nullptr)
	{
		std::string s = "Идентификатор ";
		s += lv;
		s += " не уникален";
		SemanticExit({ s });
	}
	auto tmp = new Node(new Data(type, lv));
	if (_root == nullptr)
	{
		_root = tmp;
		_current = _root;
	}
	else
	{
		_current->AddNeighbor(tmp);
		_current = tmp;
	}
	return tmp;
}

Node* SemanticTree::AddFunc(SemanticType returnedType, LexemaView funcName)
{
	if (IsUnique(funcName)) {
		Node* func = new Node(new Data{ SemanticType::Function, funcName });
		func->data->returnedType = returnedType;
		AddBlock(func);
		return func;
	}
	return nullptr;
}

void SemanticTree::SetTreePtr(Node* current)
{
	_current = current;
}

Node* SemanticTree::AddClass(LexemaView className)
{
	if (IsUnique(className)) {
		Node* _class = new Node(new Data{ SemanticType::Class, className });
		AddBlock(_class);
		return _class;
	}
	return nullptr;
}

Node* SemanticTree::AddBlock()
{
	return AddBlock(new Node());
}

Node* SemanticTree::AddBlock(Node* parentNode)
{
	_current->AddNeighbor(parentNode);
	_current = parentNode->AddChild(new Node);
	return parentNode;
}

void SemanticTree::SemanticExit(std::vector<std::string> errMsg) const
{
	_sc->ErrorMsg(MSG_ID::SEM_ERR, errMsg);
	exit(1);
}

void SemanticTree::SetParent(Node* parent)
{
	_current->SetParent(parent);
}

Node* SemanticTree::GetParent() const
{
	return _current->GetParent();
}

Node* SemanticTree::AddChild(Node* child)
{
	return _current->AddChild(child);
}

Node* SemanticTree::AddNeighbor(Node* neighbor)
{
	return _current->AddNeighbor(neighbor);
}

Node* SemanticTree::GetNeighbor()
{
	return _current->GetNeighbor();
}

Node* SemanticTree::FindUp(LexemaView id)
{
	return FindUp(_current, id);
}

Node* SemanticTree::FindUp(Node* from, LexemaView id)
{
	auto par = from;
	while (!(par == nullptr || !strcmp(par->data->id, id)))
		par = par->GetParent();
	return par;
}

Node* SemanticTree::FindChild(LexemaView id) const
{
	return FindChild(_current, id);
}

Node* SemanticTree::FindChild(Node* from, LexemaView id)
{
	auto child = from->GetChild();
	while (!(child == nullptr || !strcmp(child->data->id, id)))
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

SemanticType SemanticTree::GetType(LexType type_type, LexemaView type_view)
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
			if (nod->data->type == SemanticType::Class) {
				return SemanticType::ClassObj;
			}
		}
	}
	default:
		return SemanticType::Undefined;
	}
}

///
bool SemanticTree::IsUnique(LexemaView lv)
{
	return FindUp(lv) == nullptr;
}
std::string StringNameByView(std::vector<char*> ids)
{
	std::string name = ids[0];
	for (int i = 1; i < ids.size(); i++)
	{
		name += ".";
		name += ids[i];
	}
	return name;
}
SemanticType SemanticTree::GetTypeByView(std::vector<char*> ids, bool isFunc)
{
	auto node = FindUp(ids[0]);
	//?
	if (node == nullptr)
	{
		_sc->ErrorMsg(MSG_ID::SEM_ERR, { "Объекта с именем ", ids[0], " не существует" }); \
		return SemanticType::Undefined;
	}
	if (isFunc)
	{
		if(node->data->type != SemanticType::Function)
		{
			_sc->ErrorMsg(MSG_ID::SEM_ERR, { "Объект с именем ", ids[0], " не является функцией" });
			return SemanticType::Undefined;
		}
		return node->data->returnedType;
	}
	if (node->data->type == SemanticType::ClassObj)
	{
		//todo проверить сответсвие обращения сигнатуре класса
		//todo вернуть тип поля класса
	}
	return node->data->type;
}

SemanticType SemanticTree::GetConstType(LexemaView lv, LexType lt)
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

bool SemanticTree::IsEnableUnarySign(SemanticType type)
{
	if (type == SemanticType::LongInt || type == SemanticType::Float ||
		type == SemanticType::ShortInt)
		return true;
	_sc->ErrorMsg(MSG_ID::SEM_ERR, { "С опреандом данного типа не совместимы унарные операции " });
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

#include "SemanticTree.h"

#include <string>
#include "../Utils.h"

SemanticTree::SemanticTree(Scanner* sc) :_sc(sc)
{
	_current = nullptr;
}

Node* SemanticTree::AddClassObject(const LexemaView& objName, const LexemaView& className)
{
	const auto classDeclaration = FindUp(className);
	if (classDeclaration == nullptr)
	{
		SemanticExit({ className }, ErrorCode::ObjectNotFound);
	}
	SemanticType type = SemanticType::ClassObj;
	type.id = className;
	//add object root
	auto obj = AddVariableObject(new Data(type, objName));
	obj->SetChild(CopySubtree(classDeclaration->GetChild()));
	return obj;
}


//Получить значение переменной
//или результата выоплнения функции
Data* SemanticTree::GetNodeValue(std::vector<LexemaView> ids) const
{
	return GetNodeByView(ids)->_data->Clone();
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
Node* SemanticTree::GetTreePtr() const
{
	return _current;
}

Node* SemanticTree::FindUp(const LexemaView& type_view) const
{
	return BaseTree::FindUp(type_view);
}

std::string SemanticTree::GetFullName(Node* node)
{
	std::string result = node->_data->id;
	auto parent = node->GetParent();
	while (parent != nullptr)
	{
		if (parent->GetChild() == node && (parent->_data->type == SemanticType::ClassObj || parent->_data->type == SemanticType::Class))
			result = parent->_data->id + "." + result;
		node = parent;
		parent = node->GetParent();
	}
	return result;
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
Node* SemanticTree::AddFunctionDeclare(SemanticType returnedType, const LexemaView& funcName, Node* func_body)
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



///
void SemanticTree::CheckUnique(const LexemaView& lv) const
{
	auto n = FindUpOnLevel(lv);
	if (n != nullptr)
		SemanticExit({ lv }, ErrorCode::NotUniqueId);
}


Node* SemanticTree::GetNodeByView(std::vector<LexemaView>& ids, bool isFunc) const
{
	auto node = FindUp(ids[0]);
	//?
	if (node == nullptr)
	{
		SemanticExit({ Utils::NameToString(ids) }, ErrorCode::ObjectNotFound);
	}
	if (node->_data->type == SemanticType::ClassObj)
	{
		for (size_t i = 1; i < ids.size() && node != nullptr; i++)
		{
			node = FindChild(node, ids[i]);
		}
		if (node == nullptr)
		{
			SemanticExit({ "Неверное описание класса: " , Utils::NameToString(ids) }, ErrorCode::Undefined);
		}
	}
	if (isFunc)
	{
		if (node->_data->type != SemanticType::Function)
		{
			SemanticExit({ "Объект с именем \'", Utils::NameToString(ids), "\' не является функцией" }, ErrorCode::Undefined);
		}
	}
	return node;
}



void SemanticTree::SetData(Node* dst, Data* src)
{
}



bool SemanticTree::IsEnableUnaryOperation(SemanticType type) const
{
	if (type == SemanticType::LongInt || type == SemanticType::Float ||
		type == SemanticType::ShortInt)
		return true;
	SemanticExit({ "С опреандом типа \"",type.id, "\" не совместимы унарные операции " }, ErrorCode::WrongOperation);
	return false;
}


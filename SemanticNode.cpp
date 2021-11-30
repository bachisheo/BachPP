#include "SemanticNode.h"


Node::Node(Data data)
{
	_data = new Data(data);
}
Node::Node()
{
	_data = new Data(SemanticType::Empty, "emptyNode");
}

void Node::SetParent(Node* parent)
{
	_parent = parent;
}

Node* Node::GetParent() const
{
	return _parent;
}

Node* Node::AddChild(Data data)
{
	if (_child == nullptr) {
		_child = std::make_unique<Node>(data);
		_child->_parent = this;
		return _child.get();
	}
	else
		return _child->AddNeighbor(data);
}

Node* Node::GetChild() const
{
	return _child.get();
}

Node* Node::AddNeighbor(Data data)
{
	Node* current = this;
	while (current->_neighbor != nullptr)
		current = current->_neighbor.get();
	current->_neighbor = std::make_unique<Node>(data);
	current->_neighbor->_parent = current->_parent;
	return  current->_neighbor.get();
}

Node* Node::GetNeighbor() const
{
	return _neighbor.get();
}

void Node::Print() const
{
	std::cout << "РЈР·РµР» id: " << _data->id << ", СЃРѕСЃРµРґ: " << _neighbor->_data->id;

	std::cout << "\nРїРѕС‚РѕРјРєРё:";
	auto tmp = _child.get();
	while (tmp != nullptr) {
		std::cout << tmp->_data->id << " | ";
		tmp = tmp->_neighbor.get();
	}
}

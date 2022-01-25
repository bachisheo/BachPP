#include "SemanticNode.h"


Data::Data(SemanticType semType, const LexemaView& idView) :data_type(semType), id(idView)
{}

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

Node* Node::SetChild(Node* child)
{
	_child = child;
	return _child;
}
Node* Node::AddChild(Data data)
{
	if (_child == nullptr) {
		_child = new Node(data);
		_child->_parent = this;
		return _child;
	}
	else
		return _child->AddNeighbor(data);
}

Node* Node::GetChild() const
{
	return _child;
}

Node* Node::AddNeighbor(Data data)
{
	Node* current = this;
	while (current->_neighbor != nullptr)
		current = current->_neighbor.get();
	current->_neighbor = std::make_unique<Node>(data);
	current->_neighbor->_parent = current;
	return  current->_neighbor.get();
}

Node* Node::GetNeighbor() const
{
	return _neighbor.get();
}

void Node::Print(std::ostream& out, int tab_count) const
{
	out << "\n";
	for (int i = 0; i < tab_count; i++)
		out << '\t';
	out << "Node: " << *this;
	if (_child)
	{
		for (int i = 0; i < tab_count; i++)
			out << '\t';
		out << "\n\tchildren:";
		_child->Print(out, tab_count + 1);
	}
	if(_neighbor)
	{
		_neighbor->Print(out, tab_count);
	}
}

std::ostream& operator<<(std::ostream& out, const Node& node)
{
	out << "Type: " << node._data->data_type.id << ", name: " << node._data->id;
	return out;
}

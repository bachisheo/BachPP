#include "SemanticNode.h"

#include <string>


Data::Data(SemanticType semType, const LexemaView idView) :type(semType), id(idView)
{
	switch (type)
	{
	case SemanticType::Float: value.float_value = 0.0f; break;
	case SemanticType::ShortInt: value.short_int_value = 0; break;
	}
}

Data::Data():Data(SemanticType::Undefined, "")
{

}

LexemaView Data::GetValueView() const
{
	LexemaView view = "Undefined";
	switch (type)
	{
	case SemanticType::Float: view = std::to_string(value.float_value); break;
	case SemanticType::ShortInt: view = std::to_string(value.short_int_value); break;
	case SemanticType::ClassObj: view = "";  view.append("object of the \" " + type.id + "\" class");
	}
	return view;
}

Data::~Data()
{

	std::cout << "\n" << this;
}
FunctionData::~FunctionData()
{
	if (type == SemanticType::ClassObj && value.object_of_class != nullptr)
		delete value.object_of_class;
	 delete returned_data;


	 std::cout << "\n" << this;

}

Data* Data::Clone() const
{
	Data* clone = new Data(type, this->id);
	//todo how copy link to node?
	clone->value = value;
	return clone;
}

void Data::SetValue(short int v)
{
	if(type != SemanticType::ShortInt)
	{
		exit(200);
	}
	value.short_int_value = v;
}
void Data::SetValue(float v)
{

	if (type != SemanticType::Float)
	{
		exit(200);
	}
	value.float_value = v;
}

Node::~Node()
{

	if (_neighbor != nullptr) {
		delete _neighbor;
	}
	if (_child != nullptr) {
		delete _child;
	}
	if (_parent != nullptr)
	{
		if (_parent->GetNeighbor() == this)
			_parent->_neighbor = nullptr;
		else
			_parent->_child = nullptr;
	}

	delete _data;

	std::cout << "\n" << this;
}
FunctionData::FunctionData(SemanticType return_type, const LexemaView& id) : Data(SemanticType::Function, id)
{
	returned_type = return_type;
	returned_data = new Data(returned_type, "");
	if (returned_type == SemanticType::ClassObj)
		returned_data->value.object_of_class = nullptr;
}

LexemaView FunctionData::GetValueView() const
{

	std::string result = "[type: ";
	result += TypesName.find(returned_type)->second;
	result += ", value: ";
	result += returned_data->GetValueView();
	result += "]";
	return result;
}

FunctionData* FunctionData::Clone() const
{
	FunctionData* clone = new FunctionData(returned_type, id);
	return clone;
}



Node::Node(Data* data)
{
	_data = data;
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
	delete _child;
	_child = child;
	if (child != nullptr)
		child->SetParent(this);
	return _child;
}
Node* Node::AddChild(Data* data)
{
	if (_child == nullptr)
		return SetChild(new Node(data));
	return _child->AddNeighbor(data);
}

Node* Node::GetChild() const
{
	return _child;
}

Node* Node::AddNeighbor(Data* data)
{
	return  SetNeighbor(new Node(data));
}
Node* Node::SetNeighbor(Node* neighbor)
{
	if (neighbor == nullptr)
		return  nullptr;
	Node* current = this;
	while (current->_neighbor != nullptr)
		current = current->_neighbor;
	current->_neighbor = neighbor;
	current->_neighbor->_parent = current;
	return  current->_neighbor;
}

Node* Node::GetNeighbor() const
{
	return _neighbor;
}

void Node::Print(std::ostream& out, int tab_count) const
{
	out << "\n";
	for (int i = 0; i < tab_count; i++)
		out << '\t';
	out << "[" << *this << "]";
	if (_child)
	{
		for (int i = 0; i < tab_count; i++)
			out << '\t';
		out << "\n\tchildren:";
		_child->Print(out, tab_count + 1);
	}
	if (_neighbor)
	{
		_neighbor->Print(out, tab_count);
	}
}

std::ostream& operator<<(std::ostream& out, const Node& node)
{
	out << *(node._data);
	return out;
}
std::ostream& operator<<(std::ostream& out, const Data& _data)
{
	auto typeName = TypesName.find(_data.type)->second;
	out << "Type: " << typeName;
	if(_data.id.length() > 0)
	out << ", name: " << _data.id;
	if (_data.type == SemanticType::Function)
	{
		out << ", to return: " << _data.GetValueView();
	}
	else {
		out << ", value: " << _data.GetValueView();
	}
	return out;
}

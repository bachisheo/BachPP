#include "SemanticNode.h"


Node::Node(Data *data)
{
    data = new Data(data->type, data->id);
    _parent = nullptr;
}
Node::Node()
{
    data = new Data(SemanticType::Empty, "emptyNode" );
}

void Node::SetParent(Node* parent)
{
    _parent = parent;
}

Node* Node::GetParent()
{
    return _parent;
}

Node* Node::AddChild(Node* child)
{
    if (_child == nullptr) {
        _child = new Node();
        _child->_parent = this;
    }
    return _child->AddNeighbor(child);
}

Node* Node::GetChild()
{
    return _child;
}

Node* Node::AddNeighbor(Node* neighbor)
{
    Node* current = this;
    while (_neighbor != nullptr)
        current = current->_neighbor;
    current->_neighbor = neighbor;
    current->_neighbor->_parent = current->_parent;
    return  current->_neighbor;
}

Node* Node::GetNeighbor()
{
    return _neighbor;
}

void Node::Print()
{
    std::cout << "Узел id: " << data->id << ", сосед: " << _neighbor->data->id;

    std::cout <<  "\nпотомки:";
    auto tmp = _child;
    while (tmp != nullptr) {
        std::cout << tmp->data->id << " | ";
        tmp = tmp->_neighbor;
    }
}

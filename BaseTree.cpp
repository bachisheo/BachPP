п»ї#include "BaseTree.h"
void BaseTree::Print(std::ostream& out) const
{
	out << "\n--------------------------РџРѕР»СѓС‡РёРІС€РµРµСЃСЏ РґРµСЂРµРІРѕ";
	if (_root)
		_root->Print(out, 0);
	else
		out << "\nРґРµСЂРµРІРѕ РїСѓСЃС‚Рѕ\n";

	
}
Node* BaseTree::CopySubtree(Node* sub_root)
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
void BaseTree::RemoveObject(Node* node)
{
	if (_current == node)
	{
		_current = node->GetParent();
	}
	delete node;
}
void BaseTree::SetTreePtr(Node* current)
{
	_current = current;
}
void BaseTree::SetParent(Node* parent) const
{
	_current->SetParent(parent);
}

Node* BaseTree::GetParent() const
{
	return _current->GetParent();
}

Node* BaseTree::AddNeighbor(Data* data)
{
	_current = _current->AddNeighbor(data);
	return _current;
}

Node* BaseTree::GetNeighbor() const
{
	return _current->GetNeighbor();
}

Node* BaseTree::FindUpOnLevel(const LexemaView& id) const
{
	if (_root == nullptr)
	{
		return nullptr;
	}
	return FindUpOnLevel(_current, id);
}

Node* BaseTree::FindUpOnLevel(Node* from, const LexemaView& id)
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
Node* BaseTree::FindUp(const LexemaView& id) const
{
	if (_root == nullptr)
	{
		return nullptr;
	}
	return FindUp(_current, id);
}

Node* BaseTree::FindUp(Node* from, const LexemaView& id)
{
	auto par = from;
	while (par != nullptr && par->_data->id != id) {
		par = par->GetParent();
	}
	return par;
}

Node* BaseTree::FindChild(const LexemaView& id) const
{
	return FindChild(_current, id);
}

Node* BaseTree::FindChild(const Node* from, const LexemaView& id)
{
	auto child = from->GetChild();
	while (!(child == nullptr || child->_data->id == id))
		child = child->GetNeighbor();
	return child;
}

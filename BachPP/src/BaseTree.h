#pragma once
#include "SemanticNode.h"

class BaseTree
{
protected:
	Node* _root;
	Node* _current;
public:
	void Print(std::ostream& out) const;
	void SetParent(Node* parent) const;
	Node* GetParent() const;
	Node* AddNeighbor(Data* data);
	Node* GetNeighbor() const;
	static Node* CopySubtree(Node* sub_root);
	void SetTreePtr(Node* current);
	void RemoveObject(Node* node);
	//поиск на уровне
	Node* FindUpOnLevel(const LexemaView& id) const;
	Node* FindUp(const LexemaView& id) const;
	static Node* FindUpOnLevel(Node* from, const LexemaView& id);
	static Node* FindUp(Node* from, const LexemaView& id);

	//поиск среди потомков
	Node* FindChild(const LexemaView& id) const;
	static Node* FindChild(const Node* from, const LexemaView& id);

};

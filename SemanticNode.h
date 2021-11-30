#pragma once
#include "Scanner.h"
class Node;
struct Data {
	SemanticType type = SemanticType::Undefined;
	SemanticType returnedType = SemanticType::Empty;
	LexemaView id;
	Data(SemanticType semType, const LexemaView& idView) :type(semType)
	{
		id = idView;
	}
	Data(const LexemaView& idView, SemanticType semType) :type(semType)
	{
		id = idView;
	}
};

class Node
{
	Node* _parent = nullptr;
	std::unique_ptr<Node> _child = nullptr, _neighbor = nullptr;
public:
	Data* _data;
	Node(Data data);
	Node();
	void SetParent(Node* parent);
	Node* GetParent() const;
	Node* AddChild(Data data);
	Node* GetChild() const;
	Node* AddNeighbor(Data data);
	Node* GetNeighbor() const;

	void Print() const;
};


#pragma once
#include "Scanner.h"
class Node;
union DataValue
{
	short int short_int_value;
	float float_value;
	int long_int_value;
};

struct Data {
	SemanticType data_type = SemanticType::Undefined;
	SemanticType returned_type = SemanticType::Empty;
	DataValue data_value;
	bool is_has_return_value = false;
	LexemaView id;
	Data(SemanticType semType, const LexemaView& idView);
};

class Node
{
	Node* _parent = nullptr;
	Node* _child = nullptr;
	std::unique_ptr<Node> _neighbor = nullptr;
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
	Node* SetChild(Node* child);


	void Print(std::ostream& out, int tab_count) const;
};

std::ostream & operator<<(std::ostream & out, const Node & node);

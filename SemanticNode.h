#pragma once
#include "Scanner.h"
class Node;
union DataValue
{
	short int short_int_value;
	float float_value;
};

struct Data{
	public:
	SemanticType data_type = SemanticType::Undefined;
	LexemaView id;
	DataValue data_value;
	Data(SemanticType semType, const LexemaView& idView);
	virtual ~Data() = default;
};

struct FunctionData : Data
{
	bool is_return_operator_declarated = false;
	SemanticType returned_type = SemanticType::Empty;
	Data* returned_data;
	FunctionData(SemanticType return_type, const LexemaView& id);
};


class Node
{
	Node* _parent = nullptr;
	Node* _child = nullptr;
	std::unique_ptr<Node> _neighbor = nullptr;
public:
	Data* _data;
	Node(Data * data);
	Node();
	void SetParent(Node* parent);
	Node* GetParent() const;
	Node* AddChild(Data * data);
	Node* GetChild() const;
	Node* AddNeighbor(Data * data);
	Node* GetNeighbor() const;
	Node* SetChild(Node* child);


	void Print(std::ostream& out, int tab_count) const;
};

std::ostream & operator<<(std::ostream & out, const Node & node);

#pragma once
#include "Scanner.h"
class Node;
union DataValue
{
	short int short_int_value;
	float float_value;
};
/// <summary>
/// Структура, хранящаяся в узле дерева
/// </summary>
struct Data{
	public:
	SemanticType type = SemanticType::Undefined;
	LexemaView id;
	DataValue value;
	Data(SemanticType semType, const LexemaView& idView);
	virtual ~Data() = default;
};
/// <summary>
/// Структура для узла с описанием функции
/// </summary>
struct FunctionData : Data
{
	bool is_return_operator_declarated = false;
	SemanticType returned_type = SemanticType::Empty;
	Data* returned_data;
	FunctionData(SemanticType return_type, const LexemaView& id);
	~FunctionData() override;
};


class Node
{
	Node* _parent = nullptr;
	Node* _child = nullptr;
	Node * _neighbor = nullptr;
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

	~Node();
};


std::ostream & operator<<(std::ostream & out, const Node & node);

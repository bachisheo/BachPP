#pragma once
#include "Scanner.h"
class Node;
union DataValue
{
	short int short_int_value;
	float float_value;
	Node* object_of_class;
};
/// <summary>
/// Структура, описывающая содержимое узла дерева
/// </summary>
struct Data{
private:
	public:
	SemanticType type = SemanticType::Undefined;
	LexemaView id;
	DataValue value;
	Data(SemanticType semType, const LexemaView& idView);
	virtual LexemaView GetValueView() const;
	virtual ~Data();
	virtual Data * Clone() const;
	
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
	LexemaView GetValueView() const override ;
	FunctionData * Clone() const override;
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
	Node* SetNeighbor(Node * node);
	Node* GetNeighbor() const;
	Node* SetChild(Node* child);
	void Print(std::ostream& out, int tab_count) const;

	~Node();
};


std::ostream & operator<<(std::ostream & out, const Node & node);
std::ostream & operator<<(std::ostream & out, const Data & data);

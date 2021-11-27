#pragma once
//#include "defs.h"
#include "Scanner.h"
class Node;
struct Data {
	SemanticType type = SemanticType::Undefined;
	SemanticType returnedType = SemanticType::Empty;
	LexemaView id;
	Data(SemanticType semType,const LexemaView idView):type(semType)
	{
		strcpy_s(id, idView);
	}
	Data(const LexemaView idView, SemanticType semType):type(semType)
	{
		strcpy_s(id, idView);
	}

};

class Node
{
	Node* _parent, * _child, * _neighbor;
public:
	Data* data;
	Node(Data * data);
	Node();
	void SetParent(Node* parent);
	Node* GetParent();
	Node* AddChild(Node* child);
	Node* GetChild();
	Node* AddNeighbor(Node* neighbor);
	Node* GetNeighbor();

	void Print();
};


#pragma once
#include "SemanticNode.h"
class SemanticTree
{
	Node* _root;
	Node* _current;
	Scanner* _sc;
	bool IsDataType(SemanticType type);
public:
	SemanticTree(Scanner* sc);
	///
	///Работа с элементами синтаксического дерева
	///
	void SemanticExit(std::vector<std::string> errMsg) const;
	void SetParent(Node* parent);
	Node* GetParent() const;
	Node* AddChild(Node* child);
	Node* AddNeighbor(Node* neighbor);
	Node* GetNeighbor();
	Node* FindUp(LexemaView id);
	static Node* FindUp(Node* from, LexemaView id);
	Node* FindChild(LexemaView id) const;
	static Node* FindChild(Node* from, LexemaView id);

	///
	///семантические подпрограммы
	///
	SemanticType GetType(LexType type_type, LexType next_type);
	SemanticType GetType(LexType type_type, LexemaView type_view);
	bool IsUnique(LexemaView lv);
	//при обращении
	SemanticType GetTypeByView(std::vector<char*> ids, bool isFunc = false);
	SemanticType	GetConstType(LexemaView lv, LexType lt);
	SemanticType GetResultType(SemanticType a, SemanticType b, LexType sign);
	bool IsEnableUnarySign(SemanticType type);
	bool IsComparableType(SemanticType realType, SemanticType neededType);
	//при описании переменных
	Node* AddObject(LexemaView lv, SemanticType type);
	Node* AddFunc(SemanticType returnedType, LexemaView funcName);
	void SetTreePtr(Node* current);
	Node* AddClass(LexemaView className);
	Node* AddBlock();
	Node* AddBlock(Node* parentNode);
};

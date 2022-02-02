п»ї#pragma once
#include "BaseTree.h"

class SemanticTree : public BaseTree
{
	Scanner* _sc;
	bool IsDataType(SemanticType type) const;
public:
	bool isInterpreting = true;
	SemanticTree(Scanner* sc);
	void SemanticExit(const std::vector<std::string> & errMsg) const;
	bool IsInOperator() const;
	Node* FindCurrentFunc() const;

	static SemanticType GetType(LexType type_type, LexType next_type);
	SemanticType GetType(LexType type_type, const LexemaView& type_view) const;
	void CheckUnique(const LexemaView& lv) const;

	//РїСЂРё РѕР±СЂР°С‰РµРЅРёРё
	Node* GetNodeByView(std::vector<LexemaView> & ids, bool isFunc = false) const;
	Data* GetConstData(const LexemaView& lv, LexType lt) const;
	void SetData(Node* dst, Data* src);
	SemanticType GetResultType(SemanticType a, SemanticType b, LexType sign);
	Data* GetResult(Data * a, Data * b, LexType sign);
	bool IsEnableUnarySign(SemanticType type) const;
	bool IsComparableType(SemanticType realType, SemanticType neededType);

	Node* AddVariableObject(Data * data);
	Node* AddFunctionDeclare(SemanticType returnedType, const LexemaView& funcName);
	Node* AddClass(const LexemaView& className);
	Node* AddCompoundBlock();
	Node* AddClassObject(const LexemaView& objName, const LexemaView& className);
		
private:
	Node* AddBlock(Data * panetrData);
	std::string GetFullName(Node* node);
};




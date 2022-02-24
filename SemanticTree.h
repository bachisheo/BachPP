п»ї#pragma once
#include "BaseTree.h"

class SemanticTree : public BaseTree
{
	Scanner* _sc;
	bool IsDataType(SemanticType type) const;
	Data * CalculateFloatValue(Data* a, Data* b, LexType sign) const;
	Data * CalculateShortIntValue(Data* a, Data* b, LexType sign) const;
	Data * CalculateFloatLogic(Data* a, Data* b, LexType sign) const;
	Data * CalculateShortIntLogic(Data* a, Data* b, LexType sign) const;
public:
	bool isInterpreting = true, is_returned = false;;
	SemanticTree(Scanner* sc);
	void SemanticExit(const std::vector<std::string> & errMsg) const;
	Node* FindCurrentFunc() const;

	static SemanticType GetType(LexType type_type, LexType next_type);
	SemanticType GetType(LexType type_type, const LexemaView& type_view) const;
	void CheckUnique(const LexemaView& lv) const;

	Node* GetNodeByView(std::vector<LexemaView> & ids, bool isFunc = false) const;
	Data* GetConstData(const LexemaView& lv, LexType lt) const;
	void SetData(Node* dst, Data* src);
	SemanticType GetResultType(SemanticType a, SemanticType b, LexType sign);
	Data * BinaryOperation(Data * a, Data * b, LexType sign);
	Data * UnaryOperation(Data * a, LexType sign) const;
	Data * LogicalOperation(Data * a, Data * b, LexType sign);
	bool IsEnableUnaryOperation(SemanticType type) const;
	bool IsComparableType(SemanticType realType, SemanticType neededType);

	Node* AddVariableObject(Data * data);
	Node* AddFunctionDeclare(SemanticType returnedType, const LexemaView& funcName);
	Node* FunctionCall(std::vector<LexemaView>& ids);
	Node* AddClass(const LexemaView& className);
	Node* AddCompoundBlock();
	Node* AddClassObject(const LexemaView& objName, const LexemaView& className);
private:
	std::string GetFullName(Node* node);
};




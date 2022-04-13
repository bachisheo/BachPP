#pragma once
#include "BaseTree.h"
#include "Exception/CompilException.h"
class SemanticTree : protected BaseTree
{
	Scanner* _sc;
	bool IsDataType(SemanticType type) const;
	Data * CalculateFloatValue(Data* a, Data* b, LexType sign) const;
	Data * CalculateShortIntValue(Data* a, Data* b, LexType sign) const;
	Data * CalculateFloatLogic(Data* a, Data* b, LexType sign) const;
	Data * CalculateShortIntLogic(Data* a, Data* b, LexType sign) const;
public:
	bool isInterpreting = true;
	bool isReturned = false;
	bool isWork() const;
	SemanticTree(Scanner* sc);
	void SemanticExit(const std::vector<std::string>& errMsg,ErrorCode code) const;
	Node* FindCurrentFunc() const;
	static SemanticType GetType(LexType type_type, LexType next_type);
	SemanticType GetType(LexType type_type, const LexemaView& type_view) const;
	void CheckUnique(const LexemaView& lv) const;

	Node* GetNodeByView(std::vector<LexemaView> & ids, bool isFunc = false) const;
	Data* GetConstData(const LexemaView& lv, LexType lt) const;
	void SetData(Node* dst, Data* src);
	SemanticType GetResultType(SemanticType a, SemanticType b, LexType sign);
	Data * BinaryOperation(Data * a, Data * b, LexType sign);
	Data * UnaryOperation(Data * a, LexType sign);
	Data * LogicalOperation(Data * a, Data * b, LexType sign);
	bool IsEnableUnaryOperation(SemanticType type) const;
	Node* AddEmptyNodeAsChild();
	Node* AddVariableObject(Data * data);
	Node* AddFunctionDeclare(SemanticType returnedType, const LexemaView& funcName, Node* func_body);
	Node* AddClass(const LexemaView& className);
	Node* AddCompoundBlock();
	Node* AddClassObject(const LexemaView& objName, const LexemaView& className);
	void CheckWhileExp(Data* data) const;
	bool IsWhileExecute(Data* data) const;
	void SetReturnedData(Data* data) const;
	Data* GetNodeValue(std::vector<LexemaView> ids) const;
	Node* AddFunctionCall(Node * func);
	void RemoveFunctionCall(Node * func_call);
//BaseTree overriting mathods
	void RemoveObject(Node* node);
	void Print(std::ostream& out) const;
	void SetTreePtr(Node* current);
	Node * GetTreePtr();
private:
	bool IsComparableType(SemanticType realType, SemanticType neededType) const;
	std::string GetFullName(Node* node);
	std::string NameToString( std::vector<LexemaView> & ids) const;
};






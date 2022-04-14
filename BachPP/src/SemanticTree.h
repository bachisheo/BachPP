#pragma once
#include "BaseTree.h"
#include "Exception/CompilException.h"
class SemanticTree : protected BaseTree
{
	Scanner* _sc;

public:
	SemanticTree(Scanner* sc);
	void SemanticExit(const std::vector<std::string>& errMsg,ErrorCode code) const;
	void CheckUnique(const LexemaView& lv) const;

	Node* GetNodeByView(std::vector<LexemaView> & ids, bool isFunc = false) const;
	
	void SetData(Node* dst, Data* src);
	bool IsEnableUnaryOperation(SemanticType type) const;
	Node* AddEmptyNodeAsChild();
	Node* AddVariableObject(Data * data);
	Node* AddFunctionDeclare(SemanticType returnedType, const LexemaView& funcName, Node* func_body);
	Node* AddClass(const LexemaView& className);
	Node* AddCompoundBlock();
	Node* AddClassObject(const LexemaView& objName, const LexemaView& className);

	Data* GetNodeValue(std::vector<LexemaView> ids) const;
//BaseTree overriting mathods
	void RemoveObject(Node* node);
	void Print(std::ostream& out) const;
	void SetTreePtr(Node* current);
	Node * GetTreePtr() const;
	Node* FindUp(const LexemaView& type_view) const;
private:
	
	std::string GetFullName(Node* node);

};






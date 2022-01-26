п»ї#pragma once
#include "SemanticNode.h"
class SemanticTree
{
	Node * _root;
	Node* _current;
	Scanner* _sc;
	bool IsDataType(SemanticType type) const;
public:
	void Print(std::ostream& out) const;
	SemanticTree(Scanner* sc);
	///
	///Р Р°Р±РѕС‚Р° СЃ СЌР»РµРјРµРЅС‚Р°РјРё СЃРёРЅС‚Р°РєСЃРёС‡РµСЃРєРѕРіРѕ РґРµСЂРµРІР°
	///
	void SemanticExit(const std::vector<std::string> & errMsg) const;
	void SetParent(Node* parent) const;
	Node* GetParent() const;
	Node* AddNeighbor(Data* data);
	Node* GetNeighbor() const;
	//РїРѕРёСЃРє РЅР° СѓСЂРѕРІРЅРµ
	Node* FindUpOnLevel(const LexemaView& id) const;
	Node* FindUp(const LexemaView& id) const;
	static Node* FindUpOnLevel(Node* from, const LexemaView& id);
	static Node* FindUp(Node* from, const LexemaView& id);
	Node* FindCurrentFunc();
	//РїРѕРёСЃРє СЃСЂРµРґРё РїРѕС‚РѕРјРєРѕРІ
	Node* FindChild(const LexemaView& id) const;
	static Node* FindChild(const Node* from, const LexemaView& id);

	///
	///СЃРµРјР°РЅС‚РёС‡РµСЃРєРёРµ РїРѕРґРїСЂРѕРіСЂР°РјРјС‹
	///
	static SemanticType GetType(LexType type_type, LexType next_type);
	SemanticType GetType(LexType type_type, const LexemaView& type_view) const;
	bool IsUnique(const LexemaView& lv) const;
	//РїСЂРё РѕР±СЂР°С‰РµРЅРёРё
	SemanticType GetTypeByView(std::vector<LexemaView> & ids, bool isFunc = false) const;
	SemanticType	GetConstType(const LexemaView& lv, LexType lt) const;
	SemanticType GetResultType(SemanticType a, SemanticType b, LexType sign);
	bool IsEnableUnarySign(SemanticType type) const;
	bool IsComparableType(SemanticType realType, SemanticType neededType);
	//РїСЂРё РѕРїРёСЃР°РЅРёРё РїРµСЂРµРјРµРЅРЅС‹С…
	Node* AddObject(const LexemaView& lv, SemanticType type);
	Node* AddObject(Data * data);
	Node* AddFunc(SemanticType returnedType, const LexemaView& funcName);
	void SetTreePtr(Node* current);
	Node* AddClass(const LexemaView& className);
	Node* AddCompoundBlock();
	Node* AddClassObject(const LexemaView& objName, const LexemaView& className);
private:
	Node* AddBlock(Data * panetrData);
};

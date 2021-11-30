#include "SyntacticalAnalyzer.h"

#include <utility>

SyntacticalAnalyzer::SyntacticalAnalyzer(Scanner* sc) : _sc(sc)
{
	_tree = new SemanticTree(_sc);
}

void SyntacticalAnalyzer::Operator()
{
	auto nextLex = LookForward();
	switch (nextLex) {
		//Р±Р»РѕРє
	case LexType::LFigBracket:
	{
		_sc->Scan();
		auto ptr = _tree->AddCompoundBlock();
		Block();
		_tree->SetTreePtr(ptr);
		break;
	}
	//while СЃ РїСЂРµРґСѓСЃР»РѕРІРёРµРј
	case LexType::While:
		_sc->Scan();
		CheckScan(LexType::LRoundBracket);
		//СѓСЃР»РѕРІРёРµ
		Expression();
		CheckScan(LexType::RRoundBracket);
		Operator();
		break;
		//РїСЂРёСЃРІР°РёРІР°РЅРёРµ 
	case LexType::Id:
	{
		auto name = GetFullName();
		auto varType = _tree->GetTypeByView(name);
		CheckScan(LexType::Equal);
		auto valueType = Expression();
		_tree->IsComparableType(valueType, varType);
		CheckScan(LexType::DotComma);
		break;
	}
	case LexType::Return:
		_sc->Scan();
		Expression();
		CheckScan(LexType::DotComma);
		break;
	default:;
	}
}
// --{ declInFunc } --
void SyntacticalAnalyzer::Block()
{
	CheckScan(LexType::LFigBracket);
	DeclarateInFunction();
	CheckScan(LexType::RFigBracket);
}

//РїСЂРѕСЃРєР°РЅРёСЂРѕРІР°С‚СЊ РЅР° k СЃРёРјРІРѕР»РѕРІ РІРїРµСЂРµРґ Р±РµР· 
//РёР·РјРµРЅРµРЅРёСЏ СѓРєР°Р·Р°С‚РµР»РµР№ СЃРєР°РЅРµСЂР°
LexType SyntacticalAnalyzer::LookForward(size_t k)
{
	int _ptr, _col, _line;
	_sc->GetPtrs(_ptr, _line, _col);
	LexType lex = LexType::End;
	for (size_t i = 0; i < k; i++)
	{
		lex = _sc->Scan();
		if (lex == LexType::End)
			break;
	}
	_sc->SetPtrs(_ptr, _line, _col);
	return lex;
}

//СЃС‡РёС‚С‹РІР°РµС‚ Р»РµРєСЃРµРјСѓ Рё РІРѕР·РІСЂР°С‰Р°РµС‚ СѓРєР°Р·Р°С‚РµР»СЊ РЅР° РїСЂРµР¶РЅРµРµ РјРµСЃС‚Рѕ,
//РµСЃР»Рё Р»РµРєСЃРµРјР° РЅРµ СЂР°РІРЅР° Р·Р°РґР°РЅРЅРѕР№
bool SyntacticalAnalyzer::CheckScan(LexType neededLex, LexemaView& lv, bool needMsg)
{
	int _ptr, _col, _line;
	_sc->GetPtrs(_ptr, _line, _col);
	auto lex = _sc->Scan(lv);
	if (lex != neededLex) {
		if (needMsg)
			LexExit(neededLex);
		_sc->SetPtrs(_ptr, _line, _col);
		return false;
	}
	return true;
}
bool SyntacticalAnalyzer::CheckScan(LexType neededLex, bool needMsg)
{
	LexemaView lv;
	return CheckScan(neededLex, lv, needMsg);
}

//РџРѕР»СѓС‡РёС‚СЊ РїРѕР»РЅРѕРµ РїСЂРё РѕР±СЂР°С‰РµРЅРёРё РІРёРґР° a.b.c...
std::vector<LexemaView> SyntacticalAnalyzer::GetFullName()
{
	auto ids = std::vector<LexemaView>();
	LexType next = LookForward();
	while (next == LexType::Id) {
		LexemaView lv;
		_sc->Scan(lv);
		ids.push_back(lv);
		CheckScan(LexType::Dot, false);
		next = LookForward();
	}
	return ids;
}

SemanticType SyntacticalAnalyzer::Type(LexemaView& lv) const
{

	auto lex = _sc->Scan(lv);
	if (lex == LexType::Short || lex == LexType::Long) {
		auto nextLex = _sc->Scan();
		return _tree->GetType(lex, nextLex);
	}
	return _tree->GetType(lex, lv);
}

//---- class id { Program } ;----
void SyntacticalAnalyzer::ClassDeclarate()
{
	LexemaView className;
	std::vector<LexType> ltsBeforeProgramm = { LexType::Class, LexType::Id, LexType::LFigBracket };
	std::vector<LexType> ltsAfterProgramm = { LexType::RFigBracket, LexType::DotComma };
	CheckScan(LexType::Class);
	CheckScan(LexType::Id, className);
	const auto ptr = _tree->AddClass(className);
	Program(LexType::RFigBracket);
	_tree->SetTreePtr(ptr);
	CheckScan(LexType::RFigBracket);
	CheckScan(LexType::DotComma);
}
SemanticType SyntacticalAnalyzer::Expression()
{
	auto o1 = LogicalExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::LogEqual || nextLex == LexType::LogNotEqual) {
		auto sign = _sc->Scan();
		o1 = _tree->GetResultType(o1, LogicalExpression(), sign);
		nextLex = LookForward();
	}
	return o1;
}

SemanticType SyntacticalAnalyzer::LogicalExpression()
{
	auto o1 = ShiftExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::Less || nextLex == LexType::LessOrEqual ||
		nextLex == LexType::More || nextLex == LexType::MoreOrEqual) {
		auto sign = _sc->Scan();
		auto o2 = ShiftExpression();
		o1 = _tree->GetResultType(o1, o2, sign);
		nextLex = LookForward();
	}
	return o1;
}

SemanticType SyntacticalAnalyzer::ShiftExpression()
{
	auto o1 = AdditionalExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::ShiftLeft || nextLex == LexType::ShiftRight) {
		auto sign = _sc->Scan();
		auto o2 = AdditionalExpression();
		o1 = _tree->GetResultType(o1, o2, sign);
		nextLex = LookForward();
	}
	return o1;
}

SemanticType SyntacticalAnalyzer::AdditionalExpression()
{
	auto nextLex = LookForward();
	bool unaryOperation = false;
	if (nextLex == LexType::Plus || nextLex == LexType::Minus) {
		_sc->Scan();
		unaryOperation = true;
	}
	auto o1 = MultExpression();
	if (unaryOperation)
	{
		_tree->IsEnableUnarySign(o1);
	}
	nextLex = LookForward();
	while (nextLex == LexType::Plus || nextLex == LexType::Minus) {
		auto sign = _sc->Scan();
		auto o2 = MultExpression();
		o1 = _tree->GetResultType(o1, o2, sign);
		nextLex = LookForward();
	}
	return o1;
}

SemanticType SyntacticalAnalyzer::MultExpression()
{
	auto o1 = ElementaryExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::DivSign || nextLex == LexType::ModSign || nextLex == LexType::MultSign) {
		auto sign = _sc->Scan();
		auto o2 = ElementaryExpression();
		o1 = _tree->GetResultType(o1, o2, sign);
		nextLex = LookForward();
	}
	return o1;
}


SemanticType SyntacticalAnalyzer::ElementaryExpression()
{
	LexType lexType = LookForward();
	LexemaView lv;
	SemanticType result;
	switch (lexType) {
	case LexType::ConstExp:
	case LexType::ConstInt:
		_sc->Scan(lv);
		result = _tree->GetConstType(lv, lexType);
		break;

	case LexType::Id: {
		auto ids = GetFullName();
		lexType = LookForward();
		//РµСЃР»Рё РїСЂРѕРёСЃС…РѕРґРёС‚ РІС‹Р·РѕРІ С„СѓРЅРєС†РёРё
		if (lexType == LexType::LRoundBracket) {
			_sc->Scan();
			//РїСЂРѕРІРµСЂРёС‚СЊ, РµСЃС‚СЊ Р»Рё С‚Р°РєР°СЏ С„СѓРЅРєС†РёСЏ Рё РІРµСЂРЅСѓС‚СЊ РµРµ С‚РёРї
			result = _tree->GetTypeByView(ids, true);
			CheckScan(LexType::RRoundBracket);
		}
		else
			//РїСЂРё РѕР±СЂР°С‰РµРЅРёРё Рє РїРµСЂРµРјРµРЅРЅРѕР№
		{
			//РїСЂРѕРІРµСЂРёС‚СЊ, РµСЃС‚СЊ Р»Рё С‚Р°РєР°СЏ РїРµСЂРµРјРµРЅРЅР°СЏ Рё РІРµСЂРЅСѓС‚СЊ РµРµ С‚РёРї
			result = _tree->GetTypeByView(ids);
		}
		break;
	}
	case LexType::LRoundBracket:
		_sc->Scan();
		result = Expression();
		CheckScan(LexType::RRoundBracket);
		break;
	default:
		LexExit({ "No empty expression" });
		result = SemanticType::Undefined;
	}
	return result;
}

//     ---|     CLASS		|---
//     ---|     FUNCTION	|---
//     ---|     DATA		|---
//  ------------------------------
void SyntacticalAnalyzer::Program(LexType endLex)
{
	auto nextLex = LookForward();
	while (nextLex != endLex) {
		if (nextLex == LexType::Class)
			ClassDeclarate();
		else {
			int _ptr, _col, _line;
			_sc->GetPtrs(_ptr, _line, _col);
			Type();
			//РїРѕР»СѓС‡РёС‚СЊ СЃР»РµРґСѓСЋС‰СѓСЋ Р·Р° id Р»РµРєСЃРµРјСѓ
			nextLex = LookForward(2);
			_sc->SetPtrs(_ptr, _line, _col);
			if (nextLex == LexType::LRoundBracket)
				FunctionDeclarate();
			else
				Data();
		}
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::LexExit(const std::vector<std::string>& waiting) const
{
	_sc->ErrorMsg(MSG_ID::SYNT_ERR, waiting);
	exit(1);
}
void SyntacticalAnalyzer::LexExit(LexType waitingLex) const
{
	LexExit({ TypesName.find(waitingLex)->second });

}
//  
//     ---|     CLASS		|---
//     ---|     DATA		|---
//     ---|     OPERATOR	|---
//  ------------------------------
void SyntacticalAnalyzer::DeclarateInFunction()
{
	LexType endLex = LexType::RFigBracket;
	LexType nextLex = LookForward();
	while (nextLex != endLex) {
		if (nextLex == LexType::Class) {
			ClassDeclarate();
		}
		else
			if (std::find(TypeWords.begin(), TypeWords.end(), nextLex) != TypeWords.end())
				Data();
			else Operator();
		nextLex = LookForward();
	}
}
//---type   |--id    --|() block
//          |--main  --|
void SyntacticalAnalyzer::FunctionDeclarate()
{
	auto type = Type();
	LexemaView funcName;
	if (!CheckScan(LexType::main, funcName, false))
		CheckScan(LexType::Id, funcName);
	auto ptr = _tree->AddFunc(type, funcName);
	CheckScan(LexType::LRoundBracket);
	CheckScan(LexType::RRoundBracket);
	Block();
	_tree->SetTreePtr(ptr);
}
//type var |;
//         |, var...;
void SyntacticalAnalyzer::Data()
{
	LexemaView typeView;
	const auto varType = Type(typeView);
	LexemaView objName;
	CheckScan(LexType::Id, objName);
	if (LookForward() == LexType::Equal) {
		_sc->Scan();
		auto valueType = Expression();
		_tree->IsComparableType(varType, valueType);
	}
	
	while (LookForward() == LexType::Comma) {
		_sc->Scan();
		CheckScan(LexType::Id, objName);
		if (LookForward() == LexType::Equal) {
			_sc->Scan();
			auto valueType = Expression();
			_tree->IsComparableType(varType, valueType);
		}
		if (varType == SemanticType::ClassObj)
		{
			_tree->AddClassObject(objName, typeView);
		}
		else {
			_tree->AddObject(objName, varType);
		}
	}
	CheckScan(LexType::DotComma);
}





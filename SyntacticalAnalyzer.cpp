#include "SyntacticalAnalyzer.h"
SyntacticalAnalyzer::SyntacticalAnalyzer(Scanner* sc) : _sc(sc)
{
	_tree = new SemanticTree(_sc);
}

void SyntacticalAnalyzer::Operator()
{
	auto nextLex = LookForward();
	switch (nextLex) {
		//блок
	case LexType::LFigBracket: Block(); return;
		//while с предусловием
	case LexType::While:
		_sc->Scan();
		if (!saveCheckScan(LexType::LRoundBracket)) return;
		Expression();
		if (!saveCheckScan(LexType::RRoundBracket)) return;
		Operator();
		return;
		//присваивание 
	case LexType::Id:
		//Name();
		if (!saveCheckScan(LexType::Equal)) return;
		Expression();
		break;
	case LexType::Return:
		_sc->Scan();
		Expression();
		break;
	}
	saveCheckScan(LexType::DotComma);
	int g = 2;
	return;

}
// --{ declInFunc } --
void SyntacticalAnalyzer::Block()
{
	SavePtrs();
	if (!saveCheckScan(LexType::LFigBracket)) {
		return;
	}
	DeclarateInFunction();
	if (!saveCheckScan(LexType::RFigBracket)) {
		RestorePtrs();
		return;
	}
}

//просканировать на k символов вперед без 
//изменения указателей сканера
LexType SyntacticalAnalyzer::LookForward(int k)
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

void SyntacticalAnalyzer::SavePtrs()
{
	_sc->GetPtrs(ptr, linePtr, colPtr);
}

void SyntacticalAnalyzer::RestorePtrs()
{
	_sc->SetPtrs(ptr, linePtr, colPtr);
}
//считывает лексему и возвращает указатель на прежнее место,
//если лексема не равна заданной
bool SyntacticalAnalyzer::saveCheckScan(LexType neededLex, bool needMsg)
{
	int _ptr, _col, _line;
	_sc->GetPtrs(_ptr, _line, _col);
	auto lex = _sc->Scan();
	if (lex != neededLex) {
		if (needMsg)
			LexExit(neededLex);
		_sc->SetPtrs(_ptr, _line, _col);
		return false;
	}
	return true;
}

void SyntacticalAnalyzer::Name(char* lv)
{
	LexType next = LookForward();
	while (next == LexType::Id) {
		_sc->Scan();
		if (!saveCheckScan(LexType::Dot, false))
			return;
		next = LookForward();
	}
}

bool SyntacticalAnalyzer::Type()
{
	auto lex = sc.Scan();
	auto it = std::find(LtTypes.begin(), LtTypes.end(), lex);
	if (it == LtTypes.end()) {
		LexExit({ "Type" });
		return false;
	}
	if (lex == LexType::Long || lex == LexType::Short) {
		if (!saveCheckScan(LexType::Int)) {
			return false;
		}
	}

	return true;
}

//---- class id { Programm } ;----
void SyntacticalAnalyzer::ClassSynt()
{
	std::vector<LexType> ltsBeforeProgramm = { LexType::Class, LexType::Id, LexType::LFigBracket };
	std::vector<LexType> ltsAfterProgramm = { LexType::RFigBracket, LexType::DotComma };
	for (auto lts : ltsBeforeProgramm)
		if (!saveCheckScan(lts))
			return;
	Programm(LexType::RFigBracket);
	for (auto lts : ltsAfterProgramm)
		if (!saveCheckScan(lts))
			return;
}
SemanticType SyntacticalAnalyzer::Expression()
{
	LogicalExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::LogEqual || nextLex == LexType::NotEqual) {
		_sc->Scan();
		LogicalExpression();
		nextLex = LookForward();
	}
	return SemanticType::ShortInt;
}

void SyntacticalAnalyzer::LogicalExpression()
{
	ShiftExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::Less || nextLex == LexType::LessOrEqual ||
		nextLex == LexType::More || nextLex == LexType::MoreOrEqual) {
		_sc->Scan();
		ShiftExpression();
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::ShiftExpression()
{
	AdditionalExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::ShiftLeft || nextLex == LexType::ShiftRight) {
		_sc->Scan();
		AdditionalExpression();
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::AdditionalExpression()
{
	auto nextLex = LookForward();
	if (nextLex == LexType::Plus || nextLex == LexType::Minus)
		_sc->Scan();
	MultExpression();
	nextLex = LookForward();
	while (nextLex == LexType::Plus || nextLex == LexType::Minus) {
		_sc->Scan();
		MultExpression();
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::MultExpression()
{
	ElementaryExpression();
	auto nextLex = LookForward();
	while (nextLex == LexType::DivSign || nextLex == LexType::ModSign || nextLex == LexType::MultSign) {
		_sc->Scan();
		ElementaryExpression();
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::ElementaryExpression()
{
	LexType nextLex = LookForward();
	switch (nextLex) {
	case LexType::ConstExp: _sc->Scan(); return;
	case LexType::ConstInt: _sc->Scan(); return;
	case LexType::Id:
		//Name();
		nextLex = LookForward();
		if (nextLex == LexType::LRoundBracket) {
			_sc->Scan();
			if (!saveCheckScan(LexType::RRoundBracket)) return;
		}
		return;
	case LexType::LRoundBracket:
		_sc->Scan();
		Expression();
		if (!saveCheckScan(LexType::RRoundBracket)) return;
		return;
	default:
		LexExit({ "No empty expression" });
		return;
	}
}

//     ---|     CLASS		|---
//     ---|     FUNCTION	|---
//     ---|     DATA		|---
//  ------------------------------
void SyntacticalAnalyzer::Programm(LexType endLex)
{
	auto nextLex = LookForward();
	while (nextLex != endLex) {
		if (nextLex == LexType::Class)
			ClassSynt();
		else {
			SavePtrs();
			Type();
			//получить следующую за id лексему
			nextLex = LookForward(2);
			RestorePtrs();
			if (nextLex == LexType::LRoundBracket)
				Function();
			else
				Data();
		}
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::LexExit(std::vector<std::string> waiting)
{
	_sc->ErrorMsg(MSG_ID::SYNT_ERR, waiting);
	exit(1);
}
void SyntacticalAnalyzer::LexExit(LexType waitingLex)
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
			ClassSynt();
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
void SyntacticalAnalyzer::Function()
{
	if (!Type()) return;
	if (!saveCheckScan(LexType::main, false)) Name();
	if (!saveCheckScan(LexType::LRoundBracket))	return;
	if (!saveCheckScan(LexType::RRoundBracket)) return;
	Block();
}
//type var |;
//         |, var...;
void SyntacticalAnalyzer::Data()
{
	if (!Type()) return;
	Name();
	if (LookForward() == LexType::Equal) {
		sc.Scan();
		Expression();
	}
	while (LookForward() == LexType::Comma) {
		sc.Scan();
		Name();
		if (LookForward() == LexType::Equal) {
			sc.Scan();
			Expression();
		}
	};
	saveCheckScan(LexType::DotComma);
}

void SyntacticalAnalyzer::Variable()
{

}



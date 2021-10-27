#include "SyntacticalAnalyzer.h"
void SyntacticalAnalyzer::Operator()
{
	auto nextLex = LookForward();
	switch (nextLex) {
		//блок
	case lt::LFigBracket: Block(); return;
		//while с предусловием
	case lt::While:
		sc.Scan();
		if (!saveCheckScan(lt::LRoundBracket)) return;
		Expression();
		if (!saveCheckScan(lt::RRoundBracket)) return;
		Operator();
		return;
		//присваивание 
	case lt::Id:
		Name();
		if (!saveCheckScan(lt::Equal)) return;
		Expression();
		break;
	case lt::Return:
		sc.Scan();
		Expression();
		break;
	}
	saveCheckScan(lt::DotComma);
	int g = 2;
	return;

}
// --{ declInFunc } --
void SyntacticalAnalyzer::Block()
{
	SavePtrs();
	if (!saveCheckScan(lt::LFigBracket)) {
		return;
	}
	DeclarateInFunction();
	if (!saveCheckScan(lt::RFigBracket)) {
		RestorePtrs();
		return;
	}
}

//просканировать на k символов вперед без 
//изменения указателей сканера
lt SyntacticalAnalyzer::LookForward(int k)
{
	int _ptr, _col, _line;
	sc.GetPtrs(_ptr, _line, _col);
	lt lex = lt::End;
	for (size_t i = 0; i < k; i++)
	{
		lex = sc.Scan();
		if (lex == lt::End)
			break;
	}
	sc.SetPtrs(_ptr, _line, _col);
	return lex;
}

void SyntacticalAnalyzer::SavePtrs()
{
	sc.GetPtrs(ptr, linePtr, colPtr);
}

void SyntacticalAnalyzer::RestorePtrs()
{
	sc.SetPtrs(ptr, linePtr, colPtr);
}
//считывает лексему и возвращает указатель на прежнее место,
//если лексема не равна заданной
bool SyntacticalAnalyzer::saveCheckScan(lt neededLex, bool needMsg)
{
	int _ptr, _col, _line;
	sc.GetPtrs(_ptr, _line, _col);
	auto lex = sc.Scan();
	if (lex != neededLex) {
		if (needMsg)
			LexExit(neededLex);
		sc.SetPtrs(_ptr, _line, _col);
		return false;
	}
	return true;
}

void SyntacticalAnalyzer::Name()
{
	lt next = LookForward();
	while (next == lt::Id) {
		sc.Scan();
		if (!saveCheckScan(lt::Dot, false))
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
	if (lex == lt::Long || lex == lt::Short) {
		if (!saveCheckScan(lt::Int)) {
			return false;
		}
	}
	return true;
}

//---- class id { Programm } ;----
void SyntacticalAnalyzer::ClassSynt()
{
	std::vector<lt> ltsBeforeProgramm = { lt::Class, lt::Id, lt::LFigBracket };
	std::vector<lt> ltsAfterProgramm = { lt::RFigBracket, lt::DotComma };
	for (auto lts : ltsBeforeProgramm)
		if (!saveCheckScan(lts))
			return;
	Programm(lt::RFigBracket);
	for (auto lts : ltsAfterProgramm)
		if (!saveCheckScan(lts))
			return;
}
void SyntacticalAnalyzer::Expression()
{
	LogicalExpression();
	auto nextLex = LookForward();
	while (nextLex == lt::LogEqual || nextLex == lt::NotEqual) {
		sc.Scan();
		LogicalExpression();
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::LogicalExpression()
{
	ShiftExpression();
	auto nextLex = LookForward();
	while (nextLex == lt::Less || nextLex == lt::LessOrEqual ||
		nextLex == lt::More || nextLex == lt::MoreOrEqual) {
		sc.Scan();
		ShiftExpression();
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::ShiftExpression()
{
	AdditionalExpression();
	auto nextLex = LookForward();
	while (nextLex == lt::ShiftLeft || nextLex == lt::ShiftRight) {
		sc.Scan();
		AdditionalExpression();
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::AdditionalExpression()
{
	auto nextLex = LookForward();
	if (nextLex == lt::Plus || nextLex == lt::Minus)
		sc.Scan();
	MultExpression();
	nextLex = LookForward();
	while (nextLex == lt::Plus || nextLex == lt::Minus) {
		sc.Scan();
		MultExpression();
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::MultExpression()
{
	ElementaryExpression();
	auto nextLex = LookForward();
	while (nextLex == lt::DivSign || nextLex == lt::ModSign || nextLex == lt::MultSign) {
		sc.Scan();
		ElementaryExpression();
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::ElementaryExpression()
{
	lt nextLex = LookForward();
	switch (nextLex) {
	case lt::ConstExp: sc.Scan(); return;
	case lt::ConstInt: sc.Scan(); return;
	case lt::Id:
		Name();
		nextLex = LookForward();
		if (nextLex == lt::LRoundBracket) {
			sc.Scan();
			if (!saveCheckScan(lt::RRoundBracket)) return;
		}
		return;
	case lt::LRoundBracket:
		sc.Scan();
		Expression();
		if (!saveCheckScan(lt::RRoundBracket)) return;
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
void SyntacticalAnalyzer::Programm(lt endLex)
{
	auto nextLex = LookForward();
	while (nextLex != endLex) {
		if (nextLex == lt::Class)
			ClassSynt();
		else {
			SavePtrs();
			Type();
			//получить следующую за id лексему
			nextLex = LookForward(2);
			RestorePtrs();
			if (nextLex == lt::LRoundBracket)
				Function();
			else
				Data();
		}
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::LexExit(std::vector<std::string> waiting)
{
	sc.ErrorMsg(MSG_ID::WAIT_LEX, waiting);
	exit(1);
}
void SyntacticalAnalyzer::LexExit(lt waitingLex)
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
	lt endLex = lt::RFigBracket;
	lt nextLex = LookForward();
	while (nextLex != endLex) {
		if (nextLex == lt::Class) {
			ClassSynt();
		}
		else
			if (std::find(LtTypes.begin(), LtTypes.end(), nextLex) != LtTypes.end())
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
	if (!saveCheckScan(lt::main, false)) Name();
	if (!saveCheckScan(lt::LRoundBracket))	return;
	if (!saveCheckScan(lt::RRoundBracket)) return;
	Block();
}
//type var |;
//         |, var...;
void SyntacticalAnalyzer::Data()
{
	if (!Type()) return;
	Name();
	if (LookForward() == lt::Equal) {
		sc.Scan();
		Expression();
	}
	while (LookForward() == lt::Comma) {
		sc.Scan();
		Name();
		if (LookForward() == lt::Equal) {
			sc.Scan();
			Expression();
		}
	};
	saveCheckScan(lt::DotComma);
}

void SyntacticalAnalyzer::Variable()
{

}



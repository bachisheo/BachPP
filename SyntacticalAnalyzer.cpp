#include "SyntacticalAnalyzer.h"
// --{ declInFunc } --
void SyntacticalAnalyzer::Block()
{
	auto lex = sc.Scan();
	int ptr, lin, col;
	sc.GetPtrs(ptr, lin, col);
	if (lex != lt::LFigBracket) {
		LexExit(lt::LFigBracket);
		sc.SetPtrs(ptr, lin, col);
		return;
	}
	DeclarateInFunction();
	if (sc.Scan() != lt::RFigBracket) {
		LexExit(lt::LFigBracket);
		sc.SetPtrs(ptr, lin, col);
		return;
	}
}

//просканировать на k символов вперед без 
//ихменения указателей сканера
lt SyntacticalAnalyzer::LookForward(int k)
{
	int ptr, line, col;
	sc.GetPtrs(ptr, line, col);
	lt lex = lt::End;
	for (size_t i = 0; i < k; i++)
	{
		lex = sc.Scan();
		if (lex == lt::End)
			break;
	}
	sc.SetPtrs(ptr, line, col);
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

bool SyntacticalAnalyzer::checkScan(lt neededLex, bool needMsg)
{
	SavePtrs();
	auto lex = sc.Scan();
	if (lex != neededLex) {
		if (needMsg)
			LexExit(neededLex);
		RestorePtrs();
		return false;
	}
	return true;
}

void SyntacticalAnalyzer::Name()
{
	LexemaView lv;
	lt next = LookForward();
	while (next == lt::Id) {
		sc.Scan(lv);
		if (sc.Scan(lv) != lt::Dot)
			return;
		next = LookForward();
	}
}

void SyntacticalAnalyzer::Type()
{
	auto lex = sc.Scan();
	SavePtrs();
	if (!(lex == lt::Float || lex == lt::Short || lex == lt::Long || lex == lt::Int)) {
		LexExit({ "Type" });
		return;
	}
	if (lex == lt::Long) {
		if (sc.Scan() != lt::Int) {
			LexExit(lt::Int);
			RestorePtrs();
			return;
		}
	}
}

//---- class id { Programm } ;----
void SyntacticalAnalyzer::ClassSynt()
{
	SavePtrs();
	std::vector<lt> ltsBeforeProgramm = { lt::Class, lt::Id, lt::LFigBracket };
	std::vector<lt> ltsAfterProgramm = { lt::RFigBracket, lt::DotComma };
	for (auto lts : ltsBeforeProgramm) {
		if(!checkScan(lts))
			return;
	}
	Programm(lt::RFigBracket);
	for (auto lts : ltsAfterProgramm)
		if (!checkScan(lts))
			return;
}
void SyntacticalAnalyzer::Expression()
{
}

//     ---|     CLASS		|---
//     ---|     FUNCTION	|---
//     ---|     DATA		|---
//  ------------------------------
void SyntacticalAnalyzer::Programm(lt endLex)
{
	auto nextLex = LookForward();
	while (nextLex != endLex) {
		if (nextLex == lt::Class) {
			ClassSynt();
		}
		else {
			int ptr, lin, col;
			sc.GetPtrs(ptr, lin, col);
			Type();
			//получить следующую за id лексему
			nextLex = LookForward(2);
			if (nextLex == lt::LRoundBracket) {
				sc.SetPtrs(ptr, lin, col);
				Function();
			}
			else {
				sc.SetPtrs(ptr, lin, col);
				Data();
			}
		}
		nextLex = LookForward();
	}
}

void SyntacticalAnalyzer::LexExit(std::vector<std::string> waiting)
{
	sc.ErrorMsg(MSG_ID::WAIT_LEX, waiting);
}
void SyntacticalAnalyzer::LexExit(lt waitingLex)
{
	LexExit({ TypesName.find(waitingLex)->second });
}

void SyntacticalAnalyzer::DeclarateInFunction()
{
}
//---type   |--id    --|() block
//          |--main  --|
void SyntacticalAnalyzer::Function()
{
	Type();
	auto lex = sc.Scan();
	int ptr, lin, col;
	sc.GetPtrs(ptr, lin, col);
	if (lex != lt::main) {
		Name();
	}
	if (sc.Scan() != lt::LRoundBracket) {
		LexExit(lt::LRoundBracket);
		sc.SetPtrs(ptr, lin, col);
		return;
	}
	if (sc.Scan() != lt::RRoundBracket) {
		LexExit(lt::RRoundBracket);
		sc.SetPtrs(ptr, lin, col);
		return;
	}
	Block();
}
//type var |;
//         |, var...;
void SyntacticalAnalyzer::Data()
{
	Type();
	do {
		if (!checkScan(lt::Id))
			return; 
	} while (checkScan(lt::Comma, false));
	checkScan(lt::DotComma);
}

void SyntacticalAnalyzer::Variable()
{

}



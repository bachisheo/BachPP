#include <map>
#include <fstream>
#include <iostream>
#include <vector>

#ifndef DEFS
#define LetterSmall(c) c >= 'a' && c <= 'z'
#define LetterBig(c) c >= 'A' && c <= 'Z'
#define Letter(c) (LetterBig(c)) || (LetterSmall(c))
#define Digit(c) c >= '0' && c <= '9'
#define NotDigit(c) (Letter(c)) || c == '_'
#define MAX_LEX 10
#define MAX_TEXT 10000
enum class MSG_ID { LONG_LEX, WAIT_TYPE, WAIT_LEX };
//lexical terminals
enum class lt {
	While, Return, Class, Short, Long, Int, Float, main,
	ConstInt, ConstExp,
	Id,
	Dot, DotComma, Comma, LRoundBracket, RRoundBracket, LFigBracket, RFigBracket,
	Plus, Minus, Less, More, Equal,
	LogEqual, LessOrEqual, MoreOrEqual, NotEqual,
	ShiftLeft, ShiftRight, DivSign, ModSign, MultSign, Error, End
};
//syntaxis terminals
enum class st {
	//base
	TProgram, TListDeclarate, TInFuncDeclarate,
	TDeclarate, TFunc, TDate, TType,
	TVarList, TVar, TName, TClass,
	//expressions
	TExpr, TLogExpr, TShiftExpr, TAddExpr,
	TMultExpr, TElExpr, TConst,
	//operators
	TOperator, TAssign, TBlock
};
const std::vector<lt> LtTypes{ lt::Short, lt::Long, lt::Int, lt::Float };
const std::map<std::string, lt> KeyWords = {
	{"while", lt::While},
	{"return", lt::Return},
	{"class", lt::Class},
	{"short", lt::Short},
	{"long", lt::Long},
	{"int", lt::Int},
	{"float", lt::Float},
	{"main", lt::main}
};
const std::map<lt, std::string> TypesName = {
	{lt::main, "main"},
	{lt::While, "while"},
	{lt::Return, "return"},
	{lt::Class, "class"},
	{lt::Short, "short"},
	{lt::Long, "long"},
	{lt::Int, "int"},
	{lt::Float, "float"},
	{lt::ConstInt, "constint"},
	{lt::ConstExp, "constexp"},
	{lt::Id, "ID"},
	{lt::Dot, "Dot"},
	{lt::Comma, "Comma"},
	{lt::DotComma, "DotComma"},
	{lt::LRoundBracket, "LRoundBracket"},
	{lt::RRoundBracket, "RRoundBracket"},
	{lt::LFigBracket, "LFigBracket"},
	{lt::RFigBracket, "RFigBracket"},
	{lt::Plus, "Plus"},
	{lt::Minus, "Minus"},
	{lt::Less, "Less"},
	{lt::More, "More"},
	{lt::Equal, "Equal"},
	{lt::LogEqual, "LogEqual"},
	{lt::LessOrEqual, "LessOrEqual"},
	{lt::MoreOrEqual, "MoreOrEqual"},
	{lt::NotEqual, "NotEqual"},
	{lt::ShiftLeft, "ShiftLeft"},
	{lt::ShiftRight, "ShiftRight"},
	{lt::DivSign, "DivSign"},
	{lt::ModSign, "ModSign"},
	{lt::MultSign, "MultSign"},
	{lt::Error, "Error"},
	{lt::End, "End"}
};
const std::map<st, std::vector<st>> FirstSynt = {
		{st::TFunc, {st::TName}},
		{st::TVar, {st::TName}},
	{st::TDate, {st::TVar}},

};

const std::map<st, std::vector<lt>> FirstLex = {
	{st::TName, {lt::Id}},
	{st::TClass, {lt::Class }},
	{st::TFunc, {lt::main}},
	{st::TType, {lt::Float, lt::Short, lt::Long}}
};
#endif // !DEFS
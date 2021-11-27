/*
* Основные определения и константы, используемые в программе
*/
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
typedef char LexemaView[MAX_LEX + 1];

enum class MSG_ID { LONG_LEX, WAIT_TYPE, SYNT_ERR, SEM_ERR };

//lexical terminals
enum class LexType {
	While, Return, Class, Short, Long, Int, Float, main,
	ConstInt, ConstExp,
	Id,
	Dot, DotComma, Comma, LRoundBracket, RRoundBracket, LFigBracket, RFigBracket,
	Plus, Minus, Less, More, Equal,
	LogEqual, LessOrEqual, MoreOrEqual, LogNotEqual,
	ShiftLeft, ShiftRight, DivSign, ModSign, MultSign, Error, End
};

//syntaxis terminals
enum class SyntType {
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
//semantic types
enum class SemanticType{Function, Class, ClassObj, Data, Empty, ShortInt, LongInt, Float, Undefined, NotType};
const std::vector<LexType> TypeWords{ LexType::Short, LexType::Long, LexType::Int, LexType::Float, LexType::Id };

const std::map<std::string, LexType> KeyWords = {
	{"while", LexType::While},
	{"return", LexType::Return},
	{"class", LexType::Class},
	{"short", LexType::Short},
	{"long", LexType::Long},
	{"int", LexType::Int},
	{"float", LexType::Float},
	{"main", LexType::main}
};
const std::map<LexType, std::string> TypesName = {
	{LexType::main, "main"},
	{LexType::While, "while"},
	{LexType::Return, "return"},
	{LexType::Class, "class"},
	{LexType::Short, "short"},
	{LexType::Long, "long"},
	{LexType::Int, "int"},
	{LexType::Float, "float"},
	{LexType::ConstInt, "const_int"},
	{LexType::ConstExp, "const_exp"},
	{LexType::Id, "ID"},
	{LexType::Dot, "Dot"},
	{LexType::Comma, "Comma"},
	{LexType::DotComma, "DotComma"},
	{LexType::LRoundBracket, "LRoundBracket"},
	{LexType::RRoundBracket, "RRoundBracket"},
	{LexType::LFigBracket, "LFigBracket"},
	{LexType::RFigBracket, "RFigBracket"},
	{LexType::Plus, "Plus"},
	{LexType::Minus, "Minus"},
	{LexType::Less, "Less"},
	{LexType::More, "More"},
	{LexType::Equal, "Equal"},
	{LexType::LogEqual, "LogEqual"},
	{LexType::LessOrEqual, "LessOrEqual"},
	{LexType::MoreOrEqual, "MoreOrEqual"},
	{LexType::LogNotEqual, "LogNotEqual"},
	{LexType::ShiftLeft, "ShiftLeft"},
	{LexType::ShiftRight, "ShiftRight"},
	{LexType::DivSign, "DivSign"},
	{LexType::ModSign, "ModSign"},
	{LexType::MultSign, "MultSign"},
	{LexType::Error, "Error"},
	{LexType::End, "End"}
};
const std::map<SyntType, std::vector<SyntType>> FirstSynt = {
		{SyntType::TFunc, {SyntType::TName}},
		{SyntType::TVar, {SyntType::TName}},
	{SyntType::TDate, {SyntType::TVar}},

};

const std::map<SyntType, std::vector<LexType>> FirstLex = {
	{SyntType::TName, {LexType::Id}},
	{SyntType::TClass, {LexType::Class }},
	{SyntType::TFunc, {LexType::main}},
	{SyntType::TType, {LexType::Float, LexType::Short, LexType::Long}}
};
#endif // !DEFS
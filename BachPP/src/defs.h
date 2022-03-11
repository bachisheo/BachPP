/*
* Основные определения и константы, используемые в программе
*/
#pragma once
#include <map>
#include <iostream>
#include <vector>

#ifndef DEFS
constexpr bool IsLetterSmall(char c) { return c >= 'a' && c <= 'z'; }
constexpr bool IsLetterBig(char c) { return c >= 'A' && c <= 'Z'; }
constexpr bool IsLetter(char c) {	return IsLetterBig(c) || IsLetterSmall(c);}
constexpr bool IsDigit(char c) { return c >= '0' && c <= '9'; }
constexpr bool NotDigit(char c) { return IsLetter(c) || c == '_'; }
constexpr int MAX_LEX = 100;
constexpr int MAX_TEXT = 10000;
using LexemaView = std::string;

/**
 * \brief 
 */
# define MaxShort "32767"
# define MaxLong "2147483647"


enum class MSG_ID { LONG_LEX, WAIT_TYPE, SYNT_ERR, SEM_ERR };

//lexical terminals
enum class LexType {
	While, Return, Class, Short, Long, Int, Float, Void, main,
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
struct SemanticType {
	enum types { Function, Class, ClassObj, Data, Empty, ShortInt, LongInt, Float, Undefined, Void };
	types type;
	LexemaView id;
	operator types() const { return type; } 
	SemanticType(types t)
	{
		type = t;
	}
	SemanticType() { type = Undefined; }
};
const std::vector<LexType> TypeWords{ LexType::Short, LexType::Long, LexType::Int, LexType::Float, LexType::Id };

const std::map<SemanticType::types, std::string> TypesName = {
	{SemanticType::types::Class, "class"},
	{SemanticType::types::ClassObj, "classObj"},
	{SemanticType::types::Data, "data"},
	{SemanticType::types::Empty, "empty"},
	{SemanticType::types::Float, "float"},
	{SemanticType::types::Function, "function"},
	{SemanticType::types::LongInt, "long integer"},
	{SemanticType::types::ShortInt, "short integer"},
	{SemanticType::types::Undefined, "undefined"},
	{SemanticType::types::Void, "void"}
};

const std::map<std::string, LexType> KeyWords = {
	{"while", LexType::While},
	{"return", LexType::Return},
	{"class", LexType::Class},
	{"short", LexType::Short},
	{"long", LexType::Long},
	{"int", LexType::Int},
	{"float", LexType::Float},
	{"void", LexType::Void},
	{"main", LexType::main}
};

const std::map<LexType, std::string> LexTypesName = {
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
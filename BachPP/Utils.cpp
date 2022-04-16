#include "Utils.h"
#include "src/TriadGenerator.h"
/// <summary>
/// Проверить, можно ли привести тип выражения к типу используемой синтаксической конструкции
/// </summary>
bool Utils::IsComparableType(SemanticType realType, SemanticType neededType)
{
	if (realType == SemanticType::Void || neededType == SemanticType::Void)
		return false;
	if (realType == SemanticType::ClassObj)
	{
		return (neededType == SemanticType::ClassObj && realType.id == neededType.id);
	}
	return IsDataType(realType) && IsDataType(neededType);
}

bool Utils::IsDataType(SemanticType type)
{
	return (type == SemanticType::LongInt || type == SemanticType::ShortInt || type == SemanticType::Float);
}

int numberStrCmp(std::string a, std::string b)
{
	size_t dif = a.size() - b.size();
	if (dif) return dif;
	return strcmp(a.c_str(), b.c_str());
}

Operand* Utils::GetConstOperand(const LexemaView& lv, LexType lt, SemanticTree* st)
{
	auto result = new Operand();
	result->lexema = lv;
	switch (lt)
	{
	case LexType::ConstExp: {
		result->type = SemanticType::Float;
		break;
	}
	case LexType::ConstInt:
		if (numberStrCmp(lv, MaxShort) <= 0) {
			result->type = SemanticType::ShortInt;
		}
		else
			if (numberStrCmp(lv, MaxLong) <= 0) {
				result->type = SemanticType::LongInt;
			}
			else
				st->SemanticExit({ std::to_string(MAX_LEX) }, ErrorCode::TooLargeConstSize);
		break;
	default:
		st->SemanticExit({ lv }, ErrorCode::UndefinedConstType);
	}
	return result;
}

Operand* Utils::GetDataOperand(Data* data)
{
	auto o = new Operand();
	o->type = data->type;
	o->lexema = data->id;
	return o;
}


/// <summary>
///Поиск типа результата выполнения операции
/// </summary>
/// <param name="a">Тип первого операнда</param>
/// <param name="b">Тип второго операнда</param>
/// <param name="sign">Тип операции</param>
/// <returns>Тип результата выполнения операции</returns>
SemanticType Utils::GetResultType(SemanticType a, SemanticType b, LexType sign)
{
	if (a == SemanticType::Undefined || b == SemanticType::Undefined)
	{
		return SemanticType::Undefined;
	}
	if (!(IsDataType(a) && IsDataType(b)))
	{
		return SemanticType::Undefined;
	}
	if (a == b)
	{
		return a;
	}
	if (a == SemanticType::Float || b == SemanticType::Float)
	{
		return SemanticType::Float;
	}
	if (a == SemanticType::LongInt || b == SemanticType::LongInt)
	{
		return SemanticType::LongInt;
	}
	return SemanticType::ShortInt;
}

void Utils::CheckWhileExp(SemanticType type, SemanticTree * st)
{
	if (!IsComparableType(type, SemanticType::ShortInt))
	{
		st->SemanticExit({ "ShortInt" }, ErrorCode::WaitForOtherType);
	}
}

std::string Utils::NameToString(std::vector<LexemaView>& ids)
{
	std::string res = "";
	if (!ids.empty())
		res = ids[0];
	for (size_t i = 1; i < ids.size(); i++)
	{
		res += "." + ids[i];
	}
	return res;
}

SemanticType Utils::GetType(LexType type_type, LexType next_type)
{
	switch (type_type)
	{
	case LexType::Short:
		if (next_type != LexType::Int)
			return SemanticType::Undefined;
		return SemanticType::ShortInt;
	case LexType::Long:
		if (next_type != LexType::Int)
			return SemanticType::Undefined;
		return SemanticType::LongInt;
	default: return SemanticType::Undefined;
	}
}

SemanticType Utils::GetType(LexType type_type, const LexemaView& type_view, SemanticTree * st)
{
	switch (type_type)
	{
	case LexType::Void:
		return SemanticType::Void;
	case LexType::Int:
		return SemanticType::ShortInt;
	case LexType::Float:
		return SemanticType::Float;
	case LexType::Id:
	{
		auto nod = st->FindUp(type_view);
		if (nod != nullptr)
		{
			if (nod->_data->type == SemanticType::Class) {
				SemanticType type = SemanticType::ClassObj;
				type.id = type_view;
				return type;
			}
		}

		return SemanticType::Undefined;
	}
	default:
		return SemanticType::Undefined;
	}
}

std::string Utils::LexTypeToString(LexType a)
{
	switch(a)
	{
	case LexType::Plus: return "+";
	case LexType::Minus: return "-";
	case LexType::DivSign: return "/";
	case LexType::ModSign: return "%";
	case LexType::Equal : return "=";
	case LexType::LogEqual : return "==";
	case LexType::LogNotEqual : return "!=";
	case LexType::ShiftRight: return ">>";
	case LexType::ShiftLeft: return "<<";
	case LexType::MultSign: return "*";
	case LexType::Less: return "<";
	case LexType::More: return ">";
	case LexType::MoreOrEqual: return ">=";
	case LexType::LessOrEqual: return "<=";
	default: return "a gde type?";
	}
}




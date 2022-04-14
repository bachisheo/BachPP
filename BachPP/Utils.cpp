#include "Utils.h"
/// <summary>
/// Проверить, можно ли привести тип выражения к типу используемой синтаксической конструкции
/// </summary>
bool Utils::IsComparableType(SemanticType realType, SemanticType neededType) const
{
	if (realType == SemanticType::Void || neededType == SemanticType::Void)
		return false;
	if (realType == SemanticType::ClassObj)
	{
		return (neededType == SemanticType::ClassObj && realType.id == neededType.id);
	}
	return IsDataType(realType) && IsDataType(neededType);
}

bool Utils::IsDataType(SemanticType type) const
{
	return (type == SemanticType::LongInt || type == SemanticType::ShortInt || type == SemanticType::Float);
}

int numberStrCmp(std::string a, std::string b)
{
	size_t dif = a.size() - b.size();
	if (dif) return dif;
	return strcmp(a.c_str(), b.c_str());
}

Data* Utils::GetConstData(const LexemaView& lv, LexType lt, SemanticTree st) const
{
	Data* result = new Data();
	switch (lt)
	{
	case LexType::ConstExp: {
		result->type = SemanticType::Float;
		result->value.float_value = atof(lv.c_str());
		break;
	}
	case LexType::ConstInt:
		if (numberStrCmp(lv, MaxShort) <= 0) {
			result->type = SemanticType::ShortInt;
			result->value.short_int_value = atoi(lv.data());
		}
		else
			if (numberStrCmp(lv, MaxLong) <= 0) {
				result->type = SemanticType::LongInt;
				result->value.short_int_value = atoi(lv.data());
			}
			else
				st.SemanticExit({ std::to_string(MAX_LEX) }, ErrorCode::TooLargeConstSize);
		break;
	default:
		st.SemanticExit({ lv }, ErrorCode::UndefinedConstType);

	}
	return result;
}


/// <summary>
///Поиск типа результата выполнения операции
/// </summary>
/// <param name="a">Тип первого операнда</param>
/// <param name="b">Тип второго операнда</param>
/// <param name="sign">Тип операции</param>
/// <returns>Тип результата выполнения операции</returns>
SemanticType Utils::GetResultType(SemanticType a, SemanticType b, LexType sign) const
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

void Utils::CheckWhileExp(Data* data, SemanticTree st) const
{
	if (!IsComparableType(data->type, SemanticType::ShortInt))
	{
		st.SemanticExit({ "ShortInt" }, ErrorCode::WaitForOtherType);
	}
}




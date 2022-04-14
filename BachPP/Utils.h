#pragma once
#include "src/defs.h"
#include "src/SemanticTree.h"

static class Utils
{
public:
	static bool IsComparableType(SemanticType realType, SemanticType neededType);
	static bool IsDataType(SemanticType type);
	static Data* GetConstData(const LexemaView& lv, LexType lt, SemanticTree * st);
	static SemanticType GetResultType(SemanticType a, SemanticType b, LexType sign);
	static void CheckWhileExp(Data* data, SemanticTree * st);
	static std::string NameToString(std::vector<LexemaView>& ids);
	static SemanticType GetType(LexType type_type, LexType next_type);
	static SemanticType GetType(LexType type_type, const LexemaView& type_view, SemanticTree * st);
};


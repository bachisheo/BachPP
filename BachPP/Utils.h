#pragma once
#include "src/defs.h"
#include "src/SemanticTree.h"

static class Utils
{
public:
	bool IsComparableType(SemanticType realType, SemanticType neededType) const;
	bool IsDataType(SemanticType type) const;
	Data* GetConstData(const LexemaView& lv, LexType lt, SemanticTree st) const;
	SemanticType GetResultType(SemanticType a, SemanticType b, LexType sign) const;
	void CheckWhileExp(Data* data, SemanticTree st) const;

};


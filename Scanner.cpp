#include "Scanner.h"

#pragma warning(disable: 4996)
LexType Scanner::ScanNumber(LexemaView & lex, int& len) {
	for (; Digit(t[ptr]); ++ptr, ++len)
		if (len < MAX_LEX) {
			lex[len] = t[ptr];
			++col;
		}

	if (len > MAX_LEX) ErrorMsg(MSG_ID::LONG_LEX, line, col, {});
	return LexType::ConstInt;
}

void Scanner::SkipIgnored() {
	//SKIP IGNORE
	while (true) {
		switch (t[ptr]) {
		case '\n':
			ptr++;
			line++;
			col = 1;
			break;
		case ' ': case '\t':
			ptr++;
			col++;
			break;
		case '/':
			ptr++; col++;
			switch (t[ptr]) {
			case '/':
				while (t[++ptr] != '\n' && t[ptr] != '\0') {
					col++;
				}
				break;
			case '*': {
				bool isCom = true;
				while (isCom) {
					ptr++; col++;
					if (t[ptr] == '\0')
						return;
					if (t[ptr] == '\n') {
						col = 1;
						line++;
					}
					if (t[ptr] == '*') {
						if (t[ptr + 1] == '/') {
							isCom = false;
							ptr += 2; col += 2;
						}
					}
				}
				break;
			}
			default:
				col--; ptr--;
				return;
			}
			break;
		default: return;
		}
	}
}
static std::string cut(std::string & s)
{
	for (size_t i = 0; i < s.size(); i++)
	{
		if (s[i] == '\0') {
			s.resize(i);
			break;
		}
	}
	return s;
}
LexType Scanner::Scan(LexemaView & lex)
{
	lex.resize(MAX_LEX + 1);
	const auto type = _Scan(lex);
	cut(lex);
	return type;
}
LexType Scanner::_Scan(LexemaView & lex)
{
	SkipIgnored();

	int len = 0;
	lexBeginCol = col;
	lexBeginLine = line;
	//ID AND KEYWORDS
	if (NotDigit(t[ptr])) {
		for (; Digit(t[ptr]) || NotDigit(t[ptr]); ++ptr, ++len)
			if (len < MAX_LEX) {
				lex[len] = t[ptr];
				++col;
			}
		if (len > MAX_LEX) ErrorMsg(MSG_ID::LONG_LEX, line, col, {});
		cut(lex);
		auto keyWordIt = KeyWords.find(lex);
		if (keyWordIt == KeyWords.end())
			return LexType::Id;
		else return keyWordIt->second;
	}
	//CONSTS
	if (Digit(t[ptr])) {
		ScanNumber(lex, len);
		//NUMBER CONST
		if (t[ptr] != '.' && t[ptr] != 'e' && t[ptr] != 'E')
			return LexType::ConstInt;
		//EXPCONST
		if (t[ptr] == '.') {
			lex[len++] = t[ptr++];
			col++;
			ScanNumber(lex, len);
		}
		if (t[ptr] != 'e' && t[ptr] != 'E') {
			ErrorMsg(MSG_ID::WAIT_TYPE, line, col, { "e", "E" });
			return LexType::Error;
		}
		lex[len++] = t[ptr++];
		if (t[ptr] != '+' && t[ptr] != '-' && !(Digit(t[ptr]))) {
			ErrorMsg(MSG_ID::WAIT_TYPE, line, col, { "+", "-", "число" });
			return LexType::Error;
		}
		if (t[ptr] == '+' || t[ptr] == '-') {
			lex[len++] = t[ptr++];
			col++;
		}
		if (!(Digit(t[ptr]))) {
			ErrorMsg(MSG_ID::WAIT_TYPE, line, col, { "число" });
			return LexType::Error;
		}
		ScanNumber(lex, len);
		return LexType::ConstExp;
	}
	col++;
	lex[len] = t[ptr];
	switch (t[ptr++]) {
	case '<':
		if (t[ptr] == '<') {
			lex[++len] = t[ptr];
			ptr++; col++;
			return LexType::ShiftLeft;
		}
		if (t[ptr] == '=') {
			lex[++len] = t[ptr];
			ptr++; col++;
			return LexType::LessOrEqual;
		}
		return LexType::Less;
	case '>':
		if (t[ptr] == '>') {
			lex[++len] = t[ptr];
			ptr++; col++;
			return LexType::ShiftRight;
		}
		if (t[ptr] == '=') {
			lex[++len] = t[ptr];
			ptr++; col++;
			return LexType::MoreOrEqual;
		}
		return LexType::More;
	case '!':
		if (t[ptr] == '=') {
			lex[++len] = t[ptr];
			ptr++; col++;
			return LexType::LogNotEqual;
		}
		return LexType::Error;
	case '=':
		if (t[ptr] == '=') {
			lex[++len] = t[ptr];
			ptr++; col++;
			return LexType::LogEqual;
		}
		return LexType::Equal;
	case '+': return LexType::Plus;
	case '-': return LexType::Minus;
	case '(': return LexType::LRoundBracket;
	case ')': return LexType::RRoundBracket;
	case '{': return LexType::LFigBracket;
	case '}': return LexType::RFigBracket;
	case '.': return LexType::Dot;
	case ',': return LexType::Comma;
	case ';': return LexType::DotComma;
	case '*': return LexType::MultSign;
	case '/': return LexType::DivSign;
	case '%': return LexType::ModSign;
	case '\0':return LexType::End;
	default: return LexType::Error;
	}
}

void Scanner::ScanAll()
{
	LexType res;
	do {
		LexemaView lex;
		res = Scan(lex);
		std::cout << TypesName.find(res)->second << " --> " << lex << "\n";
	} while (res != LexType::End);
}

Scanner::Scanner(const char* name) : line(1), col(1), ptr(0), size(0), lexBeginCol(0), lexBeginLine(0)
{
	fopen_s(&fin, name, "r");
	t = new char[MAX_TEXT];
	while (!feof(fin))
		fscanf_s(fin, "%c", &t[size++]);
	size--;
	t[size] = '\0';
	fclose(fin);
}

void Scanner::ErrorMsg(MSG_ID id, const std::vector<std::string>& params, bool isBegin) {
	if (isBegin)
		ErrorMsg(id, this->lexBeginLine, this->lexBeginCol, params);
	else
		ErrorMsg(id, this->line, this->col, params);
}
void Scanner::ErrorMsg(MSG_ID id, int str, int col, const std::vector<std::string>& params)
{
	switch (id) {
	case MSG_ID::LONG_LEX: std::cout << "\nЛексема больше 10 символов!"; break;
	case MSG_ID::WAIT_TYPE:
		std::cout << "\nДля данного типа лексемы ожидается: \"";
		for (auto next : params)
			std::cout << "\'" << next << "\',  ";
		break;
	case MSG_ID::SYNT_ERR:
		std::cout << "\nДля данного синтаксиса ожидается: \"";
		for (auto next : params)
			std::cout << "\'" << next << "\',  ";
		break;
	case MSG_ID::SEM_ERR:
		std::cout << "\nСемантическая ошибка: \"";
		for (auto next : params)
			std::cout << " " << next;
		break;
	}
	std::cout << "\" Строка " << str << " символ " << col << std::endl;
}

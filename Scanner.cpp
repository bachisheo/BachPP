#include "Scanner.h"
#pragma warning(disable: 4996)
lt Scanner::ScanNumber(LexemaView lex, int& len) {
	for (; Digit(t[ptr]); ++ptr, ++len)
		if (len < MAX_LEX) {
			lex[len] = t[ptr];
			++col;
		}
	if (len > MAX_LEX) ErrorMsg(MSG_ID::LONG_LEX, line, col, {});
	return lt::ConstInt;
}

void Scanner::SkipIgnored() {
	//SKIP IGNORE
	while (true) {
		switch (t[ptr]) {
		case '\n':
			ptr++;
			line++;
			col = 0;
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
				line++;
				break;
			case '*': {
				bool isCom = true;
				while (isCom) {
					ptr++; col++;
					if (t[ptr] == '\0')
						return;
					if (t[ptr] == '\n'){
						col = 0;
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
lt Scanner::Scan(LexemaView lex)
{
	SkipIgnored();
	for (size_t i = 0; i <= MAX_LEX; i++)
		lex[i] = 0;
	int len = 0;

	//ID AND KEYWORDS
	if (NotDigit(t[ptr])) {
		for (; Digit(t[ptr]) || NotDigit(t[ptr]); ++ptr, ++len)
			if (len < MAX_LEX) {
				lex[len] = t[ptr];
				++col;
			}
		if (len > MAX_LEX) ErrorMsg(MSG_ID::LONG_LEX, line, col, {});
		auto keyWordIt = KeyWords.find(lex);
		if (keyWordIt == KeyWords.end())
			return lt::Id;
		else return keyWordIt->second;
	}
	//CONSTS
	if (Digit(t[ptr])) {
		ScanNumber(lex, len);
		//NUMBER CONST
		if (t[ptr] != '.' && t[ptr] != 'e' && t[ptr] != 'E')
			return lt::ConstInt;
		//EXPCONST
		if (t[ptr] == '.') {
			lex[len++] = t[ptr++];
			col++;
			ScanNumber(lex, len);
		}
		if (t[ptr] != 'e' && t[ptr] != 'E') {
			ErrorMsg(MSG_ID::WAIT_TYPE, line, col, { "e", "E" });
			return lt::Error;
		}
		lex[len++] = t[ptr++];
		if (t[ptr] != '+' && t[ptr] != '-' && !(Digit(t[ptr]))) {
			ErrorMsg(MSG_ID::WAIT_TYPE, line, col, { "+", "-", "число" });
			return lt::Error;
		}
		if (t[ptr] == '+' || t[ptr] == '-') {
			lex[len++] = t[ptr++];
			col++;
		}
		if (!(Digit(t[ptr]))) {
			ErrorMsg(MSG_ID::WAIT_TYPE, line, col, { "число" });
			return lt::Error;
		}
		ScanNumber(lex, len);
		return lt::ConstExp;
	}
	col++;
	lex[len] = t[ptr];
	switch (t[ptr++]) {
	case '<':
		if (t[ptr] == '<') {
			lex[++len] = t[ptr];
			ptr++, col++;
			return lt::ShiftLeft;
		};
		if (t[ptr] == '=') {
			lex[++len] = t[ptr];
			ptr++, col++;
			return lt::LessOrEqual;
		};
		return lt::Less;
	case '>':
		if (t[ptr] == '>') {
			lex[++len] = t[ptr];
			ptr++, col++;
			return lt::ShiftRight;
		};
		if (t[ptr] == '=') {
			lex[++len] = t[ptr];
			ptr++, col++;
			return lt::MoreOrEqual;
		};
		return lt::More;
	case '!':
		if (t[ptr] == '=') {
			lex[++len] = t[ptr];
			ptr++, col++;
			return lt::NotEqual;
		};
		return lt::Error;
	case '=':
		if (t[ptr] == '=') {
			lex[++len] = t[ptr];
			ptr++; col++;
			return lt::LogEqual;
		}
		return lt::Equal;
	case '+': return lt::Plus;
	case '-': return lt::Minus;
	case '(': return lt::LRoundBracket;
	case ')': return lt::RRoundBracket;
	case '{': return lt::LFigBracket;
	case '}': return lt::RFigBracket;
	case '.': return lt::Dot;
	case ',': return lt::Comma;
	case ';': return lt::DotComma;
	case '*': return lt::MultSign;
	case '/': return lt::DivSign;
	case '%': return lt::ModSign;
	case '\0':return lt::End;
	default: return lt::Error;
	}
}

void Scanner::ScanAll()
{
	LexemaView lex;
	lt res;
	do {
		res = Scan(lex);
		std::cout << TypesName.find(res)->second << " --> " << lex << "\n";
	} while (res != lt::End);
}

Scanner::Scanner(const char* name) :line(0), col(0), size(0)
{
	fin = fopen(name, "r");
	t = new char[MAX_TEXT];
	while (!feof(fin))
		fscanf(fin, "%c", &t[size++]);
	size--;
	t[size] = '\0';
	fclose(fin);
}

void Scanner::ErrorMsg(MSG_ID id, std::vector<std::string> params) {
	ErrorMsg(id, this->line, this->col, params);
}
void Scanner::ErrorMsg(MSG_ID id, int str, int col, std::vector<std::string> params)
{
	switch (id) {
	case MSG_ID::LONG_LEX: std::cout << "\nЛексема больше 10 символов!"; break;
	case MSG_ID::WAIT_TYPE:
		std::cout << "\nДля данного типа лексемы ожидаается ";
		for (auto next : params)
			std::cout << "\'" << next << "\',  ";
		break;
	case MSG_ID::WAIT_LEX:
		std::cout << "\nДля данного синтаксиса ожидаается ";
		for (auto next : params)
			std::cout << "\'" << next << "\',  ";
		break;
	}
	std::cout << " Строка " << str << " символ " << col << std::endl;
}

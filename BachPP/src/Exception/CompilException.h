#include <exception>
#include <string>
#include <vector>

enum class ErrorCode
{
	TooLargeConstSize, UndefinedConstType, WaitForOtherType, WrongSyntax, Undefined, DivisionByZero, ObjectNotFound, NotUniqueId,
	WrongTypeConversion, WrongOperation
};
class CompilException : public std::exception {

public:

	CompilException(ErrorCode code, const std::vector<std::string>& params, int line, int col) :
		_line(line),
		_col(col),
		code(code)
	{
		CreateMessage(params);
	}
	ErrorCode code;


private:
	int _line, _col;;
	std::string _msg;
	void CreateMessage(const std::vector<std::string>& params)
	{
		_msg = "\"";
		switch (code) {
		case ErrorCode::TooLargeConstSize:
			_msg += "\nЛексема больше ";
			if (params.empty())
				_msg += "максимально допустимого количества";
			else _msg += params[0];
			_msg += " символов!";
			break;
		case ErrorCode::WaitForOtherType:
			_msg += "\nОжидается тип: \"";
			for (auto next : params)
				_msg += "\'" + next + "\',  ";
			break;
		case ErrorCode::WrongSyntax:
			_msg += "\nДля данного синтаксиса ожидается: \"";
			for (auto next : params)
				_msg += "\'" + next + "\',  ";
			break;
		case ErrorCode::DivisionByZero:
			_msg += "Деление на ноль!";
			break;
		case ErrorCode::UndefinedConstType:
			_msg += "Тип константы ";
			if (!params.empty())
				_msg += "\"" + params[0] + "\"";
			_msg += " не определен";
		case ErrorCode::ObjectNotFound:
			_msg += "Идентификатор ";
			if (!params.empty())
				_msg += "\"" + params[0] + "\"";
			_msg += " не определён";
			break;
		case ErrorCode::NotUniqueId:
			_msg += "Идентификатор ";
			if (!params.empty())
				_msg += "\"" + params[0] + "\"";
			_msg += " не уникален";
			break;
		case ErrorCode::WrongTypeConversion:
			_msg += " Недопустимое приведение типов: ";
			for (auto next : params)
				_msg += "\'" + next + "\',  ";
			break;
			/*case ErrorCode::SEM_ERR:
				std::cout << "\nСемантическая ошибка: \"";
				for (auto next : params)
					std::cout << " " << next;
				break;*/
		}
		_msg += "\" Строка " + std::to_string(_line);
		_msg += " символ " + std::to_string(_col);
	}

public:
	char const* what() const override
	{
		return _msg.c_str();
	};
};

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
			_msg += "\n������� ������ ";
			if (params.empty())
				_msg += "����������� ����������� ����������";
			else _msg += params[0];
			_msg += " ��������!";
			break;
		case ErrorCode::WaitForOtherType:
			_msg += "\n��������� ���: \"";
			for (auto next : params)
				_msg += "\'" + next + "\',  ";
			break;
		case ErrorCode::WrongSyntax:
			_msg += "\n��� ������� ���������� ���������: \"";
			for (auto next : params)
				_msg += "\'" + next + "\',  ";
			break;
		case ErrorCode::DivisionByZero:
			_msg += "������� �� ����!";
			break;
		case ErrorCode::UndefinedConstType:
			_msg += "��� ��������� ";
			if (!params.empty())
				_msg += "\"" + params[0] + "\"";
			_msg += " �� ���������";
		case ErrorCode::ObjectNotFound:
			_msg += "������������� ";
			if (!params.empty())
				_msg += "\"" + params[0] + "\"";
			_msg += " �� ��������";
			break;
		case ErrorCode::NotUniqueId:
			_msg += "������������� ";
			if (!params.empty())
				_msg += "\"" + params[0] + "\"";
			_msg += " �� ��������";
			break;
		case ErrorCode::WrongTypeConversion:
			_msg += " ������������ ���������� �����: ";
			for (auto next : params)
				_msg += "\'" + next + "\',  ";
			break;
			/*case ErrorCode::SEM_ERR:
				std::cout << "\n������������� ������: \"";
				for (auto next : params)
					std::cout << " " << next;
				break;*/
		}
		_msg += "\" ������ " + std::to_string(_line);
		_msg += " ������ " + std::to_string(_col);
	}

public:
	char const* what() const override
	{
		return _msg.c_str();
	};
};

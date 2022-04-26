#pragma once
#include <string>
#include <deque>
#include <queue>
#include <stack>

class Token
{
public:

	enum class Type
	{
		NONE,
		NUMBER,
		OPERATOR,
		FUNCTION,
		LBRACKET,
		RBRACKET,
		VARIABLE
	};

	Token(Type type, std::string& string, int prec=-1, bool la=true);

	const Type type;
	const std::string str;
	const int prec;
	bool leftAssociative;
};


std::deque<Token> ExprToTokens(std::string& Expression);
std::queue<Token> ShuntingYard(std::deque<Token>& Expression);
float EvaluateExpression(std::queue<Token> RPNExpression, float x, float y);
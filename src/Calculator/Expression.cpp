#include "Expression.h"
#include <iostream>

Token::Token(Type type, std::string& string, int prec, bool la)
	:type(type),str(string),prec(prec),leftAssociative(la)
{}

std::deque<Token> ExprToTokens(std::string& expr)
{
	std::deque<Token> tokens;
    for (const char* p = expr.c_str(); *p; ++p) {
		if (isblank(*p)) continue;
		else if (isdigit(*p))
		{
			const char* b = p;
			while (isdigit(*p)) p++;
			std::string s = std::string(b, p);
			tokens.push_back({ Token::Type::NUMBER, s });
			p--;
		}
		else if(*p == ')' || *p == '(')
		{
			std::string s = std::string(p, p + 1);
			if (*p == '(') tokens.push_back({ Token::Type::LBRACKET, s });
			else if (*p == ')') tokens.push_back({ Token::Type::RBRACKET, s });
		}
		else
		{
			Token::Type type = Token::Type::FUNCTION;
			int prec = -1;
			bool la = true;
			std::string s = std::string(1,*p);

			if (*p == '+') { type = Token::Type::OPERATOR; prec = 2; }
			else if (*p == '-') { type = Token::Type::OPERATOR; prec = 2; }
			else if (*p == '*') { type = Token::Type::OPERATOR; prec = 3; }
			else if (*p == '/') { type = Token::Type::OPERATOR; prec = 3; }
			else if (*p == '^') { type = Token::Type::OPERATOR; prec = 4; la = false; }
			else if (*p == 'x') { type = Token::Type::VARIABLE; prec = 4; }
			else if (*p == 'y') { type = Token::Type::VARIABLE; prec = 4; }
			else
			{
				const char* b = p;
				while (!isblank(*p) && !isdigit(*p) && *p != ')' && *p != '(') p++;
				s = std::string(b, p);
				p--;
			}

			tokens.push_back({ type, s, prec, la });
		}
    }

	return tokens;
}

std::queue<Token> ShuntingYard(std::deque<Token>& expr)
{
	std::queue<Token> output;
	std::stack<Token> operatorStack;

	for (Token token : expr)
	{
		switch (token.type)
		{
		case Token::Type::NUMBER:
		case Token::Type::VARIABLE:
			output.push(token);
			break;
		case Token::Type::FUNCTION:
			operatorStack.push(token);
			break;
		case Token::Type::OPERATOR:
		{
			while (!operatorStack.empty())
			{
				Token o1 = token;
				Token o2 = operatorStack.top();
				if (o2.type == Token::Type::OPERATOR && (o2.prec > o1.prec || (o2.prec == o1.prec && o1.leftAssociative)))
				{
					output.push(o2);
					operatorStack.pop();
				}
				else break;
			}
			operatorStack.push(token);
			break;
		}
		case Token::Type::LBRACKET:
			operatorStack.push(token);
			break;
		case Token::Type::RBRACKET:
			while (operatorStack.top().type != Token::Type::LBRACKET)
			{
				if (operatorStack.empty()) std::cout << "HELP";
				output.push(operatorStack.top());
				operatorStack.pop();
			}
			operatorStack.pop();
			if (!operatorStack.empty() && operatorStack.top().type == Token::Type::FUNCTION)
			{
				output.push(operatorStack.top());
				operatorStack.pop();
			}
			break;
		}
	}

	while (!operatorStack.empty())
	{
		output.push(operatorStack.top());
		operatorStack.pop();
	}

	return output;
}

float EvaluateExpression(std::queue<Token> rpn, float x, float y)
{
	std::stack<float> stack;
	while (!rpn.empty())
	{
		switch (rpn.front().type)
		{
		case Token::Type::VARIABLE:
		{
			std::string var = rpn.front().str;
			rpn.pop();
			if (var == "x") stack.push(x);
			else if (var == "y") stack.push(y);
			break;
		}
		case Token::Type::NUMBER:
		{
			stack.push(std::stof(rpn.front().str));
			rpn.pop();
			break;
		}
		case Token::Type::OPERATOR:
		{
			float num1 = stack.top();
			stack.pop();
			float num2 = stack.top();
			stack.pop();
			std::string op = rpn.front().str;
			rpn.pop();
			float result;

			if (op == "+") result = num1 + num2;
			else if (op == "-") result = num1 - num2;
			else if (op == "*") result = num1 * num2;
			else if (op == "/") result = num1 / num2;
			else if (op == "^") result = pow(num2, num1);
			else exit(-1);

			stack.push(result);
			break;
		}
		case Token::Type::FUNCTION:
		{
			float num = stack.top();
			stack.pop();
			std::string func = rpn.front().str;
			rpn.pop();

			float result;
			if (func == "sin") result = sin(num);
			else if (func == "cos") result = cos(num);
			else if (func == "tan") result = tan(num);
			else if (func == "sqrt") result = sqrt(num);
			else if (func == "abs") result = abs(num);
			else exit(-1);

			stack.push(result);
			break;
		}
		}
	}

	float result = stack.top();
	return result;

}
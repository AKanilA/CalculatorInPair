#include "calculator.h"

Calculator::Token::Token() {
  type_ = TokenType::kUnknown;
}

Calculator::Token::Token(const std::string& operation,
                         const bool is_unary_minus = false) {
  value_ = 0;
  if (operation == "(") {
    type_ = TokenType::kOpenBracket;
  } else if (operation == ")") {
    type_ = TokenType::kCloseBracket;
  } else if (operation == "+") {
    type_ = TokenType::kPlus;
  } else if (operation == "*") {
    type_ = TokenType::kMultiple;
  } else if (operation == "/") {
    type_ = TokenType::kDivide;
  } else if (operation == "%") {
    type_ = TokenType::kModule;
  } else if (operation == "abs") {
    type_ = TokenType::kAbsolut;
  } else if (operation == "-") {
    if (is_unary_minus) {
      type_ = TokenType::kUnaryMinus;
    } else {
      type_ = TokenType::kMinus;
    }
  } else {
    type_ = TokenType::kConstant;
    value_ = std::stoi(operation);
  }
}

int Calculator::Priority(const Token& token) {
  switch (token.type_) {
    case TokenType::kAbsolut: { return 4; }
    case TokenType::kUnaryMinus: { return 3; }
    case TokenType::kMultiple:
    case TokenType::kDivide:
    case TokenType::kModule: { return 2; }
    case TokenType::kPlus:
    case TokenType::kMinus: { return 1; }
    case TokenType::kConstant:
    case TokenType::kUnknown:
    case TokenType::kOpenBracket:
    case TokenType::kCloseBracket: { return -1; }
  }
  return -1;
}

bool Calculator::IsOperation(const char chr) {
  return (chr == '+' || chr == '-' || chr == '*' || chr == '/' || chr == '%');
}

void Calculator::StringToTokens(const std::string& expression) {
  int pos = 0;
  bool nextMayBeUnary = true;
  while (pos < expression.length()) {
    std::string current;
    bool isUnaryMinus = false;
    if (expression.at(pos) == ' ') {
      pos++;
      continue;
    }
    if (std::isdigit(expression.at(pos))) {
      while (pos < expression.length() && std::isdigit(expression.at(pos))) {
        current += expression.at(pos);
        pos++;
      }
      nextMayBeUnary = false;
    } else if (expression.at(pos) == 'a') {
      current = "abs";
      pos += 3;
    } else {
      current = expression.at(pos);
      if (current == "-") {
        if (nextMayBeUnary) {
          isUnaryMinus = true;
        }
      }
      if (Calculator::IsOperation(expression.at(pos))
          || expression.at(pos) == '(') {
        nextMayBeUnary = true;
      }
      pos++;
    }
    Token token(current, isUnaryMinus);
    tokens_.push_back(token);
  }
}

void Calculator::ReversePolishNotation() {
  int input_pos = 0;
  int outputPos = 0;
  std::stack<Token> operationsStack;

  while (input_pos < tokens_.size()) {
    Token currToken = tokens_.at(input_pos);
    input_pos++;
    if (currToken.type_ == TokenType::kConstant) {
      tokens_.at(outputPos) = currToken;
      outputPos++;
    } else if (currToken.type_ == TokenType::kUnaryMinus
        || currToken.type_ == TokenType::kOpenBracket) {
      operationsStack.push(currToken);
    } else if (currToken.type_ == TokenType::kCloseBracket) {
      while (operationsStack.top().type_ != TokenType::kOpenBracket) {
        tokens_.at(outputPos) = operationsStack.top();
        outputPos++;
        operationsStack.pop();
      }
      operationsStack.pop();
    } else {
      while (!operationsStack.empty()
          && Priority(operationsStack.top()) >= Priority(currToken)) {
        tokens_.at(outputPos) = operationsStack.top();
        outputPos++;
        operationsStack.pop();
      }
      operationsStack.push(currToken);
    }
  }
  while (!operationsStack.empty()) {
    tokens_.at(outputPos) = operationsStack.top();
    outputPos++;
    operationsStack.pop();
  }
  tokens_.resize(outputPos);
}

int Calculator::Calculate(const std::string& expression) {
  Calculator stack_machine;
  stack_machine.StringToTokens(expression);

  stack_machine.ReversePolishNotation();

  std::stack<int> valuesStack;

  for (Token currToken : stack_machine.tokens_) {
    if (currToken.type_ == TokenType::kConstant) {  // TokenType::kConstant
      valuesStack.push(currToken.value_);
    } else if (Priority(currToken) >= 3) {  // unary operations
      int val = valuesStack.top();
      valuesStack.pop();

      switch (currToken.type_) {
        case TokenType::kAbsolut: {
          valuesStack.push(std::abs(val));
          break;
        }
        case TokenType::kUnaryMinus: {
          valuesStack.push(-1 * val);
          break;
        }
        default: { throw "Invalid unary operation\n"; }
      }
    } else {  
      int val2 = valuesStack.top();
      valuesStack.pop();
      int val1 = valuesStack.top();
      valuesStack.pop();

      switch (currToken.type_) {
        case TokenType::kPlus : {
          valuesStack.push(val1 + val2);
          break;
        }
        case TokenType::kMinus: {
          valuesStack.push(val1 - val2);
          break;
        }
        case TokenType::kMultiple : {
          valuesStack.push(val1 * val2);
          break;
        }
        case TokenType::kModule : {
          valuesStack.push(val1 % val2);
          break;
        }
        case TokenType::kDivide : {
          valuesStack.push(val1 / val2);
          break;
        }
        default: { throw "Invalid binary operation\n"; }
      }
    }
  }
  stack_machine.tokens_.clear();
  return valuesStack.top();
}

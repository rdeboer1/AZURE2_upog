#ifndef EQUATION_H
#define EQUATION_H

#include <string>
#include <vector>

///A class for parsing algebraic expressions

/*!
 * The Equation class is used in AZURE to parametrize stopping cross sections
 * as a function of energy.  An arbitrary equation can be given, and parameters
 * specified, which is then parsed and evaluated on the fly whenever
 * stopping cross section is needed.
 */

class Equation {
  enum TokenType {NUMBER,OPERATOR,VARIABLE,PARAMETER,LEFTPAR,RIGHTPAR,FUNCTION,BADTOKEN};
  enum OperatorType {ADD=0,SUBTRACT=0,MULT=1,DIVIDE=1,POWER=2,BADTYPE=10};
  typedef std::pair<TokenType,std::string> TokenPair;
 public:
  Equation();
  Equation(std::string equation, int numParams);
  Equation(std::string equation,std::vector<double> parameters);
  Equation(std::string equation,double parameters[],size_t arraySize);
  void Initialize(std::string equation, int numParams);
  void Parse();
  void SetParameter(int index, double value);
  std::vector<double> GetParameters() const;
  bool IsOperator(char) const;
  bool IsDigit(char) const;
  TokenPair GetToken(int &position) const;
  OperatorType GetOperatorType(char) const;
  std::string BinaryOperation(double left, double right, char op) const;
  double FunctionOperation(TokenPair token, double x) const;
  double GetTokenValue(TokenPair token, double x) const;
  double Evaluate(double x) const;
 private:
  std::string infixEquation_;
  std::vector<TokenPair> output_;
  std::vector<double> parameters_;
};

#endif

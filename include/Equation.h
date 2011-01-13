#ifndef EQUATION_H
#define EQUATION_H

#include <string>
#include <vector>
#include <exception>
#include <sstream>
#include <map>

///A wrapper class for function pointers used by Equation class

/*!
 * The GenericFunction class is just a wrapper for a function pointer to a function
 * of the form double function(double).
 */

class GenericFunction {
 public:
  GenericFunction() {};
  GenericFunction(double (*function)(double)) :
   function_(function) {};
  double Evaluate(double value) const {
    return (*function_)(value);
  };
 private:
  double (*function_)(double);
};

///A class for parsing algebraic expressions

/*!
 * The Equation class is used in AZURE to parametrize stopping cross sections
 * as a function of energy.  An arbitrary equation can be given, and parameters
 * specified, which is then parsed and evaluated on the fly whenever
 * stopping cross section is needed.
 */

class Equation {
 public:
  Equation();
  Equation(std::string equation, int numParams);
  Equation(std::string equation,std::vector<double> parameters);
  Equation(std::string equation,double parameters[],size_t arraySize);
  void Initialize(std::string equation, int numParams);
  void SetParameter(unsigned int index, double value);
  std::vector<double> GetParameters() const;
  double Evaluate(double x=0.0) const;
 private:
  enum Associativity {LEFT,RIGHT};
  enum TokenType {NUMBER=1,OPERATOR=2,VARIABLE=4,PARAMETER=8,LEFTPAR=16,RIGHTPAR=32,FUNCTION=64};
  enum OperatorType {ADD=0,SUBTRACT=0,MULT=1,DIVIDE=1,POWER=2,BADTYPE=10};
  typedef std::pair<TokenType,std::string> TokenPair;
  void BuildFunctionList();
  void Parse();
  bool IsOperator(char) const;
  bool IsDigit(char) const;
  unsigned int FindFunction(unsigned int &position);
  TokenPair GetToken(unsigned int &position);
  OperatorType GetOperatorType(char) const;
  Associativity GetOperatorAssociativity(char) const;
  std::string BinaryOperation(double left, double right, char op) const;
  double FunctionOperation(TokenPair token, double x) const;
  double GetTokenValue(TokenPair token, double x) const;
  std::string infixEquation_;
  std::vector<TokenPair> output_;
  std::vector<double> parameters_;
  std::vector<Equation> subEquations_;
  std::map<std::string,GenericFunction> functionList_;
};

class SyntaxError : public std::exception {
 public:
  SyntaxError(std::string equation, int type, int position=-1) {
    std::string typeMessage = GetTypeMessage(type);
    std::ostringstream stm;
    stm << "Syntax Error in " << equation <<  ':' <<  typeMessage << std::endl;
    if(position!=-1) { 
      stm << "                "; for(int i=0;i<position;i++) stm << ' '; stm << '^';
    }
    messageString_=stm.str();
    message_=messageString_.c_str();
  };
  ~SyntaxError() throw() {
    delete[] message_;
  };
  virtual const char* what() const throw() {
    return message_;
  };
 private:
  std::string GetTypeMessage(int type) {
    switch(type) {
      case 0: return " Unknown token type.";
      case 1: return " Parameter out of range.";
      case 2: return " Mismatched Parentheses.";
      case 3: return " Unexpected token.";
      default: return " Unknown error.";
    }
  };
  std::string messageString_;
  const char* message_;
};


#endif

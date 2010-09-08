#include "Equation.h"
#include <iostream>
#include <sstream>
#include <cmath>

/*!
 * Empty constructor.
 */

Equation::Equation() {
}

/*!
 * This constructor is used to create an Equation object with a equation 
 * string, and a specified number of parameters.  The parameters are only 
 * initialized, and must be set with the Equation::SetParameter() function.
 */

Equation::Equation(std::string equation, int numParams) : infixEquation_(equation) {
  for(int i=0;i<numParams;i++) {
    double tempDouble=0.0;
    parameters_.push_back(tempDouble);
  }
  Parse();
}

/*!
 * This constructor is used to create and Equation object with an equation
 * string and an already created parameter vector.  The size of the parameter 
 * vector must correspond to the number of parameters in the equation
 * string.
 */

Equation::Equation(std::string equation,std::vector<double> parameters) : 
  infixEquation_(equation), parameters_(parameters) {
  Parse();
}

/*!
 * This constructor is used to create an Equation object with an equation
 * string and an array of parameters.  The array must be of the size corresponding
 * to the number of parameters in the equation string.  
 */

Equation::Equation(std::string equation,double parameters[], size_t arraySize) : infixEquation_(equation) {
  parameters_=std::vector<double>(parameters,parameters+arraySize/sizeof(double));
  Parse();
}

/*!
 * If the empty constructor was used to create the Equation object, this 
 * function can be used to set the equation string and initialize the parameter
 * array.  The parameters must be set manually.
 */

void Equation::Initialize(std::string equation, int numParams) {
  infixEquation_=equation;
  for(int i=0;i<numParams;i++) {
    double tempDouble=0.0;
    parameters_.push_back(tempDouble);
  }
  Parse();
}

/*!
 * This equation is called from all constructors, with the exception of the
 * empty constructor, and the Equation::Initialize() function.  The purpose is 
 * to parse an infix equation string into a vector of tokens in RPN format.  
 * This is neccessary to evaluate the equation on-the-fly later without having 
 * to determine precidence each time.  The function uses the shunting-yard 
 * algorithm.
 */

void Equation::Parse() {
  int position=0;
  std::vector<TokenPair> stack;
  while(position<infixEquation_.length()) {
    TokenPair tempPair=GetToken(position);
    if(tempPair.first==NUMBER||tempPair.first==PARAMETER||tempPair.first==VARIABLE||tempPair.first==FUNCTION) {
      output_.push_back(tempPair);
    } else if(tempPair.first==OPERATOR) {
      if(stack.size()==0) stack.push_back(tempPair);
      else while(stack.size()>0) {
        if(IsOperator(stack[stack.size()-1].second[0])) {
	  char lastChar=stack[stack.size()-1].second[0];
	  if(GetOperatorType(tempPair.second[0])<=GetOperatorType(lastChar)) {
	    output_.push_back(stack[stack.size()-1]);
	    stack.pop_back();
	    if(stack.size()==0) {
	      stack.push_back(tempPair);
	      break;
	    }
	  } else {
	    stack.push_back(tempPair);
	    break;
	  }
	} else {
	  stack.push_back(tempPair);
	  break;
	}
      }
    } else if(tempPair.first==LEFTPAR) {
      stack.push_back(tempPair);
    }else if(tempPair.first==RIGHTPAR) {
      while(stack.size()>0&&stack[stack.size()-1].first!=LEFTPAR) {
	output_.push_back(stack[stack.size()-1]);
	stack.pop_back();
      }
      if(stack.size()==0) {
	std::cout << "Mismatched Parenthesis." << std::endl;
      } else if(stack[stack.size()-1].first==LEFTPAR) stack.pop_back();
    }
  }
  while(stack.size()>0) {
    output_.push_back(stack[stack.size()-1]);
    stack.pop_back();
  }
  //for(int j = 0; j<output_.size();j++) std::cout << output_[j].second << ' ';std::cout<<std::endl;
}

/*!
 * Returns the vector containing all the parameters in the Equation object.
 */

std::vector<double> Equation::GetParameters() const {
  return parameters_;
}

/*!
 * Sets the specified parameter.
 */

void Equation::SetParameter(int index, double value) {
  if(index<parameters_.size()) parameters_[index]=value;
  else std::cout << "ERROR: CANNOT SET PARAMETER." << std::endl;
}

/*!
 * Used to determine if a character is an operator.  Returns true if the character
 * is an operator, otherwise returns false.
 */

bool Equation::IsOperator(char c) const {
  if(c=='+'||c=='-'||c=='*'||c=='/'||c=='^')
    return true;
  else return false;
}

/*!
 * Returns true if the character is a digit between 1-9, otherwise returns 
 * false.
 */

bool Equation::IsDigit(char c) const {
  if(c>='0'&&c<='9') return true;
  else return false;
}

/*!
 * Reads the infix equation and returns the next token, advancing the 
 * position in the string. The return value is a token pair, which contains
 * the token itself as well as the type.
 */

Equation::TokenPair Equation::GetToken(int &position) const {
  TokenType tempType;
  std::string tempString;
  //Number (positive decimal, integer, or exponential notation)
  if(IsDigit(infixEquation_[position])) {
    tempType=NUMBER;
    while((IsDigit(infixEquation_[position])||infixEquation_[position]=='.'||infixEquation_[position]=='e'
	   ||(infixEquation_[position]=='-'&&infixEquation_[position-1]=='e')
	   ||(infixEquation_[position]=='+'&&infixEquation_[position-1]=='e'))
	  &&position<infixEquation_.length()) {
      tempString+=infixEquation_[position];
      position++;
    }
  }
  //Check for negation
  else if(infixEquation_[position]=='-'&&(position==0||IsOperator(infixEquation_[position-1])||
					    infixEquation_[position-1]=='(')) {
    tempType=FUNCTION;
    position++;
    tempString="neg";
    while(infixEquation_[position]!=')'&&!IsOperator(infixEquation_[position])&&position<infixEquation_.length()) {
      tempString+=infixEquation_[position];
      position++;
    }
  }
  //Operator (+,-,*,/, or ^)
  else if(IsOperator(infixEquation_[position])) {
    tempType=OPERATOR;
    tempString+=infixEquation_[position];
    position++;
  } 
  //Parameter (only 10 supported currently, must be of form a0,a1,...)
  else if(infixEquation_[position]=='a'&&IsDigit(infixEquation_[position+1])) {
    tempType=PARAMETER;
    position++;
    std::istringstream stm;
    stm.str(infixEquation_.substr(position,1));
    int paramNumber;stm>>paramNumber;
    if(paramNumber+1>parameters_.size()) 
      std::cout << "ERROR: PARAMETER IN " << infixEquation_ 
		<< " AT POSITION " << position 
		<< " IS GREATER THAN NUMBER OF SPECIFIED PARAMETERS." 
		<< std::endl;
    tempString+=infixEquation_[position];
    position++;
  } 
  //Dependent variable
  else if(infixEquation_[position]=='x') {
    tempType=VARIABLE;
    tempString+=infixEquation_[position];
    position++;
  } 
  //Left Parenthesis
  else if(infixEquation_[position]=='(') {
    tempType=LEFTPAR;
    tempString+=infixEquation_[position];
    position++;
  } 
  //Right Parenthesis
  else if(infixEquation_[position]==')') {
    tempType=RIGHTPAR;
    tempString+=infixEquation_[position];
    position++;
  } 
  //Functions (currently supports: exp, sin, cos, tan)
  else if(infixEquation_[position]=='e'&&
	    (infixEquation_.substr(position,4)=="exp("||infixEquation_.substr(position,3)=="e^(")) { 
    tempType=FUNCTION;
    if(infixEquation_.substr(position,4)=="exp(") position+=4;
    else position+=3;
    tempString="exp";
    int parenCount=0;
    while(infixEquation_[position]!=')'||parenCount!=0) {
      if(infixEquation_[position]=='(') parenCount++;
      else if(infixEquation_[position]==')') parenCount--;
      tempString+=infixEquation_[position];
      position++;
    } position++;
  } else if(infixEquation_.substr(position,4)=="sin(") {
    tempType=FUNCTION;
    position+=4;
    tempString="sin";
    int parenCount=0;
    while(infixEquation_[position]!=')'||parenCount!=0) {
      if(infixEquation_[position]=='(') parenCount++;
      else if(infixEquation_[position]==')') parenCount--;
      tempString+=infixEquation_[position];
      position++;
    } position++;
  } else if(infixEquation_.substr(position,4)=="cos(") {
    tempType=FUNCTION;
    position+=4;
    tempString="cos";
    int parenCount=0;
    while(infixEquation_[position]!=')'||parenCount!=0) {
      if(infixEquation_[position]=='(') parenCount++;
      else if(infixEquation_[position]==')') parenCount--;
      tempString+=infixEquation_[position];
      position++;
    } position++;
  } else if(infixEquation_.substr(position,4)=="tan(") {
    tempType=FUNCTION;
    position+=4;
    tempString="tan";
    int parenCount=0;
    while(infixEquation_[position]!=')'||parenCount!=0) {
      if(infixEquation_[position]=='(') parenCount++;
      else if(infixEquation_[position]==')') parenCount--;
      tempString+=infixEquation_[position];
      position++;
    } position++;
  }
  //Unrecognized Tokens
  else {
    std::cout << "ERROR: BAD TOKEN IN " << infixEquation_ 
	      << " AT INDEX " << position << std::endl;
    tempType=BADTOKEN;
    position++;
  }
  //std::cout << tempType << ' ' << tempString << std::endl; 
  return TokenPair(tempType,tempString);
} 

/*!
 * Returns the opterator enumerated type from a character.
 */

Equation::OperatorType Equation::GetOperatorType(char c) const {
  if(c=='+') return ADD;
  else if(c=='-') return SUBTRACT;
  else if(c=='*') return MULT;
  else if(c=='/') return DIVIDE;
  else if(c=='^') return POWER;
  else {
    std::cout << "WARNING: BAD OPERATOR " << c << std::endl;
    return BADTYPE;
  }
}

/*!
 * Evaluates an operator for two specified numbers.  Returns the result in a string.
 */

std::string Equation::BinaryOperation(double left, double right, char op) const {
  std::ostringstream stm;
  stm.precision(15);
  double result;
  if(op=='+') result=left+right;
  else if(op=='-') result=left-right;
  else if(op=='*') result=left*right;
  else if(op=='/') result=left/right;
  else if(op=='^') {
    if(left<0.0&&fabs(int(right)-right)>0.0)
      std::cout << "WARNING: NEGATIVE ARGUMENT RAISED TO FRACTIONAL POWER." << std::endl;
      result=pow(left,right);
  } else {
    result=0.0;
    std::cout << "WARNING: BAD OPERATOR " << op << std::endl;
  }
  stm<<result;
  return stm.str();
}

/*!
 * Evaluates a function token with a specified dependent variable.
 */

double Equation::FunctionOperation(TokenPair token, double x) const {
  std::istringstream stm;
  double result;
  if(token.second.substr(0,3)=="exp") {
    std::string valueString=token.second.substr(3);
    Equation tempEquation(valueString,GetParameters());
    result=exp(tempEquation.Evaluate(x));
  } else if(token.second.substr(0,3)=="sin") {
    std::string valueString=token.second.substr(3);
    Equation tempEquation(valueString,GetParameters());
    result=sin(tempEquation.Evaluate(x));
  } else if(token.second.substr(0,3)=="cos") {
    std::string valueString=token.second.substr(3);
    Equation tempEquation(valueString,GetParameters());
    result=cos(tempEquation.Evaluate(x));
  } else if(token.second.substr(0,3)=="tan") {
    std::string valueString=token.second.substr(3);
    Equation tempEquation(valueString,GetParameters());
    result=tan(tempEquation.Evaluate(x));
  }  else if(token.second.substr(0,3)=="neg") {
    std::string valueString=token.second.substr(3);
    Equation tempEquation(valueString,GetParameters());
    result=-1.*tempEquation.Evaluate(x);
  } else {
    result=0.0;
    std::cout << "ERROR: UNRECOGNIZED FUNCTION" << std::endl;
  }
  return result;
}

/*!
 * Returns the value of a token as a double.
 */

double Equation::GetTokenValue(TokenPair token, double x) const {
  double value;
  std::istringstream stm;
  if(token.first==NUMBER) {
    stm.clear();
    stm.str(token.second);
    stm >> value;
  } else if(token.first==VARIABLE) {
    value=x;
  } else if(token.first==PARAMETER) {
    int paramNumber;
    stm.clear();
    stm.str(token.second);
    stm >> paramNumber;
    value=parameters_[paramNumber];
  } else if(token.first==FUNCTION)
    value=FunctionOperation(token,x);
  else value=0.0;
  return value;
}

/*!
 * Evaluates the Equation object for a specified dependent variable.
 */

double Equation::Evaluate(double x) const {
  std::vector<TokenPair> localOutput=output_;
  std::istringstream stm;
  double result=0.0;
  int i=0;
  if(localOutput.size()==1) {
    result=GetTokenValue(localOutput[0],x);
  } else {
    while(localOutput.size()!=1) {
      if(localOutput[i].first==OPERATOR) {
	char op=localOutput[i].second[0];
	double left=GetTokenValue(localOutput[i-2],x);
	double right=GetTokenValue(localOutput[i-1],x);
	localOutput.erase(localOutput.begin()+i-2,localOutput.begin()+i+1);
	localOutput.insert(localOutput.begin()+i-2,
			   TokenPair(NUMBER,BinaryOperation(left,right,op)));
	i--;
      }
      else i++;
    }
    stm.clear();
    stm.str(localOutput[0].second);
    stm >> result;
  }
  return result;
}


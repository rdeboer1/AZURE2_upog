#include "Equation.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

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
  if(equation.size()!=0){
    for(int i=0;i<numParams;i++) {
      double tempDouble=0.0;
      parameters_.push_back(tempDouble);
    }
    Parse();
  } else {
    std::cout << "Error: empty equation." << std::endl;
    std::exit(-1);
  }
}

/*!
 * This constructor is used to create and Equation object with an equation
 * string and an already created parameter vector.  The size of the parameter 
 * vector must correspond to the number of parameters in the equation
 * string.
 */

Equation::Equation(std::string equation,std::vector<double> parameters) : 
  infixEquation_(equation), parameters_(parameters) {
  if(equation.size()!=0){
    Parse();
  } else {
    std::cout << "Error: empty equation." << std::endl;
    std::exit(-1);
  }
}

/*!
 * This constructor is used to create an Equation object with an equation
 * string and an array of parameters.  The array must be of the size corresponding
 * to the number of parameters in the equation string.  
 */

Equation::Equation(std::string equation,double parameters[], size_t arraySize) : infixEquation_(equation) {
  if(equation.size()!=0){
    parameters_=std::vector<double>(parameters,parameters+arraySize/sizeof(double));
    Parse();
  } else {
    std::cout << "Error: empty equation." << std::endl;
    std::exit(-1);
  }
}

/*!
 * If the empty constructor was used to create the Equation object, this 
 * function can be used to set the equation string and initialize the parameter
 * array.  The parameters must be set manually.
 */

void Equation::Initialize(std::string equation, int numParams) {
  if(equation.size()!=0){
    infixEquation_=equation;
    for(int i=0;i<numParams;i++) {
      double tempDouble=0.0;
      parameters_.push_back(tempDouble);
    }
    Parse();
  } else {
    std::cout << "Error: empty equation." << std::endl;
    std::exit(-1);
  }
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
  try{
    unsigned int position=0;
    std::vector<TokenPair> stack;
    //Initial expectation list
    unsigned char expecting = FUNCTION|NUMBER|PARAMETER|VARIABLE|LEFTPAR;
    while(position<infixEquation_.length()) {
      //Retrieve token
      TokenPair tempPair=GetToken(position);
      //If last token, enforce additional expectations.
      if(position>=infixEquation_.length()) 
	expecting&=FUNCTION|NUMBER|PARAMETER|VARIABLE|RIGHTPAR;
      //Check token against expectations
      if(!(expecting&tempPair.first)) throw SyntaxError(infixEquation_,3,position-1);
      //Set new expectations for next token
      if(tempPair.first==NUMBER || tempPair.first==VARIABLE || 
	 tempPair.first==PARAMETER || tempPair.first==FUNCTION) expecting=OPERATOR|RIGHTPAR;
      else if(tempPair.first==OPERATOR || tempPair.first==LEFTPAR) expecting=FUNCTION|NUMBER|PARAMETER|VARIABLE|LEFTPAR;
      else if(tempPair.first==RIGHTPAR) expecting=OPERATOR|RIGHTPAR;
      //Shunting yard algorithm
      // Numbers, functions, parameters, and variable go directly to output
      if(tempPair.first==NUMBER||tempPair.first==PARAMETER||tempPair.first==VARIABLE||tempPair.first==FUNCTION) {
	output_.push_back(tempPair);
      } else if(tempPair.first==OPERATOR) {
	if(stack.size()==0) stack.push_back(tempPair);
	else while(stack.size()>0) {
	    //if top of stack is an operator
	    if(stack[stack.size()-1].first==OPERATOR) {
	      char lastChar=stack[stack.size()-1].second[0];
	      //check precidence and associativity, while conditions are met move from stack to output
	      if((GetOperatorType(tempPair.second[0])<=GetOperatorType(lastChar)&&
		  GetOperatorAssociativity(tempPair.second[0])==LEFT)||
		 (GetOperatorType(tempPair.second[0])<GetOperatorType(lastChar)&&
		  GetOperatorAssociativity(tempPair.second[0])==RIGHT)) {
		output_.push_back(stack[stack.size()-1]);
		stack.pop_back();
		//if stack is empty, push new operator on stack and break loop
		if(stack.size()==0) {
		  stack.push_back(tempPair);
		  break;
		}
	      } else {
		//if conditions aren't met, push new operator on stack and break loop
		stack.push_back(tempPair);
		break;
	      }
	    } else {
	      //if top of stack is not operator, push new operator on stack and break loop
	      stack.push_back(tempPair);
	      break;
	    }
	  }
      } else if(tempPair.first==LEFTPAR) {
	//left parentheses go directly to the stack
	stack.push_back(tempPair);
      } else if(tempPair.first==RIGHTPAR) {
	//right parentheses initiate push of stack to output until left parenthesis is found
	while(stack.size()>0&&stack[stack.size()-1].first!=LEFTPAR) {
	  output_.push_back(stack[stack.size()-1]);
	  stack.pop_back();
	}
	if(stack.size()==0) {
	  //if stack is empty, parentheses were mismatched
	  throw SyntaxError(infixEquation_,2,position-1);
	} else if(stack[stack.size()-1].first==LEFTPAR) stack.pop_back();
      }
    }
    //push remaining stack to output after all tokens are read
    while(stack.size()>0) {
      //if left parenthesis is found, parentheses were mismatched
      if(stack[stack.size()-1].first==LEFTPAR) throw SyntaxError(infixEquation_,2);
      output_.push_back(stack[stack.size()-1]);
      stack.pop_back();
    }
  } catch (SyntaxError e) {
    std::cout << e.what() << std::endl;
    std::exit(-1);
  }
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

void Equation::SetParameter(unsigned int index, double value) {
  if(index<parameters_.size()) {
    parameters_[index]=value;
    for(unsigned int i = 0; i<subEquations_.size(); i++) subEquations_[i].SetParameter(index,value);
  } else std::cout << "Error: Parameter index " << index << " greater than vector size." << std::endl;
}

/*!
 * Used to determine if a character is an operator.  Returns true if the character
 * is an operator, otherwise returns false.
 */

bool Equation::IsOperator(char c) const {
  return (c=='+'||c=='-'||c=='*'||c=='/'||c=='^') ? (true) : (false);
}

/*!
 * Returns true if the character is a digit between 1-9, otherwise returns 
 * false.
 */

bool Equation::IsDigit(char c) const {
  return (c>='0' && c<='9') ? (true) : (false);
}

/*!
 * Reads the infix equation and returns the next token, advancing the 
 * position in the string. The return value is a token pair, which contains
 * the token itself as well as the type.
 */

Equation::TokenPair Equation::GetToken(unsigned int &position) {
  try {
    TokenType tempType;
    std::string tempString;
    //Number (positive decimal, integer, or exponential notation)
    if(IsDigit(infixEquation_[position])||infixEquation_[position]=='.') {
      tempType=NUMBER;
      while((IsDigit(infixEquation_[position])||infixEquation_[position]=='.'||infixEquation_[position]=='e'||infixEquation_[position]=='E'
	     ||(infixEquation_[position]=='-'&&(infixEquation_[position-1]=='e'||infixEquation_[position-1]=='E'))
	     ||(infixEquation_[position]=='+'&&(infixEquation_[position-1]=='e'||infixEquation_[position-1]=='E')))
	    &&position<infixEquation_.length()) {
	tempString+=infixEquation_[position];
	position++;
      }
    }
    //Check for negation. Implimented as a function.  MUST be checked before minus is parsed as an operator.
    else if(infixEquation_[position]=='-'&&(position==0||IsOperator(infixEquation_[position-1])||
					    infixEquation_[position-1]=='(')) {
      tempType=FUNCTION;
      position++;
      tempString="neg";
    }
    //Operator (+,-,*,/, or ^)
    else if(IsOperator(infixEquation_[position])) {
      tempType=OPERATOR;
      tempString+=infixEquation_[position];
      position++;
    } 
    //Parameter (must be of form a0,a1,...)
    else if(infixEquation_[position]=='a'&&IsDigit(infixEquation_[position+1])) {
      tempType=PARAMETER;
      position++;
      while(IsDigit(infixEquation_[position])&&position<infixEquation_.length()) {
	tempString+=infixEquation_[position];
	position++;
      }
      std::istringstream stm;
      stm.str(tempString);
      unsigned int paramNumber;stm>>paramNumber;
      if(paramNumber+1>parameters_.size()) throw SyntaxError(infixEquation_,1,position-1);
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
    //Functions (currently supports: exp, sin, cos, tan, ln, log)
    else if(infixEquation_[position]=='e'&&
	    (infixEquation_.substr(position,4)=="exp("||
	     infixEquation_.substr(position,3)=="e^(")) { 
      tempType=FUNCTION;
      if(infixEquation_.substr(position,4)=="exp(") position+=4;
      else position+=3;
      tempString="exp";
    } else if(infixEquation_.substr(position,4)=="sin(") {
      tempType=FUNCTION;
      position+=4;
      tempString="sin";
    } else if(infixEquation_.substr(position,4)=="cos(") {
      tempType=FUNCTION;
      position+=4;
      tempString="cos";
    } else if(infixEquation_.substr(position,4)=="tan(") {
      tempType=FUNCTION;
      position+=4;
      tempString="tan";
    } else if(infixEquation_.substr(position,4)=="log(") {
      tempType=FUNCTION;
      position+=4;
      tempString="log";
    } else if(infixEquation_.substr(position,3)=="ln(") {
      tempType=FUNCTION;
      position+=3;
      tempString="ln";
    } else if(infixEquation_.substr(position,5)=="asin(") {
      tempType=FUNCTION;
      position+=5;
      tempString="asin";
    } else if(infixEquation_.substr(position,5)=="acos(") {
      tempType=FUNCTION;
      position+=5;
      tempString="acos";
    } else if(infixEquation_.substr(position,5)=="atan(") {
      tempType=FUNCTION;
      position+=5;
      tempString="atan";
    } else if(infixEquation_.substr(position,5)=="sqrt(") {
      tempType=FUNCTION;
      position+=5;
      tempString="sqrt";
    } 
    //Unrecognized Tokens
    else throw SyntaxError(infixEquation_,0,position);
    //Read subequation if token is function
    if(tempType==FUNCTION) {
      std::string subString;
      //for negation, read until an operator, or unmatched right paranthesis, is found
      //  and create subequation
      if(tempString=="neg") {
	int parenCount=0;
	while(((infixEquation_[position]!=')'&&
		!(IsOperator(infixEquation_[position])&&
		  !(infixEquation_[position-1]=='e'||infixEquation_[position-1]=='E')))||
	       parenCount!=0)&&
	      position<infixEquation_.length()) {
	  if(infixEquation_[position]=='(') parenCount++;
	  else if(infixEquation_[position]==')') parenCount--;
	  subString+=infixEquation_[position];
	  position++;
	}
	if(parenCount>0&&position>=infixEquation_.length()) throw SyntaxError(infixEquation_,2);
      } else {
	//for regular functions, read until closing right parenthesis is found
	// and create subequation
	int parenCount=1;
	while(parenCount>0&&position<infixEquation_.length()) {
	  if(infixEquation_[position]=='(') parenCount++;
	  else if(infixEquation_[position]==')') parenCount--;
	  if(parenCount!=0) subString+=infixEquation_[position];
	  position++;
	}
	if(parenCount>0&&position>=infixEquation_.length()) throw SyntaxError(infixEquation_,2);
      }
      Equation subEquation(subString,parameters_);
      subEquations_.push_back(subEquation);
      int subIndex=subEquations_.size()-1;
      std::ostringstream stm;
      stm << subIndex;
      tempString+=stm.str();
    }
    return TokenPair(tempType,tempString);
  } catch (SyntaxError e) {
    std::cout << e.what() << std::endl;
    std::exit(-1);
  } 
}

/*!
 * Returns the opterator enumerated type from a character.
 */

Equation::OperatorType Equation::GetOperatorType(char c) const {
  switch (c) {
    case '+': return ADD;
    case '-': return SUBTRACT;
    case '*': return MULT;
    case '/': return DIVIDE;
    case '^': return POWER;
    default : return BADTYPE;
  }
}

/*!
 * Returns the Associativity of the operator as an enumerated type.
 */

Equation::Associativity Equation::GetOperatorAssociativity(char c) const {
  switch (c) {
    case '+': return LEFT;
    case '-': return LEFT;
    case '*': return LEFT;
    case '/': return LEFT;
    case '^': return RIGHT;
    default : return LEFT;
  }
}

/*!
 * Evaluates an operator for two specified numbers.  Returns the result in a string.
 */

std::string Equation::BinaryOperation(double left, double right, char op) const {
  std::ostringstream stm;
  stm.precision(15);
  double result;
  switch(op) {
    case '+': 
      result=left+right;
      break;
    case '-': 
      result=left-right;
      break;
    case '*': 
      result=left*right;
      break;
    case '/': 
      result=left/right;
      break;
    case '^': 
      if(left<0.0&&fabs(int(right)-right)>0.0) {
	std::cout << "Warning: Exponent results in unsupported imaginary number." << std::endl;
	result=0.0;
      }	else result=pow(left,right);
      break;
    default:
      result=0.0;
  }
  stm<<result;
  return stm.str();
}

/*!
 * Evaluates a function token with a specified dependent variable.
 */

double Equation::FunctionOperation(TokenPair token, double x) const {
  double result;
  if(token.second.substr(0,3)=="exp") {
    unsigned int subEquationIndex;
    std::istringstream stm;
    stm.str(token.second.substr(3));
    stm>>subEquationIndex;
    result=exp(subEquations_[subEquationIndex].Evaluate(x));
  } else if(token.second.substr(0,3)=="sin") {
    int subEquationIndex;
    std::istringstream stm;
    stm.str(token.second.substr(3));
    stm>>subEquationIndex;
    result=sin(subEquations_[subEquationIndex].Evaluate(x));
  } else if(token.second.substr(0,3)=="cos") {
    int subEquationIndex;
    std::istringstream stm;
    stm.str(token.second.substr(3));
    stm>>subEquationIndex;
    result=cos(subEquations_[subEquationIndex].Evaluate(x));
  } else if(token.second.substr(0,3)=="tan") {
    int subEquationIndex;
    std::istringstream stm;
    stm.str(token.second.substr(3));
    stm>>subEquationIndex;
    result=tan(subEquations_[subEquationIndex].Evaluate(x));
  }  else if(token.second.substr(0,3)=="neg") {
    int subEquationIndex;
    std::istringstream stm;
    stm.str(token.second.substr(3));
    stm>>subEquationIndex;
    result=-1.*subEquations_[subEquationIndex].Evaluate(x);
  } else if(token.second.substr(0,3)=="log") {
    int subEquationIndex;
    std::istringstream stm;
    stm.str(token.second.substr(3));
    stm>>subEquationIndex;
    result=log10(subEquations_[subEquationIndex].Evaluate(x));
  } else if(token.second.substr(0,2)=="ln") {
    int subEquationIndex;
    std::istringstream stm;
    stm.str(token.second.substr(2));
    stm>>subEquationIndex;
    result=log(subEquations_[subEquationIndex].Evaluate(x));
  } else if(token.second.substr(0,4)=="asin") {
    int subEquationIndex;
    std::istringstream stm;
    stm.str(token.second.substr(4));
    stm>>subEquationIndex;
    result=asin(subEquations_[subEquationIndex].Evaluate(x));
  } else if(token.second.substr(0,4)=="acos") {
    int subEquationIndex;
    std::istringstream stm;
    stm.str(token.second.substr(4));
    stm>>subEquationIndex;
    result=acos(subEquations_[subEquationIndex].Evaluate(x));
  } else if(token.second.substr(0,4)=="atan") {
    int subEquationIndex;
    std::istringstream stm;
    stm.str(token.second.substr(4));
    stm>>subEquationIndex;
    result=atan(subEquations_[subEquationIndex].Evaluate(x));
  } else if(token.second.substr(0,4)=="sqrt") {
    int subEquationIndex;
    std::istringstream stm;
    stm.str(token.second.substr(4));
    stm>>subEquationIndex;
    result=sqrt(subEquations_[subEquationIndex].Evaluate(x));
  } else result=0.0;
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
    unsigned int paramNumber;
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


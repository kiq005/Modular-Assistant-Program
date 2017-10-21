#ifndef __MODULE_CALCULATOR_H__
#define __MODULE_CALCULATOR_H__
#include <iostream>
#include <stack>
#include <cmath>
#include <regex>
/* List containing the answers to factorials from 0! to 20! */
static const unsigned long long smallFact[] = {
	1,1,2,6,24,120,720,5040,40320,362880,3628800,39916800,479001600,6227020800,
	87178291200,1307674368000,20922789888000,355687428096000,6402373705728000,
	121645100408832000,2432902008176640000};
/* CalculatorModule
 * This module is responsible to answer mathematical questions.
 */
class CalculatorModule : public Module{
	
public:
	std::string input(std::string inp){
		return calculate(inp);
	}
private:
	/* Verify operator precedence */
	inline bool hasHigherPrec(const char& top, const char& opp){
	  return (top=='*' or top=='^' or top=='/' or top=='%' or top=='c' or top=='s' or top=='t' or top=='!') 
	  && (opp=='-' or opp=='+');
	}
	/* Check if it is a operator */
	inline bool isOpp(const char& c){
	  return (
	  	// Arithmetics
	    c=='+' or c=='-' or c=='*' or c=='/' or c=='%'
	    // Trigonometrics
	    or c=='c' or c=='s' or c=='t'
	    // Others
	    or c=='^' or c=='!'
	    );
	}
	/* Check it it is a oppening character */
	inline bool isOpening(const char& c){
	  return (c=='(' or c=='[' or c=='{');
	}
	/* Check it it is a closing character */
	inline bool isClosing(const char& c){
	  return (c==')' or c==']' or c=='}');
	}
	/* Return the answer to a factorial */
	inline unsigned long long fact(const int& n){
	  if(n<21)
	    return smallFact[n];
	  return 0;
	}

	// https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
	inline bool replace(std::string& str, const std::string& from, const std::string& to) {
	    size_t start_pos = str.find(from);
	    if(start_pos == std::string::npos)
	        return false;
	    str.replace(start_pos, from.length(), to);
	    return true;
	}

	void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	    if(from.empty())
	        return;
	    size_t start_pos = 0;
	    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
	        str.replace(start_pos, from.length(), to);
	        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	    }
	}
	/* Convert from infix to postfix */
	std::string infixToPostfix(std::string str){
	  std::stack<char> oppstack;
	  std::string pos = "";
	  int i, len;
	  // Remove spaces
	  str.erase(remove(str.begin(),str.end(),' '),str.end());
	  /* Correct functions names to a single char */
	  replaceAll (str, "cos", "c");// Cossine
	  replaceAll (str, "sin", "s");// Sine
	  replaceAll (str, "tg",  "t");// Tangent
	  replaceAll (str, "tan", "t");// Tangent
	  // Exponential
	  str = regex_replace (str, static_cast<std::regex>("pow\\(([[:digit:]]*)\\,([[:digit:]]*)\\)"), "$1^$2");
	  // Avoid mistakes
	  str = regex_replace (str, static_cast<std::regex>("-\\([^)]+\\)"), "+($0)");
	  str = regex_replace (str, static_cast<std::regex>("-([^(][[:digit:]]*)"), "+($0)");
	  str = regex_replace (str, static_cast<std::regex>("[\\*/\\^]\\+"), "*");
	  /* Start postfix conversion */
	  len = str.length();
	  for(i=0;i<len; i++){
	    if(str[i]>='0' && str[i] <='9'){
	      pos+=str[i];
	    }
	    else if(isOpp(str[i])){
	      pos+=" ";
	      while(!oppstack.empty() and !isOpening(oppstack.top()) and hasHigherPrec(oppstack.top(), str[i])){
	        pos+=oppstack.top();
	        //pos+=" ";
	        oppstack.pop();
	      }
	      oppstack.push(str[i]);
	    }
	    else if(isOpening(str[i])){
	      oppstack.push(str[i]);
	    }
	    else if(isClosing(str[i])){
	      //pos+=" ";
	      while(!oppstack.empty() and !isOpening(oppstack.top()) ){
	        pos+=oppstack.top();
	        //pos+=" ";
	        oppstack.pop();
	      }
	      oppstack.pop();
	    }

	  }
	  //pos+=" ";
	  while(!oppstack.empty()){
	    pos+=oppstack.top();
	    //pos+=" ";
	    oppstack.pop();
	  }

	  //cout << "Posfix: " << pos << endl;
	  return pos;
	}
	/* Get the answer to a postfix expression */
	double evaluatePos(const std::string& exp){
	  std::stack<double> operands;
	  int len=exp.length();
	  double temp;
	  bool jumpNegate=false;
	  //cout << "Exp: " << exp << endl;

	  for(int i=0; i<len; ++i){
	    /* Get number */
	    if( (unsigned)(exp[i]-'0')<=9 ){// --> if(exp[i]>='0' && exp[i]<='9'){
	      int num = exp[i++]-'0';
	      while( (unsigned)(exp[i]-'0')<=9 ){
	        num*=10;
	        num+=exp[i++]-'0';
	      }
	      operands.push(num);
	    }

	    switch(exp[i]){
	      case '+':
	        if(operands.size()==1) continue;
	        temp = operands.top();
	        operands.pop();
	        operands.top() += temp;
	        break;
	      case '-':
	        if(operands.empty()) jumpNegate=!jumpNegate;
	        else{
	          if(!jumpNegate) operands.top() *= -1;
	          else jumpNegate = false;
	        }
	        break;
	      case '/':
	        temp = operands.top();
	        operands.pop();
	        operands.top() /= temp;
	        break;
	      case '*':
	        temp = operands.top();
	        operands.pop();
	        operands.top() *= temp;
	        break;
	      case '^':
	        temp = operands.top();
	        operands.pop();
	        operands.top() = pow(operands.top(), temp);
	        break;
	      case '%':
	        temp = operands.top();
	        operands.pop();
	        operands.top() = (int)(operands.top())%(int)temp;
	        break;
	      case 'c':
	        operands.top() = cos( operands.top() );
	        break;
	      case 's':
	        operands.top() = sin( operands.top() );
	        break;
	      case 't':
	        operands.top() = tan( operands.top() );
	        break;
	      case '!':
	        operands.top() = fact( operands.top() );
	    }
	  }

	  return operands.top();
	}
	/* Get the answer to a infix expression */
	inline float evaluate(const std::string& exp){
	  return evaluatePos(infixToPostfix(exp));
	}
	/* Check if it is a mathematical expression */
	bool isExpression(const std::string& exp){
		std::smatch match;
		std::regex e ("^(([ +\\-\\(]*(sin|cos|tan|tg|pow|sen)*[ +%\\-\\(]*(([0-9]|(sin|cos|tan|tg|pow|sen))[,+% \\-*/^]*)[! \\)]*)+)$");
		return std::regex_search(exp, match, e);
	}
	/* Get the answer to a expression */
	inline std::string calculate(const std::string& exp){
		if(not isExpression(exp))
			return "";
  		double x = evaluate(exp);
  		return exp + " = " + std::to_string(x) + "\n";
	}
};


#endif
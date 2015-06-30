#ifndef TH_CALCULATOR_FUNCTION_FORM
#define TH_CALCULATOR_FUNCTION_FORM

#include "Form.h"
#include <assert.h>

extern const thfloat (*OperatorFunctions[])(thfloat,thfloat);
extern const thfloat (*OtherSingleFunctions[])(thfloat,thfloat);
extern const thfloat (*OtherDoubleFunctions[])(thfloat,thfloat);

#define OPERATOR_PLUS 0
#define OPERATOR_MINUS 1
#define OPERATOR_MULTIPLY 2
#define OPERATOR_DIVISION 3

//Single Functions index
#define TH_CALC_SINGLE_NEGATIVE		0

#define TH_CALC_SINGLE_SIN			1
#define TH_CALC_SINGLE_COS			2
#define TH_CALC_SINGLE_TAN			3

#define TH_CALC_SINGLE_LN			4
#define TH_CALC_SINGLE_LOG10		5
#define TH_CALC_SINGLE_EXP			6

#define TH_CALC_SINGLE_SINH			7
#define TH_CALC_SINGLE_COSH			8
#define TH_CALC_SINGLE_TANH			9

#define TH_CALC_SINGLE_SQRT			10

#define TH_CALC_SINGLE_ASIN			11
#define TH_CALC_SINGLE_ACOS			12
#define TH_CALC_SINGLE_ATAN			13

#define TH_CALC_SINGLE_ABS			14

//Double Functions index
#define TH_CALC_DOUBLE_POW			0




class OperatorForm : public FunctionForm
{
public:
	OperatorForm(int _type)
	{
		GetValue=OperatorFunctions[_type];
		type=_type;
#ifndef NDEBUG
		if( (_type!=0)&&(_type!=1)&&(_type!=2)&&(_type!=3) )
		{
			assert("There is No Operator Type" && 0);
		}
#endif
	}

	bool IsSingle() const{return false;}
	int GetPriority() const
	{
		if(type<2){return 5;}
		return 7;
	}
};


class SingleFunction : public FunctionForm
{
public:
	SingleFunction(int _type)
	{
		type=_type;

		GetValue=OtherSingleFunctions[_type];
	}

	bool IsSingle() const{return true;}
	int GetPriority() const{return 9;}
};

class DoubleFunction : public FunctionForm
{
public:
	DoubleFunction(int _type)
	{
		type=_type;

		GetValue=OtherDoubleFunctions[_type];
	}

	bool IsSingle() const{return false;}
	int GetPriority() const{return 9;}
};
#endif
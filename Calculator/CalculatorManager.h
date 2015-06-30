#ifndef H_CALCULATOR_MANAGER
#define TH_CALCULATOR_MANAGER

#include <THPrivate.h>

#include "Form.h"
#include "NonFunctionForm.h"
#include "FunctionForm.h"

#define ARRAY_INITIAL 20

#define TH_CALC_PI 3.1415926535897932384626433832795
#define TH_CALC_E 2.7182818284590452353602874713527

//int Parse() return Errors
#define PAREN_IS_NOT_MATCHING 1
#define TOO_MANY_DOTS 2

class CalculatorManager
{
public:
	//¥ð x t
	thfloat x,t;

	CalculatorManager():backFormula(ARRAY_INITIAL*2),backFunctions(ARRAY_INITIAL),values(ARRAY_INITIAL)
	{
		tempValueForm=new ConstantForm(0.0);
	}

	void DetermindParenLeft();
	void DetermindParenRight();
	void DetermindFunction(const FunctionForm* form);
	void DetermindForm(const Form* form);

	//Calculate from parsed buffer values
	thfloat Result();

	//parse string to buffer
	int Parse(const char* text);


protected:
	THArray<const Form*> backFormula;
	THArray<const FunctionForm*> backFunctions;
	THArray<const NonFunctionForm*> values;
	ConstantForm* tempValueForm;
};

#endif
#ifndef TH_CALCULATOR_NONFUNCTION_FORM
#define TH_CALCULATOR_NONFUNCTION_FORM

#include "Form.h"

class ConstantForm : public NonFunctionForm
{
public:
	thfloat value;
	ConstantForm(thfloat v){value=v;}

	thfloat GetValue() const{return value;}
};

class VariableForm : public NonFunctionForm
{
public:
	thfloat* value;
	VariableForm(thfloat* p){value=p;}

	thfloat GetValue() const{return *value;}
};


#endif
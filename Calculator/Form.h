#ifndef TH_CALCULATOR_FORM_H
#define TH_CALCULATOR_FORM_H

typedef long double thfloat;
typedef thfloat (*THFunctionType)(thfloat,thfloat);

class Form
{
public:
	virtual bool IsFunction() const=0;
};

class FunctionForm : public Form
{
public:
	int type;
	const thfloat (*GetValue)(thfloat,thfloat);

	bool IsFunction() const{return true;}
	virtual bool IsSingle() const=0;
	virtual int GetPriority() const=0;
};
class NonFunctionForm : public Form
{
public:
	bool IsFunction() const{return false;}
	virtual thfloat GetValue() const=0;
};

#endif
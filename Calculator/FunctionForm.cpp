#include "FunctionForm.h"
#include <cmath>

static const thfloat PLUS(thfloat a,thfloat b)
{
	return a+b;
}
static const thfloat MINUS(thfloat a,thfloat b)
{
	return a-b;
}
static const thfloat MULTIPLY(thfloat a,thfloat b)
{
	return a*b;
}
static const thfloat DIVISION(thfloat a,thfloat b)
{
	return a/b;
}
extern const thfloat (*OperatorFunctions[])(thfloat,thfloat)={PLUS,MINUS,MULTIPLY,DIVISION};


#define MAKE_SINGLE(name,func)\
	static const thfloat name(thfloat a,thfloat b){return func(a);}
static const thfloat NEGATIVE(thfloat a,thfloat b)
{
	return -a;
}
MAKE_SINGLE(SIN,sin)
MAKE_SINGLE(COS,cos)
MAKE_SINGLE(TAN,tan)
MAKE_SINGLE(LN,log)
MAKE_SINGLE(LOG10,log10)
MAKE_SINGLE(EXP,exp)
MAKE_SINGLE(SINH,sinh);
MAKE_SINGLE(COSH,cosh);
MAKE_SINGLE(TANH,tanh);
MAKE_SINGLE(SQRT,sqrt);
MAKE_SINGLE(ASIN,asin);
MAKE_SINGLE(ACOS,acos);
MAKE_SINGLE(ATAN,atan);
MAKE_SINGLE(ABS,abs);

extern const thfloat (*OtherSingleFunctions[])(thfloat,thfloat)={
	NEGATIVE,SIN,COS,TAN,LN,
	LOG10,EXP,SINH,COSH,TANH,
	SQRT,ASIN,ACOS,ATAN,ABS
};


#define MAKE_DOUBLE(name,func)\
	static const thfloat name(thfloat a,thfloat b){return func(a,b);}
MAKE_DOUBLE(POW,pow)
extern const thfloat (*OtherDoubleFunctions[])(thfloat,thfloat)={POW};
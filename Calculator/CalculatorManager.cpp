#include "CalculatorManager.h"
#include "Form.h"
#include "FunctionForm.h"
#include "NonFunctionForm.h"

void CalculatorManager::DetermindParenLeft()
{
	//그냥 스택에!
	backFunctions.Push(0);
}
void CalculatorManager::DetermindParenRight()
{
	const FunctionForm* tf;
	//왼괄호까지 집어넣기
	for(tf=backFunctions.Pop();tf!=0;tf=backFunctions.Pop())
	{
		backFormula.Push(tf);
	}
}
void CalculatorManager::DetermindFunction(const FunctionForm* form)
{
	if(backFunctions.num)
	{
		const FunctionForm* last=backFunctions.GetLast();

		if(last && last->GetPriority()>form->GetPriority())
		{
			backFunctions.Pop();
			backFormula.Push(last);
		}
	}
	backFunctions.Push(form);
}

#include <stdio.h>
thfloat CalculatorManager::Result()
{
	values.Clear();

	const Form* form;
	const NonFunctionForm *nf1,*nf2;
	const FunctionForm* ff;

	for(unsigned int i=0;i<backFormula.num;++i)
	{
		
		form=backFormula.arr[i];

		if(form->IsFunction())
		{
			ff=(const FunctionForm*)form;
			if(ff->IsSingle())
			{
				nf1=values.Pop();
				thfloat tempd=ff->GetValue(nf1->GetValue(),0.0);
				tempValueForm->value=tempd;
				values.Push(tempValueForm);
			}else
			{
				nf2=values.Pop();
				nf1=values.Pop();

				thfloat tempd=ff->GetValue(nf1->GetValue(),nf2->GetValue());
				tempValueForm->value=tempd;
				values.Push(tempValueForm);
			}
		}else
		{
			values.Push((const NonFunctionForm*)form);
		}
	}

	return values.Pop()->GetValue();
}



#define ISNUMBER(x) ('0'<=x&&x<='9')
#define GETNUMBER(x) (thfloat)(x-'0')
#define ISALPHABET(x) ('a'<=x&&x<='z')
//범위 안 전부 Number 아니면 Dot 인경우만. Dot 중복인경우에 1 return
static int str2thfloat(thfloat* p,const char* text,unsigned int count)
{
	thfloat doub=0.0;
	thfloat numPow=1.0;
	bool dotPassed=false;
	char cc;

	for(unsigned int i=0;i<count;++i)
	{
		cc=text[i];
		if(ISNUMBER(cc))
		{
			if(dotPassed){
				numPow*=0.1;
				doub=doub+GETNUMBER(cc)*numPow;
			}else
			{
				doub=doub*10.0 + GETNUMBER(cc);
			}
		}
		else
		{
			if(dotPassed){return 1;}
			dotPassed=true;
		}
	}

	*p=doub;

	return 0;
}
static bool StrEqual(const char* text,const char* src,unsigned int count,unsigned int& si)
{
	for(unsigned int i=0;i<count;++i)
	{
		if(text[i]==0){si=i;return false;}
		if(text[i]!=src[i]){si=i;return false;}
	}
	return true;
}

int CalculatorManager::Parse(const char* text)
{
	//초기화
	bool isNextFunction=true;
	unsigned int lastNumInd=0;
	unsigned int parenFactor=0;

	unsigned int sameIndex;

	backFormula.Clear();
	backFunctions.Clear();

	unsigned int i=0;
	char cc;
	const char* tt;
	while((cc=text[i])!=0)
	{
		tt=text+i;
		++i;

		//숫자
		if(ISNUMBER(cc) || cc=='.')
		{
			//숫자세기
			if(lastNumInd==0)
			{
				lastNumInd=i;
			}

			//다음문자가 숫자가 아니면
			if(!(ISNUMBER(text[i]) || text[i]=='.'))
			{
				thfloat tempd;
				if(str2thfloat(&tempd,text+(lastNumInd-1),i+1-lastNumInd)){return TOO_MANY_DOTS;}

				THLog("%lf",tempd);

				isNextFunction=false;
				backFormula.Push(new ConstantForm(tempd));

				//초기화
				lastNumInd=0;
			}
			continue;
		}

		
		//왼괄호
		if(cc=='(')
		{ 
			THLog("(");
			++parenFactor;
			DetermindParenLeft();
			isNextFunction=true;
			continue;
		}
		//오른괄호 - 왼괄호 없으면 오류
		else if(cc==')')
		{
			THLog(")");
			if(parenFactor==0)
			{
				return PAREN_IS_NOT_MATCHING;
			}
			DetermindParenRight();
			isNextFunction=false;
			continue;
		}

		

		//연산자인경우
		else if(cc=='+')
		{
			THLog("+");
			DetermindFunction(new OperatorForm(OPERATOR_PLUS));
			isNextFunction=true;
		}else if(cc=='*')
		{
			THLog("*");
			DetermindFunction(new OperatorForm(OPERATOR_MULTIPLY));
			isNextFunction=true;
		}else if(cc=='/' || cc=='÷')
		{
			THLog("/");
			DetermindFunction(new OperatorForm(OPERATOR_DIVISION));
			isNextFunction=true;
		}
		//Minus는 Negative 함수인지도 판별
		else if(cc=='-')
		{
			if(!isNextFunction)
			{
				THLog("-");
				//Operator
				DetermindFunction(new OperatorForm(OPERATOR_MINUS));
			}else
			{
				THLog("-f");
				DetermindFunction(new SingleFunction(TH_CALC_SINGLE_NEGATIVE));
				//Function
			}
			isNextFunction=true;
		}
		

		else if(StrEqual(tt,"sinh",4,sameIndex))
		{
			THLog("sinh");
			i+=3;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_SINH));
		}
		else if(sameIndex==3)
		{
			THLog("sin");
			i+=2;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_SIN));
		}
		else if(StrEqual(tt,"cosh",4,sameIndex))
		{
			THLog("cosh");
			i+=3;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_COSH));
		}
		else if(sameIndex==3)
		{
			THLog("cos");
			i+=2;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_COS));
		}
		else if(StrEqual(tt,"tanh",4,sameIndex))
		{
			THLog("tanh");
			i+=3;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_TANH));
		}
		else if(sameIndex==3)
		{
			THLog("tan");
			i+=2;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_TAN));
		}
		else if(StrEqual(tt,"ln",2,sameIndex))
		{
			THLog("ln");
			++i;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_LN));
		}
		else if(StrEqual(tt,"log10",5,sameIndex))
		{
			THLog("log10");
			i+=4;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_LOG10));
		}
		else if(StrEqual(tt,"exp",3,sameIndex))
		{
			THLog("exp");
			i+=2;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_EXP));
		}
		else if(StrEqual(tt,"sqrt",4,sameIndex))
		{
			THLog("sqrt");
			i+=3;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_SQRT));
		}
		else if(StrEqual(tt,"abs",3,sameIndex))
		{
			THLog("abs");
			i+=2;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_ABS));
		}
		else if(StrEqual(tt,"asin",4,sameIndex))
		{
			THLog("asin");
			i+=3;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_ASIN));
		}
		else if(StrEqual(tt,"acos",4,sameIndex))
		{
			THLog("acos");
			i+=3;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_ACOS));
		}
		else if(StrEqual(tt,"atan",4,sameIndex))
		{
			THLog("atan");
			i+=3;
			isNextFunction=true;
			DetermindFunction(new SingleFunction(TH_CALC_SINGLE_ATAN));
		}
		else if(StrEqual(tt,"pow",3,sameIndex))
		{
			THLog("pow");
			i+=2;
			isNextFunction=true;
			DetermindFunction(new DoubleFunction(TH_CALC_DOUBLE_POW));
		}

		else if(StrEqual(tt,"pi",2,sameIndex))
		{
			i+=1;
			THLog("PI");
			isNextFunction=false;
			backFormula.Push(new ConstantForm(TH_CALC_PI));
		}
		//변수 인경우 x, t
		else if(cc=='x')
		{
			THLog("x");
			isNextFunction=false;
			backFormula.Push(new VariableForm(&x));
		}
		else if(cc=='t')
		{
			THLog("t");
			isNextFunction=false;
			backFormula.Push(new VariableForm(&t));
		}
		else if(cc=='e')
		{
			THLog("e");
			isNextFunction=false;
			backFormula.Push(new ConstantForm(TH_CALC_E));
		}
		else if(cc=='π')
		{
			THLog("PI");
			isNextFunction=false;
			backFormula.Push(new ConstantForm(TH_CALC_PI));
		}
		else if(cc='^')
		{
			THLog("^");
			isNextFunction=true;
			DetermindFunction(new DoubleFunction(TH_CALC_DOUBLE_POW));
		}
		
	}

	//연산자스택 비우기
	i=backFunctions.num;
	while(i)
	{
		--i;
		backFormula.Push(backFunctions.arr[i]);
	}

	return 0;
}
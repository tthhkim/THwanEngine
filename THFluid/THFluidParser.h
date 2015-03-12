#ifndef TH_FLUID_PARSER
#define TH_FLUID_PARSER

#include <THFluid/THFluid.h>



class THParseSetGroup
{
public:
	int dataMin,dataMax;
	THParseSetGroup *next,*prev;
	bool isStatic;
	THParticleGroup *pointer;
	int gap;

	THParseSetGroup()
	{
		next=0;
		prev=0;
	}
};
class THParseSetPoint
{
public:
	int data;
	THVector2 *pointer;
	THParseSetPoint *next,*prev;

	THParseSetPoint()
	{
		next=prev=0;
	}
};

class THFluidParser
{
public:
	THFluidParser()
	{
		m_listGroup=0;
		m_listPoint=0;
	}
	~THFluidParser()
	{
		if(m_listGroup)
		{
			THParseSetGroup *p=m_listGroup,*np;
			while(p)
			{
				np=p->next;
				delete p;
				p=np;
			}
		}
		if(m_listPoint)
		{
			THParseSetPoint *p=m_listPoint,*np;
			while(p)
			{
				np=p->next;
				delete p;
				p=np;
			}
		}
	}

	void AddSet(int dataMin,int dataMax,THParticleGroup *group,int gap=1,bool isStatic=false);
	void AddSet(int data,THVector2 *point);
	void Parse(THFluidEngine *_engine,unsigned char *data,size_t w,size_t h);
	void CheckData(int data,const THVector2& pos,int x,int y);

protected:
	THParseSetGroup *m_listGroup;
	THParseSetPoint *m_listPoint;
	THFluidEngine *engine;
};

#endif
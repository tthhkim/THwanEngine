#ifndef TH_FLUID_PARSER
#define TH_FLUID_PARSER

#include <THFluid/THFluid.h>
#include <GameEngine/Util/THVectorArray.h>
#include <THPrivate.h>



class THParseSetGroup : public THLinkedNode
{
public:
	int data;
	bool isStatic;
	THParticleGroup *pointer;
	int gap;
};
class THParseSetPoint : public THLinkedNode
{
public:
	int data;
	THVector2Array *pointer;
};

class THFluidParser
{
public:
	~THFluidParser()
	{
		THParseSetGroup *p=(THParseSetGroup*)m_listGroup.GetList(),*np;
		while(p)
		{
			np=(THParseSetGroup*)p->GetLinkedNext();

			delete p;
			p=np;
		}

		THParseSetPoint *set=(THParseSetPoint*)m_listPoint.GetList(),*ns;
		while(set)
		{
			ns=(THParseSetPoint*)set->GetLinkedNext();
			delete set;
			set=ns;
		}
	}

	void AddSet(int data,THParticleGroup *group,int gap=1,bool isStatic=false);
	void AddSet(int data,THVector2Array *point);
	void Parse(THFluidEngine *_engine,unsigned char *data,size_t w,size_t h);
	

protected:
	THLinkedList m_listGroup;
	THLinkedList m_listPoint;
	THFluidEngine *engine;

	void CheckData(int data,const THVector2& pos,int x,int y);
};

#endif
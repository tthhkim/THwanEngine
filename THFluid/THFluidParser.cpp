#include "THFluidParser.h"

void THFluidParser::AddSet(int data,THParticleGroup *group,int _gap,bool _isStatic)
{
	THParseSetGroup *set=new THParseSetGroup;
	set->data=data;
	set->isStatic=_isStatic;
	set->pointer=group;
	set->gap=_gap;

	//set->next=m_listGroup;
	//if(m_listGroup){m_listGroup->prev=set;}
	//m_listGroup=set;
	m_listGroup.Push(set);
}
void THFluidParser::AddSet(int data,THVector2Array *point)
{
	THParseSetPoint *set=new THParseSetPoint;
	set->data=data;
	set->pointer=point;

	m_listPoint.Push(set);
}
void THFluidParser::CheckData(int data,const THVector2& pos,int x,int y)
{
	//if(data==0){return;}
	THParseSetGroup *p=(THParseSetGroup*)m_listGroup.GetList();
	while(p)
	{
		if(p->data==data)
		{
			if( p->isStatic || ( (x%p->gap==0)&&(y%p->gap==0) ) )
			{
				engine->AddParticle(p->pointer,pos);
			}
				
			m_listGroup.Delete(p);
			m_listGroup.Push(p);
			break;
		}
		p=(THParseSetGroup*)p->GetLinkedNext();
	}

	THParseSetPoint *set=(THParseSetPoint*)m_listPoint.GetList();
	while(set)
	{
		if(set->data==data)
		{
			set->pointer->Push(pos);
			//m_listPoint.Delete(set);
			//m_listPoint.Push(set);

			break;
		}
		set=(THParseSetPoint*)set->GetLinkedNext();
	}

}
/*
2bytes width
2bytes height
row-based

for(~~~)
	1byte data
*/
void THFluidParser::Parse(THFluidEngine *_engine,unsigned char *data,size_t w,size_t h)
{
	engine=_engine;

	const int width=w;
	const int height=h;

	//engine->LoadBoundaryMap(width,height);

	const THVector2 offs=engine->m_mingrid;
	THVector2 scale=engine->m_maxgrid-engine->m_mingrid;
	scale.x*=(1.0f/(float)width);
	scale.y*=(1.0f/(float)height);

	int i,j;
	for(j=0;j<height;++j)
	{
		for(i=0;i<width;++i)
		{
			CheckData(data[j*width + i],THVector2( (float)(i+0.5f)*scale.x + offs.x ,(float)(height-j-1+0.5f)*scale.y + offs.y ),i,j);
			/*
			if(num==0)
			{
				data+=2;
				num=data[0];
				group=data[1];
			}
			--num;
			CheckData( group, );
			*/
		}
	}
}
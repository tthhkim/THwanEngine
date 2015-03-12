#include "THFluidParser.h"

void THFluidParser::AddSet(int dataMin,int dataMax,THParticleGroup *group,int _gap,bool isStatic)
{
	THParseSetGroup *set=new THParseSetGroup;
	set->dataMin=dataMin;
	set->dataMax=dataMax;
	set->isStatic=isStatic;
	set->pointer=group;
	set->gap=_gap;

	set->next=m_listGroup;
	if(m_listGroup){m_listGroup->prev=set;}
	m_listGroup=set;
}
void THFluidParser::AddSet(int data,THVector2 *point)
{
	THParseSetPoint *set=new THParseSetPoint;
	set->data=data;
	set->pointer=point;

	set->next=m_listPoint;
	if(m_listPoint){m_listPoint->prev=set;}
	m_listPoint=set;
}
void THFluidParser::CheckData(int data,const THVector2& pos,int x,int y)
{
	//if(data==0){return;}
	THParseSetGroup *p=m_listGroup;
	while(p)
	{
		if(p->dataMin<=data && p->dataMax>data)
		{
			if(p->isStatic || ( (x%p->gap==0)&&(y%p->gap==0)) )
			{
				engine->AddParticle((THParticleGroup*)(p->pointer),pos);
			}
				

			if(p==m_listGroup){break;}
			p->prev->next=p->next;
			if(p->next){p->next->prev=p->prev;}

			p->next=m_listGroup;
			m_listGroup->prev=p;
			m_listGroup=p;

			break;
		}
		p=p->next;
	}

	THParseSetPoint *set=m_listPoint;
	while(set)
	{
		if(set->data==data)
		{
			*(set->pointer)=pos;

			if(set->prev){set->prev->next=set->next;}
			if(set->next){set->next->prev=set->prev;}
			if(set==m_listPoint){m_listPoint=set->next;}

			delete set;
			break;
		}
		set=set->next;
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
#include "THRopeGroup.h"

bool THRopeHanger::HitTest(const THVector2& p)
{
	return ((p-position).LengthSquared()<0.3f*0.3f);
}
void THRope::Step(float invdt)
{
	THVector2 delta=(m_left->position-m_p1->position)*invdt;
	m_p1->position=m_left->position;
	m_p1->velocity+=delta;

	delta=(m_right->position-m_p2->position)*invdt;
	m_p2->position=m_right->position;
	m_p2->velocity+=delta;

	for(unsigned int i=0;i<m_springs.num;++i)
	{
		m_springs.arr[i].Step(invdt);
	}
}
void THRope::DeleteParticles()
{
	THParticle *p=GetList(),*np;
	while(p)
	{
		np=(THParticle*)p->data.pointer;

		p->GetGrid()->Remove(p);
		p->GetGroup()->Remove(p);

		delete p;
		p=np;
	}
}

void THRopeGroup::Load(unsigned int hangerscap)
{
	SetCollideEach(false);
	SetMass(20.0f);
	//SetStatic(false);
	SetPressure(10.0f,0.5f,10.0f);
	SetAutoRemove(false);
	SetViscosity(10.0f);

	layer=(1<<DEFAULT_BIT);

	m_hangers.Load(hangerscap);
	m_temparr.Load(64);
	//m_temparr2.Load(64);

	m_clicked=0;
	m_ropes=0;
}
THRope* THRopeGroup::LoadRope(const THVector2 *arr,unsigned int count,float l0)
{
	THRope *rope=new THRope(count);
	THParticle *curp,*prevp;

	curp=m_engine->AddParticle(this,arr[0]);
	rope->m_p1=curp;
	prevp=curp;
	for(unsigned int i=1;i<count;++i)
	{
		curp=m_engine->AddParticle(this,arr[i]);
		curp->data.pointer=prevp;
		rope->m_springs.Push(THRopeSpring(prevp,curp,l0));
		prevp=curp;
	}
	rope->m_p2=curp;
	THLog("Rope Create : %d Springs",rope->m_springs.num);

	return rope;
}
void THRopeGroup::ClearHanger(THRopeHanger *h)
{
	THRope *r=(THRope*)h->m_ropes.GetList(),*nr;
	while(r)
	{
		nr=(THRope*)r->GetLinkedNext();

		DeleteRope(r);
		r->DeleteParticles();
		r->m_left->DeleteRope(r);
		r->m_right->DeleteRope(r);
		delete r;

		r=nr;
	}
}
void THRopeGroup::Step(float invdt)
{
	for(unsigned int i=0;i<m_hangers.num;++i)
	{
		m_hangers.arr[i].Step(invdt);
	}
	THRope *r=m_ropes;
	while(r)
	{
		r->Step(invdt);
		r=r->m_rnext;
	}
}
THRopeHanger *THRopeGroup::OnTouchDown(const THVector2& p)
{
	for(unsigned int i=0;i<m_hangers.num;++i)
	{
		if(m_hangers.arr[i].HitTest(p))
		{
			THLog("Hanger Chosen : %d",i);
			m_clicked=&m_hangers.arr[i];
			m_temparr.Clear();
			m_temparr.Push(p);
			return m_clicked;
		}
	}
	return 0;
}
THRopeHanger *THRopeGroup::OnTouchMove(const THVector2& p)
{
	if(m_clicked&&(m_temparr.GetLast()-p).LengthSquared()>ROPE_GAP*ROPE_GAP)
	{
		m_temparr.Push(p);
	}
	for(unsigned int i=0;i<m_hangers.num;++i)
	{
		if(m_hangers.arr[i].HitTest(p))
		{
			THLog("Move Hanger Chosen : %d",i);
			return &m_hangers.arr[i];
		}
	}
	return 0;
}
THRopeHanger *THRopeGroup::OnTouchUp(const THVector2& p)
{
	THRopeHanger *clicked=0;

	if(m_clicked==0){return 0;}
	for(unsigned int i=0;i<m_hangers.num;++i)
	{
		THRopeHanger& hanger=m_hangers.arr[i];
		if(hanger.HitTest(p))
		{
			clicked=&hanger;
			if(&hanger==m_clicked)
			{
				THLog("Hanger Chosen Rope deleting.. : %d",i);
				ClearHanger(m_clicked);
				break;
			}else
			{
				THLog("Hanger Chosen Rope making.. : %d",i);
			
				m_temparr.Push(p);
				//m_temparr2.Clear();
				//m_temparr.Bridge(m_temparr2,ROPE_GAP);

				MakeRope(m_clicked,&hanger,m_temparr.arr,m_temparr.num);
				
				break;
			}
		}
	}
	m_clicked=0;
	m_temparr.Clear();
	//m_temparr2.Clear();

	return clicked;
}
void THRopeGroup::AddRope(THRope *r)
{
	r->m_rnext=m_ropes;
	if(m_ropes)
	{
		m_ropes->m_rprev=r;
	}
	m_ropes=r;
}
void THRopeGroup::DeleteRope(THRope *r)
{
	if(r->m_rprev)
	{
		r->m_rprev->m_rnext=r->m_rnext;
	}else
	{
		m_ropes=r->m_rnext;
	}
	if(r->m_rnext)
	{
		r->m_rnext->m_rprev=r->m_rprev;
	}
}
void THRopeGroup::MakeRope(THRopeHanger *h1,THRopeHanger *h2,const THVector2 *arr,unsigned int count)
{
	THRope *r=LoadRope(arr,count,ROPE_GAP);

	h1->AddRope(r);
	h2->AddRope(r);
	r->m_left=h1;
	r->m_right=h2;

	AddRope(r);
}
void THRopeGroup::MakeRope(THRopeHanger *h1,THRopeHanger *h2,float coeff)
{
	THVector2 rel=h2->position-h1->position;
	const THVector2& p0=h1->position;
	float l0=rel.Normalize();
	float count=ceilf(l0*coeff/ROPE_GAP);
	unsigned int ucount=(unsigned int)count;

	rel*=(l0/count);

	THVector2Array varr(ucount+2);
	for(unsigned int i=0;i<ucount;++i)
	{
		varr.Push(p0 + (float)i*rel);
	}
	
	THRope *r=LoadRope(varr.arr,ucount,ROPE_GAP);

	h1->AddRope(r);
	h2->AddRope(r);
	r->m_left=h1;
	r->m_right=h2;

	AddRope(r);
}
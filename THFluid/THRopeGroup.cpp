#include "THRopeGroup.h"

bool THRopeHanger::HitTest(const THVector2& p)
{
	return ((p-position).LengthSquared()<0.3f*0.3f);
}
void THRope::Step(float invdt)
{
	THLog("Springs step %d",m_springs.num);
	for(unsigned int i=0;i<m_springs.num;++i)
	{
		m_springs.arr[i].Step(invdt);
	}
}
void THRope::Delete()
{
	m_left->m_linked=0;
	m_left->m_linkedp=0;
	m_right->m_linked=0;
	m_right->m_linkedp=0;

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
	SetMass(10.0f);
	//SetStatic(false);
	SetPressure(10.0f,0.5f,10.0f);
	SetAutoRemove(false);

	layer=(1<<DEFAULT_BIT);

	m_hangers.Load(hangerscap);
	m_temparr.Load(64);
	m_temparr2.Load(64);

	m_clicked=0;
}
THRope* THRopeGroup::LoadRope(const THVector2 *arr,unsigned int count)
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
		rope->m_springs.Push(THRopeSpring(prevp,curp));
		prevp=curp;
	}
	rope->m_p2=curp;
	THLog("Rope Create : %d Springs",rope->m_springs.num);

	return rope;
}
void THRopeGroup::DeleteRope(THRope *r)
{
	m_ropes.Delete(r);
	r->Delete();
	delete r;
}
void THRopeGroup::Step(float invdt)
{
	for(unsigned int i=0;i<m_hangers.num;++i)
	{
		m_hangers.arr[i].Step(invdt);
	}
	THRope *r=(THRope*)m_ropes.GetList();
	while(r)
	{
		THLog("RopeStep");
		r->Step(invdt);
		r=(THRope*)r->GetLinkedNext();
	}
}
THRopeHanger *THRopeGroup::OnTouchDown(const THVector2& p)
{
	for(unsigned int i=0;i<m_hangers.num;++i)
	{
		if(m_hangers.arr[i].HitTest(p)&&(m_hangers.arr[i].IsHanged()==false))
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
	if(m_clicked&&(m_temparr.GetLast()-p).LengthSquared()>0.2f*0.2f)
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
		if(hanger.HitTest(p)&&(&hanger!=m_clicked)&&(hanger.IsHanged()==false))
		{
			clicked=&hanger;

			THLog("Hanger Chosen Rope making.. : %d",i);
			THParticlePair pair;
			
			m_temparr2.Clear();
			m_temparr.Bridge(m_temparr2,0.24f);

			THRope *r=LoadRope(m_temparr2.arr,m_temparr2.num);

			m_clicked->m_linked=r;
			m_clicked->m_linkedp=r->m_p1;
			hanger.m_linked=r;
			hanger.m_linkedp=r->m_p2;
			r->m_left=m_clicked;
			r->m_right=&hanger;

			m_ropes.Push(r);
			break;
		}
	}
	m_temparr.Clear();
	m_temparr2.Clear();

	return clicked;
}
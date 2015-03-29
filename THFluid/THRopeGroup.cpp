#include "THRopeGroup.h"
/*
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
*/
void THRopeGroup::Load(unsigned int springcap)
{
	SetCollideEach(false);
	SetMass(25.0f);
	//SetStatic(false);
	SetPressure(10.0f,0.5f,10.0f);
	SetAutoRemove(false);
	SetResistance(10.0f);
	SetCollisionListener(this);

	layer=(1<<DEFAULT_BIT)|(1<<ROPE_BIT)|(1<<ROPE_DAMP_BIT);

	m_springs.Load(springcap);
	//m_springs.Clear();
	m_temparr.Load(64);
}
void THRopeGroup::LoadRope(const THVector2 *arr,unsigned int count)
{
	THParticle *curp=0,*prevp=0;

	curp=m_engine->AddParticle(this,arr[0]);
	THParticlePair pair(0,curp);
	m_engine->QueryCircle(arr[0],ROPE_GAP,1<<ROPE_DAMP_BIT,this,&pair);
	for(unsigned int i=1;i<count;++i)
	{
		prevp=curp;
		curp=m_engine->AddParticle(this,arr[i]);
		//curp->data.pointer=prevp;
		m_springs.Push(THRopeSpring(prevp,curp,ROPE_GAP));
	}
	pair.p1=prevp;
	pair.p2=curp;
	m_engine->QueryCircle(arr[count-1],ROPE_GAP,1<<ROPE_DAMP_BIT,this,&pair);
}
void THRopeGroup::LoadRope(const THVector2& p1,const THVector2& p2,float factor)
{
	THVector2 rel=p2-p1;
	float l=rel.Normalize();
	rel*=ROPE_GAP/factor;
	int count=(int)(l*factor/ROPE_GAP);
	THVector2Array varr(count+3);
	for(int i=0;i<count;++i)
	{
		varr.Push(p1 + rel*float(i));
	}
	varr.Push(p2);

	LoadRope(varr.arr,varr.num);
}
void THRopeGroup::FindSpringAndDelete(THParticle *p)
{
	unsigned int i=m_springs.num;
	while(i)
	{
		--i;
		if(m_springs.arr[i].Contains(p))
		{
			m_springs.DeleteBack(i);
			++i;
			//p->GetGrid()->Remove(p);
			//p->GetGroup()->Remove(p);
			//delete p;
		}
	}
	p->GetGrid()->Remove(p);
	p->GetGroup()->Remove(p);
	delete p;
}
void THRopeGroup::DeleteRope(const THVector2& p)
{
	m_engine->QueryCircle(p,ROPE_GAP,1<<ROPE_BIT,this,0);
}
bool THRopeGroup::QueryCallback(THParticle *particle,void *data)
{
	if(data==0)
	{
		FindSpringAndDelete(particle);
		return true;
	}
	const THParticlePair& pair=*(THParticlePair*)data;
	if(particle!=pair.p1&&particle!=pair.p2)
	{
		m_springs.Push(THRopeSpring(particle,pair.p2));
		//return false;
	}
	return true;
}
int THRopeGroup::ParticleCollide(THParticle *p1,THParticle *p2,float fraction)
{
	if(p2->GetGroup()->layer&(1<<ROPE_DAMP_BIT)){return 1;}
	return 0;
}
void THRopeGroup::TestStep(const THVector2& grav)
{
	THParticle *p;
	p=list;
	while(p)
	{
		p->position+=grav;
		p=p->GetNext();
	}
}
void THRopeGroup::Step(float invdt)
{
	for(unsigned int i=0;i<m_springs.num;++i)
	{
		m_springs.arr[i].Step(invdt);
	}
}
bool THRopeGroup::OnTouchDown(const THVector2& p)
{
	m_temparr.Push(p);
	return false;
}
bool THRopeGroup::OnTouchMove(const THVector2& p)
{
	if((m_temparr.GetLast()-p).LengthSquared()>ROPE_GAP*ROPE_GAP)
	{
		m_temparr.Push(p);
	}
	return false;
}
bool THRopeGroup::OnTouchUp(const THVector2& p)
{
	if(m_temparr.num<=2)
	{
		DeleteRope(p);
		//Deleting rope
	}else
	{
		m_temparr.Push(p);
		LoadRope(m_temparr.arr,m_temparr.num);
	}
	m_temparr.Clear();
	return false;
}
/*
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
*/
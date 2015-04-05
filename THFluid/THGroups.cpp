#include "THGroups.h"


void THWaterGroup::Load()
{
	m_mass.SetMass(1.0f);
	SetPressure(17.0f,0.7f,15.0f);
	SetResistance(0.8f);

	layer=(1<<DEFAULT_BIT)|(1<<WATER_BIT);
}
void THWaterGroup::Create(const THVector2& p,float gap)
{
	m_engine->AddParticle(this,p+THVector2(THRandf(-gap,gap),THRandf(-gap,gap)));
}
void THRopeDampGroup::Load()
{
	SetCollideEach(false);
	m_mass.SetMass(2.0f,0.0f);
	SetPressure(10.0f,0.4f,6.0f);

	layer=(1<<ROPE_DAMP_BIT);
}
void THBoundaryGroup::Load()
{
	SetCollideEach(false);
	m_mass.SetMass(2.0f,0.0f);
	SetPressure(10.0f,0.4f,6.0f);
	SetFriction(1.0f);

	layer=(1<<DEFAULT_BIT)|(1<<BOUNDARY_BIT);
}
bool THBoundaryGroup::QueryCallback(THParticle *particle,void *data)
{
	particle->GetGrid()->Remove(particle);
	particle->GetGroup()->Remove(particle);
	delete particle;
	return true;
}
bool THBoundaryGroup::QueryCellCallback(THCell *cell,void *data)
{
	if(cell->particle==0)
	{
		cell->particle=m_engine->AddParticle(this,cell->position);
	}
	return true;
}

void THWheelBody::Load()
{
	SetCollideEach(false);
	m_mass.SetMass(0.7f);
	SetPressure(10.0f,0.5f,10.0f);

	layer=(1<<DEFAULT_BIT)|(1<<WHEEL_BIT);
	
	SetCollisionListener(this);
}
void THWheelBody::LoadParticles(const THVector2& position,float length,float gap,THFluidEngine *engine)
{
	const int count=5;
	const float ainc=TH_2PI/(float)count;
	float theta;
	const float hg=gap*0.5f;
	for(int i=0;i<count;++i)
	{
		theta=i*(float)count;
		const THRot2 rotmat(theta);

		for(float y=0.0f;y<length;y+=gap)
		{
			engine->AddParticle(this,position + rotmat*THVector2(-hg,y));
			engine->AddParticle(this,position + rotmat*THVector2(hg,y));
		}
	}

	CalculateMass();
	CalculateInertia();
	m_bodyinertia.SetInvMass(0.0f);
}
int THWheelBody::ParticleCollide(THParticle *p1,THParticle *p2,float fraction)
{
	if(p2->GetGroup()->layer & ((1<<WATER_BIT)) )
	{
		return 0;
	}
	return 1;
}

void THFlameGroup::Load()
{
	SetCollideEach(true);
	m_mass.SetMass(0.5f);
	SetPressure(10.0f,0.8f,5.0f);

	SetGravityScale(-1.0f);

	layer=(1<<FLAME_BIT)|(1<<BOUNDARY_BIT);
}
void THFlameGroup::Step(float dt)
{
	THParticle *particle=list,*np;
	while(particle)
	{
		np=particle->GetNext();
		
		particle->data.fdata+=dt;
		if(particle->data.fdata>m_lasttime)
		{
			particle->GetGrid()->Remove(particle);
			Remove(particle);
			delete particle;
		}

		particle=np;
	}
}
void THFlameGroup::CreateNew(const THVector2& position)
{
	float theta=THRandf(-TH_PI,TH_PI);
	m_engine->AddParticle(this,position + THRandf(0.0f,m_radius)*THVector2(cosf(theta),sinf(theta)));
}

void THStone::Load()
{
	SetCollideEach(false);
	m_mass.SetMass(1.0f);
	SetPressure(10.0f,0.5f,10.0f);
	SetResistance(1.0f);
	SetFriction(100.0f);

	layer=(1<<DEFAULT_BIT)|(1<<ROPE_DAMP_BIT);
}

void THClickGroup::Load()
{
	SetCollideEach(false);
	m_mass.SetMass(2.0f,0.0f);
	SetPressure(10.0f,0.5f,10.0f);
	SetFriction(100.0f);

	layer=(1<<DEFAULT_BIT);
}
void THClickGroup::DeleteAll()
{
	THParticle *p=list,*np;
	while(p)
	{
		np=p->GetNext();

		p->GetGrid()->Remove(p);
		p->GetGroup()->Remove(p);
		delete p;

		p=np;
	}
}
bool THClickGroup::HitTest(const THVector2& p)
{
	bool a=false;
	m_engine->QueryCircle(p,0.1f,0xffffffff,this,&a);
	return a;
}

bool THClickGroup::QueryCallback(THParticle *particle,void *data)
{
	if(particle->GetGroup()==this)
	{
		*((bool*)data)=true;
		return false;
	}
	return true;
}


void THEndPoint::Load()
{
	SetCollideEach(true);
	m_mass.SetMass(0.5f);
	SetPressure(10.0f,0.8f,5.0f);
	SetResistance(0.5f);
	SetGravityScale(0.0f);

	layer=(1<<END_BIT);
}
bool THEndPoint::QueryCallback(THParticle *particle,void *data)
{
	THVector2 rel=position-particle->position;
	float x=rel.Normalize()/TH_ENDPOINT_RADIUS;
	if(x<TH_ENDPOINT_END_FACTOR)
	{
		particle->GetGroup()->Remove(particle);
		particle->GetGrid()->Remove(particle);

		//this->Add(particle);

		//++m_pcount;

		delete particle;

		Create();
	}
	else
	{
		x=1.0f-x;
		float s=2.0f*sqrtf(1.0f-x*x)*x;
	
		particle->force+=(s*k)*rel;
	}
	
	return true;
}
void THEndPoint::Step()
{
	m_engine->QueryCircle(position,TH_ENDPOINT_RADIUS,1u<<WATER_BIT,this,0);

	THParticle *p=list;
	THVector2 rel;
	float x;
	while(p)
	{
		rel=position-p->position;
		x=rel.Normalize()/TH_ENDPOINT_RADIUS;
		x=1.0f-x;
		float s=2.0f*sqrtf(1.0f-x*x)*x;
	
		p->force+=(s*3.0f)*rel;

		p=p->GetNext();
	}
}
void THEndPoint::Create()
{
	float theta=THRandf(-TH_PI,TH_PI);
	m_engine->AddParticle(this,position + THRandf(0.0f,TH_ENDPOINT_RADIUS)*THVector2(cosf(theta),sinf(theta)));
}
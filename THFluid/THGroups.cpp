#include "THGroups.h"


void THWaterGroup::Load()
{
	SetMass(1.0f);
	SetPressure(10.0f,0.6f,15.0f);
	SetViscosity(0.0f);

	layer=(1<<DEFAULT_BIT)|(1<<WATER_BIT);
}
void THWaterGroup::Step()
{
	THParticle *particle=list;
	while(particle)
	{
		if(particle->position.y<2.0f)
		{
			particle->force.y=1000.0f*(2.0f-particle->position.y);
		}

		particle=particle->GetNext();
	}
}

void THSteamGroup::Load()
{
	SetMass(0.8f);
	SetPressure(5.0f,0.5f,15.0f);
	SetViscosity(1.0f);
	gravityScale=-1.0f;

	layer=(1<<DEFAULT_BIT)|(1<<STEAM_BIT);
}

void THBoundaryGroup::Load()
{
	SetCollideEach(false);
	SetMass(2.0f);
	SetStatic(true);
	SetPressure(10.0f,0.4f,6.0f);

	layer=(1<<DEFAULT_BIT)|(1<<BOUNDARY_BIT);
}
bool THBoundaryGroup::QueryCallback(THParticle *particle,void *data)
{
	particle->GetGrid()->Remove(particle);
	particle->GetGroup()->Remove(particle);
	delete particle;
	return true;
}

void THWheelBody::Load()
{
	SetCollideEach(false);
	SetMass(0.7f);
	SetStatic(false);
	SetPressure(10.0f,0.5f,10.0f);
	angularDamping=0.4f;

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
	b_invMass=0.0f;
}
int THWheelBody::ParticleCollide(THParticle *p1,THParticle *p2,float fraction)
{
	if(p2->GetGroup()->layer & ((1<<WATER_BIT)|(1<<STEAM_BIT)))
	{
		return 0;
	}
	return 1;
}

void THFlameGroup::Load()
{
	SetCollideEach(true);
	SetMass(0.5f);
	SetStatic(false);
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



void THParticleRope::Load(unsigned int springcap)
{
	SetCollideEach(false);
	SetMass(10.0f);
	//SetStatic(false);
	SetPressure(10.0f,0.5f,10.0f);
	SetAutoRemove(false);

	layer=(1<<DEFAULT_BIT);

	m_springs.Load(springcap);
}
void THParticleRope::LoadParticles(const THVector2 *arr,unsigned int count,THParticlePair *endp)
{
	THParticle *curp;

	curp=m_engine->AddParticle(this,arr[0]);
	if(endp)
	{
		endp->p1=curp;
	}
	for(unsigned int i=1;i<count;++i)
	{
		curp=m_engine->AddParticle(this,arr[i]);
	}
	if(endp)
	{
		endp->p2=curp;
	}
}
void THParticleRope::Step(float k)
{
	for(unsigned int i=0;i<m_springs.num;++i)
	{
		m_springs.arr[i].Step(k);
	}
}
void THParticleRope::LoadSprings(float thre)
{
	float tsq=thre*thre;
	float lsq;
	THParticle *p1,*p2;
	p1=list;
	while(p1)
	{
		p2=p1->GetNext();
		while(p2)
		{
			lsq=(p1->position-p2->position).LengthSquared();
			if(lsq<tsq)
			{
				m_springs.Push(THParticleSpring(p1,p2));
			}

			p2=p2->GetNext();
		}
		p1=p1->GetNext();
	}
}
#include "THFluidObject.h"
#include <assert.h>

void THParticleGroup::SetPressure(float _RestDensity,float _PressureK,float _NearPressureK)
{
	restDensity=_RestDensity;
	pressureK=_PressureK;
	nearPressureK=_NearPressureK;
}


void THParticleGroup::Add(THParticle *p)
{
	assert(p->group==0);
	++count;
	p->next=list;
	if(list){list->prev=p;}
	list=p;
	p->group=this;
}
void THParticleGroup::Remove(THParticle *p)
{
	assert(p->group==this);
	--count;
	if(p->next)
	{
		p->next->prev=p->prev;
	}
	if(p->prev)
	{
		p->prev->next=p->next;
	}
	if(p==list)
	{
		list=p->next;
	}
	p->next=0;
	p->prev=0;
	p->group=0;
}
void THParticleGroup::FreeAll()
{
	THParticle *particle=list,*nextp;
	while(particle)
	{
		nextp=particle->next;
		delete particle;
		particle=nextp;
	}
	list=0;
}
void THParticleGroup::CalculatePressure()
{
	THParticle *particle=list;
	while(particle)
	{
		//particle->p=pressureK*(particle->p-restDensity*0.01f*(100.0f-particle->kelvin));
		particle->p=pressureK*(particle->p-restDensity);
		particle->pnear*=nearPressureK;

		particle=particle->next;
	}
}
void THParticleGroup::ApplyDeltaForce(THParticle *p,const THVector2& delta,float invdt)
{
	p->position+=delta;
	p->velocity+=delta*invdt;
}
void THParticleGroup::ApplyForceAndAdvect(const THTimeStep& step)
{
	if(m_mass.m_isstatic)
	{
		THParticle *particle=list;
		while(particle)
		{
			particle->force.SetZero();

			particle=particle->next;
		}
		return;
	}
	THParticle *particle=list;
	const float invdt2=m_mass.m_invmass*step.dt2;
	const float invdt=m_mass.m_invmass*step.dt;

	THVector2 f;
	while(particle)
	{
		f=particle->force;

		particle->position+=f*invdt2;
		particle->velocity+=f*invdt;
		particle->force.SetZero();

		particle->position+=(particle->velocity*step.dt);
		
		particle=particle->next;
	}
}

void THParticleBody::CalculateMass()
{
	THParticle *particle=list;

	THVector2 pp(0.0f,0.0f);
	
	while(particle)
	{
		pp+=particle->position;

		particle=particle->next;
	}
	position=pp*(1.0f/(float)count);

	m_bodymass.SetMass(m_mass.m_mass*float(count));
}
void THParticleBody::CalculateInertia()
{
	float lsq=0.0f;
	THVector2 rel;
	THParticle *particle=list;
	while(particle)
	{
		rel=particle->position-position;
		lsq+=rel.LengthSquared();
		particle=particle->next;
	}
	m_bodyinertia.SetMass(m_mass.m_mass*lsq);
}
void THParticleBody::ApplyDeltaForce(THParticle *p,const THVector2& delta,float invdt)
{
	p->force+=delta*(invdt*invdt);
}
void THParticleBody::ApplyForceAndAdvect(const THTimeStep& step)
{
	THParticle *particle=list;
	THVector2 force(0.0f,0.0f);
	float torque=0.0f;
	while(particle)
	{
		torque+=THCrossScalar(particle->position-position,particle->force);
		force+=particle->force;
		particle->force.SetZero();

		particle=particle->next;
	}
	torque*=m_bodyinertia.m_invmass;
	force*=m_bodymass.m_invmass;

	float anginc=step.dt2*torque;
	THVector2 posinc=step.dt2*force;

	m_angularVelocity+=torque*step.dt;
	m_velocity+=step.dt*force;

	anginc+=m_angularVelocity*step.dt;
	posinc+=m_velocity*step.dt;
	const THRot2 rotmat(anginc);

	particle=list;
	THVector2 rel,p0;
	while(particle)
	{
		p0=particle->position;
		rel=particle->position-position;
		particle->position=position + (rotmat*rel) + posinc;
		particle->velocity=(particle->position-p0)*step.dt_inv;

		particle=particle->next;
	}
	position+=posinc;
	angle+=anginc;
	if(angle>TH_2PI){angle-=TH_2PI;}
	else if(angle<0.0f){angle+=TH_2PI;}

}

void THParticleBody::Append(const THVector2& pos)
{
	THParticle *particle=list;
	while(particle)
	{
		particle->position+=pos;

		particle=particle->next;
	}
}
void THParticleBody::SetPosition(const THVector2& pos)
{
	Append(pos-position);
}
void THParticleBody::Rotate(float _angle)
{
	angle+=_angle;
	const THRot2 rot(_angle);
	THParticle *particle=list;
	THVector2 rel;
	while(particle)
	{
		rel=particle->position-position;

		particle->position=position+rot*rel;

		particle=particle->next;
	}
	if(angle>TH_2PI){angle-=TH_2PI;}
	else if(angle<0.0f){angle+=TH_2PI;}
}
void THParticleBody::SetAngle(float r)
{
	Rotate(r-angle);
}
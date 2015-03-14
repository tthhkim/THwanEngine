#ifndef TH_SPONGE_BODY
#define TH_SPONGE_BODY

#include <THPrivate.h>
#include <GameEngine/Util/THVectorArray.h>

#include <THFluid/THFluid.h>

#define DEFAULT_BIT  0
#define BOUNDARY_BIT 1
#define WATER_BIT 2
#define STEAM_BIT 3
#define WHEEL_BIT 4
#define FLAME_BIT 5
#define LIGHT_BIT 6


class THWaterGroup : public THParticleGroup
{
public:
	void Load();
	void Step();
protected:
};
class THSteamGroup : public THParticleGroup
{
public:
	void Load();
protected:
};

class THBoundaryGroup : public THParticleGroup,public THParticleQuery
{
public:
	void Load();
	void Delete(const THVector2& position,float radius)
	{
		m_engine->QueryCircle(position,radius,(1<<BOUNDARY_BIT),this,0);
	}
protected:

	bool QueryCallback(THParticle *particle,void *data);
};

class THWheelBody : public THParticleBody , public THParticleCollisionListener
{
public:
	void Load();
	void LoadParticles(const THVector2& position,float length,float gap,THFluidEngine *engine);
	float GetAngularVelocityAbs() const{return m_angularVelocity>0.0f?m_angularVelocity:-m_angularVelocity;}
protected:
	int ParticleCollide(THParticle *p1,THParticle *p2,float fraction);
};

class THFlameGroup : public THParticleGroup
{
public:
	void Load();
	void Step(float dt);
	void CreateNew(const THVector2& position);
	void SetFlame(float radius,float last)
	{
		m_radius=radius;
		m_lasttime=last;
		m_invtime=1.0f/last;
	}
	inline float GetSizeFactor(float t) const
	{
		//return 1.0f-t*m_invtime;
		return 1.0f/(t*2.0f+1.0f);
	}
protected:
	float m_radius;
	float m_lasttime,m_invtime;
};

class THParticleSpring
{
public:
	THParticle *p1,*p2;
	float l0;

	THParticleSpring(THParticle *_p1,THParticle *_p2)
	{
		p1=_p1;
		p2=_p2;
		
		l0=(p1->position-p2->position).Length();
	}
	void Step(float coeff)
	{
		THVector2 rel=p1->position-p2->position;
		float l=rel.Normalize();

		float k1=l-l0;
		float k2=1.0f-l0/(l+0.01f);

		//rel=rel*(k1*300.0f + k2*100.0f);
		//rel=rel*k1*4500.0f;
		rel=rel*k2*1500.0f;
		
		p2->force+=rel;
		p1->force-=rel;
	}
protected:
};

class THParticleRope : public THParticleGroup
{
public:
	void Load(unsigned int springcap);
	void LoadParticles(const THVector2 *arr,unsigned int count,THParticlePair *endp);
	void Step(float k);
	void LoadSprings(float thre);
protected:
	THArray<THParticleSpring> m_springs;
};
#endif
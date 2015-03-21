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

class THBoundaryGroup : public THParticleGroup,public THParticleQuery,public THCellQuery
{
public:
	void Load();
	void Delete(const THVector2& position,float radius)
	{
		m_engine->QueryCircle(position,radius,(1<<BOUNDARY_BIT),this,0);
	}
	void Create(const THVector2& p,float r)
	{
		m_engine->QueryCellCircle(p,r,this,0);
	}
protected:

	bool QueryCallback(THParticle *particle,void *data);
	bool QueryCellCallback(THCell *cell,void *data);
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
#endif
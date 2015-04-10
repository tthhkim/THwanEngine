#ifndef TH_SPONGE_BODY
#define TH_SPONGE_BODY

#include <THPrivate.h>
#include <GameEngine/Util/THVectorArray.h>

#include <THFluid/THFluid.h>

#define DEFAULT_BIT  0
#define BOUNDARY_BIT 1
#define WATER_BIT 2
#define WHEEL_BIT 4
#define FLAME_BIT 5
#define ROPE_BIT 6
#define ROPE_DAMP_BIT 7
#define END_BIT 8


class THWaterGroup : public THParticleGroup
{
public:
	void Load();
	void Create(const THVector2& p,float gap);
protected:
};

class THRopeDampGroup : public THParticleGroup
{
public:
	void Load();
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

class THStone : public THParticleBody
{
public:
	void Load();
protected:
};

class THClickGroup : public THParticleGroup,public THParticleQuery
{
public:
	void Load();
	bool HitTest(const THVector2& p);
	void DeleteAll();
	bool QueryCallback(THParticle *particle,void *data);
protected:
};
class THMassBody : public THParticleBody
{
public:
	void Load(float m);
};

#define TH_ENDPOINT_RADIUS 0.7f
#define TH_ENDPOINT_END_FACTOR 0.3f
class THEndPoint : public THParticleQuery , public THParticleGroup
{
public:
	THVector2 position;
	float k;

	THEndPoint()
	{
		k=1.0f;
	}

	void Load();
	void Create();
	bool QueryCallback(THParticle *particle,void *data);
	inline unsigned int GetPositiveCount() const{return m_pcount;}
	inline unsigned int GetNegativeCount() const{return m_ncount;}
	void Refresh(){m_pcount=0;m_ncount=0;}
	void Step();
protected:
	unsigned int m_pcount,m_ncount;
};
#endif
#ifndef TH_FLUID_OBJECTS
#define TH_FLUID_OBJECTS

#include <GameEngine/Util/THMath3D.h>
#include <THFluid/THParticleCallbacks.h>

union THDataUnion
{
	void *pointer;
	float fdata;
	int idata;
	unsigned int uidata;
};
class THTimeStep
{
public:
	float dt;
	float dt2;
	float dt_inv;

	THTimeStep(float m_dt)
	{
		dt=m_dt;
		dt2=m_dt*m_dt;
		dt_inv=1.0f/m_dt;
	}
};

typedef unsigned int THParticleFilter;
class THFluidEngine;
class THGrid;
class THParticleGroup;
class THParticleBody;
class THParticle
{
	friend class THFluidEngine;
	friend class THGrid;
	friend class THParticleGroup;
	friend class THParticleBody;
public:
	THVector2 position;
	THVector2 velocity;
	THVector2 force;
	union THDataUnion data;

	THParticle():velocity(0.0f,0.0f),force(0.0f,0.0f)
	{
		gnext=0;
		gprev=0;
		next=0;
		prev=0;
		grid=0;
		data.idata=0;
	}
	inline THParticle* GetNext() const
	{
		return next;
	}
	inline THParticle* GetPrev() const
	{
		return prev;
	}
	inline THParticleGroup*& GetGroup()
	{
		return group;
	}
	inline THGrid*& GetGrid()
	{
		return grid;
	}
	inline float GetP() const{return p;}

	
protected:
	float p,pnear;

	THParticle *gnext,*gprev;
	THParticle *next,*prev;

	THParticleGroup *group;
	THGrid *grid;
};

struct THNeighbor
{
	THParticle *p1,*p2;
	THVector2 one2two;
	float q,q2;
};

class THCell
{
public:
	THParticle *particle;
	//THParticleFilter layer;
	THVector2 position;
};

class THParticleGroup
{
	friend class THFluidEngine;
public:
	THParticle *list;
	THParticleFilter layer;
	int count;

	THParticleGroup()
	{
		gravityScale=1.0f;
		viscosity=0.0f;

		list=0;
		layer=0xffffffff;
		count=0;
		m_collideEach=true;
		m_isStatic=false;
		m_autoremove=true;

		collisionListener=0;

		m_engine=0;
	}
	void Add(THParticle *p);
	void Remove(THParticle *p);
	void FreeAll();
	

	void SetMass(float m);
	void SetPressure(float _RestDensity,float _PressureK,float _NearPressureK);
	void SetStatic(bool _isStatic);
	void SetViscosity(float v){viscosity=v;}
	void SetGravityScale(float s){gravityScale=s;}
	void SetCollideEach(bool isCollide){m_collideEach=isCollide;}
	inline float GetMass() const{return m_mass;}
	void SetCollisionListener(THParticleCollisionListener *listener){collisionListener=listener;}
	inline THFluidEngine* GetEngine() const{return m_engine;}
	inline void SetAutoRemove(bool ar){m_autoremove=ar;}

protected:
	THFluidEngine *m_engine;

	bool m_isStatic;

	float m_mass;
	float m_invMass;
	float gravityScale;
	float restDensity;
	float pressureK;
	float nearPressureK;
	float viscosity;

	bool m_collideEach;
	bool m_autoremove;

	THParticleCollisionListener *collisionListener;

	void CalculatePressure();
	virtual void ApplyForceAndAdvect(const THTimeStep& step);
};
class THParticleBody : public THParticleGroup
{
	friend class THFluidEngine;
public:
	THVector2 position;
	float angle;
	float angularDamping;
	float b_invMass,b_invInertia;
	

	THParticleBody():THParticleGroup()
	{
		m_collideEach=false;
		angle=0.0f;
		angularDamping=0.0f;
	}

	void CalculateMass();
	void CalculateInertia();
	void Append(const THVector2& pos);
	void SetPosition(const THVector2& pos);
	void Rotate(float angle);
	void SetAngle(float r);
	
protected:
	float m_angularVelocity;
	THVector2 m_velocity;
	

	void ApplyForceAndAdvect(const THTimeStep& step);
};
class THGrid
{
	friend class THFluidEngine;

public:
	THGrid()
	{
	}
	inline void Init(int _cx,int _cy)
	{
		x=_cx;
		y=_cy;
		list=0;
	}
	void Add(THParticle *p)
	{
		p->gnext=list;
		if(list){list->gprev=p;}
		list=p;
		p->grid=this;
	}
	void Remove(THParticle *p)
	{
		if(p->gnext)
		{
			p->gnext->gprev=p->gprev;
		}
		if(p->gprev)
		{
			p->gprev->gnext=p->gnext;
		}
		if(p==list)
		{
			list=p->gnext;
		}
		p->gnext=0;
		p->gprev=0;
		p->grid=0;
	}
protected:
	int x,y;
	THParticle *list;
};

class THParticlePair
{
public:
	THParticle *p1,*p2;
	union THDataUnion data;

	THParticlePair(THParticle *_p1,THParticle *_p2)
	{
		p1=_p1;
		p2=_p2;
	}
	THParticlePair()
	{
		p1=p2=0;
	}
};
struct THRaycastOut
{
	THParticle *particle;
	THVector2 point;
};



#endif
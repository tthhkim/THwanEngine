#ifndef TH_ROPE_GROUP
#define TH_ROPE_GROUP

#define ROPE_GAP 0.175f


#include <THFluid/THGroups.h>
class THRopeGroup;
class THRopeSpring
{
public:
	THParticle *p1,*p2;
	float l0;

	THRopeSpring(THParticle *_p1,THParticle *_p2)
	{
		Set(_p1,_p2);
		l0=(p1->position-p2->position).Length();
	}
	THRopeSpring(THParticle *_p1,THParticle *_p2,float _l0)
	{
		Set(_p1,_p2);
		l0=_l0;
	}
	inline bool Contains(THParticle *p)
	{
		return p1==p || p2==p;
	}
	void Set(THParticle *_p1,THParticle *_p2)
	{
		/*
		if(_p1<_p2)
		{
			p1=_p1;
			p2=_p2;
		}else
		{
			p1=_p2;
			p2=_p1;
		}
		*/
		p1=_p1;
		p2=_p2;
	}
	void Step(float invdt)
	{
		THVector2 rel=p1->position-p2->position;
		float l=rel.Normalize();

		float delta=(l-l0); //extension is positive
		float p1m=p1->GetGroup()->GetInvMass();
		float p2m=p2->GetGroup()->GetInvMass();
		rel*=delta/(p1m+p2m);
		//rel*=(delta*0.5f);
		
		//p1m=p1->GetGroup()->GetInvMass();
		//p2m=p2->GetGroup()->GetInvMass();
		p1->position-=rel*p1m;
		p2->position+=rel*p2m;
		rel*=invdt;
		p1->velocity-=rel*p1m;
		p2->velocity+=rel*p2m;
	}
	void StepNVelocity()
	{
		THVector2 rel=p1->position-p2->position;
		float l=rel.Normalize();

		float delta=(l-l0); //extension is positive
		float p1m=p1->GetGroup()->GetInvMass();
		float p2m=p2->GetGroup()->GetInvMass();
		rel*=delta/(p1m+p2m);

		p1->position-=rel*p1m;
		p2->position+=rel*p2m;
	}
protected:
};
/*
class THRope : public THLinkedNode
{
	friend class THRopeGroup;
	friend class THRopeHanger;
	friend class THRopeHangerDef;
public:
	THRope(unsigned int cap):m_springs(cap)
	{
		m_rnext=0;
		m_rprev=0;
	}

	void Step(float invdt);
	inline THParticle *GetList(){return m_p2;}
protected:
	THParticle *m_p1,*m_p2;
	THRopeHanger *m_left,*m_right;
	THArray<THRopeSpring> m_springs;

	THRope *m_rnext,*m_rprev;

	void DeleteParticles();
};
*/


class THRopeGroup : public THParticleGroup,public THParticleQuery,public THParticleCollisionListener
{
public:
	void Load(unsigned int springcap);
	//void MakeRope(THRopeHanger *h1,THRopeHanger *h2,const THVector2 *arr,unsigned int count);
	//void MakeRope(THRopeHanger *h1,THRopeHanger *h2,float coeff);
	//void AddRope(THRope *r);
	//void DeleteRope(THRope *r);
	void LoadRope(const THVector2 *arr,unsigned int count);
	void LoadRope(const THVector2& p1,const THVector2& p2,float factor);
	void FindSpringAndDelete(THParticle *p);
	void DeleteRope(const THVector2& p);
	//void ClearHanger(THRopeHanger *h);

	void Step(float invdt);
	void TestStep(const THVector2& grav);

	bool OnTouchDown(const THVector2& p);
	bool OnTouchMove(const THVector2& p);
	bool OnTouchUp(const THVector2& p);

	inline const THVector2Array& GetTempArr() const
	{
		return m_temparr;
	}
	bool QueryCallback(THParticle *particle,void *data);
	int ParticleCollide(THParticle *p1,THParticle *p2,float fraction);
protected:
	//THRope *m_ropes;
	//THLinkedList m_ropes;

	THArray<THRopeSpring> m_springs;
	THVector2Array m_temparr;//,m_temparr2;
};


#endif
#ifndef TH_ROPE_GROUP
#define TH_ROPE_GROUP

#define ROPE_GAP 0.175f

#include <THFluid/THGroups.h>
class THRopeHanger;
class THRopeGroup;
class THRopeHangerDef;
class THRopeSpring
{
public:
	THParticle *p1,*p2;
	float l0;

	THRopeSpring(THParticle *_p1,THParticle *_p2)
	{
		p1=_p1;
		p2=_p2;
		
		l0=(p1->position-p2->position).Length();
	}
	THRopeSpring(THParticle *_p1,THParticle *_p2,float _l0)
	{
		p1=_p1;
		p2=_p2;
		l0=_l0;
	}
	void Step(float invdt)
	{
		THVector2 rel=p1->position-p2->position;
		float l=rel.Normalize();

		float delta=(l-l0); //extension is positive
		float p1m=p1->GetGroup()->GetMass();
		float p2m=p2->GetGroup()->GetMass();
		rel*=delta*p1m*p2m/(p1m+p2m);
		//rel*=(delta*0.5f);
		
		p1m=p1->GetGroup()->GetInvMass();
		p2m=p2->GetGroup()->GetInvMass();
		p1->position-=rel*p1m;
		p2->position+=rel*p2m;
		rel*=invdt;
		p1->velocity-=rel*p1m;
		p2->velocity+=rel*p2m;
		
		
	}
protected:
};
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
class THRopeHanger
{
	friend class THRopeGroup;
	friend class THRope;
public:
	THVector2 position;
	THParticle *syncedparticle;

	THRopeHanger()
	{
		syncedparticle=0;
	}
	bool HitTest(const THVector2& p);
	void AddRope(THRope *r)
	{
		m_ropes.Push(r);
	}
	void DeleteRope(THRope *r)
	{
		m_ropes.Delete(r);
	}
	bool IsHanged()
	{
		return m_ropes.GetList()!=0;
	}
	void Step(float dtinv)
	{
		if(syncedparticle){position=syncedparticle->position;}
	}

protected:
	THLinkedList m_ropes;
};



class THRopeGroup : public THParticleGroup
{
public:
	void Load(unsigned int hangerscap);
	void MakeRope(THRopeHanger *h1,THRopeHanger *h2,const THVector2 *arr,unsigned int count);
	void MakeRope(THRopeHanger *h1,THRopeHanger *h2,float coeff);
	void AddRope(THRope *r);
	void DeleteRope(THRope *r);
	THRope* LoadRope(const THVector2 *arr,unsigned int count,float l0);
	void ClearHanger(THRopeHanger *h);

	void Step(float invdt);

	THRopeHanger *OnTouchDown(const THVector2& p);
	THRopeHanger *OnTouchMove(const THVector2& p);
	THRopeHanger *OnTouchUp(const THVector2& p);

	THRopeHanger *NewHanger()
	{
		m_hangers.Push(THRopeHanger());
		return &m_hangers.GetLast();
	}
	THArray<THRopeHanger>& GetHangerList()
	{
		return m_hangers;
	}
	inline const THVector2Array& GetTempArr() const
	{
		return m_temparr;
	}
protected:
	THRope *m_ropes;
	//THLinkedList m_ropes;

	THArray<THRopeHanger> m_hangers;
	THRopeHanger *m_clicked;
	THVector2Array m_temparr;//,m_temparr2;
};


#endif
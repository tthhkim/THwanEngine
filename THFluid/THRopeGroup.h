#ifndef TH_ROPE_GROUP
#define TH_ROPE_GROUP

#include <THFluid/THGroups.h>
class THRopeHanger;
class THRopeGroup;
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
		//rel*=delta*invdt*invdt*p1m*p2m/(p1m+p2m);
		rel*=(delta*0.5f);

		//p1->force-=rel;
		//p2->force+=rel;
		
		p1->position-=rel;
		p2->position+=rel;
		rel*=invdt;
		p1->velocity-=rel;
		p2->velocity+=rel;
		
	}
protected:
};
class THRope : public THLinkedNode
{
	friend class THRopeGroup;
	friend class THRopeHanger;
public:
	THRope(unsigned int cap):m_springs(cap)
	{
	}

	void Step(float invdt);
	inline THParticle *GetList(){return m_p2;}
protected:
	THParticle *m_p1,*m_p2;
	THRopeHanger *m_left,*m_right;
	THArray<THRopeSpring> m_springs;

	void Delete();
};
class THRopeHanger
{
	friend class THRopeGroup;
	friend class THRope;
public:
	THVector2 position;

	THRopeHanger()
	{
		m_linked=0;
		m_linkedp=0;
	}
	bool HitTest(const THVector2& p);
	bool IsHanged()
	{
		return m_linked!=0;
	}
	void Step(float dtinv)
	{
		if(m_linkedp)
		{
			THVector2 delta=(position-m_linkedp->position)*dtinv;
			m_linkedp->position=position;
			m_linkedp->velocity+=delta;
		}
	}

protected:
	THRope *m_linked;
	THParticle *m_linkedp;
};



class THRopeGroup : public THParticleGroup
{
public:
	void Load(unsigned int hangerscap);
	THRope* LoadRope(const THVector2 *arr,unsigned int count);
	void DeleteRope(THRope *r);

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
protected:
	THLinkedList m_ropes;

	THArray<THRopeHanger> m_hangers;
	THRopeHanger *m_clicked;
	THVector2Array m_temparr;
};


#endif
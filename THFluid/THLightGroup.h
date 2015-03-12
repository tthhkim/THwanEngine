#ifndef TH_LIGHT_GROUP
#define TH_LIGHT_GROUP

#include <THPrivate.h>
#include <THFluid/THGroups.h>

class THLightManager;
class THLightShader : public THProgram
{
public:
	void Load();
	inline void SyncProjection(const THVector2& minp,const THVector2& maxp) const
	{
		GLfloat proj[6];
		THOrthoMatrix33(proj,minp,maxp);
		glUniform3fv(GetUniformLocation("projMat"),2,proj);
	}
	inline void PreDraw() const
	{
		glEnableVertexAttribArray(vertexHandler);
	}
	inline void PostDraw() const
	{
		glDisableVertexAttribArray(vertexHandler);
	}
	
	inline void SetColor(float r,float g,float b,float a)
	{
		glUniform4f(colorHandler,r,g,b,a);
	}
	/*
	inline void SetPosition(const THVector2& p)
	{
		glUniform2f(positionHandler,p.x,p.y);
	}
	inline void SetRadius(float r)
	{
		glUniform1f(radiusHandler,r);
	}
	*/
	void Draw(const THArray<THVector2>& vertices,const THArray<unsigned int>& indices);

protected:
	GLint vertexHandler;
	GLint colorHandler;//,positionHandler,radiusHandler;
};

bool THLineRaycast(const THVector2& p0,const THVector2& dir,float& maxfraction,const THVector2& l1,const THVector2& l2);

class THShape
{
public:
	inline THRot2& GetAngle(){return m_angle;}
	inline THVector2& GetPosition(){return m_position;}

	virtual void ApplyAttributes()=0;
	virtual bool HitTest(const THVector2& p)=0;
	virtual bool Raycast(const THVector2& p0,const THVector2& dir,float& maxfraction)=0;
protected:
	THRot2 m_angle;
	THVector2 m_position;
};
class THBoxShape : public THShape
{
public:
	void ApplyAttributes();
	bool HitTest(const THVector2& p);
	bool Raycast(const THVector2& p0,const THVector2& dir,float& maxfraction);
	inline THVector2& GetHalfSize(){return m_halfsize;}
protected:
	THVector2 m_halfsize;
	THVector2 m_vertices[4];
};
class THCircleShape : public THShape
{
public:
	void ApplyAttributes();
	bool HitTest(const THVector2& p);
	bool Raycast(const THVector2& p0,const THVector2& dir,float& maxfraction);

	void SetRadius(float r){m_radius=r;m_radiussq=r*r;}
protected:
	float m_radius,m_radiussq;
};

class THLightManager : public THParticleGroup
{
public:
	THLightManager()
	{
	}
	~THLightManager()
	{
		m_shapes.DeleteAll();
	}
	inline float GetRadius() const{return m_radius;}

	void Load(int vertexcount,unsigned int shapescap);
	void LightTest(bool shaperefresh);
	void SetRadius(float r){m_radius=r;}
	void LoadParticles(float gap,int layer);
	inline THParticle *GetNextParticle()
	{
		if(m_nextparticle)
		{
			THParticle *rp=m_nextparticle;
			m_nextparticle=m_nextparticle->GetNext();
			return rp;
		}
		return m_engine->NewParticle(this);
	}
	void SetOutLeftParticles();
	void AddShape(THShape *s){m_shapes.Push(s);}
	inline THVector2& GetPosition(){return m_position;}
	bool HitTest(const THVector2& p)
	{
		return (p-m_position).LengthSquared()<m_radius*m_radius;
	}
	void OnTouchDown(const THVector2& p);
	void OnTouchMove(const THVector2& p);
	void OnTouchUp(const THVector2& p);

	void Draw(THLightShader& shader);
protected:
	THArray<THShape*> m_shapes;

	THArray<THVector2> m_vertices;
	THArray<unsigned int> m_indices;
	int m_verticescount;
	float m_angleincrease;

	THVector2 m_position;
	float m_radius;

	THParticle *m_nextparticle; 

	bool m_istouched;
	THVector2 m_touchrel;
};

#endif
#include "THLightGroup.h"

void THLightShader::Load()
{
	LoadFile("D:/Resources/Games/Fluid/lightshade.vs","D:/Resources/Games/Fluid/lightshade.fs");

	vertexHandler=GetAttribLocation("vert");
	
	colorHandler=GetUniformLocation("u_color");
	/*
	positionHandler=GetUniformLocation("u_lpos");
	radiusHandler=GetUniformLocation("u_radius");
	*/
}
void THLightShader::Draw(const THArray<THVector2>& vertices,const THArray<unsigned int>& indices)
{
	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,(const GLvoid*)vertices.arr);
	unsigned int i=0,i0;
	while(i<indices.num)
	{
		i0=indices.arr[i];
		glDrawArrays(GL_TRIANGLE_STRIP,i0,indices.arr[i+1]-i0);
		i+=2;
	}
}
void THLightManager::Draw(THLightShader& shader)
{
	//shader.SetPosition(m_position);
	//shader.SetRadius(m_radius);
	shader.Draw(m_vertices,m_indices);
}

bool THLineRaycast(const THVector2& p0,const THVector2& dir,float& maxfraction,const THVector2& l1,const THVector2& l2)
{
	THVector2 rel=l2-l1;
	const THMatrix22 mat(
		dir.x,rel.x,
		dir.y,rel.y
		);
	const float d=mat.Discriminant();
	if(d==0.0f){return false;}
	rel=mat.Solve(1.0f/d,l2-p0);
	if(rel.y>=0.0f&&rel.y<1.0f&&rel.x>0.0f&&rel.x<maxfraction){maxfraction=rel.x;return true;}
	return false;
}

void THBoxShape::ApplyAttributes()
{
	const THVector2 rel1=m_angle*m_halfsize;
	const THVector2 rel2=m_angle*THVector2(m_halfsize.x,-m_halfsize.y);
	m_vertices[0]=m_position+rel1;
	m_vertices[1]=m_position+rel2;
	m_vertices[2]=m_position-rel1;
	m_vertices[3]=m_position-rel2;
}
bool THBoxShape::HitTest(const THVector2& p)
{
	const THVector2 rel=InverseMultiply(m_angle,p-m_position);

	return rel.x>-m_halfsize.x&&rel.x<m_halfsize.x&&rel.y>-m_halfsize.y&&rel.y<m_halfsize.y;
}
bool THBoxShape::Raycast(const THVector2& p0,const THVector2& dir,float& maxfraction)
{
	bool contacted=false;

	for(unsigned int i=0;i<4;++i)
	{
		if(THLineRaycast(p0,dir,maxfraction,i?m_vertices[i-1]:m_vertices[3],m_vertices[i]))
		{
			contacted=true;
			//if others have ever contacted
		}
	}
	return contacted;
}
void THCircleShape::ApplyAttributes(){}
bool THCircleShape::HitTest(const THVector2& p)
{
	return (p-m_position).LengthSquared()<m_radiussq;
}
bool THCircleShape::Raycast(const THVector2& p0,const THVector2& dir,float& maxfraction)
{
	const THVector2 d=p0-m_position;
	const float b=THDot(d,dir);
	float a=dir.LengthSquared();
	float det=b*b-a*(d.LengthSquared()-m_radiussq);
	if(det<0.0f){return false;}
	det=sqrtf(det);
	a=1.0f/a;
	float k=(-b-det)*a;
	if(k>0.0f)
	{
		if(k<maxfraction)
		{
			maxfraction=k;
			return true;
		}
		return false;
	}else
	{
		k=(-b+det)*a;
		if(k>0.0f&&k<maxfraction)
		{
			maxfraction=k;
			return true;
		}
	}
	return false;
}



void THLightManager::Load(int vcount,unsigned int shapescap)
{
	m_shapes.Load(shapescap);

	m_vertices.Load(vcount + 5);
	m_verticescount=vcount;
	m_indices.Load((shapescap<<1)+3);

	m_angleincrease=TH_2PI/(float)vcount;

	SetCollideEach(false);
	SetMass(20.0f);
	SetStatic(true);
	SetPressure(10.0f,0.4f,6.0f);
	SetAutoRemove(false);

	layer=(1<<DEFAULT_BIT)|(1<<LIGHT_BIT);
}
void THLightManager::LightTest(bool shaperefresh)
{
	if(shaperefresh)
	{
		for(unsigned int i=0;i<m_shapes.num;++i)
		{
			m_shapes.arr[i]->ApplyAttributes();
		}
	}
	/*
	unsigned int isinbit=0;
	assert(m_shapes.num<32);
	for(unsigned int j=0;j<m_shapes.num;++j)
	{
		if(shaperefresh){m_shapes.arr[j]->ApplyAttributes();}
		isinbit|=m_shapes.arr[j]->HitTest(m_position)?(1u<<j):0;
	}
	*/

	m_vertices.Clear();
	m_indices.Clear();
	float fraction;
	bool prevc=false,curc;
	for(int i=0;i<m_verticescount+1;++i)
	{
		const float theta=i*m_angleincrease;
		const THVector2 dir(cosf(theta),sinf(theta));

		fraction=m_radius;
		curc=false;
		for(unsigned int j=0;j<m_shapes.num;++j)
		{
			curc=curc|m_shapes.arr[j]->Raycast(m_position,dir,fraction);
		}
		if(curc)
		{
			if(prevc==false)
			{
				//Push start index
				m_indices.Push(m_vertices.num);
			}
			m_vertices.Push(m_position + fraction*dir);
			m_vertices.Push(m_position + m_radius*dir);
		}else if(prevc)
		{
			//Push end index
			m_indices.Push(m_vertices.num);
		}

		prevc=curc;
	}
	if(prevc)
	{
		m_indices.Push(m_vertices.num);
	}
}
void THLightManager::LoadParticles(float gap,int layer)
{
	m_nextparticle=list;
	const float gapsq=gap*gap;

	float dlength,l;
	THVector2 dir;
	unsigned int i=0,i1,j;
	while(i<m_indices.num)
	{
		j=m_indices.arr[i];
		i1=m_indices.arr[i+1];
		i+=2;

		while(j<i1)
		{
			const THVector2& p0=m_vertices.arr[j];
			const THVector2& p=m_vertices.arr[j+1];
			j+=2;
			dir=p-p0;
			dlength=dir.Normalize();

			for(l=0.0f;l<=dlength;l+=gap)
			{
				GetNextParticle()->position=p0+(dir*l);
			}
		}
	}
	SetOutLeftParticles();
}
void THLightManager::SetOutLeftParticles()
{
	while(m_nextparticle)
	{
		m_engine->ParticleOut(m_nextparticle);

		m_nextparticle=m_nextparticle->GetNext();
	}
}
void THLightManager::OnTouchDown(const THVector2& p)
{
	m_istouched=HitTest(p);
	m_touchrel=m_position-p;
}
void THLightManager::OnTouchMove(const THVector2& p)
{
	if(m_istouched)
	{
		for(unsigned int i=0;i<m_shapes.num;++i)
		{
			if(m_shapes.arr[i]->HitTest(p+m_touchrel)){return;}
		}
		m_position=p+m_touchrel;
		LightTest(false);
		LoadParticles(0.2f,3);
	}
}
void THLightManager::OnTouchUp(const THVector2& p)
{
	m_istouched=false;
}
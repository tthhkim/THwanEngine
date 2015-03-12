#include "THDisplayObject.h"
#include <THPrivate.h>

#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/Util/THMath.h>

#include <malloc.h>
#include <assert.h>

void THDisplayObject::LoadChildren(unsigned int cap)
{
	if(cap>0)
	{
		m_children.Load(cap);
	}
}
void THDisplayObject::AddChild(THDisplayObject *obj)
{
	assert(m_children.IsAllocated());

	m_children.Push(obj);
	obj->m_parent=this;
}
void THDisplayObject::CalculateWorldAttrib()
{
	if(m_parent)
	{
		m_worldposition=m_parent->m_worldposition + (position*m_parent->m_worldrotation);
		m_worldrotation=rotation*m_parent->m_worldrotation;
	}else
	{
		m_worldposition=position;
		m_worldrotation=rotation;
	}
	for(unsigned int i=0;i<m_children.num;++i)
	{
		m_children.arr[i]->CalculateWorldAttrib();
	}
}


void THDisplayObject::Draw()
{
	if(texture)
	{
		glBindTexture(GL_TEXTURE_2D,texture->image->textureID);

		glVertexAttrib2fv(THDefaultProgram.rotationHandler,(const GLfloat*)&m_worldrotation);
		glVertexAttrib2fv(THDefaultProgram.scaleHandler,(const GLfloat*)&size);
		glVertexAttrib2fv(THDefaultProgram.positionHandler,(const GLfloat*)&m_worldposition);
		glVertexAttrib4f(THDefaultProgram.textureHandler,texture->position.x,texture->position.y,texture->size.x,texture->size.y);
		glVertexAttrib2fv(THDefaultProgram.centerHandler,(const GLfloat*)&center);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	for(unsigned int i=0;i<m_children.num;++i)
	{
		m_children.arr[i]->Draw();
	}
}


void THButton::SetPosition(const THVector2& p)
{
	m_maxbound=m_maxbound-m_minbound+p;
	m_minbound=p;
}
void THButton::SetSize(const THVector2& s)
{
	m_maxbound=m_minbound+s;
}
void THButton::Synchronize(const THDisplayObject *obj,const THVector2& extraBound)
{
	assert(obj);

	const THVector2 mp=obj->GetWorldPosition();
	const THVector2 m=obj->size*obj->center;

	m_minbound=mp-m-extraBound;
	m_maxbound=mp+obj->size-m+extraBound;
}

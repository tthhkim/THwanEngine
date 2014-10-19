#include "THDisplayObject.h"
#include <THPrivate.h>

#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/Util/THMath.h>

#include <malloc.h>
#include <assert.h>

THVector2 drawingOffset;
THVector2 tempPoition;
//Offset Coordinations;

using namespace THDefaultProgram;

void THMovieClip::Draw() const
{
	tempPoition=drawingOffset+position;

	glBindTexture(GL_TEXTURE_2D,texture->image->textureID);
	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,vertexBuffer);
	glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,texture->textureBuffer);

	glVertexAttrib4fv(rotationHandler,(const GLfloat*)&rotation);
	glVertexAttrib2fv(positionHandler,(const GLfloat*)&tempPoition);

	glVertexAttrib1f(hasColorHandler,0.0f);
	glVertexAttrib4f(colorHandler,0.0f,0.0f,0.0f,0.0f);
	glVertexAttrib4f(colorMultiplyHandler,1.0f,1.0f,1.0f,1.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}



void THGroupClip::Draw() const
{
	THDisplayObject* object;
	drawingOffset+=position;
	for(unsigned i=0;i<list.num;++i)
	{
		object=(THDisplayObject*)list.arr[i];
		if(object->visible)
		{object->Draw();}
	}
	drawingOffset-=position;
}			  

void THGroupClip::AddChild(THDisplayObject* displayObject)
{
	assert(displayObject->frame==0);
	list.Push(displayObject);
	displayObject->frame=this;
}
inline void THGroupClip::ReAddChild(THDisplayObject* displayObject)
{
	assert(displayObject->frame==this);
	list.Repush(displayObject);
}

void THButton::SetPosition(float _x,float _y)
{
	const THVector2 p(_x,_y);
	maxBound=maxBound-minBound+p;
	minBound=p;
}
void THButton::SetSize(float w,float h)
{
	maxBound=minBound+THVector2(w,h);
}
void THButton::Synchronize(const THVector2& extraBound)
{
	assert(clip);

	const THVector2* arrs=(const THVector2*)clip->vertexBuffer;

	const THVector2 minc=arrs[0];
	const THVector2 maxc=arrs[3];

	minBound=clip->position+minc-extraBound;
	maxBound=clip->position+maxc+extraBound;
}

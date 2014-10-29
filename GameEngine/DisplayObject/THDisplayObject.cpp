#include "THDisplayObject.h"
#include <THPrivate.h>

#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/Util/THMath.h>

#include <malloc.h>
#include <assert.h>

THVector2 tempVector;

void THGroupClip::Draw()
{
	THDisplayObject* obj;
	for(unsigned int i=0;i<objectList.num;++i)
	{
		obj=objectList.arr[i];

		if(obj->visible)
		{
			obj->DrawObject();
		}
	}
}
void THMovieClip::Draw()
{
	glBindTexture(GL_TEXTURE_2D,texture->image->textureID);
	glVertexAttribPointer(THDefaultProgram.vertexHandler,2,GL_FLOAT,GL_FALSE,0,vertexBuffer);
	glVertexAttribPointer(THDefaultProgram.textureHandler,2,GL_FLOAT,GL_FALSE,0,texture->textureBuffer);

	glVertexAttrib4fv(THDefaultProgram.rotationHandler,(const GLfloat*)&rotation);
	glVertexAttrib2fv(THDefaultProgram.positionHandler,(const GLfloat*)&worldPosition);

	glVertexAttrib1f(THDefaultProgram.hasColorHandler,0.0f);
	glVertexAttrib4f(THDefaultProgram.colorHandler,0.0f,0.0f,0.0f,0.0f);
	glVertexAttrib4f(THDefaultProgram.colorMultiplyHandler,1.0f,1.0f,1.0f,1.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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

	const THVector2& minc=arrs[0];
	const THVector2& maxc=arrs[3];
	const THVector2 mp=clip->GetWorldPosition();

	minBound=mp+minc-extraBound;
	maxBound=mp+maxc+extraBound;
}

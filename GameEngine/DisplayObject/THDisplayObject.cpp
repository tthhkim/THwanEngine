#include "THDisplayObject.h"
#include <THPrivate.h>

#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/Util/THMath.h>

#include <malloc.h>
#include <assert.h>

void DrawTHMovieClip(const THMovieClip* obj)
{
	glBindTexture(GL_TEXTURE_2D,obj->texture->image->textureID);
	glVertexAttribPointer(THDefaultProgram.vertexHandler,2,GL_FLOAT,GL_FALSE,0,obj->vertexBuffer);
	glVertexAttribPointer(THDefaultProgram.textureHandler,2,GL_FLOAT,GL_FALSE,0,obj->texture->textureBuffer);

	glVertexAttrib2fv(THDefaultProgram.rotationHandler,(const GLfloat*)&obj->rotation);
	glVertexAttrib2fv(THDefaultProgram.positionHandler,(const GLfloat*)&obj->worldPosition);

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

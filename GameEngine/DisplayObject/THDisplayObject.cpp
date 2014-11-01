#include "THDisplayObject.h"
#include <THPrivate.h>

#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/Util/THMath.h>

#include <malloc.h>
#include <assert.h>

void DrawTHMovieClip(const THMovieClip* obj)
{
	glBindTexture(GL_TEXTURE_2D,obj->texture->image->textureID);
	
	//glVertexAttribPointer(THDefaultProgram.vertexHandler,2,GL_FLOAT,GL_FALSE,0,obj->vertexBuffer);
	
	glVertexAttribPointer(THDefaultProgram.textureHandler,2,GL_FLOAT,GL_FALSE,0,obj->texture->textureBuffer);

	if(obj->vertexBuffer)
	{
		glEnableVertexAttribArray(THDefaultProgram.vertexHandler);
		glVertexAttribPointer(THDefaultProgram.vertexHandler,2,GL_FLOAT,GL_FALSE,0,obj->vertexBuffer);
	}
	else
	{
		THHalfVertices.BeginDrawing(THDefaultProgram.vertexHandler);
		glVertexAttribPointer(THDefaultProgram.vertexHandler,2,GL_FLOAT,GL_FALSE,0,0);
	}

	glVertexAttrib2fv(THDefaultProgram.rotationHandler,(const GLfloat*)&obj->rotation);
	glVertexAttrib2fv(THDefaultProgram.scaleHandler,(const GLfloat*)&obj->size);
	glVertexAttrib2fv(THDefaultProgram.positionHandler,(const GLfloat*)&obj->worldPosition);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	THHalfVertices.EndDrawing(THDefaultProgram.vertexHandler);
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

	const THVector2 mp=clip->GetWorldPosition();

	if(clip->vertexBuffer)
	{
		const THVector2 minc=((THVector2*)clip->vertexBuffer)[0]*clip->size;
		const THVector2 maxc=((THVector2*)clip->vertexBuffer)[3]*clip->size;

		minBound=mp+minc-extraBound;
		maxBound=mp+maxc+extraBound;
	}else
	{
		const THVector2 bc=clip->size*0.5f;
		minBound=mp-bc-extraBound;
		maxBound=mp+bc+extraBound;
	}
}

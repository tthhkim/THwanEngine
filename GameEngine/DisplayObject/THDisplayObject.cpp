#include "THDisplayObject.h"
#include <THPrivate.h>

#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/Util/THMath.h>

#include <malloc.h>
#include <assert.h>

THVector2 drawingOffset;
THVector2 tempPoition;
//Offset Coordinations;

void THMovieClip::draw()
{
	tempPoition=drawingOffset+position;

	glBindTexture(GL_TEXTURE_2D,texture->image->textureID);
	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,vertexBuffer);
	glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,texture->textureBuffer);

	glVertexAttrib4fv(rotationHandler,(const GLfloat*)&rotation);
	glVertexAttrib2fv(positionHandler,(const GLfloat*)&tempPoition);

	glVertexAttrib1f(hasColorHandler,0.0f);
	//glVertexAttrib4f(colorHandler,1.0f,1.0f,1.0f,1.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}



void THGroupClip::draw()
{
	THDisplayObject* object;
	drawingOffset+=position;
	for(unsigned i=0;i<list.num;++i)
	{
		object=(THDisplayObject*)list.arr[i];
		if(object->visible)
		{object->draw();}
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
	xplusw=xplusw-x+_x;
	x=_x;

	yplush=yplush-y+_y;
	y=_y;
}
void THButton::SetSize(float w,float h)
{
	xplusw=x+w;
	yplush=y+h;
}

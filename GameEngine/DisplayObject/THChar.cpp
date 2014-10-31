#include "THChar.h"
#include <THPrivate.h>

void THChar::Set(THImage* _image,const THVector2& pos,const THVector2& _size)
{
	THTexture::Set(_image,pos,_size);

	const THVector2 sxy2=_size/_image->size;

	length=sxy2.x;

	vertexBuffer[0].Set(0.0f,0.0f);
	vertexBuffer[1].Set(sxy2.x,0.0f);
	vertexBuffer[2].Set(0.0f,sxy2.y);
	vertexBuffer[3].Set(sxy2.x,sxy2.y);
}

void THString::SetString(const THChar** charArr)
{
	chars.Clear();
	length=0.0f;
	unsigned int i=0;

	while(charArr[i]!=0)
	{
		length+=charArr[i]->length;
		chars.Push(charArr[i]);
		++i;
	}
}
void THString::SetWidth(float w)
{
	const THVector2 cp=GetPosition();
	scale=w/length;
	SetPosition(cp,positionOffset);
}

void DrawTHString(const THString* obj)
{
	THVector2 pos=obj->position;

	const float scale=obj->scale;
	const THChar* ch;
	const unsigned int cn=obj->chars.num;
	const THChar **carrs=obj->chars.arr;

	const float r=obj->red;
	const float g=obj->green;
	const float b=obj->blue;
	const float a=obj->alpha;
	
	for(unsigned int i=0;i<cn;++i)
	{
		ch=carrs[i];

		glBindTexture(GL_TEXTURE_2D,ch->image->textureID);

		glVertexAttribPointer(THDefaultProgram.vertexHandler,2,GL_FLOAT,GL_FALSE,0,ch->vertexBuffer);
		glVertexAttribPointer(THDefaultProgram.textureHandler,2,GL_FLOAT,GL_FALSE,0,ch->textureBuffer);

		glVertexAttrib2f(THDefaultProgram.rotationHandler,1.0f,0.0f);
		glVertexAttrib2f(THDefaultProgram.scaleHandler,scale,scale);
		glVertexAttrib2fv(THDefaultProgram.positionHandler,(const GLfloat*)&pos);

		glVertexAttrib1f(THDefaultProgram.hasColorHandler,0.0f);
		glVertexAttrib4f(THDefaultProgram.colorHandler,0.0f,0.0f,0.0f,0.0f);
		glVertexAttrib4f(THDefaultProgram.colorMultiplyHandler,r,g,b,a);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		pos.x+=ch->length*scale;
	}
}
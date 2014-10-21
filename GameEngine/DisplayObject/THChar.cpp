#include "THChar.h"
#include <THPrivate.h>
using namespace THDefaultProgram;
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
		length+=charArr[i]->Length();
		chars.Push(charArr[i]);
		++i;
	}

	length*=scale;
}
void THString::Draw()
{
	float cl=position.x;
	const THChar* ch;
	for(unsigned int i=0;i<chars.num;++i)
	{
		ch=chars.arr[i];

		glBindTexture(GL_TEXTURE_2D,ch->image->textureID);

		glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,ch->vertexBuffer);
		glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,ch->textureBuffer);

		glVertexAttrib4f(rotationHandler,scale,0.0f,0.0f,scale);
		glVertexAttrib2f(positionHandler,cl,position.y);

		glVertexAttrib1f(hasColorHandler,0.0f);
		glVertexAttrib4f(colorHandler,0.0f,0.0f,0.0f,0.0f);
		glVertexAttrib4f(colorMultiplyHandler,red,green,blue,alpha);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		cl+=ch->Length()*scale;
	}
}
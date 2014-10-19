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

void THString::SetString(const THChar** charArr,unsigned int cCount)
{
	count=cCount;
	chars=new THChar*[count];
	memcpy(chars,charArr,sizeof(THChar*)*count);

	length=0.0f;
	for(unsigned int i=0;i<count;++i)
	{
		length+=chars[i]->Length();
	}
	length*=scale;
}
void THString::SetStringMulti(const THChar** charArr,unsigned int cCount)
{
	count=cCount;
	chars=new THChar*[count];
	memcpy(chars,charArr,sizeof(THChar*)*count);

	const float la=lineHeight>0.0f?lineHeight:-lineHeight;
	length=la;
	for(unsigned int i=0;i<count;++i)
	{
		if(chars[i]==0)
		{
			length+=la;
		}
	}
}
void THString::Draw() const
{
	float cl=position.x;
	float cy=position.y;
	const THChar* ch;
	for(unsigned int i=0;i<count;++i)
	{
		ch=chars[i];

		if(ch==0)
		{
			cy+=lineHeight;
			cl=position.x;
			continue;
		}

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
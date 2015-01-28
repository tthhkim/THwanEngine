#include "THFrame.h"
#include <GameEngine/Rendering/THDrawing.h>
#include <THPrivate.h>

void THFrame::DrawObjects(unsigned int start,unsigned int end) const
{
	assert(start<objectList.num && end>start && end<=objectList.num);
	THDefaultProgram.Use();
	glEnableVertexAttribArray(THDefaultProgram.vertexHandler);
	glEnableVertexAttribArray(THDefaultProgram.textureHandler);

	THDisplayObject* object;
	for(unsigned int i=start;i<end;++i)
	{
		object=objectList.arr[i];
		if(object->visible)
		{
			object->DrawObject();
		}
	}

	glDisableVertexAttribArray(THDefaultProgram.textureHandler);
	glDisableVertexAttribArray(THDefaultProgram.vertexHandler);
	
}
#ifndef NDEBUG

void THFrame::DrawButtonDebug(const THColor& color) const
{
	THDefaultProgram.Use();
	glEnableVertexAttribArray(THDefaultProgram.vertexHandler);

	THDefaultProgram.SetColorMultiply(0.0f,0.0f,0.0f,0.0f);
	THDefaultProgram.SetColorAdd(color.red,color.green,color.blue,color.alpha);

	glVertexAttribPointer(THDefaultProgram.vertexHandler,2,GL_FLOAT,GL_FALSE,0,THZeroVertices);
	glVertexAttrib2f(THDefaultProgram.rotationHandler,1.0f,0.0f);

	for(unsigned int i=0;i<buttonList.num;++i)
	{
		const THButton *btn=buttonList.arr[i];

		THVector2 s=btn->GetSize();
		glVertexAttrib2fv(THDefaultProgram.scaleHandler,(const GLfloat*)&s);
		glVertexAttrib2fv(THDefaultProgram.positionHandler,(const GLfloat*)&btn->minBound);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glDisableVertexAttribArray(THDefaultProgram.vertexHandler);

	THDefaultProgram.SetColorMultiply(1.0f,1.0f,1.0f,1.0f);
	THDefaultProgram.SetColorAdd(0.0f,0.0f,0.0f,0.0f);
}
#endif
void THFrame::Draw()
{
	DrawObjects();
}
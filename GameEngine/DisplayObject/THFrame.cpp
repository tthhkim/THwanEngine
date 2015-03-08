#include "THFrame.h"
#include <GameEngine/Rendering/THDrawing.h>
#include <THPrivate.h>

void THFrame::DrawObjects(unsigned int start,unsigned int end) const
{
	assert(start<objectList.num && end>start && end<=objectList.num);
	THDefaultProgram.Use();
	glEnableVertexAttribArray(THDefaultProgram.vertexHandler);
	THZeroVertices.BeginDrawing();
	glVertexAttribPointer(THDefaultProgram.vertexHandler,2,GL_FLOAT,GL_FALSE,0,0);

	THDisplayObject* object;
	for(unsigned int i=start;i<end;++i)
	{
		object=objectList.arr[i];
		if(object->visible)
		{
			object->CalculateWorldAttrib();
			object->Draw();
		}
	}

	glDisableVertexAttribArray(THDefaultProgram.vertexHandler);
	THZeroVertices.EndDrawing();
	
}
#ifndef NDEBUG

#endif
void THFrame::Draw()
{
	DrawObjects();
}
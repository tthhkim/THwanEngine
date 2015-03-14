#include "THFrame.h"
#include <GameEngine/Rendering/THDrawing.h>
#include <THPrivate.h>

void THFrame::DrawObjects(unsigned int start,unsigned int count) const
{
	assert(start<objectList.num && start+count<=objectList.num);
	THDefaultProgram.Use();
	glEnableVertexAttribArray(THDefaultProgram.vertexHandler);
	THZeroVertices.BeginDrawing();
	glVertexAttribPointer(THDefaultProgram.vertexHandler,2,GL_FLOAT,GL_FALSE,0,0);

	THDisplayObject* object;
	for(unsigned int i=0;i<count;++i)
	{
		object=objectList.arr[i+start];
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
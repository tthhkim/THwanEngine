#include "THFrame.h"
#include <GameEngine/Rendering/THDrawing.h>
#include <THPrivate.h>

void THFrame::DrawObjects(unsigned int start,unsigned int end) const
{
	assert(start<objectList.num && end>start && end<=objectList.num);
	THDefaultProgram.defaultProgram.Use();
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
void THFrame::Draw()
{
	DrawObjects();
}
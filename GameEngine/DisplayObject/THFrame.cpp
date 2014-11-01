#include "THFrame.h"
#include <GameEngine/Rendering/THDrawing.h>
#include <THPrivate.h>

void THFrame::DrawObjects() const
{
	THDefaultProgram.defaultProgram.Use();
	glEnableVertexAttribArray(THDefaultProgram.vertexHandler);
	glEnableVertexAttribArray(THDefaultProgram.textureHandler);

	THDisplayObject* object;
	for(unsigned int i=0;i<objectList.num;++i)
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
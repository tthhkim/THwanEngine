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
			object->Draw();
		}
	}
	glDisableVertexAttribArray(THDefaultProgram.textureHandler);

#ifndef NDEBUG
	THButton* btn;
	for(unsigned int i=0;i<buttonList.num;++i)
	{
		btn=buttonList.arr[i];
		if(btn->enable)
		{
			btn->DrawDebug(0.1f,0.3f,0.9f);
		}else
		{
			btn->DrawDebug(0.9f,0.3f,0.1f);
		}
	}
#endif

	glDisableVertexAttribArray(THDefaultProgram.vertexHandler);
	
}
void THFrame::Draw()
{
	DrawObjects();
}
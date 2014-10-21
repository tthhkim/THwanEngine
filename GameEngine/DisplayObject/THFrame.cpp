#include "THFrame.h"
#include <malloc.h>
#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <GameEngine/Rendering/THDrawing.h>
#include <THPrivate.h>

void THFrame::AddChild(THDisplayObject* object)
{
	assert(object->frame==0);
	object->frame=this;
	objectList.Push(object);
}
void THFrame::ReAddChild(THDisplayObject* object)
{
	assert(object->frame==this);
	objectList.Repush(object);
}
void THFrame::AddButton(THButton* button)
{
	assert(button->frame==0);
	button->frame=this;
	buttonList.Push(button);
}
void THFrame::ReAddButton(THButton* button)
{
	assert(button->frame==this);
	buttonList.Repush(button);
}
void THFrame::RemoveChild(THDisplayObject* object)
{
	objectList.Delete(object);
}
void THFrame::RemoveButton(THButton* button)
{
	buttonList.Delete(button);
}
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
void THFrame::Draw(float dt)
{
	DrawObjects();
}
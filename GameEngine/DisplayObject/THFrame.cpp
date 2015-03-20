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
void THFrame::Draw()
{
	DrawObjects();
}

void THFrame::FrameDown(const THVector2& p)
{
	m_lasttouched=p;
	OnTouchDown(p);

	unsigned int i=buttonList.num;
	THButton* btn;

	while(i)
	{
		--i;
		btn=buttonList.arr[i];

		if(btn->enable && btn->HitTest(p))
		{
			m_buttondown=btn;
			if(btn->onDown){btn->onDown(p,btn);}
			btn->Swap();
			return;
		}
	}
}
void THFrame::FrameMove(const THVector2& p)
{
	OnTouchMove(p,p-m_lasttouched);
	m_lasttouched=p;
}
void THFrame::FrameUp(const THVector2& p)
{
	OnTouchUp(p);

	unsigned int i=buttonList.num;
	THButton* btn;

	while(i)
	{
		--i;
		btn=buttonList.arr[i];

		if(btn->enable && btn->HitTest(p))
		{
			if(m_buttondown==btn)
			{
				if(btn->onRelease){btn->onRelease(p,btn);}
				break;
			}
			
		}
	}

	if(m_buttondown)
	{
		m_buttondown->Swap();
		m_buttondown=0;
	}		
}
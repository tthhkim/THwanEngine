#include "THFrame.h"
#include <GameEngine/Rendering/THDrawing.h>
#include <THPrivate.h>

void THFrame::PreDraw()
{
	THDefaultProgram.PreDraw();
}
void THFrame::PostDraw()
{
	THDefaultProgram.PostDraw();
}
void THFrame::DrawTexture(const THVector2& p,const THVector2& center,const THVector2& size,const THRot2& rot,const THTexture *tex)
{
	glBindTexture(GL_TEXTURE_2D,tex->image->textureID);
	glVertexAttrib2fv(THDefaultProgram.rotationHandler,(const GLfloat*)&rot);
	glVertexAttrib2fv(THDefaultProgram.scaleHandler,(const GLfloat*)&size);
	glVertexAttrib2fv(THDefaultProgram.positionHandler,(const GLfloat*)&p);
	glVertexAttrib4f(THDefaultProgram.textureHandler,tex->position.x,tex->position.y,tex->size.x,tex->size.y);
	glVertexAttrib2fv(THDefaultProgram.centerHandler,(const GLfloat*)&center);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void THFrame::DrawObjects(unsigned int start,unsigned int count) const
{
	assert(start<objectList.num && start+count<=objectList.num);

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
}
void THFrame::Draw()
{
	PreDraw();
	DrawObjects();
	PostDraw();
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
			if(btn->m_callback){btn->m_callback->OnDown(p,btn);}
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
				if(btn->m_callback){btn->m_callback->OnRelease(p,btn);}
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
#include "THApplication.h"

#include <GameEngine/DisplayObject/THFrame.h>

#include <EGL/egl.h>

THApplication THApp;
THApplication& GetApplication()
{
	return THApp;
}
float GetDeltaTime()
{
	return THApp.m_dt;
}
THVertexBuffer& GetOneVBO()
{
	return THApp.GetOneVBO();
}
THApplication::THApplication()
{
	m_isrunning=false;
	m_lastMicroSec=0;
	SetFrameRate(60.0f);

	//GameSize
	//THVector2 m_gameScale;
	//THVector2 m_gameMinBound;
	//THVector2 m_gameMaxBound;

	//WindowSize
	//THVector2 m_windowSize;
	//GLsizei m_windowWidthi;
	//GLsizei m_windowHeighti;

	//GLfloat THProjectMatrix[6];

	//THVertexBuffer m_zerooneVBO;

	m_currentFrame=0;

	//THLinkedList m_timerlist;

	m_dt=0.0f;
}
void THApplication::GLInit()
{
	//glClearColor(0.7f,0.6f,0.5f,1.0f);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	TH_GLERROR_CHECK("glBlendFunc")

		/*
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	*/
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	ViewportInit();
	TH_GLERROR_CHECK("Viewport")

	const GLfloat verts[8]=MAKE_VERTEX(-1.0f,-1.0f,1.0f,1.0f);
	m_oneVBO.Load((void*)verts,sizeof(GLfloat)*8,GL_STATIC_DRAW);
}
void THApplication::Start()
{
	m_isrunning=true;
	m_lastMicroSec=GetCurrentTimeMicro();
}
void THApplication::SetWindowSize(int w,int h)
{
	m_windowWidthi=w;
	m_windowHeighti=h;
	m_windowSize.Set((float)w,(float)h);
}

void THApplication::SetFrameRate(float _frameRate)
{
	m_minTimeGap=(long long)(1000000.0f/_frameRate);
	m_maxTimeGap=m_minTimeGap<<1;
}
void THApplication::Loop()
{
	const long long ct=GetCurrentTimeMicro();
	const long long gap=ct-m_lastMicroSec;

	if(gap>m_minTimeGap)
	{
		m_dt=(float)((double)(gap<m_maxTimeGap?gap:m_maxTimeGap)*1e-6);
		if(m_isrunning){OnEnterFrame();}
		OnDrawFrame();
		m_lastMicroSec=ct;
	}
}
void THApplication::OnDrawFrame()
{
	//Draw Start
	
	glClear(
		GL_COLOR_BUFFER_BIT
		|GL_DEPTH_BUFFER_BIT
		);

	m_currentFrame->Draw();

	SwapBuffer();
	//Draw End
}
void THApplication::OnEnterFrame()
{
#ifdef TH_ISDEBUG
	if(m_currentFrame==0){THError("Frame Not Set");assert(0);}
#endif
	//timer step
	THTimerDef *timer=(THTimerDef*)m_timerlist.GetList(),*tnext;
	while(timer)
	{
		tnext=(THTimerDef*)timer->GetLinkedNext();
		timer->timeSkip-=m_dt;
		if(timer->timeSkip<=0.0f)
		{
			timer->action(timer->data);
			m_timerlist.Delete(timer);
			delete timer;
		}
		timer=tnext;
	}

	m_currentFrame->OnEnterFrame();
}
void THApplication::SetOrtho(const THVector2& minp,const THVector2& maxp)
{
	m_gameMinBound=minp;
	m_gameMaxBound=maxp;

	m_gameScale=(maxp-minp) / m_windowSize;

	THOrthoMatrix33(THProjectMatrix,minp,maxp);

	THLog("Set Ortho : %.1f , %.1f / %.1f , %.1f",minp.x,minp.y,maxp.x,maxp.y);
}
void THApplication::GoFrame(THFrame* f,void* data)
{
	if(m_currentFrame){m_currentFrame->OnChangeFrame(f);}
	f->Action(m_currentFrame,data);									 
	m_currentFrame = f;
}
void THApplication::AddTimer(float _timeSkip,void (*_action)(void*),void* _data)
{
	assert(_timeSkip>0.0f);
	assert(_action);

	THLog("Timer Add %.2f Seconds",_timeSkip);

	THTimerDef* timer=new THTimerDef;
	
	timer->action=_action;
	timer->data=_data;
	timer->timeSkip=_timeSkip;

	m_timerlist.Push(timer);
}

void THApplication::OnTouchDown(const THVector2& p)
{
	if(m_currentFrame->canTouch==false){return;}
	const THVector2 p2(getGameX(p.x),getGameY(p.y));
	m_currentFrame->FrameDown(p2);
}
void THApplication::OnTouchMove(const THVector2& p)
{
	if(m_currentFrame->canTouch==false){return;}
	const THVector2 p2(getGameX(p.x),getGameY(p.y));
	m_currentFrame->FrameMove(p2);
}
void THApplication::OnTouchUp(const THVector2& p)
{
	if(m_currentFrame->canTouch==false){return;}
	const THVector2 p2(getGameX(p.x),getGameY(p.y));
	m_currentFrame->FrameUp(p2);
}
int THApplication::OnBackReleased()
{
	return m_currentFrame->OnBackReleased();
}
void THApplication::OnKeyUp(int k)
{
	m_currentFrame->OnKeyUp(k);
}
void THApplication::OnKeyDown(int k)
{
	m_currentFrame->OnKeyUp(k);
}
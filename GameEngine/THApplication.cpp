#include "THApplication.h"

#include <GameEngine/DisplayObject/THFrame.h>

#include <EGL/egl.h>

THApplication THApp;
THApplication::THApplication()
{
	m_isrunning=false;
	m_lastMicroSec=0;
	m_minTimeGap=0;m_maxTimeGap=0;

	//EGL Variables
	m_eglDisplay=0;
	m_eglSurface=0;
	m_eglContext=0;
	m_eglConfig=0;
	m_glesversion=2;
	m_egldepthsize=0;

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

	THFrame* m_currentFrame=0;

	//THLinkedList m_timerlist;

	m_dt=0.0f;

	SetFrameRate(60.0f);
}
#include <GLES2/gl2.h>
void THApplication::GLInit()
{
	//glClearColor(0.7f,0.6f,0.5f,1.0f);
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	TH_GLERROR_CHECK()

	if(m_egldepthsize)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
	}else
	{
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
	}
	ViewportInit();
	TH_GLERROR_CHECK()

/*
	THDefaultProgram.Load();
	THDefaultProgram.SetColorAdd(0.0f,0.0f,0.0f,0.0f);
	THDefaultProgram.SetColorMultiply(1.0f,1.0f,1.0f,1.0f);

	TH_GLERROR_CHECK()
	*/
}
void THApplication::Start()
{
	m_isrunning=true;
	m_lastMicroSec=GetCurrentTimeMicro();
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
	if(m_eglDisplay == EGL_NO_DISPLAY){return;}
	//Draw Start
	
	glClear(
		GL_COLOR_BUFFER_BIT
#if USE_DEPTH_BUFFER==1
		|GL_DEPTH_BUFFER_BIT
#endif
		);

	m_currentFrame->Draw();

	eglSwapBuffers(m_eglDisplay, m_eglSurface);
	//Draw End
}
void THApplication::OnEnterFrame()
{
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

void THApplication::SetEGL(int glesversion,int depthsize)
{
	m_glesversion=glesversion;
	m_egldepthsize=depthsize;
}
void THApplication::EGLInit(EGLNativeWindowType window)
{
	EGLInitDisplay();
	EGLChooseConfig();
	EGLInitSurface(window);
	EGLMakeContext();
}
void THApplication::EGLInitDisplay()
{
	m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint eglMajor,eglMinor;
    eglInitialize(m_eglDisplay, &eglMajor,&eglMinor);
	THLog("EGL Initialization : %d.%d",eglMajor,eglMinor);
}
void THApplication::EGLChooseConfig()
{
	const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
			EGL_DEPTH_SIZE,m_egldepthsize,
            EGL_NONE
    };
	EGLint numConfigs;
    eglChooseConfig(m_eglDisplay, attribs, &m_eglConfig, 1, &numConfigs);
}
void THApplication::EGLMakeContext()
{
	const EGLint attrib_list[] = {
		EGL_CONTEXT_CLIENT_VERSION, m_glesversion,
        EGL_NONE
    };
    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, NULL, attrib_list);
	/*
#if THPLATFORM==THPLATFORM_WINDOWS
	eglBindAPI(EGL_OPENGL_ES_API);
#endif
	*/
	if (eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext) == EGL_FALSE) {
		THError("Unable to eglMakeCurrent");
		assert(0);
		return;
    }
	EGLint sw,sh;
    eglQuerySurface(m_eglDisplay, m_eglSurface, EGL_WIDTH, &sw);
    eglQuerySurface(m_eglDisplay, m_eglSurface, EGL_HEIGHT, &sh);
	m_windowWidthi=sw;
	m_windowHeighti=sh;
	m_windowSize.Set((float)sw,(float)sh);
}
void THApplication::SetOrtho(const THVector2& minp,const THVector2& maxp)
{
	m_gameMinBound=minp;
	m_gameMaxBound=maxp;

	m_gameScale=(maxp-minp) / m_windowSize;

	THOrthoMatrix33(THProjectMatrix,minp,maxp);
	//glUniform3fv(THDefaultProgram.projectMatrixHandler,2,THProjectMatrix);

	THLog("Set Ortho : %.1f , %.1f / %.1f , %.1f",minp.x,minp.y,maxp.x,maxp.y);
}
void THApplication::InitZeroOneVBO()
{
	const GLfloat verts[]=MAKE_VERTEX(0.0f,0.0f,1.0f,1.0f);
	m_zerooneVBO.Load((void*)verts,sizeof(GLfloat)*8,GL_STATIC_DRAW);
}
void THApplication::TermDisplay() {
    if (m_eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(m_eglDisplay, m_eglContext);
        }
        if (m_eglSurface != EGL_NO_SURFACE) {
            eglDestroySurface(m_eglDisplay, m_eglSurface);
        }
        eglTerminate(m_eglDisplay);
    }
    m_eglDisplay = EGL_NO_DISPLAY;
    m_eglContext = EGL_NO_CONTEXT;
    m_eglSurface = EGL_NO_SURFACE;
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
void THApplication::OnKeyUp(unsigned int k)
{
	m_currentFrame->OnKeyUp(k);
}
void THApplication::OnKeyDown(unsigned int k)
{
	m_currentFrame->OnKeyUp(k);
}
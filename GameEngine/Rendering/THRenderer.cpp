#include "THRenderer.h"
#include <THPrivate.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <GameEngine/DisplayObject/THFrame.h>

#include <GameEngine/Util/THMath.h>
#include <GameEngine/Util/THTween.h>
#include <GameEngine/Util/THArray.h>

#include <stdlib.h>

extern EGLDisplay eglDisplay=EGL_NO_DISPLAY;
static EGLSurface eglSurface=EGL_NO_SURFACE;
static EGLContext eglContext=EGL_NO_CONTEXT;

extern THVector2 windowSize(0.0f,0.0f);
extern GLsizei windowWidthi=0;
extern GLsizei windowHeighti=0;
extern THVector2 gameScale(0.0f,0.0f);
extern THVector2 gameMinBound(0.0f,0.0f);
extern THVector2 gameMaxBound(0.0f,0.0f);
extern GLfloat THProjectMatrix[6]={0.0f};

extern THDefProgram THDefaultProgram=THDefProgram();

extern THFrame* currentFrame=0;
extern THButton* downedButton=0;

extern const GLfloat THFullVertices[]=MAKE_CENTER_VERTEX(1.0f,1.0f);
extern GLfloat THGameFullVertices[8]={0.0};
extern const GLfloat THZeroVertices[]=MAKE_VERTEX(0.0f,0.0f,1.0f,1.0f);

extern THVertexBuffer THHalfVertices=THVertexBuffer();

extern float THDeltaTime=0.0f;

void OnDrawFrame()
{
	//Draw Start
	
	glClear(
		GL_COLOR_BUFFER_BIT
#if USE_DEPTH_BUFFER==1
		|GL_DEPTH_BUFFER_BIT
#endif
		);

	currentFrame->Draw();

	eglSwapBuffers(eglDisplay, eglSurface);
	//Draw End
}
void SetOrtho(const THVector2& minp,const THVector2& maxp)
{
	gameMinBound=minp;
	gameMaxBound=maxp;

	gameScale=(maxp-minp) / windowSize;

	THOrthoMatrix33(THProjectMatrix,minp,maxp);
	glUniform3fv(THDefaultProgram.projectMatrixHandler,2,THProjectMatrix);

	const GLfloat fv[]=MAKE_VERTEX(minp.x,minp.y,maxp.x,maxp.y);
	memcpy(THGameFullVertices,fv,sizeof(GLfloat)*8);

	THLog("Set Ortho : %.1f , %.1f / %.1f , %.1f",minp.x,minp.y,maxp.x,maxp.y);
}



void THDefProgram::Load()
{
	const GLchar* vs=
			"precision mediump float;"
			"uniform vec3 projMat[2];"
			"attribute vec2 vert;"
			"attribute vec2 rot;"
			"attribute vec2 sScale;"
			"attribute vec2 pos;"
			"attribute vec2 aTex;"
			"varying vec2 vTex;"
			"void main(){"
			"vec2 svert=vert*sScale;"
			"vec3 lastP=vec3("
			"dot( vec2(rot.x,-rot.y) , svert )+pos.x , dot( vec2(rot.y,rot.x) , svert )+pos.y"
			",1.0);"
			"gl_Position=vec4(dot(projMat[0],lastP),dot(projMat[1],lastP),0.0,1.0);"
			"vTex=aTex;"
			"}";

	const GLchar* fs=
			"precision mediump float;"
			"varying vec2 vTex;"
			"uniform sampler2D sTexture;"
			"uniform vec4 aColor;"
			"uniform vec4 mColor;"
			"void main(){"
			"gl_FragColor=texture2D(sTexture,vTex)*mColor + aColor;"
			"}";
	THProgram::Load(vs,fs);

	vertexHandler=GetAttribLocation("vert");
	rotationHandler=GetAttribLocation("rot");
	scaleHandler=GetAttribLocation("sScale");
	positionHandler=GetAttribLocation("pos");
	textureHandler=GetAttribLocation("aTex");

	projectMatrixHandler=GetUniformLocation("projMat");
	colorAddHandler=GetUniformLocation("aColor");
	colorMultiplyHandler=GetUniformLocation("mColor");
}
void THGLInit()
{
    //glClearColor(0.7f,0.6f,0.5f,1.0f);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if USE_DEPTH_BUFFER==1
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
#else
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
#endif
	glViewport(0, 0, windowWidthi,windowHeighti);

	const GLfloat halfVerts[]=MAKE_CENTER_VERTEX(0.5f,0.5f);
	THHalfVertices.Load((void*)halfVerts,sizeof(GLfloat)*8,GL_STATIC_DRAW);

	THDefaultProgram.Load();
	THDefaultProgram.SetColorAdd(0.0f,0.0f,0.0f,0.0f);
	THDefaultProgram.SetColorMultiply(1.0f,1.0f,1.0f,1.0f);

	OnSurfaceCreated();
#ifndef NDEBUG
	if(gameScale.x==0.0f)
	{
		THError("GameSize Not Set! Must be set in OnSurfaceCreated()");
		assert(0);
	}
#endif
}
void THTerm_Display() {
    if (eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(eglDisplay, eglContext);
        }
        if (eglSurface != EGL_NO_SURFACE) {
            eglDestroySurface(eglDisplay, eglSurface);
        }
        eglTerminate(eglDisplay);
    }
    eglDisplay = EGL_NO_DISPLAY;
    eglContext = EGL_NO_CONTEXT;
    eglSurface = EGL_NO_SURFACE;
}
void THEGLInit(THApplicaation* state)
{
    eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint eglMajor,eglMinor;
    eglInitialize(eglDisplay, &eglMajor,&eglMinor);
	THLog("EGL Initialization : %d.%d",eglMajor,eglMinor);

	const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
#if USE_DEPTH_BUFFER==1
			EGL_DEPTH_SIZE,8,
#endif
            EGL_NONE
    };
	EGLint numConfigs;
    EGLConfig config;
    eglChooseConfig(eglDisplay, attribs, &config, 1, &numConfigs);

#if THPLATFORM==THPLATFORM_ANDROID
	EGLint format;
    eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
	
    ANativeWindow_setBuffersGeometry(state->window, 0, 0, format);

    eglSurface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)(state->window), NULL);
#elif THPLATFORM==THPLATFORM_WINDOWS

	eglSurface = eglCreateWindowSurface(eglDisplay, config, *state, NULL);
	
	if(eglSurface == EGL_NO_SURFACE)
	{
		eglGetError(); // Clear error
		eglSurface = eglCreateWindowSurface(eglDisplay, config, NULL, NULL);
	}
#endif


	const EGLint attrib_list[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    eglContext = eglCreateContext(eglDisplay, config, NULL, attrib_list);
#if THPLATFORM==THPLATFORM_WINDOWS
	eglBindAPI(EGL_OPENGL_ES_API);
#endif

	if (eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext) == EGL_FALSE) {
		THError("Unable to eglMakeCurrent");
		assert(0);
		return;
    }

	EGLint sw,sh;
    eglQuerySurface(eglDisplay, eglSurface, EGL_WIDTH, &sw);
    eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &sh);
	windowWidthi=sw;
	windowHeighti=sh;
	windowSize.Set((float)sw,(float)sh);
	gameScale.Set(0.0f,0.0f);
}



void Touch_Point_Down(const float x,const float y)
{
	unsigned int i=currentFrame->buttonList.num;
	THButton** const list=(THButton**)currentFrame->buttonList.arr;
	THButton* btn;

	while(i)
	{
		--i;
		btn=list[i];

		if(btn->enable && btn->HitTest(x,y))
		{
			downedButton=btn;
			if(btn->onDown){btn->onDown(x,y,btn);}
			btn->SetDowned();
			return;
		}
	}
}
void Touch_Point_Up(const float x,const float y)
{
	unsigned int i=currentFrame->buttonList.num;
	THButton** const list=(THButton**)currentFrame->buttonList.arr;
	THButton* btn;

	while(i)
	{
		--i;
		btn=list[i];

		if(btn->enable && btn->HitTest(x,y))
		{
			if(downedButton==btn)
			{
				if(btn->onRelease){btn->onRelease(x,y,btn);}
				break;
			}
			
		}
	}

	if(downedButton)
	{
		downedButton->SetUpped();
		downedButton=0;
	}		
}
void GoFrame(THFrame* f,void* data)
{
	if(currentFrame){currentFrame->OnChangeFrame(f);}
	f->Action(currentFrame,data);									 
	currentFrame = f;
}



THTween* tweenList;
THArray<THTween*> tweenDeleteList(TWEEN_DELETE_LIST_SIZE);

//Timer Function Definition
struct THTimerDef
{
	void (*action)(void* data);
	float timeSkip;
	void* data;

	struct THTimerDef *prev,*next;
};

struct THTimerDef *timerList;
THArray<struct THTimerDef*> timerDeleteList(TIMER_DELETE_LIST_SIZE);





void AddTween(THTween* tween)
{
	//THLog("Tween Adding %.2f Seconds",tween->time);
	tween->next=tweenList;
	if(tweenList)
	{
		tweenList->prev=tween;
	}
	tweenList=tween;
}
static void DeleteTween(THTween* tween)
{
	if(tween->prev)
	{
		tween->prev->next=tween->next;
	}
	if(tween->next)
	{
		tween->next->prev=tween->prev;
	}
	if(tween==tweenList)
	{
		tweenList=tween->next;
	}
}


static void AddTimerToList(struct THTimerDef* timer)
{
	timer->next=timerList;
	if(timerList)
	{
		timerList->prev=timer;
	}
	timerList=timer;
}
static void DeleteTimer(struct THTimerDef* timer)
{
	if(timer->prev)
	{
		timer->prev->next=timer->next;
	}
	if(timer->next)
	{
		timer->next->prev=timer->prev;
	}
	if(timer==timerList)
	{
		timerList=timer->next;
	}
}
void AddTimer(float _timeSkip,void (*_action)(void*),void* _data)
{
	assert(_timeSkip>0.0f);
	assert(_action);

	//THLog("Timer Adding %.2f Seconds",_timeSkip);

	struct THTimerDef* timer=(struct THTimerDef*)malloc(sizeof(struct THTimerDef));
	
	timer->action=_action;
	timer->data=_data;
	timer->timeSkip=_timeSkip;

	timer->next=0;
	timer->prev=0;

	AddTimerToList(timer);
}



void MainEnterFrame()
{
	THTween* tween;
	for(tween=tweenList;tween;tween=tween->next)
	{
		if(tween->step())
		{
			//THLog("Tween End");
			tweenDeleteList.Push(tween);
		}
	}
	struct THTimerDef* timer;
	for(timer=timerList;timer;timer=timer->next)
	{
		timer->timeSkip-=THDeltaTime;
		if(timer->timeSkip<=0.0f)
		{
			//THLog("Timer End");
			timerDeleteList.Push(timer);
		}
	}

	//Deleting, Destruction
	unsigned int i;
	for(i=0;i<tweenDeleteList.num;++i)
	{
		tween=(THTween*)tweenDeleteList.arr[i];
		if(tween->onEndTween)
		{
			tween->onEndTween(tween->data);
		}
		DeleteTween(tween);
		if(tween->autoDelete){delete tween;}
	}
	tweenDeleteList.Clear();
	for(i=0;i<timerDeleteList.num;++i)
	{
		timer=(struct THTimerDef*)timerDeleteList.arr[i];
		timer->action(timer->data);
		DeleteTimer(timer);
		free(timer);
	}
	timerDeleteList.Clear();

	currentFrame->OnEnterFrame();
}
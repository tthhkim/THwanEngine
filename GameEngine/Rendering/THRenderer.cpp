#include "THRenderer.h"
#include <THPrivate.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <GameEngine/DisplayObject/THFrame.h>

#include <GameEngine/Util/THMath.h>
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

extern GLfloat THGameFullVertices[8]={0.0};

extern THVertexBuffer THZeroVertices=THVertexBuffer();

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
			"attribute vec2 aVert;"
			"attribute vec2 aRot;"
			"attribute vec2 aScale;"
			"attribute vec2 aPos;"
			"attribute vec2 aCenter;"
			"attribute vec4 aTex;"

			"varying vec2 vTex;"
			"void main(){"

			"vec2 svert=(aVert-aCenter)*aScale;"
			"vec2 rp=vec2( dot(vec2(aRot.x,-aRot.y),svert) , dot(vec2(aRot.y,aRot.x),svert) );"
			"vec3 rrp=vec3(rp + aPos , 1.0);"
			"gl_Position=vec4(dot(projMat[0],rrp),dot(projMat[1],rrp),0.0,1.0);"

			"vTex=aVert*aTex.zw + aTex.xy;"

			"gl_PointSize=4.0;"
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

	vertexHandler=GetAttribLocation("aVert");
	rotationHandler=GetAttribLocation("aRot");
	scaleHandler=GetAttribLocation("aScale");
	positionHandler=GetAttribLocation("aPos");
	textureHandler=GetAttribLocation("aTex");
	centerHandler=GetAttribLocation("aCenter");

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

	const GLfloat verts[]=MAKE_VERTEX(0.0f,0.0f,1.0f,1.0f);
	THZeroVertices.Load((void*)verts,sizeof(GLfloat)*8,GL_STATIC_DRAW);

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

void GoFrame(THFrame* f,void* data)
{
	if(currentFrame){currentFrame->OnChangeFrame(f);}
	f->Action(currentFrame,data);									 
	currentFrame = f;
}

//Timer Function Definition
class THTimerDef : public THLinkedNode
{
public:
	void (*action)(void* data);
	float timeSkip;
	void* data;
};

THLinkedList m_timerlist;

void AddTimer(float _timeSkip,void (*_action)(void*),void* _data)
{
	assert(_timeSkip>0.0f);
	assert(_action);

	//THLog("Timer Adding %.2f Seconds",_timeSkip);

	THTimerDef* timer=new THTimerDef;
	
	timer->action=_action;
	timer->data=_data;
	timer->timeSkip=_timeSkip;

	m_timerlist.Push(timer);
}



void MainEnterFrame()
{
	THTimerDef *timer=(THTimerDef*)m_timerlist.GetList(),*tnext;
	while(timer)
	{
		tnext=(THTimerDef*)timer->GetLinkedNext();

		timer->timeSkip-=THDeltaTime;
		if(timer->timeSkip<=0.0f)
		{
			timer->action(timer->data);
			m_timerlist.Delete(timer);
			delete timer;
		}

		timer=tnext;
	}

	currentFrame->OnEnterFrame();
}
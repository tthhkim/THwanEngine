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

static EGLDisplay eglDisplay=EGL_NO_DISPLAY;
static EGLSurface eglSurface=EGL_NO_SURFACE;
static EGLContext eglContext=EGL_NO_CONTEXT;

extern THVector2 windowSize(0.0f,0.0f);
extern GLsizei windowWidthi=0;
extern GLsizei windowHeighti=0;
extern THVector2 gameScale(0.0f,0.0f);
extern THVector2 gameMinBound(0.0f,0.0f);
extern THVector2 gameMaxBound(0.0f,0.0f);

extern struct THDPS THDefaultProgram={THProgram(),0,0,0,0,0,0,0,0};

extern THFrame* currentFrame=0;
extern THButton* downedButton=0;

extern const GLfloat defaultFullVertices[]=MAKE_CENTER_VERTEX(1.0f,1.0f);
extern GLfloat fullScreenVertices[8]={0.0};

extern float THDeltaTime=0.0f;

void OnDrawFrame()
{
	//Draw Start
	//glUseProgram(program);
	
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

	const THVector2 size=maxp-minp;

	gameScale=size / windowSize;

	const THVector2 sizeI=1.0f/size;
	const THVector2 mid=(minp+maxp);
	const GLfloat matx[]={
		sizeI.x*2.0f , 0.0f , -mid.x*sizeI.x,
		0.0f , 2.0f*sizeI.y , -mid.y*sizeI.y
	};
	glUniform3fv(THDefaultProgram.projectMatrixHandler,2,matx);

	const GLfloat fv[]=MAKE_VERTEX(minp.x,minp.y,maxp.x,maxp.y);
	memcpy(fullScreenVertices,fv,sizeof(GLfloat)*8);

	THLog("Set Ortho : %.1f , %.1f / %.1f , %.1f",minp.x,minp.y,maxp.x,maxp.y);
}



static GLuint InitShader(const GLchar* source,GLenum type)
{
	GLuint shader=glCreateShader(type);
	glShaderSource(shader,1,&source,NULL);
	glCompileShader(shader);

#ifndef NDEBUG
	//Shader Error Checking
	const char* shaderName=0;
	switch(type)
	{
	case GL_VERTEX_SHADER:
		shaderName="Vertex";
		break;
	case GL_FRAGMENT_SHADER:
		shaderName="Fragment";
		break;
	}

	GLint compileSt;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&compileSt);
	THLog("%sShader Compile : %s",shaderName,compileSt==GL_TRUE?"SUCCESS":"FAIL");

	GLint buflen;
	glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&buflen);

	if(buflen>12)
	{
		GLchar* log_string=new char[buflen+3];
		glGetShaderInfoLog(shader,buflen,0,log_string);

		THError("%sShader Log : \n%s",shaderName,log_string);

		delete[] log_string;
	}
#endif

	return shader;
}
void THProgram::Load(const GLchar* vs,const GLchar* fs)
{
	vertex=InitShader(vs,GL_VERTEX_SHADER);
	fragment=InitShader(fs,GL_FRAGMENT_SHADER);
	program=glCreateProgram();
	glAttachShader(program,vertex);
	glAttachShader(program,fragment);
	glLinkProgram(program);
	glUseProgram(program);
}
void THGLInit()
{
	const GLchar* vs=
			"precision mediump float;"
			"uniform vec3 projectionMat[2];"
			"attribute vec2 vert;"
			"attribute vec2 rot;"
			"attribute vec2 pos;"
			"attribute vec2 aTex;"
			"varying vec2 vTex;"
			"attribute float aHasColor;"
			"varying float vHasColor;"
			"attribute vec4 aColor;"
			"varying vec4 vColor;"
			"attribute vec4 aColorM;"
			"varying vec4 vColorM;"
			"void main(){"
			"vec3 lastP=vec3("
			"dot(vec2(rot.x,-rot.y),vert)+pos.x , dot(vec2(rot.y,rot.x),vert)+pos.y"
			",1.0);"
			"gl_Position=vec4(dot(projectionMat[0],lastP),dot(projectionMat[1],lastP),0.0,1.0);"
			"vTex=aTex;"
			"vHasColor=aHasColor;"
			"vColor=aColor;"
			"vColorM=aColorM;"
			"}";

	const GLchar* fs=
			"precision mediump float;"
			"varying vec2 vTex;"
			"varying mediump vec4 vColor;"
			"varying vec4 vColorM;"
			"varying float vHasColor;"
			"uniform sampler2D sTexture;"
			"void main(){"
			"vec4 cColor;"
			"if(vHasColor>0.5){"
			"gl_FragColor=vColor*vColorM;"
			"}else{"
			"gl_FragColor=texture2D(sTexture,vTex)*vColorM + vColor;"
			"}"
			"}";
	THDefaultProgram.defaultProgram.Load(vs,fs);

	const THProgram& mprogram=THDefaultProgram.defaultProgram;

	THDefaultProgram.vertexHandler=mprogram.GetAttribLocation("vert");
	THDefaultProgram.rotationHandler=mprogram.GetAttribLocation("rot");
	THDefaultProgram.positionHandler=mprogram.GetAttribLocation("pos");
	THDefaultProgram.textureHandler=mprogram.GetAttribLocation("aTex");
	THDefaultProgram.colorHandler=mprogram.GetAttribLocation("aColor");
	THDefaultProgram.colorMultiplyHandler=mprogram.GetAttribLocation("aColorM");
	THDefaultProgram.hasColorHandler=mprogram.GetAttribLocation("aHasColor");
	THDefaultProgram.projectMatrixHandler=mprogram.GetUniformLocation("projectionMat");

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
	glViewport(0, 0, (GLsizei)windowSize.x,(GLsizei)windowSize.y);

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
	
    ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);

    eglSurface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)(app->window), NULL);
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
	THLog("Tween Adding %.2f Seconds",tween->time);
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

	THLog("Timer Adding %.2f Seconds",_timeSkip);

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
			THLog("Tween End");
			tweenDeleteList.Push(tween);
		}
	}
	struct THTimerDef* timer;
	for(timer=timerList;timer;timer=timer->next)
	{
		timer->timeSkip-=THDeltaTime;
		if(timer->timeSkip<=0.0f)
		{
			THLog("Timer End");
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
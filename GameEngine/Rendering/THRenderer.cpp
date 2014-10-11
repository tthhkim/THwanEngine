#include "THRenderer.h"
#include <THPrivate.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <GameEngine/DisplayObject/THFrame.h>

#include <GameEngine/Util/THMath.h>
#include <GameEngine/Util/THTween.h>
#include <GameEngine/Util/THArray.h>

extern EGLDisplay eglDisplay=0;
extern EGLSurface eglSurface=0;
extern EGLContext eglContext=0;

extern THVector2 windowSize(0.0f,0.0f);
extern THVector2 gameSize(0.0f,0.0f);
extern THVector2 gameScale(0.0f,0.0f);
extern THVector2 viewport(0.0f,0.0f);

extern THProgramObject defaultProgram=THProgramObject();
extern GLuint vertexHandler=0;
extern GLuint rotationHandler=0;
extern GLuint positionHandler=0;
extern GLuint textureHandler=0;
extern GLuint projectMatrixHandler=0;
extern GLuint hasColorHandler=0,colorHandler=0;

extern THFrame* currentFrame=0;
THButton* downedButton=0;


const MAKE_CENTER_VERTEX(defaultFullVertices,1.0f,1.0f);





void OnDrawFrame(float dt)
{
	//Draw Start
	//glUseProgram(program);
	
	glClear(GL_COLOR_BUFFER_BIT);

	currentFrame->Draw(dt);

	eglSwapBuffers(eglDisplay, eglSurface);
	//Draw End
}
void SetOrtho(const THVector2& minp,const THVector2& maxp)
{
	const THVector2& size=(maxp-minp);

	gameSize=size;
	viewport=minp;
	
	gameScale=gameSize / windowSize;

	const THVector2& sizeI=1.0f/size;
	const THVector2& mid=(minp+maxp);
	const GLfloat matx[]={
		sizeI.x*2.0f , 0.0f , -mid.x*sizeI.x,
		0.0f , 2.0f*sizeI.y , -mid.y*sizeI.y
	};
	glUniform3fv(projectMatrixHandler,2,matx);

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

	if(buflen>1)
	{
		GLchar* log_string=new char[buflen+3];
		glGetShaderInfoLog(shader,buflen,0,log_string);

		THError("%sShader Log : \n%s",shaderName,log_string);

		delete[] log_string;
	}
#endif

	return shader;
}
void THProgramObject::Load(const GLchar* vs,const GLchar* fs)
{
	vertex=InitShader(vs,GL_VERTEX_SHADER);
	fragment=InitShader(fs,GL_FRAGMENT_SHADER);
	programObject=glCreateProgram();
	glAttachShader(programObject,vertex);
	glAttachShader(programObject,fragment);
	glLinkProgram(programObject);
	glUseProgram(programObject);
}
void THGLInit()
{
	const GLchar* vs=
			"precision mediump float;"
			"uniform vec3 projectionMat[2];"
			"attribute vec2 vert;"
			"attribute vec4 rotmat;"
			"attribute vec2 pos;"
			"attribute vec2 aTex;"
			"varying vec2 vTex;"
			"attribute float aHasColor;"
			"varying float vHasColor;"
			"attribute mediump vec4 aColor;"
			"varying mediump vec4 vColor;"
			"void main(){"
			"vec3 lastP=vec3(vec2(dot(rotmat.rg,vert),dot(rotmat.ba,vert))+pos,1.0);"
			"gl_Position=vec4(dot(projectionMat[0],lastP),dot(projectionMat[1],lastP),0.0,1.0);"
			"vTex=aTex;"
			"vHasColor=aHasColor;"
			"vColor=aColor;"
			"}";

	const GLchar* fs=
			"precision mediump float;"
			"varying vec2 vTex;"
			"varying mediump vec4 vColor;"
			"varying float vHasColor;"
			"uniform sampler2D sTexture;"
			"void main(){"
			"vec4 cColor;"
			"if(vHasColor>0.5){"
			"gl_FragColor=vColor;"
			"}else{"
			"gl_FragColor=texture2D(sTexture,vTex);"
			"}"
			"}";
	defaultProgram.Load(vs,fs);

	const THProgramObject& mprogram=defaultProgram;

	vertexHandler=mprogram.GetAttribLocation("vert");
	rotationHandler=mprogram.GetAttribLocation("rotmat");
	positionHandler=mprogram.GetAttribLocation("pos");
	textureHandler=mprogram.GetAttribLocation("aTex");
	colorHandler=mprogram.GetAttribLocation("aColor");
	hasColorHandler=mprogram.GetAttribLocation("aHasColor");
	projectMatrixHandler=mprogram.GetUniformLocation("projectionMat");

    //glClearColor(0.7f,0.6f,0.5f,1.0f);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
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
	if(currentFrame){currentFrame->onChangeFrame(f);}
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




static void AddTweenToList(THTween* tween)
{
	THLog("Tween Adding...");
	tween->next=tweenList;
	if(tweenList)
	{
		tweenList->prev=tween;
	}
	tweenList=tween;
}
THTween* AddTween(THVector2* _src,const THVector2& _dest,float seconds,float acceleration)
{
	THTween* tween=new THTweenDouble(_src,_dest,seconds,acceleration);
	AddTweenToList(tween);

	return tween;
}
THTween* AddTween(float* _src,float _dest,float seconds,float acceleration)
{
	THTween* tween=new THTweenSingle(_src,_dest,seconds,acceleration);
	AddTweenToList(tween);

	return tween;
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
	THLog("Timer Adding...");
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
void AddTimerFunction(float _timeSkip,void (*_action)(void*),void* _data)
{
	assert(_timeSkip>0.0f);
	assert(_action);

	struct THTimerDef* timer=(struct THTimerDef*)malloc(sizeof(struct THTimerDef));
	
	timer->action=_action;
	timer->data=_data;
	timer->timeSkip=_timeSkip;

	timer->next=0;
	timer->prev=0;

	AddTimerToList(timer);
}



void MainEnterFrame(float dt)
{
	THTween* tween;
	for(tween=tweenList;tween;tween=tween->next)
	{
		if(tween->step(dt))
		{
			THLog("Tween End");
			tweenDeleteList.Push(tween);
		}
	}
	struct THTimerDef* timer;
	for(timer=timerList;timer;timer=timer->next)
	{
		timer->timeSkip-=dt;
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
			THLog("Tween Deleting...");
			tween->onEndTween(tween->data);
		}
		DeleteTween(tween);
		delete tween;
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

	currentFrame->onEnterFrame(dt);
}
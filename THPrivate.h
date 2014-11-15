#ifndef THKim_THPrivate
#define THKim_THPrivate

#include <THSettings.h>

#if THPLATFORM==THPLATFORM_ANDROID
#include <android_native_app_glue.h>
#include <android/log.h>
#include <android/input.h>
#include <android/asset_manager.h>
#endif

#include <GLES2/gl2.h>

#include <assert.h>
#include <GameEngine/Rendering/THDrawing.h>

#include <GameEngine/Util/THMath.h>

class THFrame;
class THTween;

#if THPLATFORM==THPLATFORM_WINDOWS
#define APPLICATION_NAME "TestApplicatonName"
#define WINDOW_CLASS_NAME "SAMPLE_WIDOW_CLASSNAME"
#endif

#ifdef NDEBUG
	#define THLog(...)
	#define THError(...)
#else
	#define TH_DEBUG
#if THPLATFORM==THPLATFORM_ANDROID
#if SHOW_LOG==1
	#define THLog(...) __android_log_print(ANDROID_LOG_DEBUG,"TH-DEBUG",__VA_ARGS__)
#else
	#define THLog(...)
#endif
	#define THError(...) __android_log_print(ANDROID_LOG_ERROR,"TH-ERROR",__VA_ARGS__)
#elif THPLATFORM==THPLATFORM_WINDOWS
	#include <cstdio>
#if SHOW_LOG==1
	#define THLog(...) printf("\n"__VA_ARGS__)
#else
	#define THLog(...)
#endif
	#define THError(...) printf("\n"__VA_ARGS__)
#endif
#endif

#define MAKE_CENTER_VERTEX(hw,hh) \
	{-hw, -hh,hw, -hh,-hw,hh,hw,hh}
#define MAKE_VERTEX(left,bottom,right,top) \
	{left, bottom,right, bottom,left,top,right,top}

extern const GLfloat THFullVertices[];
extern GLfloat THGameFullVertices[];
extern const GLfloat THZeroVertices[];
extern GLfloat THProjectMatrix[];
extern THVertexBuffer THHalfVertices;

extern const THVector2 THVector_zero;

#ifndef NDEBUG
extern float cFPS;
#endif

extern THVector2 windowSize;
extern GLsizei windowWidthi;
extern GLsizei windowHeighti;
extern THVector2 gameScale;

extern THVector2 gameMaxBound;
extern THVector2 gameMinBound;

extern THFrame* currentFrame;

extern float THDeltaTime;

struct THDPS
{
	THProgram defaultProgram;
	GLuint vertexHandler;
	GLuint rotationHandler;
	GLuint scaleHandler;
	GLuint positionHandler;
	GLuint textureHandler;
	GLuint projectMatrixHandler;
	GLuint colorAddHandler,colorMultiplyHandler;
};
extern struct THDPS THDefaultProgram;


void GoFrame(THFrame* f,void* data=0);

//pthread_t newThread(void* (*function)(void*), void* data);

void SetOrtho(const THVector2& minp,const THVector2& maxp);

#if THPLATFORM==THPLATFORM_ANDROID
typedef struct android_app THApplicaation;
typedef long THTimeType;
#elif THPLATFORM==THPLATFORM_WINDOWS
#include <Windows.h>
typedef HWND THApplicaation;
typedef long long THTimeType;
#endif

void OnCreate(THApplicaation* state);
void OnSurfaceCreated();
//void OnSaveInstanceState(saved_state* saved);
//void OnLoadInstanceState(saved_state* loaded);
void OnResume();
void OnPause();
void OnDestroy();
void SetFrameRate(float rate);

static inline void SetColorAdd(float r,float g,float b,float a)
{
	glUniform4f(THDefaultProgram.colorAddHandler,r,g,b,a);
}
static inline void SetColorMultiply(float r,float g,float b,float a)
{
	glUniform4f(THDefaultProgram.colorMultiplyHandler,r,g,b,a);
}
static inline THVector2 ProjectVector(const THVector2& v)
{
	return THVector2( v.x*THProjectMatrix[0]+THProjectMatrix[2] , v.y*THProjectMatrix[3]+THProjectMatrix[5] );
}

THTimeType GetCurrentTimeMicro();

void AddTimer(float timeSkip,void (*action)(void*),void* data);
void AddTween(THTween* tween);

static inline float getGameX(float _x)
{
	return _x*gameScale.x + gameMinBound.x;
}
static inline float getGameY(float _y)
{
	return -_y*gameScale.y + gameMaxBound.y;
}
static inline THVector2 GetGameRelative(const THVector2& v)
{
	return gameMinBound+((gameMaxBound-gameMinBound)*v);
}

#endif

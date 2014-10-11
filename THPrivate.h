#ifndef THKim_THPrivate
#define THKim_THPrivate

#define MAX_FILE_NAME 8
#define TIMER_DELETE_LIST_SIZE 4
#define TWEEN_DELETE_LIST_SIZE 4

#define USE_LIBPNG 0

#define THPLATFORM_WINDOWS 1
#define THPLATFORM_ANDROID 2

#define THPLATFORM THPLATFORM_WINDOWS

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
#define WINDOW_WIDTH 360
#define WINDOW_HEIGHT 640
#define APPLICATION_NAME "TestApplicatonName"
#define WINDOW_CLASS_NAME "SAMPLE_WIDOW_CLASSNAME"
#endif

#ifdef NDEBUG
	#define THLog(...)
	#define THError(...)
#else
	#define TH_DEBUG
#if THPLATFORM==THPLATFORM_ANDROID
	#define THLog(...) __android_log_print(ANDROID_LOG_DEBUG,"TH-DEBUG",__VA_ARGS__)
	#define THError(...) __android_log_print(ANDROID_LOG_ERROR,"TH-ERROR",__VA_ARGS__)
#elif THPLATFORM==THPLATFORM_WINDOWS
	#include <cstdio>
	#define THLog(...) printf("\n"__VA_ARGS__)
	#define THError(...) printf("\nERROR	:	"__VA_ARGS__)
#endif
#endif

#define MAKE_CENTER_VERTEX(name,hw,hh) \
	GLfloat name[]={-hw, hh,hw, hh,hw,-hh,-hw,-hh}
#define MAKE_VERTEX(name,left,right,top,bottom) \
	GLfloat name[]={left, bottom,right, bottom,right,top,left,top}

extern const GLfloat defaultFullVertices[];

#ifndef NDEBUG
extern float cFPS;
#endif

extern THVector2 windowSize;
extern THVector2 gameSize;
extern THVector2 gameScale;
extern THVector2 viewport;
extern THFrame* currentFrame;

extern THProgramObject defaultProgram;
extern GLuint vertexHandler;
extern GLuint rotationHandler;
extern GLuint positionHandler;
extern GLuint textureHandler;
extern GLuint projectMatrixHandler;
extern GLuint hasColorHandler,colorHandler;

extern THVector2 windowSize;
extern THVector2 gameSize;
extern THVector2 gameScale;
extern THVector2 viewport;
extern THFrame* currentFrame;


void GoFrame(THFrame* f,void* data);

//pthread_t newThread(void* (*function)(void*), void* data);

void SetOrtho(const THVector2& minp,const THVector2& maxp);

#if THPLATFORM==THPLATFORM_ANDROID
typedef struct android_app* THApplicaation;
typedef long THTimeType;
#elif THPLATFORM==THPLATFORM_WINDOWS
typedef void THApplicaation;
typedef long long THTimeType;
#endif

void OnCreate(THApplicaation* state);
THImage LoadTexture(const char* name);
void OnSurfaceCreated();
//void OnSaveInstanceState(saved_state* saved);
//void OnLoadInstanceState(saved_state* loaded);
void OnResume();
void OnPause();
void OnDestroy();
void SetFrameRate(float rate);


THTimeType GetCurrentTimeMicro();
void AddTimerFunction(float timeSkip,void (*action)(void*),void* data);

THTween* AddTween(THVector2* _src,const THVector2& _dest,float seconds,float acceleration);
THTween* AddTween(float* _src,float _dest,float seconds,float acceleration);

static inline float getGameX(float _x)
{
	return _x*gameScale.x + viewport.x;
}
static inline float getGameY(float _y)
{
	return _y*gameScale.y + viewport.y;
}

#endif

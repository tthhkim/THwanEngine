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

extern GLfloat THGameFullVertices[];
extern GLfloat THProjectMatrix[];
extern THVertexBuffer THZeroVertices;

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

class THDefProgram : public THProgram
{
public:
	void Load();
	GLint vertexHandler;
	GLint rotationHandler;
	GLint scaleHandler;
	GLint positionHandler;
	GLint centerHandler;
	GLint textureHandler;
	GLint projectMatrixHandler;
	GLint colorAddHandler,colorMultiplyHandler;

	inline void EnableVertex() const
	{
		glEnableVertexAttribArray(vertexHandler);
	}
	inline void EnableTexture() const
	{
		glEnableVertexAttribArray(textureHandler);
	}
	inline void DisableVertex() const
	{
		glDisableVertexAttribArray(vertexHandler);
	}
	inline void DisableTexture() const
	{
		glDisableVertexAttribArray(textureHandler);
	}
	inline void SetRotation(float c,float s) const
	{
		glVertexAttrib2f(rotationHandler,c,s);
	}
	inline void SetScale(float x,float y) const
	{
		glVertexAttrib2f(scaleHandler,x,y);
	}
	inline void SetPosition(float x,float y) const
	{
		glVertexAttrib2f(positionHandler,x,y);
	}
	inline void SetRotation(const GLfloat *v) const
	{
		glVertexAttrib2fv(rotationHandler,v);
	}
	inline void SetScale(const GLfloat *v) const
	{
		glVertexAttrib2fv(scaleHandler,v);
	}
	inline void SetPosition(const GLfloat *v) const
	{
		glVertexAttrib2fv(positionHandler,v);
	}
	inline void SetColorAdd(float r,float g,float b,float a)
	{
		glUniform4f(colorAddHandler,r,g,b,a);
	}
	inline void SetColorMultiply(float r,float g,float b,float a)
	{
		glUniform4f(colorMultiplyHandler,r,g,b,a);
	}
protected:
};
extern THDefProgram THDefaultProgram;


void GoFrame(THFrame* f,void* data=0);

//pthread_t newThread(void* (*function)(void*), void* data);

void SetOrtho(const THVector2& minp,const THVector2& maxp);

#if THPLATFORM==THPLATFORM_ANDROID
typedef struct android_app THApplicaation;
typedef long THTimeType;
typedef AAsset* THAsset;
typedef int THAssetMode;
#elif THPLATFORM==THPLATFORM_WINDOWS
#include <Windows.h>
typedef HWND THApplicaation;
typedef long long THTimeType;
typedef FILE* THAsset;
typedef const char* THAssetMode;
#endif

void OnCreate(THApplicaation* state);
void OnSurfaceCreated();
//void OnSaveInstanceState(saved_state* saved);
//void OnLoadInstanceState(saved_state* loaded);
void OnResume();
void OnPause();
void OnDestroy();
void SetFrameRate(float rate);
#define TH_PNG_RGB 1
#define TH_PNG_RGBA 2
#define TH_PNG_GREY 3
#define TH_PNG_GREY_ALPHA 4
GLenum THImage2GLImageType(int type);
unsigned char* LoadImageBuffer(const char *filename,size_t& width,size_t& height,int colorType);
THAsset THAsset_open(const char *name,
	THAssetMode mode=
#if THPLATFORM==THPLATFORM_ANDROID
	AASSET_MODE_STREAMING
#elif THPLATFORM==THPLATFORM_WINDOWS
	"rb"
#endif
	);
void THAsset_close(THAsset asset);
size_t THAsset_seek(THAsset asset,size_t offset,int whence);
size_t THAsset_read(THAsset asset,void *data,size_t bytes);
size_t THAsset_length(THAsset asset);


static inline void ViewportInit()
{
	glViewport(0,0,windowWidthi,windowHeighti);
}
static inline THVector2 ProjectVector(const THVector2& v)
{
	return THVector2( v.x*THProjectMatrix[0]+THProjectMatrix[2] , v.y*THProjectMatrix[3]+THProjectMatrix[5] );
}

THTimeType GetCurrentTimeMicro();

void AddTimer(float timeSkip,void (*action)(void*),void* data);

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

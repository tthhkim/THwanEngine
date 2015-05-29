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
#include <GameEngine/Util/THMath3D.h>

#ifndef NDEBUG
#define TH_ISDEBUG 1
#endif

#ifdef TH_ISDEBUG
	#define TH_GLERROR_CHECK()\
		{\
			const GLenum gl_estatus=glGetError();\
			if(gl_estatus!=GL_NO_ERROR)\
			{\
				THError("GL Error : %d",gl_estatus);\
			}\
		}
#else
	#define TH_GLERROR_CHECK
#endif
#define MAKE_VERTEX(l,t,r,b)\
		{l,t,\
		r,t,\
		l,b,\
		r,b};

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


#if THPLATFORM==THPLATFORM_ANDROID
typedef AAsset* THAsset;
typedef int THAssetMode;
#elif THPLATFORM==THPLATFORM_WINDOWS
#include <Windows.h>
typedef FILE* THAsset;
typedef const char* THAssetMode;
#endif

long long GetCurrentTimeMicro();

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

#if THPLATFORM==THPLATFORM_ANDROID
#if USE_ACCELEROMETER_SENSOR==1
#include <android/sensor.h>
THVector3& GetSensorVector();
#endif
#endif




#endif

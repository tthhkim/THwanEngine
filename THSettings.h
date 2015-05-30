#ifndef THKim_THSetting
#define THKim_THSetting

#ifndef NDEBUG
#define TH_ISDEBUG 1
#endif

#define SHOW_LOG 1

#define THPLATFORM_WINDOWS 1
#define THPLATFORM_ANDROID 2

#define THPLATFORM 1

#if THPLATFORM==THPLATFORM_WINDOWS
#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#elif THPLATFORM==THPLATFORM_ANDROID
#include <GLES2/gl2.h>
#endif

#define USE_ACCELEROMETER_SENSOR 0
#define TH_SENSOR_DELAY 100000
//60fps
//In Micro unit


#define DEFAULT_ASSET_PATH "D:/assets/"

#define WINDOW_WIDTH 360
#define WINDOW_HEIGHT 640

#endif

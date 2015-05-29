#ifndef THKim_THSetting
#define THKim_THSetting

#define USE_GLES_VERSION 2
#if USE_GLES_VERSION==2
#define GLES_HEADER "GLES2/gl2.h"
#elif USE_GLES_VERSION==3
#define GLES_HEADER "GLES3/gl3.h"
#elif USE_GLES_VERSION==31
#define GLES_HEADER "GLES3/gl31.h"
#endif

#define DEPTH_SIZE_BIT 0

#define SHOW_LOG 1

#define THPLATFORM_WINDOWS 1
#define THPLATFORM_ANDROID 2

#define THPLATFORM THPLATFORM_WINDOWS

#define USE_ACCELEROMETER_SENSOR 0
#define TH_SENSOR_DELAY 100000
//60fps
//In Micro unit


#define DEFAULT_ASSET_PATH "D:/assets/"

#define WINDOW_WIDTH 360
#define WINDOW_HEIGHT 640

#endif

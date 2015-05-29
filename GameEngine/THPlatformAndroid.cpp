#include "THApplication.h"
#include <THPrivate.h>
#include <time.h>

#if THPLATFORM==THPLATFORM_ANDROID

extern THApplication THApp;

timespec timesp;
long long GetCurrentTimeMicro()
{
	clock_gettime(CLOCK_MONOTONIC,&timesp);
	return timesp.tv_sec*1000000l + timesp.tv_nsec*1e-3f;
}

#include <android/input.h>
#include <android/keycodes.h>

#if USE_ACCELEROMETER_SENSOR==1
#include <android/sensor.h>

ASensorManager* sensorManager;
const ASensor* accelerometerSensor;
ASensorEventQueue* asensorEventQueue;
THVector3 asensorVector;
THVector3& GetSensorVector()
{
	return asensorVector;
}
#endif

static void init_display(struct android_app* app) {
	THApp.EGLInit(app->window);
	THApp.GLInit();
	THApp.OnSurfaceCreated();
}
static int32_t handle_input(struct android_app* app, AInputEvent* event) {
	if(AINPUT_EVENT_TYPE_MOTION==AInputEvent_getType(event))
	{
		const int32_t action=AMotionEvent_getAction(event)&AMOTION_EVENT_ACTION_MASK;

		const THVector2 p(AMotionEvent_getX(event,0),AMotionEvent_getY(event,0));

		//currentFrame->OnTouchEvent(event,action,p);

		switch(action)
		{
		case AMOTION_EVENT_ACTION_DOWN:
			THApp.OnTouchDown(p);
			break;
		case AMOTION_EVENT_ACTION_UP:
			THApp.OnTouchUp(p);
			break;
		case AMOTION_EVENT_ACTION_MOVE:
			THApp.OnTouchMove(p);
			break;
		}
		return 1;
	}else
	{
		if(AKEYCODE_BACK==AKeyEvent_getKeyCode(event))
		{
			if(AKEY_EVENT_ACTION_UP==AKeyEvent_getAction(event))
			{
				return THApp.OnBackReleased();
			}
		}
		
	}
	return 0;
}


static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
			THLog("Init Window()");
            if (app->window) {
                init_display(app);
            }
            break;
		case APP_CMD_TERM_WINDOW:
			THLog("Term Window()");
			THApp.TermDisplay();
			break;
		case APP_CMD_GAINED_FOCUS:
			THLog("Gained Focus()");

			THApp.Init();

#if USE_ACCELEROMETER_SENSOR==1
			ASensorEventQueue_enableSensor(asensorEventQueue,accelerometerSensor);
			ASensorEventQueue_setEventRate(asensorEventQueue,accelerometerSensor, TH_SENSOR_DELAY);
#endif

			THApp.OnResume();
			break;
		case APP_CMD_LOST_FOCUS:
			THLog("Lost Focus()");

			THApp.SetRunning(false);
#if USE_ACCELEROMETER_SENSOR==1
			ASensorEventQueue_disableSensor(asensorEventQueue,accelerometerSensor);
#endif

			THApp.OnPause();
			break;

			/*
		case APP_CMD_SAVE_STATE:
		{
			
			THLog("Save States()");
			saved_state* savesth=(saved_state*)malloc(sizeof(saved_state));
			OnSaveInstanceState(savesth);
			app->savedState=savesth;
			app->savedStateSize=sizeof(saved_state);
			
		}
            break;
			*/
		case APP_CMD_DESTROY:
			THLog("OnDestroy()");
			OnDestroy();
			break;
    }
}
#include <android/asset_manager.h>
AAssetManager* assetManager;

void android_main(struct android_app* state)
{
	InitVars();

	THLog("OnCreate()");
	app_dummy();

	assetManager=state->activity->assetManager;

#if USE_ACCELEROMETER_SENSOR==1
	sensorManager = ASensorManager_getInstance();
    accelerometerSensor = ASensorManager_getDefaultSensor(sensorManager,ASENSOR_TYPE_ACCELEROMETER);
    asensorEventQueue = ASensorManager_createEventQueue(sensorManager,state->looper, LOOPER_ID_USER, NULL, NULL);
#endif

	THApp.OnCreate();
#ifndef NDEBUG
	if(currentFrame==0)
	{
		THError("Frame Not Set! Must be set in OnCreate()");
		assert(0);
	}
#endif

	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent=handle_input;
	/*
	if (state->savedState) {
		THLog("Load From Saved State");
		OnLoadInstanceState((saved_state*)state->savedState);
    }
	*/
	struct android_poll_source* source;
    int events,ident;


	THLastNanosec=GetCurrentTimeMicro();
	while (1) {
        while ((ident=ALooper_pollAll(0, NULL, &events,
                (void**)&source)) >= 0) {
            if (source) {
                source->process(state, source);
            }
#if USE_ACCELEROMETER_SENSOR==1
			if (ident == LOOPER_ID_USER)
			{
				ASensorEvent aevent;
				while (ASensorEventQueue_getEvents(asensorEventQueue,&aevent, 1) > 0)
				{
					asensorVector.Set(aevent.acceleration.x, aevent.acceleration.y,aevent.acceleration.z);
				}
			}
#endif
            if(state->destroyRequested)
            {
                THLog("Destroy Requested");
                THApp.TermDisplay();
				return;
            }
        }
		THApp.Loop();
    }
}

void THApplication::EGLInitSurface(EGLNativeWindowType window)
{
	EGLint format;
    eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
	
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);

    eglSurface = eglCreateWindowSurface(eglDisplay, m_eglConfig, window, NULL);
}

THAsset THAsset_open(const char *name,THAssetMode mode)
{
	return AAssetManager_open(assetManager, name, mode);
}
void THAsset_close(THAsset asset)
{
	AAsset_close(asset);
}
size_t THAsset_seek(THAsset asset,size_t offset,int whence)
{
	return AAsset_seek(asset, offset, whence);
}
size_t THAsset_read(THAsset asset,void *data,size_t bytes)
{
	return AAsset_read(asset, data, bytes);
}
size_t THAsset_length(THAsset asset)
{
	return AAsset_getLength(asset);
}

#endif
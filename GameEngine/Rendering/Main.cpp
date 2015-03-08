#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include <time.h>

#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/DisplayObject/THFrame.h>

#include <GameEngine/Util/THMath.h>
#include <THPrivate.h>

#include <GameEngine/Rendering/THRenderer.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>


static THTimeType THFrameRate;

#if THPLATFORM==THPLATFORM_ANDROID

timespec timesp;
THTimeType GetCurrentTimeMicro()
{
	clock_gettime(CLOCK_MONOTONIC,&timesp);
	return timesp.tv_sec*1000000l + timesp.tv_nsec*1e-3f;
}
void SetFrameRate(float _frameRate)
{
	THFrameRate=1000000000l/_frameRate;
}
#elif THPLATFORM==THPLATFORM_WINDOWS


#include <Windows.h>

static LARGE_INTEGER timesp;
static double THTimeFrequency;

THTimeType GetCurrentTimeMicro()
{
	QueryPerformanceCounter(&timesp);
	return (long long)((double)timesp.QuadPart*THTimeFrequency);
}
void SetFrameRate(float _frameRate)
{
	THFrameRate=(THTimeType)(1000000.0f/_frameRate);
}
#endif

//randomize random field
static void Randomize()
{
	unsigned int rv=(unsigned int)time(0);
	unsigned int cvp=0;
	unsigned int cvm=1;
	while(rv)
	{
		cvp+=rv;
		rv=rv>>1;
		cvm*=rv?rv:1;
	}
	srand( (unsigned int)((cvp>>1) + (cvm>>1)) );


	extern double InvRandomMax;
	InvRandomMax=1.0/RAND_MAX;
}


static THTimeType THLastNanosec;
static bool THisRunning;

#ifndef NDEBUG
extern float cFPS=0.0f;
#endif

#if THPLATFORM==THPLATFORM_ANDROID
extern EGLDisplay eglDisplay;
static void RenderEnterFrame()
{

	clock_gettime(CLOCK_MONOTONIC,&timesp);
	const long ct=timesp.tv_sec*1000000000l + timesp.tv_nsec;
	const long gap=ct-THLastNanosec;

#ifndef NDEBUG
	cFPS=(float)((double)1000000000/(double)gap);
#endif

	if(gap>THFrameRate)
	{
		THDeltaTime=(float)((double)gap*1e-9);
		if(THisRunning){MainEnterFrame();}
		if(eglDisplay!=EGL_NO_DISPLAY){OnDrawFrame();}
		THLastNanosec=ct;
	}
}
#elif THPLATFORM==THPLATFORM_WINDOWS

static void RenderEnterFrame()
{
	const THTimeType ct=GetCurrentTimeMicro();
	const THTimeType gap=ct-THLastNanosec;


#ifndef NDEBUG
	cFPS=(float)(1000000.0/(double)gap);
#endif

	if(gap>THFrameRate)
	{
		THDeltaTime=(float)((double)gap*1e-6);
		if(THisRunning){MainEnterFrame();}
		OnDrawFrame();
		THLastNanosec=ct;
	}
}

#endif

static THVector2 Touch_last;

#if THPLATFORM==THPLATFORM_ANDROID
#include <android/input.h>
#include <android/keycodes.h>

static void init_display(struct android_app* app) {
	THEGLInit(app);
	THGLInit();
}
static int32_t handle_input(struct android_app* app, AInputEvent* event) {
	if(AINPUT_EVENT_TYPE_MOTION==AInputEvent_getType(event))
	{
		if(currentFrame->canTouch==false){return 0;}
		const int32_t action=AMotionEvent_getAction(event)&AMOTION_EVENT_ACTION_MASK;

		const THVector2 p(getGameX(AMotionEvent_getX(event,0)),getGameY(AMotionEvent_getY(event,0)));

		currentFrame->OnTouchEvent(event,action,p);

		switch(action)
		{
		case AMOTION_EVENT_ACTION_DOWN:
			currentFrame->OnTouchDown(p);
			Touch_Point_Down(p);
			Touch_last=p;
			break;
		case AMOTION_EVENT_ACTION_UP:
			currentFrame->OnTouchUp(p);
			Touch_Point_Up(p);
			break;
		case AMOTION_EVENT_ACTION_MOVE:
			currentFrame->OnTouchMove(p,p-Touch_last);
			Touch_last=p;
			break;
		}
		return 1;
	}else
	{
		if(AKEYCODE_BACK==AKeyEvent_getKeyCode(event))
		{
			if(AKEY_EVENT_ACTION_UP==AKeyEvent_getAction(event))
			{
				return currentFrame->OnBackReleased();
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
			THTerm_Display();
			break;
		case APP_CMD_GAINED_FOCUS:
			THLog("Gained Focus()");

			THisRunning=true;

			clock_gettime(CLOCK_MONOTONIC,&timesp);

			THLastNanosec=timesp.tv_sec*1000000000l + timesp.tv_nsec;

			OnResume();
			break;
		case APP_CMD_LOST_FOCUS:
			THLog("Lost Focus()");

			THisRunning=false;

			OnPause();
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
	THLog("OnCreate()");
	app_dummy();

	assetManager=state->activity->assetManager;

	SetFrameRate(60.0f);

	Randomize();

	OnCreate(state);
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
    int events;

	clock_gettime(CLOCK_MONOTONIC,&timesp);
	THLastNanosec=timesp.tv_sec*1000000000l + timesp.tv_nsec;
	while (1) {
        while (ALooper_pollAll(0, NULL, &events,
                (void**)&source) >= 0) {
            if (source) {
                source->process(state, source);
            }

            if(state->destroyRequested)
            {
                THLog("Destroy Requested");
                THTerm_Display();
				return;
            }
        }
		RenderEnterFrame();
    }
}
#elif THPLATFORM==THPLATFORM_WINDOWS

#include <windowsx.h>
static bool destroyRequested=false;
static bool isMouseDown;
LRESULT CALLBACK HandleWindowMessages(HWND nativeWindow, UINT message, WPARAM windowParameters, LPARAM longWindowParameters)
{
	
	switch (message)
	{
	case WM_SYSCOMMAND:
		// Handle 2 system messages: screen saving and monitor power. We need to prevent them whilst we're rendering for a short time.
		{
			switch (windowParameters)
			{
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
				{
					// Return 0 to let Windows know we don't want to sleep or turn the monitor off right now.
					return 0;
				}
			}
			break;
		}
	case WM_CLOSE:
		// Handle the close message when a user clicks the quit icon of the window
		{
			// Tell the demo that it should stop rendering.
			destroyRequested = true;

			// Post a quit message
			PostQuitMessage(0);

			// Return 1 to let Windows know the message has been successfully handled
			return 1;
		}
	case WM_MOUSEMOVE:
	{
		if(currentFrame->canTouch==false || isMouseDown==false){return 0;}
		const THVector2 p(getGameX((float)(GET_X_LPARAM(longWindowParameters))),getGameY((float)(GET_Y_LPARAM(longWindowParameters))));
		currentFrame->OnTouchEvent((THMotionEvent*)message,longWindowParameters,p);
		currentFrame->OnTouchMove(p,p-Touch_last);
		Touch_last=p;

		return 1;
	}
		break;
	case WM_LBUTTONDOWN:
	{
		if(currentFrame->canTouch==false){return 0;}
		isMouseDown=true;
		const THVector2 p(getGameX((float)(GET_X_LPARAM(longWindowParameters))),getGameY((float)(GET_Y_LPARAM(longWindowParameters))));
		currentFrame->OnTouchEvent((THMotionEvent*)message,longWindowParameters,p);
		currentFrame->OnTouchDown(p);
		Touch_last=p;
		Touch_Point_Down(p);

		return 1;
	}
		break;
	case WM_LBUTTONUP:
	{
		if(currentFrame->canTouch==false){return 0;}
		isMouseDown=false;
		const THVector2 p(getGameX((float)(GET_X_LPARAM(longWindowParameters))),getGameY((float)(GET_Y_LPARAM(longWindowParameters))));
		currentFrame->OnTouchEvent((THMotionEvent*)message,longWindowParameters,p);
		currentFrame->OnTouchUp(p);
		Touch_Point_Up(p);

		return 1;
	}
		break;
	case WM_KEYUP:
		if(windowParameters==VK_BACK)
		{
			if(currentFrame->OnBackReleased()==0)
			{
				destroyRequested = true;

				// Post a quit message
				PostQuitMessage(0);

				return 1;
			}
		}else
		{
			currentFrame->OnKeyUp(windowParameters);
			return 1;
		}
		break;
	case WM_KEYDOWN:
		if(windowParameters!=VK_BACK)
		{
			currentFrame->OnKeyDown(windowParameters);
			return 1;
		}
		break;
	case WM_RBUTTONDOWN:
	{
		const THVector2 p(getGameX((float)(GET_X_LPARAM(longWindowParameters))),getGameY((float)(GET_Y_LPARAM(longWindowParameters))));
		currentFrame->OnRightTouchDown(p);
	}
		break;
	}
	return DefWindowProc(nativeWindow, message, windowParameters, longWindowParameters);
}
static bool CreateWindowAndDisplay( HINSTANCE applicationInstance, HWND &nativeWindow, HDC &deviceContext ) 
{
	// Describe the native window in a window class structure
	WNDCLASS nativeWindowDescription;
	nativeWindowDescription.style = CS_HREDRAW | CS_VREDRAW;
	nativeWindowDescription.lpfnWndProc = HandleWindowMessages;
	nativeWindowDescription.cbClsExtra = 0;
	nativeWindowDescription.cbWndExtra = 0;
	nativeWindowDescription.hInstance = applicationInstance;
	nativeWindowDescription.hIcon = 0;
	nativeWindowDescription.hCursor = 0;
	nativeWindowDescription.lpszMenuName = 0;
	nativeWindowDescription.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	nativeWindowDescription.lpszClassName = WINDOW_CLASS_NAME;

	// Register the windows class with the OS.
	ATOM registerClass = RegisterClass(&nativeWindowDescription);
	if (!registerClass)
	{
		THError("Failed to register the window class");
	}

	// Create a rectangle describing the area of the window
	RECT windowRectangle;
	SetRect(&windowRectangle, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	AdjustWindowRectEx(&windowRectangle, WS_CAPTION | WS_SYSMENU, false, 0);

	// Create the window from the available information
	nativeWindow = CreateWindow( WINDOW_CLASS_NAME, APPLICATION_NAME, WS_VISIBLE | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 
	                             windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top,
	                             NULL, NULL, applicationInstance, NULL);
	if (!nativeWindow)
	{
		THError("Failed to create the window");
		return false;
	}

	// Get the associated device context from the window
	deviceContext = GetDC(nativeWindow);
	if (!deviceContext)
	{
		THError("Failed to create the device context");
		return false;
	}

	return true;
}

int WINAPI WinMain(HINSTANCE applicationInstance, HINSTANCE previousInstance, TCHAR* /*commandLineString*/, int /*showCommand*/)
{
	Randomize();

	SetFrameRate(60.0f);
	AllocConsole();
	using namespace std;
	freopen("conin$","r",stdin);
	freopen("conout$","w",stdout);
	freopen("conout$","w",stderr);


	THLog("OnCreate()");
	OnCreate(0);


	// Windows variables
	HWND				nativeWindow = NULL;
	HDC					deviceContext = NULL;
	CreateWindowAndDisplay(applicationInstance,nativeWindow,deviceContext);
	
	THLog("EGLInit");
	THEGLInit(&nativeWindow);
	THLog("GLInit");
	THGLInit();

	THLog("GainFocus");
	OnResume();

	THisRunning=true;

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	THTimeFrequency=1000000.0/(double)freq.QuadPart;

	THLastNanosec=GetCurrentTimeMicro();
	while(destroyRequested==false)
	{
		// Check for messages from the windowing system. These will pass through the callback registered earlier.
		MSG eventMessage;
		while(PeekMessage(&eventMessage, nativeWindow, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&eventMessage);
			DispatchMessage(&eventMessage);
		}

		RenderEnterFrame();
	}
	THisRunning=false;

	THLog("LostFocus");
	OnPause();

	THLog("OnDestroy()");
	OnDestroy();

	THDefaultProgram.Delete();
	THTerm_Display();

	if (deviceContext)
	{
		ReleaseDC(nativeWindow, deviceContext);
	}

	// Destroy the window
	if (nativeWindow)
	{
		DestroyWindow(nativeWindow);
	}
}

#endif

#if USE_PNG==1

#include <lodepng.h>
unsigned char* LoadImageBuffer(const char *filename,size_t& width,size_t& height,int colorType)
{
	THAsset asset=THAsset_open(filename,
#if THPLATFORM==THPLATFORM_ANDROID
		AASSET_MODE_STREAMING
#elif THPLATFORM==THPLATFORM_WINDOWS
		"rb"
#endif
		);
	int size=THAsset_length(asset);
	unsigned char* mem=new unsigned char[size];
	THAsset_read(asset,mem,size);

	unsigned char* colorBuf;

	LodePNGColorType ctype;
	switch(colorType)
	{
	case TH_PNG_GREY:
		ctype=LCT_GREY;
		break;
	case TH_PNG_GREY_ALPHA:
		ctype=LCT_GREY_ALPHA;
		break;
	case TH_PNG_RGB:
		ctype=LCT_RGB;
		break;
	case TH_PNG_RGBA:
		ctype=LCT_RGBA;
		break;
	}
	LodePNGState state;
	lodepng_state_init(&state);
	state.info_raw.colortype=ctype;
	state.info_raw.bitdepth=8;
	state.decoder.read_text_chunks=0;
	lodepng_decode(&colorBuf,&width,&height,&state,mem,size);
	lodepng_state_cleanup(&state);

	delete[] mem;
	THLog("LibPNG // Width : %d , Height : %d",width,height);

	return colorBuf;
}


GLenum THImage2GLImageType(int type)
{
	switch(type)
	{
	case TH_PNG_RGB:
		return GL_RGB;
	case TH_PNG_RGBA:
		return GL_RGBA;
	case TH_PNG_GREY:
		return GL_LUMINANCE;
	case TH_PNG_GREY_ALPHA:
		return GL_LUMINANCE_ALPHA;
	}
	return 0;
}
void THImage::LoadFile(const char* name,int colorType,GLfloat filter,bool isRepeat)
{
	size_t widthi,heighti;
	void* colorBuf=LoadImageBuffer(name,widthi,heighti,colorType);

	SetSize(widthi,heighti);

	Load(colorBuf,THImage2GLImageType(colorType),filter,isRepeat);
	free(colorBuf);
}

#if THPLATFORM==THPLATFORM_ANDROID
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
#elif THPLATFORM==THPLATFORM_WINDOWS
THAsset THAsset_open(const char *name,THAssetMode mode)
{
	return fopen(name,mode);
}
void THAsset_close(THAsset asset)
{
	fclose(asset);
}
size_t THAsset_seek(THAsset asset,size_t offset,int whence)
{
	return fseek(asset,offset,whence);
}
size_t THAsset_read(THAsset asset,void *data,size_t bytes)
{
	return fread(data,bytes,1,asset);
}
size_t THAsset_length(THAsset asset)
{
	long cur=ftell(asset);
	fseek(asset,0,SEEK_END);
	long len=ftell(asset);
	fseek(asset,cur,SEEK_SET);
	return len;
}


#endif
#endif
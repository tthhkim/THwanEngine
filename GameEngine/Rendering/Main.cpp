#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include <time.h>

#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/DisplayObject/THFrame.h>

#include <THPrivate.h>

#include <GameEngine/Rendering/THRenderer.h>


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

static float Touch_lastX,Touch_lastY;

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

		const float x=getGameX(AMotionEvent_getX(event,0));
		const float y=getGameY(AMotionEvent_getY(event,0));

		currentFrame->OnTouchEvent(event,action,x,y);

		switch(action)
		{
		case AMOTION_EVENT_ACTION_DOWN:
			currentFrame->OnTouchDown(x,y);
			Touch_Point_Down(x,y);
			Touch_lastX=x;
			Touch_lastY=y;
			break;
		case AMOTION_EVENT_ACTION_UP:
			currentFrame->OnTouchUp(x,y);
			Touch_Point_Up(x,y);
			break;
		case AMOTION_EVENT_ACTION_MOVE:
			currentFrame->OnTouchMove(x,y,x-Touch_lastX,y-Touch_lastY);
			Touch_lastX=x;
			Touch_lastY=y;
			break;
			/*
		case AMOTION_EVENT_ACTION_POINTER_DOWN:
		{
			const int32_t pointIndex=(AMotionEvent_getAction(event)&AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>>AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			const float px=getGameX(AMotionEvent_getX(event,pointIndex));
			const float py=getGameY(AMotionEvent_getY(event,pointIndex));

			touch_point_down(px,py);
		}
			break;
		case AMOTION_EVENT_ACTION_POINTER_UP:
		{
			const int32_t pointIndex=(AMotionEvent_getAction(event)&AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)>>AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
			const float px=getGameX(AMotionEvent_getX(event,pointIndex));
			const float py=getGameY(AMotionEvent_getY(event,pointIndex));

			touch_point_up(px,py);
		}
			break;
			*/
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
	lastNanosec=timesp.tv_sec*1000000000l + timesp.tv_nsec;
	while (1) {
        while (ALooper_pollAll(0, NULL, &events,
                (void**)&source) >= 0) {
            if (source) {
                source->process(state, source);
            }

            if(state->destroyRequested)
            {
                THLog("Destroy Requested");
				delete[] dataPath;
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
		const float px=getGameX((float)(GET_X_LPARAM(longWindowParameters)));
		const float py=getGameY((float)(GET_Y_LPARAM(longWindowParameters)));
		currentFrame->OnTouchEvent((THMotionEvent*)message,longWindowParameters,px,py);
		currentFrame->OnTouchMove(px,py,px-Touch_lastX,py-Touch_lastY);
		Touch_lastX=px;
		Touch_lastY=py;

		return 1;
	}
		break;
	case WM_LBUTTONDOWN:
	{
		if(currentFrame->canTouch==false){return 0;}
		isMouseDown=true;
		const float px=getGameX((float)(GET_X_LPARAM(longWindowParameters)));
		const float py=getGameY((float)(GET_Y_LPARAM(longWindowParameters)));
		currentFrame->OnTouchEvent((THMotionEvent*)message,longWindowParameters,px,py);
		currentFrame->OnTouchDown(px,py);
		Touch_lastX=px;
		Touch_lastY=py;
		Touch_Point_Down(px,py);

		return 1;
	}
		break;
	case WM_LBUTTONUP:
	{
		if(currentFrame->canTouch==false){return 0;}
		isMouseDown=false;
		const float px=getGameX((float)(GET_X_LPARAM(longWindowParameters)));
		const float py=getGameY((float)(GET_Y_LPARAM(longWindowParameters)));
		currentFrame->OnTouchEvent((THMotionEvent*)message,longWindowParameters,px,py);
		currentFrame->OnTouchUp(px,py);
		Touch_Point_Up(px,py);

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
	case WM_RBUTTONDOWN:
	{
		const float px=getGameX((float)(GET_X_LPARAM(longWindowParameters)));
		const float py=getGameY((float)(GET_Y_LPARAM(longWindowParameters)));
		currentFrame->OnRightTouchDown(px,py);
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

	THDefaultProgram.defaultProgram.Delete();
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

#if THPLATFORM==THPLATFORM_ANDROID
#include <libpng/png.h>
#include <libpng/pngstruct.h>
static void ReadDataFromAsset(png_structp png_ptr, png_bytep data, png_size_t bytesToRead)
{
	assert(png_ptr->io_ptr);

	AAsset_read((AAsset*)(png_ptr->io_ptr),data,bytesToRead);
}
unsigned char* LoadImageBuffer(const char *filename,size_t& width,size_t& height,int& colorType)
{
	AAsset* aasset=AAssetManager_open(assetManager, name, AASSET_MODE_STREAMING);
	{
	char pngSig[8];
	AAsset_read(aasset,pngSig,8);
#ifndef NDEBUG
	st=
#endif
	png_sig_cmp((png_const_bytep)pngSig,0, 8);
	}
	assert(st==0 && "PNG Sig Check Fail");
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	assert(png_ptr);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	assert(info_ptr);

	png_set_read_fn(png_ptr, aasset, ReadDataFromAsset);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	png_uint_32 widthi,heighti;
	int bitDepth;
	png_get_IHDR(png_ptr, info_ptr,
	   &widthi,
	   &heighti,
	   &bitDepth,
	   &colorType,
	   NULL, NULL, NULL);
	THLog("LibPNG // Width : %d , Height : %d",widthi,heighti);
#ifndef NDEBUG
		st=widthi&(widthi-1);
		assert(st==0);
		st=heighti&(heighti-1);
		assert(st==0);
#endif
		width=widthi;
		height=heighti;

	const png_uint_32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
	png_bytep colorBuf=(png_bytep)malloc(bytesPerRow*heighti);
	png_bytep* rowBytes=(png_bytep*)malloc(heighti*sizeof(png_bytep));
	
	unsigned int i=heighti;
	while(i)
	{
		--i;
		rowBytes[i]=colorBuf+(bytesPerRow*i);
		//png_read_row(png_ptr, (png_bytep)colorBuf+(i*bytesPerRow), NULL);
	}
	png_read_image(png_ptr,rowBytes);
	free(rowBytes);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	AAsset_close(aasset);

	return colorBuf;
}
void THImage::LoadFile(const char* name,GLfloat filter,bool isRepeat)
{
	size_t widthi,heighti;
	int colorType;
	void* colorBuf=LoadImageBuffer(name,widthi,heighti,colorType);

	SetSize(widthi,heighti);

	Load(colorBuf,(colorType&PNG_COLOR_MASK_ALPHA)?GL_RGBA:GL_RGB,filter,isRepeat);
	free(colorBuf);
}
#elif THPLATFORM==THPLATFORM_WINDOWS

#include <lodepng.h>
unsigned char* LoadImageBuffer(const char *filename,size_t& width,size_t& height,int& colorType)
{
	FILE* filep=fopen(filename,"rb");
	fseek(filep,0,SEEK_END);

	long size=ftell(filep);
	fseek(filep,0,SEEK_SET);
	unsigned char* mem=new unsigned char[size];
	fread(mem,size,1,filep);

	unsigned char* colorBuf;
	unsigned widthi=0,heighti=0;

	lodepng_decode_memory(&colorBuf, &width,&height,
                               mem, size,
                               LCT_RGBA, 8);
	colorType=LCT_RGBA;
	delete[] mem;
	THLog("LibPNG // Width : %d , Height : %d",widthi,heighti);

	return colorBuf;
}
void THImage::LoadFile(const char* name,GLfloat filter,bool isRepeat)
{
	size_t widthi,heighti;
	int colorType;
	void* colorBuf=LoadImageBuffer(name,widthi,heighti,colorType);

	SetSize(widthi,heighti);

	Load(colorBuf,GL_RGBA,filter,isRepeat);
	free(colorBuf);
}
#endif
#endif
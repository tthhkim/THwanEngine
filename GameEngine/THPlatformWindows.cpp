#include "THApplication.h"
#include <THPrivate.h>
#include <time.h>

#if THPLATFORM==THPLATFORM_WINDOWS

extern THApplication THApp;

#include <Windows.h>

static LARGE_INTEGER timesp;
static double THTimeFrequency;

static void PrepareTime()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	THTimeFrequency=1000000.0/(double)freq.QuadPart;
}
long long GetCurrentTimeMicro()
{
	QueryPerformanceCounter(&timesp);
	return (long long)((double)timesp.QuadPart*THTimeFrequency);
}

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
		if(isMouseDown==false){return 0;}
		const THVector2 p((float)(GET_X_LPARAM(longWindowParameters)),(float)(GET_Y_LPARAM(longWindowParameters)));
		THApp.OnTouchMove(p);

		return 1;
	}
		break;
	case WM_LBUTTONDOWN:
	{
		isMouseDown=true;
		const THVector2 p((float)(GET_X_LPARAM(longWindowParameters)),(float)(GET_Y_LPARAM(longWindowParameters)));
		THApp.OnTouchDown(p);

		return 1;
	}
		break;
	case WM_LBUTTONUP:
	{
		isMouseDown=false;
		const THVector2 p((float)(GET_X_LPARAM(longWindowParameters)),(float)(GET_Y_LPARAM(longWindowParameters)));
		THApp.OnTouchUp(p);

		return 1;
	}
		break;
	case WM_KEYUP:
		if(windowParameters==VK_BACK)
		{
			if(THApp.OnBackReleased()==0)
			{
				destroyRequested = true;

				// Post a quit message
				PostQuitMessage(0);

				return 1;
			}
		}else
		{
			THApp.OnKeyUp(windowParameters);
			return 1;
		}
		break;
	case WM_KEYDOWN:
		if(windowParameters!=VK_BACK)
		{
			THApp.OnKeyDown(windowParameters);
			return 1;
		}
		break;
		/*
	case WM_RBUTTONDOWN:
	{
		const THVector2 p(getGameX((float)(GET_X_LPARAM(longWindowParameters))),getGameY((float)(GET_Y_LPARAM(longWindowParameters))));
		currentFrame->OnRightTouchDown(p);
	}
		break;
		*/
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
	PrepareTime();

	AllocConsole();
	using namespace std;
	freopen("conin$","r",stdin);
	freopen("conout$","w",stdout);
	freopen("conout$","w",stderr);


	THLog("OnCreate()");
	THApp.OnCreate();


	// Windows variables
	HWND				nativeWindow = NULL;
	HDC					deviceContext = NULL;
	CreateWindowAndDisplay(applicationInstance,nativeWindow,deviceContext);
	
	THLog("EGLInit");
	THApp.EGLInit(nativeWindow);
	THLog("GLInit");
	
	THApp.GLInit();
	THApp.OnSurfaceCreated();

	THLog("GainFocus");
	THApp.OnResume();

	THApp.Start();

	while(destroyRequested==false)
	{
		// Check for messages from the windowing system. These will pass through the callback registered earlier.
		MSG eventMessage;
		while(PeekMessage(&eventMessage, nativeWindow, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&eventMessage);
			DispatchMessage(&eventMessage);
		}

		THApp.Loop();
	}
	THApp.SetRunning(false);

	THLog("LostFocus");
	THApp.OnPause();

	THLog("OnDestroy()");
	THApp.OnDestroy();

	//THDefaultProgram.Delete();
	THApp.TermDisplay();

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
void THApplication::EGLInitSurface(EGLNativeWindowType window)
{
	m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, window, NULL);
	if(m_eglSurface == EGL_NO_SURFACE)
	{
		eglGetError(); // Clear error
		m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, NULL, NULL);
	}
}

THAsset THAsset_open(const char *name,THAssetMode mode)
{
	char *paths=new char[strlen(DEFAULT_ASSET_PATH)+strlen(name)+2];
	paths[0]=0;
	strcat(paths,DEFAULT_ASSET_PATH);
	strcat(paths,name);
	THAsset f=fopen(paths,mode);
	delete paths;
	return f;
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
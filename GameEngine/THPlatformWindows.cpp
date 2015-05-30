#include "THApplication.h"
#include <THPrivate.h>
#include <time.h>

#if THPLATFORM==THPLATFORM_WINDOWS

extern THApplication THApp;

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


static void key_callback(GLFWwindow *window,int key,int scancode,int action,int modes)
{
	if(action==GLFW_PRESS)
	{
		THApp.OnKeyDown(key);
	}else if(action==GLFW_RELEASE)
	{
		if(key==GLFW_KEY_BACKSPACE)
		{
			if(THApp.OnBackReleased()==0)
			{
				glfwSetWindowShouldClose(window,1);
			}
		}
		
		THApp.OnKeyUp(key);
	}
}
static bool isMouseDown;
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(isMouseDown==false){return;}
	const THVector2 p((float)xpos,(float)ypos);
	THApp.OnTouchMove(p);
}
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos,ypos;
	glfwGetCursorPos(window,&xpos,&ypos);
	const THVector2 p((float)xpos,(float)ypos);
	if(action==GLFW_PRESS)
	{
		isMouseDown=true;
		THApp.OnTouchDown(p);
	}else if(action==GLFW_RELEASE)
	{
		isMouseDown=false;
		THApp.OnTouchUp(p);
	}
	
}
int main()
{
	Randomize();
	PrepareTime();

	//AllocConsole();
	//using namespace std;
	//freopen("conin$","r",stdin);
	//freopen("conout$","w",stdout);
	//freopen("conout$","w",stderr);


	THLog("OnCreate()");
	THApp.OnCreate();

	if(!glfwInit())
	{
		THError("glfwInit Failed");
		return 0;
	}
	GLFWwindow *window=glfwCreateWindow(WINDOW_WIDTH,WINDOW_HEIGHT,APPLICATION_NAME,0,0);
	if(window==0)
	{
		THError("glfwCreateWindow Failed");
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);

	GLenum err=glewInit();
	if(err!=GLEW_OK)
	{
		THError("GLEW Init Failed");
		return 0;
	}
	THLog("GLEW Initialed");

	THApp.data=window;
	glfwSetKeyCallback(window,key_callback);
	glfwSetMouseButtonCallback(window,mouse_button_callback);
	glfwSetCursorPosCallback(window,cursor_position_callback);

	int width,height;
	glfwGetFramebufferSize(window,&width,&height);
	THApp.SetWindowSize(width,height);
	THLog("SurfaceInitialed");

	THApp.GLInit();
	THApp.OnSurfaceCreated();
	THLog("GLInit");


	THLog("GainFocus");
	THApp.OnResume();

	THApp.Start();

	while(glfwWindowShouldClose(window)==false)
	{
		glfwPollEvents();
		THApp.Loop();
		//keep running
	}
	THApp.SetRunning(false);

	THLog("LostFocus");
	THApp.OnPause();

	THLog("OnDestroy()");
	THApp.OnDestroy();

	//THDefaultProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
void THApplication::SwapBuffer()
{
	glfwSwapBuffers((GLFWwindow*)data);
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
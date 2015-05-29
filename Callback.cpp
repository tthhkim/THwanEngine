#include <string.h>

#include <THPrivate.h>

#include <GameEngine/Util/THMath3D.h>
#include <GameEngine/Util/THTween.h>

#include <GameEngine/Rendering/THDrawing.h>

#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <GameEngine/DisplayObject/THChar.h>
#include <GameEngine/DisplayObject/THFrame.h>

#if THPLATFORM==THPLATFORM_ANDROID
#include <android/asset_manager.h>
#include <unistd.h>
#endif

#include <GameEngine/THApplication.h>

void THApplication::OnCreate()
{
	THLog("%f",THRand01());
	//SetEGL(3,0);
	//SetEGL(int glesversion,int depthsize);
}
void THApplication::OnSurfaceCreated()
{
	//SetOrtho( THVector2(0.0f,0.0f) , THVector2(72.0f,128.0f) );
	//TextureLoading
}
void THApplication::OnResume()
{
}
void THApplication::OnPause()
{
}
void THApplication::OnDestroy()
{
	//Memory Free
}
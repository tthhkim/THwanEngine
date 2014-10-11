#include <THPrivate.h>
#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/DisplayObject/THDisplayObject.h>

#include <malloc.h>

#include <GameEngine/DisplayObject/THFrame.h>

#if THPLATFORM==THPLATFORM_ANDROID
#include <android/asset_manager.h>
#include <unistd.h>
#endif


void OnCreate(THApplicaation* state)
{
	//GoFrame
}
void OnSurfaceCreated()
{
	//SetOrtho( THVector2() , THVector2() );
	//TextureLoading?
}

void OnResume()
{
}

void OnPause()
{
}

void OnDestroy()
{		
	//Memory Free
}


/*
void OnSaveInstanceState(saved_state* saved)
{
}
void OnLoadInstanceState(saved_state* loaded)
{
}
*/
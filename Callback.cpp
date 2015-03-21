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

#include <THFluid/GameFrame.h>

GameFrame f;

void OnCreate(THApplicaation* state)
{
	GoFrame(&f,0);
}
void OnSurfaceCreated()
{
	SetOrtho( THVector2(0.0f,0.0f) , THVector2(7.20f,12.80f) );

	f.Load();
	//TextureLoading
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
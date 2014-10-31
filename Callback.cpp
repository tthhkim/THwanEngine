#include <string.h>

#include <THPrivate.h>

#include <GameEngine/Util/THMath3D.h>

#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/Rendering/THEffects.h>
#include <GameEngine/Rendering/THShadowEffect.h>

#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <GameEngine/DisplayObject/THChar.h>
#include <GameEngine/DisplayObject/THFrame.h>

#if THPLATFORM==THPLATFORM_ANDROID
#include <android/asset_manager.h>
#include <unistd.h>
#endif

const GLfloat ver[]=MAKE_CENTER_VERTEX(0.5f,0.5f);

THFrame f;
THTexture tex;
THMovieClip mc(ver,&tex);

void OnCreate(THApplicaation* state)
{
	GoFrame(&f);
	f.AddChild(&mc);
	mc.width=30.0f;
	mc.height=30.0f;
	mc.position.Set(36.0f,64.0f);
}
void OnSurfaceCreated()
{
	SetOrtho( THVector2(0.0f,0.0f) , THVector2(72.0f,128.0f) );
	THImage img=LoadTexture("D:/Program/Workspace/THwanEngine/THwanEngine/qq.png");
	tex.Set(&img);
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

/*
const GLfloat myver[]=MAKE_CENTER_VERTEX(360.0f,640.0f);
class MyFrame :public THFrame
{
	void Draw()
	{
		glEnableVertexAttribArray(THDefaultProgram.vertexHandler);
		glVertexAttribPointer(THDefaultProgram.vertexHandler,2,GL_FLOAT,GL_FALSE,0,myver);

		glVertexAttrib2f(THDefaultProgram.rotationHandler,1.0f,0.0f);
		glVertexAttrib2f(THDefaultProgram.positionHandler,0.0f,0.0f);

		glVertexAttrib1f(THDefaultProgram.hasColorHandler,1.0f);
		glVertexAttrib4f(THDefaultProgram.colorHandler,1.0f,0.8f,0.3f,1.0f);
		glVertexAttrib4f(THDefaultProgram.colorMultiplyHandler,1.0f,1.0f,1.0f,1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
};

MyFrame f;

void OnCreate(THApplicaation* state)
{
	GoFrame(&f);
}
void OnSurfaceCreated()
{
	SetOrtho( THVector2() , THVector2(720.0f,1280.0f) );
}
*/
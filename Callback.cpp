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

void OnCreate(THApplicaation* state)
{
	//GoFrame();
}
void OnSurfaceCreated()
{
	//SetOrtho( THVector2(0.0f,0.0f) , THVector2(72.0f,128.0f) );
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

/*
THFrame f;
THTexture tex;
THTexture t2;
THMovieClip mc(&tex);
THButton btn(1.0f,1.0f);
THDisplayObject par;

void OnCreate(THApplicaation* state)
{
	GoFrame(&f);
	f.AddChild(&par);
	f.AddChild(&mc);
	f.AddButton(&btn);

	mc.parent=&par;
	mc.width=50.0f;
	mc.rotation.Set(0.2f);
	mc.height=30.0f;
	mc.position.Set(36.0f,64.0f);

	par.position.Set(10.0f,0.0f);
	par.CalcWorldPosition();
	btn.clip=&mc;
	mc.CalcWorldPosition();
	btn.Synchronize(THVector_zero);
	btn.downed=&t2;
}
void OnSurfaceCreated()
{
	SetOrtho( THVector2(0.0f,0.0f) , THVector2(72.0f,128.0f) );
	THImage img=LoadTexture("D:/Program/Workspace/THwanEngine/THwanEngine/qq.png");
	tex.Set(&img);
	t2.Set(&img);
	t2.UpsideDown();
	//TextureLoading
}
*/
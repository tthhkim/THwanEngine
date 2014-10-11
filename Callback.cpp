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
	//GoFrame();
}
void OnSurfaceCreated()
{
	//SetOrtho( THVector2() , THVector2() );
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
const MAKE_CENTER_VERTEX(myver,360.0f,640.0f);
class MyFrame :public THFrame
{
	void Draw(float dt)
	{
		glEnableVertexAttribArray(vertexHandler);
		glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,myver);

		glVertexAttrib4f(rotationHandler,1.0f,0.0f,0.0f,1.0f);
		glVertexAttrib2f(positionHandler,0.0f,0.0f);

		glVertexAttrib1f(hasColorHandler,1.0f);
		glVertexAttrib4f(colorHandler,1.0f,0.8f,0.3f,1.0f);
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
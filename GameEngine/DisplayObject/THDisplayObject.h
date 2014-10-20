#ifndef THKim_THDisplayObject
#define THKim_THDisplayObject

#include <assert.h>
#include <GLES2/gl2.h>

#include <GameEngine/Util/THMath.h>
#include <GameEngine/Util/THArray.h>


class THTexture;

//THDisplayObject-----------------------------------------

class THDisplayObject
{
public:
	bool visible;
	void* frame;
	void* userData;

	virtual void Draw()=0;

	THDisplayObject()
	{
		visible=true;
		frame=0;
		userData=0;
	}
};

class THMovieClip : public THDisplayObject
{
public:
	THTexture* texture;
	THVector2 position;
	THRot rotation;
	const GLfloat* vertexBuffer;
	

	THMovieClip(const GLfloat* vertex,THTexture* _texture):THDisplayObject()
	{
		texture=_texture;
		vertexBuffer=vertex;
	}

	void Draw();
};


class THButton
{
public:
	THMovieClip* clip;
	void* frame;
	bool enable;

	THTexture* downed;
	

	void (*onDown)(float,float,THButton*);
	void (*onRelease)(float,float,THButton*);

	THButton(float w,float h):minBound(),maxBound(w,h)
	{
		frame=0;
		enable=true;

		downed=0;
		normal=0;
		onDown=0;
		onRelease=0;
	}

	void SetPosition(float _x,float _y);
	void SetSize(float w,float h);

	void Synchronize(const THVector2& extraBound);

	inline bool HitTest(float px,float py) const
	{
		return (minBound.x<px)&&(maxBound.x>px) && (minBound.y<py)&&(maxBound.y>py);
	}
	void SetDowned()
	{
		if(clip && downed)
		{
			normal=clip->texture;
			clip->texture=downed;
		}
	}
	void SetUpped()
	{
		if(clip && normal)
		{
			clip->texture=normal;
			normal=0;
		}
	}
	void SwapTexture()
	{
		if(clip && downed)
		{
			normal=clip->texture;
			clip->texture=downed;
			downed=normal;
			normal=0;
		}
	}

#ifndef NDEBUG
	void DrawDebug(float r,float g,float b) const
	{
		using namespace THDefaultProgram;
		const GLfloat vertic[]=
		{
			minBound.x,minBound.y,
			maxBound.x,minBound.y,
			maxBound.x,maxBound.y,
			minBound.x,maxBound.y
		};

		glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,vertic);
		glVertexAttrib4f(rotationHandler,1.0f,0.0f,0.0f,1.0f);
		glVertexAttrib2f(positionHandler,0.0f,0.0f);

		glVertexAttrib1f(hasColorHandler,1.0f);
		glVertexAttrib4f(colorHandler,r,g,b,1.0f);
		glVertexAttrib4f(colorMultiplyHandler,1.0f,1.0f,1.0f,1.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
	}
#endif

protected:
	THVector2 minBound,maxBound;
	THTexture* normal;
};

#endif

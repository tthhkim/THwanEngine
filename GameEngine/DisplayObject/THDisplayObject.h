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

	virtual void draw()=0;

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

	void draw();
};

class THGroupClip : public THDisplayObject
{
public:
	THArray<THDisplayObject*> list;
	THVector2 position;

	THGroupClip(const unsigned int listCount=5):THDisplayObject(),list(listCount)
	{
	}

	void draw();
	void AddChild(THDisplayObject* object);
	void ReAddChild(THDisplayObject* object);
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

	THButton(float w,float h)
	{
		x=0.0f;
		y=0.0f;
		xplusw=w;
		yplush=h;

		frame=0;
		enable=true;

		downed=0;
		normal=0;
		onDown=0;
		onRelease=0;
	}

	void SetPosition(float _x,float _y);
	void SetSize(float w,float h);

	inline bool HitTest(float px,float py) const
	{
		return (x<px)&&(xplusw>px) && (y<py)&&(yplush>py);
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

protected:
	float x;
	float y;
	float xplusw;
	float yplush;
	THTexture* normal;
};

#endif

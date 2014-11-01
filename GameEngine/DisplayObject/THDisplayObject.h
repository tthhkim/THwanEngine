#ifndef THKim_THDisplayObject
#define THKim_THDisplayObject

#include <assert.h>
#include <GLES2/gl2.h>

#include <GameEngine/Util/THMath.h>
#include <GameEngine/Util/THArray.h>


class THTexture;

//THDisplayObject-----------------------------------------

class THDisplayObject;
static void DrawNothing(const THDisplayObject* obj){}

typedef void (*THDrawingFunction)(const THDisplayObject*);

class THDisplayObject
{
public:
	bool visible;
	void* userData;
	THDisplayObject* parent;
	THVector2 position;

	void (*Draw)(const THDisplayObject*);

	THDisplayObject(THDrawingFunction drawfunction=DrawNothing):position(0.0f,0.0f),worldPosition(0.0f,0.0f)
	{
		visible=true;
		userData=0;
		parent=0;

		Draw=drawfunction;
	}

	void CalcWorldPositionParent();
	inline void CalcWorldPosition()
	{
		if(parent){worldPosition=position+parent->worldPosition;}
		else{worldPosition=position;}
	}

	inline const THVector2& GetWorldPosition() const
	{
		return worldPosition;
	}
	inline void DrawObject()
	{
		CalcWorldPosition();

		assert(Draw);
		Draw(this);
	}

protected:
	THVector2 worldPosition;
};

class THMovieClip : public THDisplayObject
{
public:
	THTexture* texture;
	THRot2 rotation;
	THVector2 size;
	const GLfloat *vertexBuffer;
	

	THMovieClip(const THVector2& _size,THTexture* _texture):THDisplayObject((THDrawingFunction)DrawTHMovieClip)
	{
		texture=_texture;
		size=_size;
		vertexBuffer=0;
	}

	friend void DrawTHMovieClip(const THMovieClip* obj);
};


class THButton
{
public:
	THMovieClip* clip;
	bool enable;

	THTexture* downed;
	

	void (*onDown)(float,float,THButton*);
	void (*onRelease)(float,float,THButton*);

	THButton(float w,float h):minBound(0.0f,0.0f),maxBound(w,h)
	{
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

protected:
	THVector2 minBound,maxBound;
	THTexture* normal;
};

#endif

#ifndef THKim_THDisplayObject
#define THKim_THDisplayObject

#include <assert.h>
#include <GLES2/gl2.h>

#include <GameEngine/Util/THMath.h>
#include <GameEngine/Util/THArray.h>


class THTexture;
class THGroupClip;

//THDisplayObject-----------------------------------------

class THDisplayObject
{
public:
	bool visible;
	void* userData;
	THDisplayObject* parent;
	THVector2 position;
	
	virtual void Draw(){}

	THDisplayObject():position(0.0f,0.0f),worldPosition(0.0f,0.0f)
	{
		visible=true;
		userData=0;
		parent=0;
	}

	inline void CalcWorldPosition()
	{
		if(parent){worldPosition=position+parent->worldPosition;}
		else{worldPosition=position;}
	}

	inline THVector2& GetWorldPosition()
	{
		return worldPosition;
	}
	inline void DrawObject()
	{
		CalcWorldPosition();
		Draw();
	}

protected:
	THVector2 worldPosition;
};

class THGroupClip : public THDisplayObject
{
public:
	THArray<THDisplayObject*> objectList;

	THGroupClip(unsigned int capacity=5):THDisplayObject(),objectList(capacity)
	{
	}
	void Add(THDisplayObject* obj)
	{
		objectList.Push(obj);
		obj->parent=this;
	}
	void Remove(THDisplayObject* obj)
	{
		assert(obj->parent==this);
		objectList.Delete(obj);
		obj->parent=0;
	}
	void Draw();
};



class THMovieClip : public THDisplayObject
{
public:
	THTexture* texture;
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
	bool enable;

	THTexture* downed;
	

	void (*onDown)(float,float,THButton*);
	void (*onRelease)(float,float,THButton*);

	THButton(float w,float h):minBound(),maxBound(w,h)
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

#ifndef NDEBUG
	void DrawDebug(float r,float g,float b) const
	{
		const GLfloat vertic[]=
		{
			minBound.x,minBound.y,
			maxBound.x,minBound.y,
			maxBound.x,maxBound.y,
			minBound.x,maxBound.y
		};

		glVertexAttribPointer(THDefaultProgram.vertexHandler,2,GL_FLOAT,GL_FALSE,0,vertic);
		glVertexAttrib4f(THDefaultProgram.rotationHandler,1.0f,0.0f,0.0f,1.0f);
		glVertexAttrib2f(THDefaultProgram.positionHandler,0.0f,0.0f);

		glVertexAttrib1f(THDefaultProgram.hasColorHandler,1.0f);
		glVertexAttrib4f(THDefaultProgram.colorHandler,r,g,b,1.0f);
		glVertexAttrib4f(THDefaultProgram.colorMultiplyHandler,1.0f,1.0f,1.0f,1.0f);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
	}
#endif

protected:
	THVector2 minBound,maxBound;
	THTexture* normal;
};

#endif

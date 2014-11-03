#ifndef THKim_THChar
#define THKim_THChar

#include <assert.h>
#include <GLES2/gl2.h>

#include <GameEngine/Util/THMath.h>
#include <GameEngine/Util/THArray.h>
#include <GameEngine/Rendering/THDrawing.h>

#include <GameEngine/DisplayObject/THDisplayObject.h>

class THChar;

class THString : public THDisplayObject
{
public:
	float scale;

	THString(unsigned int cap=10):THDisplayObject((THDrawingFunction)DrawTHString),chars(cap)
	{
		scale=1.0f;
		positionOffset=0.0f;
	}
	
	/*
		Last char must be 0
	*/
	void SetString(const THChar** charArr);
	void SetWidth(float w);
	friend void DrawTHString(const THString* obj);

	inline float Length() const
	{
		return length*scale;
	}
	inline void SetPosition(const THVector2& p,float _positionOffset)
	{
		position.Set(p.x-length*scale*_positionOffset,p.y);
		positionOffset=_positionOffset;
	}
	inline THVector2 GetPosition() const
	{
		return THVector2(position.x + length*scale*positionOffset,position.y);
	}
	inline float GetPositionOffset() const
	{
		return positionOffset;
	}
	static void DrawTHString(const THString* obj);

protected:
	THArray<const THChar*> chars;
	float length;
	float positionOffset;
};

class THChar : public THTexture
{
	friend class THString;
public:
	THVector2 vertexBuffer[4];

	void Set(THImage* _image,const THVector2& pos,const THVector2& size);
	inline float Length() const
	{
		return length;
	}

protected:
	float length;
};



#endif

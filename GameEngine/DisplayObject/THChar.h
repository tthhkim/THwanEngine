#ifndef THKim_THChar
#define THKim_THChar

#include <assert.h>
#include <GLES2/gl2.h>

#include <GameEngine/Util/THMath.h>
#include <GameEngine/Util/THArray.h>
#include <GameEngine/Rendering/THDrawing.h>

#include <GameEngine/DisplayObject/THDisplayObject.h>

class THChar : public THTexture
{
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

class THString : public THDisplayObject
{
	
public:
	THVector2 position;
	float scale;
	float red,green,blue,alpha;

	THString(unsigned int cap=10):THDisplayObject(),chars(cap)
	{
		scale=1.0f;
		red=green=blue=alpha=1.0f;
	}
	
	/*
		Last char must be 0
	*/
	void SetString(const THChar** charArr);
	void Draw();
	void SetWidth(float w);

	inline void Delete()
	{
	}
	inline float Length() const
	{
		return length*scale;
	}

protected:
	THArray<const THChar*> chars;
	float length;
};

#endif

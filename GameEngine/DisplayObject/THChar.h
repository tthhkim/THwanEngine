#ifndef THKim_THChar
#define THKim_THChar

#include <assert.h>
#include <GLES2/gl2.h>

#include <GameEngine/Util/THMath.h>
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

	THString(float _scale):THDisplayObject()
	{
		scale=_scale;
		red=green=blue=alpha=1.0f;
	}
	
	
	void SetString(const THChar** charArr,unsigned int cCount);
	void Draw();

	inline void Delete()
	{
		delete[] chars;
	}
	inline float Length() const
	{
		return length;
	}

protected:
	THChar **chars;
	unsigned int count;
	float length;
};

#endif

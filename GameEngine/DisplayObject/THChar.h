#ifndef THKim_THChar
#define THKim_THChar

#include <assert.h>

#include <GameEngine/Util/THMath.h>
#include <GameEngine/Util/THArray.h>
#include <GameEngine/Rendering/THDrawing.h>

#include <GameEngine/DisplayObject/THDisplayObject.h>

class THChar;

class THString : public THDisplayObject
{
public:
	

	THString(unsigned int cap=10)
	{
		m_scale=1.0f;
		LoadChildren(cap);
	}
	~THString()
	{
		Refresh();
	}
	
	void Refresh();
	/*
		Last char must be 0
	*/
	void SetString(const THChar** charArr);
	void SetScale(float s);

	inline float Length() const
	{
		return m_length;
	}
	inline float GetScale() const
	{
		return m_scale;
	}


protected:
	float m_length,m_scale;
};

class THChar : public THTexture
{
	friend class THString;
public:
	inline float Length() const
	{
		return size.x;
	}
};



#endif

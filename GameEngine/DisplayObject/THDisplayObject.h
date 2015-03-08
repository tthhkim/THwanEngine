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
	void* userData;
	
	//individual attributes relative to parent
	THVector2 position;
	THRot2 rotation;
	THVector2 center;
	THVector2 size;

	const THTexture* texture;

	THDisplayObject():position(0.0f,0.0f),m_worldposition(0.0f,0.0f)
	{
		visible=true;
		userData=0;
		m_parent=0;

		texture=0;
	}
	void LoadChildren(unsigned int cap);
	void AddChild(THDisplayObject *obj);
	void CalculateWorldAttrib();


	void Draw();

protected:
	//world attributes
	THDisplayObject* m_parent;
	THVector2 m_worldposition;
	THRot2 m_worldrotation;

	THArray<THDisplayObject*> m_children;
};


class THFrame;

class THButton
{
	friend class THFrame;
public:
	bool enable;

	void (*onDown)(const THVector2&,THButton*);
	void (*onRelease)(const THVector2&,THButton*);

	THButton(const THVector2& _size):m_minbound(0.0f,0.0f),m_maxbound(_size)
	{
		enable=true;

		onDown=0;
		onRelease=0;
	}

	void SetPosition(const THVector2& p);
	void SetSize(const THVector2& s);
	inline THVector2 GetSize() const
	{
		return m_maxbound-m_minbound;
	}

	void Synchronize(const THVector2& extraBound);

	inline bool HitTest(const THVector2& p) const
	{
		return m_minbound.x<p.x&&m_minbound.y<p.y&&m_maxbound.x>p.x&&m_maxbound.y>p.y;
	}
	void SetSwap(void **src,void *dest)
	{
		m_src=src;
		m_swap=dest;
	}
	void Swap()
	{
		if(m_src)
		{
			void *temp=*m_src;
			*m_src=m_swap;
			m_swap=temp;
		}
	}
	

protected:
	THVector2 m_minbound,m_maxbound;

	void **m_src;
	void *m_swap;
};

#endif

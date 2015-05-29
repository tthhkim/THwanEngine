#ifndef TH_GLINSTANT_THFRAME
#define TH_GLINSTANT_THFRAME

#include <THPrivate.h>
#include <GameEngine/Util/THArray.h>
#include <GameEngine/DisplayObject/THDisplayObject.h>

#if THPLATFORM==THPLATFORM_ANDROID
#include <android/input.h>
typedef AInputEvent THMotionEvent;
typedef int32_t THActionType;
#elif THPLATFORM==THPLATFORM_WINDOWS
#include <Windows.h>
typedef UINT THMotionEvent;
typedef LPARAM THActionType;
#endif

class THDisplayObject;
class THButton;

class THFrame
{
public:
	THArray<THDisplayObject*> objectList;
	THArray<THButton*> buttonList;

	bool canTouch;

	THFrame(const unsigned int objectCount=2,const unsigned int buttonCount=2):objectList(objectCount),buttonList(buttonCount)
	{
		canTouch=true;
		m_buttondown=0;
	}

	virtual void OnTouchEvent(THMotionEvent* event,THActionType action,const THVector2& p){}
	virtual void OnTouchDown(const THVector2& p){}
	virtual void OnTouchUp(const THVector2& p){}
	virtual void OnTouchMove(const THVector2& p,const THVector2& delta){}
	
	//for windows
	virtual void OnKeyUp(unsigned int key){}
	virtual void OnKeyDown(unsigned int key){}

	virtual int OnBackReleased(){return 0;}
	virtual void OnEnterFrame(){}
	virtual void Action(THFrame* callFrame,void* data){}
	virtual void OnChangeFrame(THFrame* toFrame){}

	

	virtual void Draw();

	/*
	void DrawObjects(unsigned int start,unsigned int count) const;
	inline void DrawObjects() const
	{
		DrawObjects(0,objectList.num);
	}
	void PreDraw();
	void PostDraw();
	void DrawImage(const THVector2& p,const THVector2& size,const THImage& img);
	void DrawTexture(const THVector2& p,const THVector2& center,const THVector2& size,const THRot2& rot,const THTexture *tex);
	*/
#ifndef NDEBUG
#endif

	void AddChild(THDisplayObject* object)
	{
		objectList.Push(object);
	}

	// offset position from last pushed displayobject
	inline void AddChild(THDisplayObject *object,const THVector2& posOffsetLast)
	{
		object->position=objectList.GetLast()->position+posOffsetLast;
		AddChild(object);
	}

	void ReAddChild(THDisplayObject* object)
	{
		objectList.Repush(object);
	}
	inline void AddButton(THButton* button)
	{
		buttonList.Push(button);
	}
	inline void ReAddButton(THButton* button)
	{
		buttonList.Repush(button);
	}
	inline void RemoveChild(THDisplayObject* object)
	{
		int ind=objectList.Find(object);
		if(ind!=-1)
		{
			objectList.Delete(ind);
		}
	}
	inline void RemoveButton(THButton* button)
	{
		int ind=buttonList.Find(button);
		if(ind!=-1)
		{
			buttonList.Delete(ind);
		}
	}

	void FrameDown(const THVector2& p);
	void FrameMove(const THVector2& p);
	void FrameUp(const THVector2& p);

protected:
	THButton *m_buttondown;
	THVector2 m_lasttouched;
};

#endif
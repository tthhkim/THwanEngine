#ifndef TH_GLINSTANT_THFRAME
#define TH_GLINSTANT_THFRAME

#include <THPrivate.h>
#include <GameEngine/Util/THArray.h>
#include <GameEngine/DisplayObject/THDisplayObject.h>

#if THPLATFORM==THPLATFORM_ANDROID
typedef AMotionEvent THMotionEvent;
typedef int32_t THActionType
#include <android/input.h>
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
	}

	virtual void OnTouchEvent(THMotionEvent* event,THActionType action,float x,float y){}
	virtual void OnTouchDown(float x,float y){}
	virtual void OnTouchUp(float x,float y){}
	virtual void OnTouchMove(float x,float y,float deltaX,float deltaY){}

	virtual int OnBackReleased(){return 0;}
	virtual void OnEnterFrame(){}
	virtual void Action(THFrame* callFrame,void* data){}
	virtual void OnChangeFrame(THFrame* toFrame){}

	

	virtual void Draw();


	void DrawObjects() const;
	inline void AddChild(THDisplayObject* object)
	{
		objectList.Push(object);
	}
	inline void ReAddChild(THDisplayObject* object)
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
		objectList.Delete(object);
	}
	inline void RemoveButton(THButton* button)
	{
		buttonList.Delete(button);
	}

	
};

#endif
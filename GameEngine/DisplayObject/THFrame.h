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
	void AddChild(THDisplayObject* object)
	{
		assert(object->frame==0);
		object->frame=this;
		objectList.Push(object);
	}
	void ReAddChild(THDisplayObject* object)
	{
		assert(object->frame==this);
		objectList.Repush(object);
	}
	void AddButton(THButton* button)
	{
		assert(button->frame==0);
		button->frame=this;
		buttonList.Push(button);
	}
	void ReAddButton(THButton* button)
	{
		assert(button->frame==this);
		buttonList.Repush(button);
	}
	void RemoveChild(THDisplayObject* object)
	{
		objectList.Delete(object);
	}
	void RemoveButton(THButton* button)
	{
		buttonList.Delete(button);
	}

	
};

#endif
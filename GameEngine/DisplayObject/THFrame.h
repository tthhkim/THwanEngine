#ifndef TH_GLINSTANT_THFRAME
#define TH_GLINSTANT_THFRAME

#include <THPrivate.h>
#include <GameEngine/Util/THArray.h>

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

	THFrame(const unsigned int objectCount,const unsigned int buttonCount):objectList(objectCount),buttonList(buttonCount)
	{
		canTouch=true;
	}
	void DrawObjects() const;
	virtual void Draw(float dt);

	void AddChild(THDisplayObject* object);
	void ReAddChild(THDisplayObject* object);
	void AddButton(THButton* button);
	void ReAddButton(THButton* button);
	void RemoveChild(THDisplayObject* object);
	void RemoveButton(THButton* button);

	virtual void onTouchEvent(THMotionEvent* event,THActionType action,float x,float y){}
	virtual int onBackReleased(){return 0;}
	virtual void onEnterFrame(float dt){}
	virtual void Action(THFrame* callFrame,void* data){}
	virtual void onChangeFrame(THFrame* toFrame){}
};

#endif
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


	void DrawObjects(unsigned int start,unsigned int end) const;
	inline void DrawObjects() const
	{
		DrawObjects(0,objectList.num);
	}
#ifndef NDEBUG
	void DrawButtonDebug(const THColor& color) const;
#endif

	void AddChild(THDisplayObject* object)
	{
#ifndef NDEBUG
		if(ParentCheck(object))
		{
			THError("Parent Not Included first");
			assert(0);
		}
#endif
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
#ifndef NDEBUG
		if(ParentCheck(object))
		{
			THError("Parent Not Included first");
			assert(0);
		}
#endif
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

private:
	#ifndef NDEBUG
	bool ParentCheck(THDisplayObject *object) const
	{
		if(object->parent)
		{
			THDisplayObject *par=object->parent;
			while(par)
			{
				if(objectList.Find(par)==-1)
				{
					return true;
				}
				par=par->parent;
			}
		}
		return false;
	}
#endif
};

#endif
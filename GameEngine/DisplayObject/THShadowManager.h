#ifndef THSHADOWMANAGER
#define THSHADOWMANAGER

#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <GameEngine/Util/THMath3D.h>
#include <GameEngine/Util/THArray.h>

class THShadowManager : public THDisplayObject
{
public:
	THVector3 lightNormal;
	THProgramObject object;

	THShadowManager():THDisplayObject()
	{
	}
	void SetLight(const THVector3& normal)
	{
		lightNormal=normal;
	}
	void Load();
	void draw();
};

#endif
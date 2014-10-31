#ifndef THKim_THShadowEffects
#define THKim_THShadowEffects

#include <GLES2/gl2.h>
#include <GameEngine/Rendering/THDrawing.h>
#include <THPrivate.h>
#include <GameEngine/Util/THMath.h>
#include <GameEngine/Util/THMath3D.h>
#include <GameEngine/Util/THArray.h>

class THMovieClip;

typedef struct
{
	THMovieClip* mc;
	float z;
} THShadowObject;

class THShadowEffect
{
public:
	THProgram program;
	THArray<THShadowObject> objects;

	THShadowEffect(unsigned int objectCapacity):objects(objectCapacity)
	{
	}
	void SetOrtho(const THVector2& minp=gameMinBound,const THVector2& maxp=gameMaxBound);
	void Load(const THVector2& minp=gameMinBound,const THVector2& maxp=gameMaxBound);
	void Draw();

	inline void SetLightPosition(float x,float y,float z) const
	{
		glUniform3f(lightPositionHandler,x,y,z);
	}
	inline void SetShadowColor(float r,float g,float b,float a) const
	{
		program.SetUniform("shadowColor",r,g,b,a);
	}

	void Push(THMovieClip* mc,float z)
	{
		objects.Push(THShadowObject{mc,z});
	}
	void SetZ(THMovieClip* mc,float z)
	{
		for(unsigned int i=0;i<objects.num;++i)
		{
			if(objects.arr[i].mc==mc)
			{
				objects.arr[i].z=z;
				return;
			}
		}
		THError("There is no shadowObject you chose");
	}
	void Delete(THMovieClip* mc)
	{
		for(unsigned int i=0;i<objects.num;++i)
		{
			if(objects.arr[i].mc==mc)
			{
				objects.Delete(i);
				return;
			}
		}
	}

protected:
	GLuint lightPositionHandler;

	GLuint vertexHandler,textureHandler;
	GLuint positionHandler;
	GLuint rotationHandler;
};


#endif

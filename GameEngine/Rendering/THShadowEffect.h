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
	void Load();
	void Draw() const;

	void SyncProjection();
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
		THShadowObject obj={mc,z};
		objects.Push(obj);
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
	GLuint scaleHandler;
};

class THShadowBlurEffect
{
public:
	THProgram program;
	THTexture *srcTexture;
	const GLfloat *vertex;
	THVector2 direction;
	

	THShadowBlurEffect():direction(1.0f,0.0f)
	{
		vertex=THFullVertices;
	}
	void Load(THTexture *src);
	void Draw() const;

	void SetTexture(THTexture *src)
	{
		srcTexture=src;

		const THVector2 invTex=1.0f/src->image->size;
		program.SetUniform("textureInvert",invTex.x,invTex.y);
	}

	void SetRadius(float r)
	{
		glUniform1f(radiusHandler,1.0f/r);
		glUniform1i(stepHandler,(int)r);
	}

protected:
	GLuint vertexHandler,textureHandler;
	GLuint radiusHandler,stepHandler,directionHandler;
};


#endif

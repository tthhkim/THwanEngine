#ifndef THKim_THEffects
#define THKim_THEffects

#include <GLES2/gl2.h>
#include <GameEngine/Rendering/THDrawing.h>
#include <THPrivate.h>

class THEffect
{
public:
	THProgramObject programObject;
	const GLfloat *vertex;

	THEffect(){vertex=defaultFullVertices;}
};

class THLightBaseCircle : public THEffect
{
public:
	THLightBaseCircle():THEffect()
	{
	}
	void Load();

	void SetScreenSize(float w,float h)
	{
		programObject.UseProgram();
		glUniform2f(programObject.GetUniformLocation("screenScale"),w,h);
	}
	void SetPosition(float x,float y)
	{
		programObject.UseProgram();
		glUniform2f(positionHandler,x,y);
	}

	void SetRadius(float lightRadius,float circleRadius)
	{
		programObject.UseProgram();
		glUniform1f(circleRadiusHandler,circleRadius);
		glUniform1f(radiusHandler,lightRadius);
		if(lightRadius!=circleRadius)
		{glUniform1f(lcGapiHandler,1.0f/(lightRadius-circleRadius));}
		glUniform1f(radiusiHandler,1.0f/lightRadius);
	}


	void Draw();

protected:

	GLuint positionHandler;
	GLuint radiusHandler;
	GLuint radiusiHandler;

	GLuint circleRadiusHandler;
	GLuint lcGapiHandler;

	GLuint vertexHandler;
};
class THLightBasePolygon : public THEffect
{
public:
	THLightBasePolygon():THEffect()
	{
	}
	void Load();

	void SetScreenSize(float w,float h)
	{
		programObject.UseProgram();
		glUniform2f(programObject.GetUniformLocation("screenScale"),2.0f/w,2.0f/h);
	}
	void SetPosition(float x,float y)
	{
		programObject.UseProgram();
		glUniform2f(positionHandler,x,y);
	}
	
	void SetRadius(float r)
	{
		programObject.UseProgram();
		glUniform1f(radiusHandler,r);
	}
	void SetVertices(const GLfloat* vertexs,const GLfloat* normals,unsigned int count)
	{
		programObject.UseProgram();
		glUniform2fv(programObject.GetUniformLocation("sVertices"),count,vertexs);
		glUniform2fv(programObject.GetUniformLocation("sNormals"),count,normals);
		glUniform1i(programObject.GetUniformLocation("pCount"),count);
		pCount=count;
		vertex=vertexs;
	}


	void Draw();

protected:
	GLuint positionHandler;
	GLuint radiusHandler;

	GLuint vertexHandler;

	unsigned int pCount;
};


class THLightEffect : public THEffect
{
public:
	THLightEffect():THEffect(){}

	void Load(THTexture* dest,THTexture* src);
	void SetAmbient(float r,float g,float b)
	{
		programObject.UseProgram();
		glUniform3f(lightAmbientHandler,r,g,b);
	}
	void SetAmplitude(float a)
	{
		programObject.UseProgram();
		glUniform1f(lighAmplitudeHandler,a);
	}

	void Draw();

protected:
	GLuint lightAmbientHandler;
	GLuint lighAmplitudeHandler;

	GLuint vertexHandler;
	GLuint destHandler,srcHandler;

	THTexture *destTexture,*srcTexture;
};

/*

Range : -Amplitude ~ +Amplitude
Radius/sec


*/

class THWaveEffect : public THEffect
{
public:
	THWaveEffect():THEffect(){}

	void Load(THTexture* src);
	void Draw(float time);

	void SetFrequency(float f)
	{
		programObject.UseProgram();
		glUniform1f(programObject.GetUniformLocation("waveFreq"),f);
	}
	void SetLightCoeff(float l)
	{
		programObject.UseProgram();
		glUniform1f(programObject.GetUniformLocation("lightAmpli"),l);
	}
	void SetPosition(float x,float y)
	{
		programObject.UseProgram();
		glUniform2f(programObject.GetUniformLocation("wavePos"),x,y);
	}
	void SetAmplitude(float a)
	{
		programObject.UseProgram();
		glUniform1f(programObject.GetUniformLocation("waveAmpli"),a);
	}

protected:
	GLuint timeHandler;

	GLuint vertexHandler,textureHandler;
	GLuint waveNormalHandler;

	THTexture *srcTexture;
};

class THShockWaveEffect : public THEffect
{
public:
	THShockWaveEffect() : THEffect()
	{
	}

	void Load(THTexture* src);
	void Draw(float dt); 

	void SetWaveSize(float w,float h)
	{
		programObject.UseProgram();
		glUniform2f(programObject.GetUniformLocation("waveWidth"),w,1.0f/w);
		glUniform1f(programObject.GetUniformLocation("waveHeight"),h);
	}
	void SetPosition(float x,float y)
	{
		programObject.UseProgram();
		glUniform2f(positionHandler,x,y);
	}
	void SetTime(float t)
	{
		programObject.UseProgram();
		glUniform1f(timeHandler,t*timeCoeff);
	}
	void SetVelocity(float v)
	{
		timeCoeff=v;
	}
	void SetLightCoeff(float a)
	{
		glUniform1f(programObject.GetUniformLocation("lightCoeff"),a);
	}
	void Switch(bool isOn)
	{
		programObject.UseProgram();
		glUniform1i(isOnHandler,isOn?1:0);
	}

	void Shock()
	{
		shockTime=0.0f;
		Switch(true);
	}

protected:
	float shockTime,timeCoeff;
	THTexture* srcTexture;
	GLuint vertexHandler,textureHandler;
	GLuint positionHandler,timeHandler,isOnHandler;
};
class THSwirlEffect : public THEffect
{
public:
	THSwirlEffect():THEffect()
	{
	}

	void Load(THTexture* src);
	void Draw();


	void SetRadius(float r)
	{
		programObject.UseProgram();
		glUniform2f(radiusHandler,r,1.0f/r);
	}
	void SetAngle(float r)
	{
		programObject.UseProgram();
		glUniform1f(rotationHandler,r);
	}
	void SetPosition(float x,float y)
	{
		programObject.UseProgram();
		glUniform2f(programObject.GetUniformLocation("cPosition"),x,y);
	}
	void SetCoeff(float a)
	{
		programObject.UseProgram();
		glUniform1f(programObject.GetUniformLocation("angleCoeff"),a);
	}

protected:
	THTexture* srcTexture;
	GLuint vertexHandler,textureHandler;

	GLuint rotationHandler;
	GLuint radiusHandler;
};

#endif

#ifndef THKim_THEffects
#define THKim_THEffects

#include <GLES2/gl2.h>
#include <GameEngine/Rendering/THDrawing.h>
#include <THPrivate.h>


class THLightEffect
{
public:
	THProgram program;
	THTexture *destTexture,*srcTexture;
	const GLfloat *vertex;
	THLightEffect(){ vertex=defaultFullVertices; }

	void Load(THTexture* dest,THTexture* src);
	void SetAmbient(float r,float g,float b) const
	{
		program.SetUniform("ambient",r,g,b);
	}
	void SetAmplitude(float a) const
	{
		glUniform1f(amplitudeHandler,a);
	}

	void Draw() const;

protected:
	GLuint vertexHandler;
	GLuint destHandler,srcHandler;

	GLuint amplitudeHandler;
};

class THBlurEffect
{
public:
	THProgram program;
	THTexture* srcTexture;
	const GLfloat* vertex;

	THBlurEffect(int steps)
	{
		assert(steps<=12);
		stepCount=steps;
		vertex=defaultFullVertices;
	}

	void Load(THTexture* src);
	void Draw() const;

	void SetDirection(float x,float y) const
	{
		program.SetUniform("dir",x,y);
	}
	void SetRadius(float r) const
	{
		const THVector2& bl=(r/(float)stepCount) / srcTexture->image->size;
		program.SetUniform("blur",bl.x,bl.y);
	}

protected:
	GLuint vertexHandler;
	GLuint textureHandler;

	int stepCount;
};


class THLightBaseCircle
{
public:
	THProgram program;

	void Load(const THVector2& resolution);
	void Draw();

	
	void SetPosition(float x,float y)
	{
		glUniform2f(positionHandler,x,y);
	}

	void SetRadius(float lightRadius,float circleRadius)
	{
		program.SetUniform("cRadius",circleRadius);
		program.SetUniform("lRadius",lightRadius);
		program.SetUniform("lRadiusi",1.0f/lightRadius);
		if(lightRadius!=circleRadius)
		{
			program.SetUniform("lcGapi",1.0f/(lightRadius-circleRadius));
		}
		
	}


	

protected:
	GLuint vertexHandler;

	GLuint positionHandler;
};



/*

Range : -Amplitude ~ +Amplitude
Radius/sec


*/
/*
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
*/
class THShockWaveEffect
{
public:
	THProgram program;
	THShockWaveEffect()
	{
	}

	void Load(THTexture* src);
	void Draw(float dt); 

	void SetWaveSize(float w,float h)
	{
		program.SetUniform("waveWidth",w,1.0f/w);
		program.SetUniform("waveHeight",h);
	}
	void SetPosition(float x,float y)
	{
		glUniform2f(positionHandler,x,y);
	}
	void SetTime(float t)
	{
		glUniform1f(timeHandler,t*timeCoeff);
	}
	void SetVelocity(float v)
	{
		timeCoeff=v;
	}
	void SetLightCoeff(float a)
	{
		program.SetUniform("lightCoeff",a);
	}
	void Switch(bool isOn)
	{
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

class THSwirlEffect
{
public:
	THProgram program;
	const GLfloat* vertex;

	THSwirlEffect()
	{
		vertex=defaultFullVertices;
	}

	void Load(THTexture* src);
	void Draw();


	void SetRadius(float r)
	{
		program.SetUniform("cRadius",r,1.0f/r);
	}
	void SetAngle(float r)
	{
		glUniform1f(rotationHandler,r);
	}
	void SetPosition(float x,float y)
	{
		program.SetUniform("cPosition",x,y);
	}
	void SetCoeff(float a)
	{
		program.SetUniform("angleCoeff",a);
	}

protected:
	THTexture* srcTexture;
	GLuint vertexHandler,textureHandler;

	GLuint rotationHandler;
};

#endif

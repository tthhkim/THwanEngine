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
	const GLfloat* vertex;

	THBlurEffect(int steps)
	{
		assert(steps<=20);
		stepCount=steps;
		vertex=defaultFullVertices;
	}

	void Load(THTexture* src);
	void Draw() const;

	void SetBlurOpposite(bool opp) const
	{
		glUniform1i(blurOppositeHandler,opp?1:0);
	}
	void SetDirection(float x,float y) const
	{
		glUniform2f(directionHandler,x,y);
	}
	void SetRadius(float r) const
	{
		const THVector2& bl=(r/(float)stepCount) / srcTexture->image->size;
		glUniform2f(radiusHandler,bl.x,bl.y);
	}

protected:
	THTexture* srcTexture;

	GLuint vertexHandler;
	GLuint textureHandler;

	GLuint blurOppositeHandler;
	GLuint directionHandler;
	GLuint radiusHandler;

	int stepCount;
};


class THLightBaseCircle
{
public:
	THProgram program;

	void Load(const THVector2& resolution);
	void Draw();

	
	void SetPosition(float x,float y) const
	{
		glUniform2f(positionHandler,x,y);
	}

	void SetRadius(float lightRadius,float circleRadius) const
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

/*

Range : -Amplitude ~ +Amplitude
Radius/sec


*/

class THLinearWaveEffect
{
public:
	THProgram program;
	const GLfloat* vertex;
	
	THLinearWaveEffect(){time=0.0f;vertex=defaultFullVertices;}

	void Load(THTexture* src);
	void Draw(float dt);

	void SetWave(float amplitude,float velocity,float frequency)
	{
		program.SetUniform("waveVeli",100.0f/velocity);
		program.SetUniform("waveFreq",frequency);
		program.SetUniform("waveAmpli",amplitude*0.01f);
		freqI=10.0f/frequency;
	}
	void SetDirection(float x,float y) const
	{
		const THVector2& sizeI=1.0f/srcTexture->image->size;
		glUniform2f(directionHandler,x,y);
		glUniform2f(directionSkew,-y*sizeI.x,x*sizeI.y);
	}
	void SetLightCoeff(float l) const
	{
		program.SetUniform("lightAmpli",l);
	}

protected:
	float time;
	float freqI;

	GLuint timeHandler;
	GLuint directionHandler,directionSkew;
	GLuint vertexHandler,textureHandler;

	THTexture *srcTexture;
};

class THAngularWaveEffect
{
public:
	THProgram program;
	const GLfloat* vertex;

	THAngularWaveEffect():time(0.0f){vertex=defaultFullVertices;}

	void Load(THTexture* src);
	void Draw(float dt);

	void SetCenter(float x,float y) const
	{
		program.SetUniform("center",x*0.01f,y*0.01f);
	}
	void SetWave(float frequency,float amplitude,float velocity)
	{
		program.SetUniform("frequency",frequency);
		program.SetUniform("amplitude",amplitude);
		program.SetUniform("angularVel",velocity);
		timeLimit=6.283185f/velocity;
	}
protected:
	float time;
	float timeLimit;
	GLuint vertexHandler,textureHandler;
	GLuint timeHandler;

	THTexture* srcTexture;
};

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



#endif

#ifndef TH_GLINSTANT_TWEEN
#define TH_GLINSTANT_TWEEN

#include <GameEngine/Util/THMath.h>

class THTween
{
public:
	THTween *next,*prev;

	//Callback when tween ends;
	void (*onEndTween)(void* data);
	//UserData
	void* data;
	//Wheather this tween will be deleted by program when it ends
	bool autoDelete;

	const float time;

	virtual bool step()=0;

	THTween(float _time):time(_time)
	{
		onEndTween=0;
		next=0;
		prev=0;
		autoDelete=true;
	}
};

//return accelated value between [0,1]
class THInterpolater
{
public:
	float accel,v0;
	const float time;

	THInterpolater(float seconds,float coeff):time(seconds)
	{
		const float vel=1.0f/seconds;
		const float vc=vel*coeff;
		accel=2.0f*vel*vc;
		v0=vel-vc;
	}
	inline float GetValue(float ct) const
	{
		return 0.5f*accel*ct*ct + v0*ct;
	}
};

class THLinearTween : public THTween
{
public:
	THLinearTween(THVector2 *_src,const THVector2& deltaPos,const THInterpolater& interpolator):THTween(interpolator.time),
		src0(*_src),src(_src),
		delta(deltaPos),
		interpolater(interpolator)
	{
		ct=0.0f;
	}

	bool step()
	{
		ct+=THDeltaTime;
		if(ct<time)
		{
			*src=src0+(interpolater.GetValue(ct)*delta);
			return false;
		}
		*src=src0+delta;
		return true;
	}

protected:
	float ct;

	//Source
	const THVector2 src0;
	THVector2* const src;

	//Destination
	const THVector2 delta;

	//Interpolater
	const THInterpolater interpolater;
};

#endif
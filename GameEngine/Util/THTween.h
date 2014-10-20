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

	virtual bool step(float dt)=0;

	THTween(float _time):time(_time)
	{
		onEndTween=0;
		next=0;
		prev=0;
		autoDelete=true;
	}
};

class THLinearTweenDef
{
public:
	THVector2 v0;
	THVector2 accel;
	THVector2 delta;
	const float time;

	THLinearTweenDef(const THVector2& deltaPos,float seconds,float coeff):time(seconds)
	{
		delta=deltaPos;
		const float inv_time=1.0f/seconds;
		const THVector2 vel=deltaPos*inv_time;
		const THVector2 vc=vel*coeff;

		accel=(2.0f*inv_time)*vc;
		v0=vel-vc;
	}
};

class THLinearTween : public THTween
{
public:
	THLinearTween(THVector2 *_src,const THLinearTweenDef& tweenDef):THTween(tweenDef.time),
		src0(*src),src(src),
		delta(tweenDef.delta),v0(tweenDef.v0),accel(tweenDef.accel)
	{
		//THLog("New Tween Added : %d Frames , %.2f Seconds",frames,(float)frames/(float)GLTHREAD_FRAMERATE);

		ct=0.0f;
	}

	bool step(float dt)
	{
		ct+=dt;
		if(ct<time)
		{
			*src=src0+0.5f*accel*ct*ct + v0*ct;
			return false;
		}
		*src=src0+delta;
		return true;
	}

protected:
	float ct;
	const THVector2 src0;
	THVector2* const src;
	//Source
	const THVector2 delta;
	//Destination
	const THVector2 v0;
	//Velocity Per Sec
	const THVector2 accel;
};

#endif
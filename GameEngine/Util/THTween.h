#ifndef TH_GLINSTANT_TWEEN
#define TH_GLINSTANT_TWEEN

#include <GameEngine/Util/THMath.h>

class THTween
{
public:
	THTween *next,*prev;
	void (*onEndTween)(void* data);
	//Callback when tween ends;
	void* data;

	virtual bool step(float dt)=0;

	THTween(float _time):time(_time)
	{
		onEndTween=0;
		next=0;
		prev=0;
	}

protected:
	const float time;
};

class THTweenSingle : public THTween
{
public:
	float* src;
	//Source
	float dest;
	//Destination
	float v0;
	float accel;
	//Velocity Per Frame
	THTweenSingle(float* _src,float _dest,float seconds,float coeff):THTween(seconds)
	{
		//THLog("New Tween Added : %d Frames , %.2f Seconds",frames,(float)frames/(float)GLTHREAD_FRAMERATE);

		src=_src;
		src0=*_src;
		dest=_dest;
		ct=0.0f;

		const float inv_time=1.0f/seconds;
		const float vel=(dest-*src)*inv_time;
		const float vc=vel*coeff;

		accel=2.0f*vc*inv_time;
		v0=vel-vc;
	}

	bool step(float dt)
	{
		ct+=dt;
		if(ct<time)
		{
			*src=src0+0.5f*accel*ct*ct + v0*ct;
			return false;
		}
		*src=dest;
		return true;
	}

protected:
	float ct;
	float src0;
};
class THTweenDouble : public THTween
{
public:
	THVector2* src;
	//Source
	THVector2 dest;
	//Destination
	THVector2 v0;
	THVector2 accel;
	//Velocity Per Frame

	THTweenDouble(THVector2 *_src,const THVector2& _dest,float seconds,float coeff):THTween(seconds)
	{
		//THLog("New Tween Added : %d Frames , %.2f Seconds",frames,(float)frames/(float)GLTHREAD_FRAMERATE);

		src=_src;
		src0=*_src;
		dest=_dest;
		ct=0.0f;

		const float inv_time=1.0f/seconds;
		const THVector2 vel=(dest-*src)*inv_time;
		const THVector2 vc=vel*coeff;

		accel=2.0f*vc*inv_time;
		v0=vel-vc;
	}

	bool step(float dt)
	{
		ct+=dt;
		if(ct<time)
		{
			*src=src0+0.5f*accel*ct*ct + v0*ct;
			return false;
		}
		*src=dest;
		return true;
	}

protected:
	float ct;
	THVector2 src0;
};

#endif
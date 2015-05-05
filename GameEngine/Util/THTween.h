#ifndef TH_GLINSTANT_TWEEN
#define TH_GLINSTANT_TWEEN

#include <GameEngine/Util/THMath.h>

//return accelated value between [0,1]
class THInterpolator
{
public:
	void Load(float seconds,float coeff)
	{
		m_time=0.0f;
		m_seconds=seconds;

		const float vel=1.0f/seconds;
		const float vc=vel*coeff;
		m_halfaccel=vel*vc;
		m_v0=vel-vc;
	}
	inline float GetCurTime() const{return m_time;}
	inline float GetMaxSeconds() const{return m_seconds;}
	inline bool IsOut() const{return m_time>=m_seconds;}
	inline float GetDelta(float dt)
	{
		m_time+=dt;

		return m_halfaccel*(2.0f*m_time*dt - dt*dt) + m_v0*dt;
	}
protected:
	float m_halfaccel,m_v0;
	float m_seconds,m_time;
};

template <typename T>
class THTweenVector
{
public:
	void Load(T *src,const T& delta,const THInterpolator& interpolate)
	{
		m_src=src;
		m_delta=delta;

		m_interpolator=interpolate;
	}

	//return if the time is over
	bool Step(float dt)
	{
		*m_src += m_delta*m_interpolator.GetDelta(dt);
		return m_interpolator.IsOut();
	}

protected:
	T *m_src;
	const T m_delta;

	//Interpolater
	THInterpolator m_interpolator;
};

#endif
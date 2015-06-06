#include "THTween.h"

void THInterpolator::Load(float seconds,float coeff)
{
	m_time=0.0f;
	m_seconds=seconds;

	const float vel=1.0f/seconds;
	const float vc=vel*coeff;
	m_halfaccel=vel*vc;
	m_v0=vel-vc;
}
float THInterpolator::GetFraction(float dt)
{
	m_time+=dt;
	return m_halfaccel*m_time*m_time+m_v0*m_time;
}
float THInterpolator::GetFractionTime(float t)
{
	return m_halfaccel*t*t+m_v0*t;
}
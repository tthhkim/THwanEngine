#ifndef TH_GLINSTANT_TWEEN
#define TH_GLINSTANT_TWEEN

//return accelated value between [0,1]
class THInterpolator
{
public:
	void Load(float maxseconds,float coeff);
	inline float GetCurTime() const{return m_time;}
	inline float GetMaxSeconds() const{return m_seconds;}
	inline bool IsEnd() const{return m_time>=m_seconds;}
	//with relative dt
	float GetFraction(float dt);
	//with absolute time
	float GetFractionTime(float t);
protected:
	float m_halfaccel,m_v0;
	float m_seconds,m_time;
};

#endif
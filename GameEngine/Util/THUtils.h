#ifndef TH_UTILS_H
#define TH_UTILS_H

#include "THArray.h"
#include "THMath.h"
#include "THMath3D.h"
#include "THTween.h"

class THPingPong
{
public:
	void *one,*two;

	THPingPong()
	{
		m_isone=true;
	}
	void Load(void *_one,void *_two);
	inline void* Get(){return m_isone?one:two;}
	inline void Change(){m_isone=!m_isone;}
protected:
	bool m_isone;
};

#endif
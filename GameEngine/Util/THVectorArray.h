#ifndef TH_GLINSTANT_THVARRAY
#define TH_GLINSTANT_THVARRAY

#include "THMath.h"
#include <GameEngine/Util/THArray.h>

class THVector2Array : public THArray<THVector2>
{
public:
	THVector2Array(unsigned int cap):THArray(cap)
	{
	}
	THVector2Array():THArray()
	{
	}

	void operator +=(const THVector2& v)
	{
		for(unsigned int i=0;i<num;++i)
		{
			arr[i]+=v;
		}
	}
	void operator -=(const THVector2& v)
	{
		for(unsigned int i=0;i<num;++i)
		{
			arr[i]-=v;
		}
	}
	void operator *=(float a)
	{
		for(unsigned int i=0;i<num;++i)
		{
			arr[i]*=a;
		}
	}
	void operator *=(const THMatrix22& rot)
	{
		for(unsigned int i=0;i<num;++i)
		{
			arr[i]=rot*arr[i];
		}
	}
	void operator *=(const THRot2& rot)
	{
		for(unsigned int i=0;i<num;++i)
		{
			arr[i]=rot*arr[i];
		}
	}
	void Bridge(THArray<THVector2>& dest,float gap);
protected:
};


#endif
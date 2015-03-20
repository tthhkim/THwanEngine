#include "THVectorArray.h"

void THVector2Array::Bridge(THArray<THVector2>& dest,float gap)
{
	float left=0.0f,k;
	THVector2 rel;
	for(unsigned int i=1;i<num;++i)
	{
		const THVector2& p0=arr[i-1];
		rel=arr[i]-p0;
		k=left;
		left+=rel.Normalize();

		for(k;k<left;k+=gap)
		{
			dest.Push(p0 + (rel*k));
		}
		left=k-left;
	}
	if(left>0.5f*gap)
	{
		dest.Push(arr[num-1]);
	}
}
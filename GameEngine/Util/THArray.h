#ifndef TH_GLINSTANT_THARRAY
#define TH_GLINSTANT_THARRAY

#include <malloc.h>
#include <string.h>
#include <THPrivate.h>

template <typename T>
class THArray
{
public:
	unsigned int num,maxNum;
	T* arr;

	THArray(const unsigned int cap)
	{
		num=0;
		maxNum=cap;
		arr=(T*)malloc(sizeof(T)*cap);
	}
	~THArray()
	{
		free(arr);
	}

	void Push(const T& object)
	{		
		if(num==maxNum)
		{
			THError("THArray : ReAlloced()");
			maxNum<<=1;
			arr=(T*)realloc(arr,sizeof(T)*maxNum);
			assert(arr);
		}
		arr[num]=object;
		++num;
	}
	void Repush(const T& object)
	{
		unsigned int i;
		for(i=0;i<num-1;++i)
		{
			if(arr[i]==object)
			{
				memmove(arr+i,arr+(i+1),(num-i-1)*sizeof(T));
				arr[num-1]=object;
				return;
			}
		}
	}
	void Delete(const T& object)
	{
		unsigned int i;
		for(i=0;i<num-1;++i)
		{
			if(arr[i]==object)
			{
				--num;
				memmove(arr+i,arr+(i+1),(num-i)*sizeof(T));
				return;
			}
		}
	}
	inline void Clear()
	{
		num=0;
	}
};

#endif
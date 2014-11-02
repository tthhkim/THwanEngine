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
	void PushAt(const T& obj,unsigned int i)
	{
		if(i>=num)
		{
			arr[num]=obj;
		}
		else
		{
			memmove(arr+(i+1),arr+i,sizeof(T)*(num-i));
			arr[i]=obj;
		}
		++num;
	}
	int Find(const T& object) const
	{
		for(unsigned int i=0;i<num;++i)
		{
			if(arr[i]==object)
			{
				return i;
			}
		}
		return -1;
	}
	int FindBack(const T& object) const
	{
		unsigned int i=num;
		while(i)
		{
			--i;
			if(arr[i]==object)
			{
				return i;
			}
		}
		return -1;
	}
	void Delete(unsigned int index)
	{
		assert(index<num);
		--num;
		memmove(arr+index,arr+(index+1),(num-index)*sizeof(T));
	}
	void Delete(const T& object)
	{
		int ind=Find(object);
		if(ind!=-1)
		{
			Delete(ind);
		}
	}
	void Repush(const T& object)
	{
		for(unsigned i=0;i<num-1;++i)
		{
			if(arr[i]==object)
			{
				memmove(arr+i,arr+(i+1),(num-i-1)*sizeof(T));
				arr[num-1]=object;
				return;
			}
		}
	}
	
	inline T& GetLast() const
	{
		assert(num>0);
		return arr[num-1];
	}
	inline void Clear()
	{
		num=0;
	}

	void FreeAll()
	{
		for(unsigned int i=0;i<num;++i)
		{
			free(arr[i]);
		}
	}
};

#endif
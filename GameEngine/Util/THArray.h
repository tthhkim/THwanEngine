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
	THArray()
	{
		arr=0;
		num=0;
		maxNum=0;
	}
	~THArray()
	{
		Free();
	}

	void Load(unsigned int cap)
	{
		if(arr==0)
		{
			arr=(T*)malloc(sizeof(T)*cap);
			num=0;
			maxNum=cap;
		}
	}
	void Free()
	{
		if(arr)
		{
			free(arr);
			arr=0;
			num=0;
			maxNum=0;
		}
	}
	void Extend(unsigned int cap)
	{
		if(cap>maxNum)
		{
			maxNum=cap+1;
			Realloc();
		}
	}
	bool IsAllocated() const
	{
		return arr!=0;
	}
	void Push(const T& object)
	{		
		if(num==maxNum)
		{
			THError("THArray : ReAlloced()");
			maxNum<<=1;
			Realloc();
		}
		arr[num]=object;
		++num;
	}
	void PushAt(const T& obj,unsigned int i)
	{
		assert(i<num);
		if(num==maxNum)
		{
			THError("THArray : ReAlloced()");
			maxNum<<=1;
			Realloc();
		}
		memmove(arr+(i+1),arr+i,sizeof(T)*(num-i));
		arr[i]=obj;
		++num;
	}
	void PushArray(const T *_arr,unsigned int _count)
	{
		if(num+_count>maxNum)
		{
			maxNum=(maxNum<<1)+_count;
			Realloc();
		}
		memcpy(arr+num,_arr,sizeof(T)*_count);
		num+=_count;
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
	void DeleteBack(unsigned int index)
	{
		assert(index<num);
		--num;
		if(index!=num)
		{
			arr[index]=arr[num];
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
	T& Pop()
	{
		assert(num>0);
		--num;
		return arr[num];
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
protected:
	void Realloc()
	{
		arr=(T*)realloc(arr,sizeof(T)*maxNum);
		assert(arr);
	}
};

#endif
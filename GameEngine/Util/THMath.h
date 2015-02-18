#ifndef TH_GLINSTANT_THMATH
#define TH_GLINSTANT_THMATH

#include <math.h>

#define TH_PI 3.14159265359f
#define TH_2PI 6.28318530718f
#define TH_HPI 1.57079632679f

static inline bool THPowerOfTwo(int x)
{
	const int st=x&(x-1);
	return st==0;
}
static inline int THBitCount(int x)
{
	int b=0;
	while((x&1)==0)
	{
		x>>=1;
		++b;
	}
	return b;
}

class THVector2
{
public:
	float x,y;

	
	THVector2()
	{
	}
	THVector2(const float _x,const float _y)
	{
		x=_x;
		y=_y;
	}
	
	inline void SetZero()
	{
		x=0.0f;
		y=0.0f;
	}
	inline void Set(const float _x,const float _y)
	{
		x=_x;
		y=_y;
	}
	THVector2 operator -() {return THVector2(-x,-y);}
	void operator +=(const THVector2& v){ x+=v.x; y+=v.y; }
	void operator -=(const THVector2& v){ x-=v.x; y-=v.y; }
	void operator *=(float& a){ x*=a; y*=a; }
	void operator /=(float& a){const float ai=1.0f/a;x*=ai;y*=ai;}

	void operator +=(float& a){x+=a; y+=a;}
	void operator -=(float& a){x-=a; y-=a;}

	void Skew()
	{
		const float tx=x;
		x=-y;
		y=tx;
	}
	float LengthSquared() const 
	{
		return x*x + y*y;
	}
	float Length() const
	{
		return sqrtf(x*x + y*y);
	}

	float Normalize()
	{
		const float length=Length();
		const float invL=1.0f/length;

		x*=invL;
		y*=invL;

		return length;
	}
};
class THRot2
{
public:
	float c,s;

	THRot2()
	{
		c=1.0f;
		s=0.0f;
	}
	THRot2(const float _c,const float _s)
	{
		c=_c;
		s=_s;
	}
	THRot2(const float angle)
	{
		Set(angle);
	}

	inline void Set(const float _c,const float _s)
	{
		c=_c;
		s=_s;
	}
	inline void SetIdentity()
	{
		c=1.0f;
		s=0.0f;
	}
	void Set(const float angle)
	{
		const float _c=cosf(angle);
		const float _s=sinf(angle);
		Set(_c,_s);
	}
	
	inline THRot2 Inverse() const
	{
		return THRot2(c,-s);
	}
	inline void Rotate(const float _c,const float _s)
	{
		Set(c*_c - s*_s , c*_s + s*_c);
	}
	void Rotate(const float angle)
	{
		const float _c=cosf(angle);
		const float _s=sinf(angle);

		Rotate(_c,_s);
	}
};

inline float THDot(const THVector2& a, const THVector2& b)
{
	return a.x*b.x + a.y*b.y;
}
inline float THCrossScalar(const THVector2& a,const THVector2& b)
{
	return a.x*b.y-a.y*b.x;
}
inline THVector2 THMin(const THVector2& a,const THVector2& b)
{
	return THVector2(a.x<b.x?a.x:b.x,a.y<b.y?a.y:b.y);
}
inline THVector2 THMax(const THVector2& a,const THVector2& b)
{
	return THVector2(a.x<b.x?b.x:a.x , a.y<b.y?b.y:a.y);
}


inline THVector2 operator +(const THVector2& a,const THVector2& b)
{
	return THVector2(a.x+b.x, a.y+b.y);
}
inline THVector2 operator -(const THVector2& a,const THVector2& b)
{
	return THVector2(a.x-b.x, a.y-b.y);
}
inline THVector2 operator *(const float s,const THVector2& a)
{
	return THVector2(a.x*s, a.y*s);
}
inline THVector2 operator *(const THVector2& a,const float s)
{
	return THVector2(a.x*s, a.y*s);
}
inline THVector2 operator /(const THVector2& v,const float s)
{
	const float is=1.0f/s;
	return THVector2(v.x*is,v.y*is);
}
inline THVector2 operator /(const float s,const THVector2& v)
{
	return THVector2(s/v.x,s/v.y);
}
inline THVector2 operator *(const THVector2& a, const THVector2& b)
{
	return THVector2(a.x*b.x,a.y*b.y);
}
inline THVector2 operator /(const THVector2& a, const THVector2& b)
{
	return THVector2(a.x/b.x,a.y/b.y);
}
inline THVector2 operator +(const THVector2& a,const float s)
{
	return THVector2(a.x+s,a.y+s);
}
inline THVector2 operator -(const THVector2& a,const float s)
{
	return THVector2(a.x-s,a.y-s);
}
inline bool operator ==(const THVector2& a, const THVector2& b)
{
	return (a.x==b.x)&&(a.y==b.y);
}
inline THVector2 Reflect(const THVector2& in,const THVector2& n,float e)
{
	return in-((1.0f+e)*THDot(n,in))*n;
}
inline THVector2 Reflect(const THVector2& in,const THVector2& n)
{
	return in-(2.0f*THDot(n,in))*n;
}



class THMatrix22
{
public:
	THVector2 row1,row2;

	THMatrix22(float _r1c1=1.0f,float _r1c2=0.0f,float _r2c1=0.0f,float _r2c2=1.0f):row1(_r1c1,_r1c2),row2(_r2c1,_r2c2)
	{
	}
	THMatrix22(const THVector2& r1,const THVector2& r2):row1(r1),row2(r2)
	{
	}
	THMatrix22(const THRot2& rot):row1(rot.c,-rot.s),row2(rot.s,rot.c)
	{
	}

	void operator +=(const THMatrix22& m)
	{
		row1+=m.row1;
		row2+=m.row2;
	}
	void operator -=(const THMatrix22& m)
	{
		row1-=m.row1;
		row2-=m.row2;
	}
	void operator *=(float& a)
	{
		row1*=a;
		row2*=a;
	}
	void operator *=(const THMatrix22& m)
	{
		float mt=row1.x;
		row1.x=mt*m.row1.x + row1.y*m.row2.x;
		row1.y=mt*m.row1.y + row1.y*m.row2.y;

		mt=row2.x;
		row2.x=mt*m.row1.x + row2.y*m.row2.x;
		row2.y=mt*m.row1.y + row2.y*m.row2.y;
	}

	void SetIdentity()
	{
		row1.Set(1.0f,0.0f);
		row2.Set(0.0f,1.0f);
	}
	void Set(const float _r1c1,const float _r1c2,const float _r2c1,const float _r2c2)
	{
		row1.Set(_r1c1,_r1c2);
		row2.Set(_r2c1,_r2c2);
	}
	void Set(const THVector2& r1,const THVector2& r2)
	{
		row1=r1;
		row2=r2;
	}
	float Discriminant() const
	{
		return row1.x*row2.y - row1.y*row2.x;
	}
	inline bool HasInverse() const
	{
		return Discriminant()!=0.0f;
	}
	THMatrix22 Inverse() const//unsafe division by zero
	{
		const float a=1.0f/Discriminant();
		return THMatrix22(
			row2.y*a , -row1.y*a,
			-row2.x*a , row1.x*a);
	}
	THMatrix22 Inverse(float invD) const//unsafe division by zero
	{
		return THMatrix22(
			row2.y*invD , -row1.y*invD,
			-row2.x*invD , row1.x*invD);
	}
	THMatrix22 Transpose() const
	{
		return THMatrix22
			( 
			THVector2(row1.x,row2.x),
			THVector2(row1.y,row2.y)
			);
	}

	THVector2 Solve(const THVector2& v) const
	{
		const float a=1.0f/Discriminant();
		return THVector2(
			a*(row2.y*v.x - row1.y*v.y),
			a*(row1.x*v.y - row2.x*v.x)
			);
	}
	THVector2 Solve(float invD,const THVector2& v) const
	{
		return THVector2(
			invD*(row2.y*v.x - row1.y*v.y),
			invD*(row1.x*v.y - row2.x*v.x)
			);
	}
};

void THOrthoMatrix33(float *mat,const THVector2& min,const THVector2& max);

inline bool operator ==(const THMatrix22& a,const THMatrix22& b)
{
	return (a.row1==b.row1)&&(a.row2==b.row2);
}

inline THMatrix22 operator +(const THMatrix22& a,const THMatrix22& b)
{
	return THMatrix22(a.row1+b.row1,a.row2+b.row2);
}
inline THMatrix22 operator -(const THMatrix22& a,const THMatrix22& b)
{
	return THMatrix22(a.row1-b.row1,a.row2-b.row2);
}
inline THMatrix22 operator *(const THMatrix22& m,const float a)
{
	return THMatrix22(m.row1*a,m.row2*a);
}
inline THMatrix22 operator *(const float a,const THMatrix22& m)
{
	return THMatrix22(m.row1*a,m.row2*a);
}
inline THVector2 operator *(const THMatrix22& m,const THVector2& v)
{
	return THVector2(THDot(m.row1,v),THDot(m.row2,v));
}
inline THMatrix22 operator *(const THMatrix22& a,const THMatrix22& b)
{
	return THMatrix22(
		a.row1.x*b.row1.x + a.row1.y*b.row2.x , a.row1.x*b.row1.y + a.row1.y*b.row2.y,
		a.row2.x*b.row1.x + a.row2.y*b.row2.x , a.row2.x*b.row1.y + a.row2.y*b.row2.y
		);
}

inline bool operator ==(const THRot2& a,const THRot2& b)
{
	return a.c==b.c && a.s==b.s;
}
inline bool operator ==(const THRot2& a,const THMatrix22& b)
{
	return b.row1.x==a.c && b.row1.y==-a.s && b.row2.x==a.s && b.row2.y==a.c;
}
inline bool operator ==(const THMatrix22& b,const THRot2& a)
{
	return b.row1.x==a.c && b.row1.y==-a.s && b.row2.x==a.s && b.row2.y==a.c;
}


inline THMatrix22 operator *(const THMatrix22& a,const THRot2& b)
{
	return THMatrix22(
		a.row1.x*b.c+a.row1.y*b.s , a.row1.x*-b.s+a.row1.y*b.c,
		a.row2.x*b.c+a.row2.y*b.s , a.row2.x*-b.s+a.row2.y*b.c
		);
}
inline THMatrix22 operator *(const THRot2& b,const THMatrix22& a)
{
	return THMatrix22(
		a.row1.x*b.c-a.row2.x*b.s , a.row1.y*b.c-a.row2.y*b.s,
		a.row1.x*b.s+a.row2.x*b.c , a.row1.y*b.s+a.row2.y*b.c
		);
}
inline THRot2 operator *(const THRot2& a,const THRot2& b)
{
	return THRot2(
		a.c*b.c - a.s*b.s,
		a.c*b.s + a.s*b.c
		);
}

inline THVector2 operator *(const THRot2& a,const THVector2& v)
{
	return THVector2(
		a.c*v.x - a.s*v.y,
		a.s*v.x + a.c*v.y
		);
}
static inline THVector2 InverseMultiply(const THRot2& a,const THVector2& v)
{
	return THVector2(
		a.c*v.x + a.s*v.y,
		a.c*v.y - a.s*v.x
		);
}



void GetNormals(const THVector2 *points,THVector2* normals,int count);
void GetCirclePolygonVertices(THVector2* arr,int count,float radius);

#include <stdlib.h>
extern double InvRandomMax;

//in a range [0,1)
static inline double THRand01()
{ return rand()*InvRandomMax; }

//in a range [a,b)
static inline double THRand(double a,double b)
{ return a+(b-a)*THRand01(); }
//in a range [a,b)
static inline float THRandf(float a,float b)
{ return a+(float)((b-a)*THRand01()); }
//in a range [a,b)
static inline int THRandi(int a,int b)
{ return a+(int)((b-a)*THRand01()); }

#endif
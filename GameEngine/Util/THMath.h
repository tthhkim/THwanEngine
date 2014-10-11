#ifndef TH_GLINSTANT_THMATH
#define TH_GLINSTANT_THMATH

#include <math.h>

class THVector2
{
public:
	float x,y;

	THVector2(const float _x,const float _y)
	{
		x=_x;
		y=_y;
	}
	THVector2()
	{
		x=0.0f;
		y=0.0f;
	}
	
	void Set(const float _x,const float _y)
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


class THMatrix22
{
public:
	THVector2 row1,row2;

	THMatrix22():row1(1.0f,0.0f),row2(0.0f,1.0f)
	{
	}
	THMatrix22(float _r1c1,float _r1c2,float _r2c1,float _r2c2):row1(_r1c1,_r1c2),row2(_r2c1,_r2c2)
	{
	}
	THMatrix22(const THVector2& r1,const THVector2& r2)
	{
		row1=r1;
		row2=r2;
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
};
class THRot : public THMatrix22
{
public:
	THRot():THMatrix22()
	{
	}
	THRot(const float c,const float s):THMatrix22(c,-s,s,c)
	{
	}
	THRot(const float angle)
	{
		const float c=cosf(angle);
		const float s=sinf(angle);
		THMatrix22(c,-s,s,c);
	}

	inline void Set(const float c,const float s)
	{
		THMatrix22::Set(c,-s,s,c);
	}
	void Set(const float angle)
	{
		const float c=cosf(angle);
		const float s=sinf(angle);
		Set(c,s);
	}
	
	inline THRot Inverse()
	{
		return THRot(row1.x,row1.y);
	}
	void Rotate(const float c,const float s)
	{
		const float mc=c;

		Set(row1.x*mc - row2.x*s , row1.x*s + row2.x*mc);
	}
	inline void Rotate(const float angle)
	{
		const float c=cosf(angle);
		const float s=sinf(angle);

		Rotate(c,s);
	}
};

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


void GetNormals(const THVector2 *points,THVector2* normals,unsigned int count);
void GetScaledVertices(const THVector2 *points,THVector2 *out,unsigned int count,const THVector2& min,const THVector2& max);

#endif
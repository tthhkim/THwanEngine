#ifndef TH_GLINSTANT_THMATH3D
#define TH_GLINSTANT_THMATH3D

#include "THMath.h"

class THVector3
{
public:
	float x,y,z;

	THVector3(){}
	THVector3(const float _x,const float _y,const float _z)
	{
		x=_x;
		y=_y;
		z=_z;
	}
	THVector3(const THVector2& v2,const float _z)
	{
		x=v2.x;
		y=v2.y;
		z=_z;
	}
	
	void SetZero()
	{
		x=0.0f;
		y=0.0f;
		z=0.0f;
	}
	void Set(const float _x,const float _y,const float _z)
	{
		x=_x;
		y=_y;
		z=_z;
	}
	THVector3 operator -() const {return THVector3(-x,-y,-z);}
	void operator +=(const THVector3& v){ x+=v.x; y+=v.y; z+=v.z; }
	void operator -=(const THVector3& v){ x-=v.x; y-=v.y; z-=v.z; }
	void operator *=(float a){ x*=a; y*=a; z*=a; }

	void operator +=(float a){x+=a; y+=a; z+=a;}
	void operator -=(float a){x-=a; y-=a; z-=a;}

	float LengthSquared() const 
	{
		return x*x + y*y + z*z;
	}
	float Length() const
	{
		return sqrtf(x*x + y*y + z*z);
	}
	float InvLength() const
	{
		return THInvSqrt2(x*x+y*y+z*z);
	}

	float Normalize()
	{
		const float length=Length();
		const float invL=1.0f/length;

		x*=invL;
		y*=invL;
		z*=invL;

		return length;
	}
};

inline float THDot(const THVector3& a, const THVector3& b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
inline THVector3 THCross(const THVector3& a,const THVector3& b)
{
	return THVector3(
		a.y*b.z - a.z*b.y,
		a.z*b.x - a.x*b.z,
		a.x*b.y - a.y*b.x
		);
}


inline THVector3 operator +(const THVector3& a,const THVector3& b)
{
	return THVector3(a.x+b.x, a.y+b.y , a.z+b.z);
}
inline THVector3 operator -(const THVector3& a,const THVector3& b)
{
	return THVector3(a.x-b.x, a.y-b.y , a.z-b.z);
}
inline THVector3 operator *(const float s,const THVector3& a)
{
	return THVector3(a.x*s, a.y*s , a.z*s);
}
inline THVector3 operator *(const THVector3& a,const float s)
{
	return THVector3(a.x*s, a.y*s , a.z*s);
}
inline THVector3 operator /(const THVector3& v,const float s)
{
	const float is=1.0f/s;
	return THVector3(v.x*is , v.y*is , v.z*is);
}
inline THVector3 operator /(const float s,const THVector3& v)
{
	return THVector3(s/v.x,s/v.y,s/v.z);
}
inline THVector3 operator *(const THVector3& a, const THVector3& b)
{
	return THVector3(a.x*b.x,a.y*b.y,a.z*b.z);
}
inline THVector3 operator /(const THVector3& a, const THVector3& b)
{
	return THVector3(a.x/b.x,a.y/b.y,a.z/b.z);
}
inline THVector3 operator +(const THVector3& a,const float s)
{
	return THVector3(a.x+s,a.y+s,a.z+s);
}
inline THVector3 operator -(const THVector3& a,const float s)
{
	return THVector3(a.x-s,a.y-s,a.z-s);
}
inline bool operator ==(const THVector3& a, const THVector3& b)
{
	return (a.x==b.x)&&(a.y==b.y)&&(a.z==b.z);
}

/* W(4th) input componet must be z*z
	z component must be a positive value
*/
void THPerspectiveMatrix44(float* mat,const THVector3& min,const THVector3& max);

class THMatrix33
{
public:
	THVector3 row1,row2,row3;

	THMatrix33():row1(1.0f,0.0f,0.0f),row2(0.0f,1.0f,0.0f),row3(0.0f,0.0f,1.0f)
	{
	}
	THMatrix33(const THVector3& r1,const THVector3& r2,const THVector3& r3)
	{
		row1=r1;
		row2=r2;
		row3=r3;
	}
	THMatrix33(const float* arr):row1(arr[0],arr[1],arr[2]),row2(arr[3],arr[4],arr[5]),row3(arr[6],arr[7],arr[8])
	{
	}

	THMatrix33 operator -() const
	{
		return THMatrix33(-row1,-row2,-row3);
	}
	void operator +=(const THMatrix33& m)
	{
		row1+=m.row1;
		row2+=m.row2;
		row3+=m.row3;
	}
	void operator -=(const THMatrix33& m)
	{
		row1-=m.row1;
		row2-=m.row2;
		row3-=m.row3;
	}
	void operator *=(float& a)
	{
		row1*=a;
		row2*=a;
		row3*=a;
	}
	void operator *=(const THMatrix33& m)
	{
		const THVector3 c1(m.row1.x,m.row2.x,m.row3.x);
		const THVector3 c2(m.row1.y,m.row2.y,m.row3.y);
		const THVector3 c3(m.row1.z,m.row2.z,m.row3.z);

		row1.Set( THDot(row1,c1) , THDot(row1,c2) , THDot(row1,c3) );
		row2.Set( THDot(row2,c1) , THDot(row2,c2) , THDot(row2,c3) );
		row3.Set( THDot(row3,c1) , THDot(row3,c2) , THDot(row3,c3) );
	}

	void SetIdentity()
	{
		row1.Set(1.0f,0.0f,0.0f);
		row2.Set(0.0f,1.0f,0.0f);
		row3.Set(0.0f,0.0f,1.0f);
	}
	void Set(const THVector3& r1,const THVector3& r2,const THVector3& r3)
	{
		row1=r1;
		row2=r2;
		row3=r3;
	}
	THMatrix33 Transpose() const
	{
		return THMatrix33
			(
			THVector3(row1.x,row2.x,row3.x),
			THVector3(row1.y,row2.y,row3.y),
			THVector3(row1.z,row2.z,row3.z)
			);
	}

	float Discriminant() const;
	THMatrix33 Inverse() const;

	THVector3 Solve(const THVector3& v) const;
	THVector3 Solve(const THVector3& v,float invD) const;

	static THMatrix33 RotateAxis(const THVector3& axis,const THRot2& rot);


	/*
	returns 3 by 3 matrix which has rotation from p1 to p2
	*/
	static THMatrix33 RotatePoint(const THVector3& p1,const THVector3& p2);
	/*
	multiply this matrix with vec3 object, will transform into normal coordinate from eye-looking eyenormal vector
	In Radian

	positive right for yRotation
	positive down for xRotation
	*/
	static THMatrix33 EyeTrnsformMatrix(const THRot2& yrot,const THRot2& xrot,THVector3* eyenorm);
};
#define THROT_AXIS_X 1
#define THROT_AXIS_Y 2
#define THROT_AXIS_Z 3
class THRot3 : public THMatrix33
{
public:
	THRot3():THMatrix33(){}
	THRot3(const THRot2& rot,int axis)
	{
		switch(axis)
		{
		case THROT_AXIS_X:
			SetX(rot);
			return;
		case THROT_AXIS_Y:
			SetY(rot);
			return;
		case THROT_AXIS_Z:
			SetZ(rot);
			return;
		}
	}
	THRot3(float angle,int axis)
	{
		switch(axis)
		{
		case THROT_AXIS_X:
			SetX(THRot2(angle));
			return;
		case THROT_AXIS_Y:
			SetY(THRot2(angle));
			return;
		case THROT_AXIS_Z:
			SetZ(THRot2(angle));
			return;
		}
	}
	
	void SetX(const THRot2& rot)
	{
			row1.Set(1.0f,0.0f,0.0f);
			row2.Set(0.0f,rot.x,-rot.y);
			row3.Set(0.f,rot.y,rot.x);
	}
	void SetY(const THRot2& rot)
	{
			row1.Set(rot.y,0.0f,rot.x);
			row2.Set(0.0f,1.0f,0.0f);
			row3.Set(rot.x,0.0f,-rot.y);
	}
	void SetZ(const THRot2& rot)
	{
			row1.Set(rot.x,-rot.y,0.0f);
			row2.Set(rot.y,rot.x,0.0f);
			row3.Set(0.0f,0.0f,1.0f);
	}
	THMatrix33 Inverse() const
	{
		return Transpose();
	}
};

inline bool operator ==(const THMatrix33& a,const THMatrix33& b)
{
	return (a.row1==b.row1)&&(a.row2==b.row2)&&(a.row3==b.row3);
}

inline THMatrix33 operator +(const THMatrix33& a,const THMatrix33& b)
{
	return THMatrix33(a.row1+b.row1,a.row2+b.row2,a.row3+b.row3);
}
inline THMatrix33 operator -(const THMatrix33& a,const THMatrix33& b)
{
	return THMatrix33(a.row1-b.row1,a.row2-b.row2,a.row3-b.row3);
}
inline THMatrix33 operator *(const THMatrix33& m,float a)
{
	return THMatrix33(m.row1*a,m.row2*a,m.row3*a);
}
inline THMatrix33 operator *(float a,const THMatrix33& m)
{
	return THMatrix33(m.row1*a,m.row2*a,m.row3*a);
}
inline THVector3 operator *(const THMatrix33& m,const THVector3& v)
{
	return THVector3(THDot(m.row1,v),THDot(m.row2,v),THDot(m.row3,v));
}
static THMatrix33 operator *(const THMatrix33& a,const THMatrix33& b)
{
	const THVector3 c1(b.row1.x,b.row2.x,b.row3.x);
	const THVector3 c2(b.row1.y,b.row2.y,b.row3.y);
	const THVector3 c3(b.row1.z,b.row2.z,b.row3.z);

	return THMatrix33(
		THVector3( THDot(a.row1,c1) , THDot(a.row1,c2) , THDot(a.row1,c3) ),
		THVector3( THDot(a.row2,c1) , THDot(a.row2,c2) , THDot(a.row2,c3) ),
		THVector3( THDot(a.row3,c1) , THDot(a.row3,c2) , THDot(a.row3,c3) )
		);
}


#endif
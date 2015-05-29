#include "THMath.h"
#include "THMath3D.h"
#include <THPrivate.h>
#include <stdlib.h>
#include <time.h>

//For Random
double InvRandomMax=0.0;
//randomize random field
void Randomize()
{
	unsigned int rv=(unsigned int)time(0);
	unsigned int cvp=0;
	unsigned int cvm=1;
	while(rv)
	{
		cvp+=rv;
		rv=rv>>1;
		cvm*=rv?rv:1;
	}
	srand( (unsigned int)((cvp>>1) + (cvm>>1)) );


	extern double InvRandomMax;
	InvRandomMax=1.0/RAND_MAX;
}
double THRand01()
{ return rand()*InvRandomMax; }
double THRand(double a,double b)
{ return a+(b-a)*THRand01(); }
float THRandf(float a,float b)
{ return a+(float)((b-a)*THRand01()); }
int THRandi(int a,int b)
{ return a+(int)((b-a)*THRand01()); }

float THInvSqrt(float x)
{
	union 
	{
		float f;
		int i;
	} v;
	v.f=x;
	const float xhalf = 0.5f*x;

	v.i=0x5f375a86-(v.i>>1);
	return v.f*(1.5f-xhalf*v.f*v.f); // Newton step, repeating increases accuracy
}
float THInvSqrt2(float x)
{
	union 
	{
		float f;
		int i;
	} v;
	v.f=x;
	const float xhalf = 0.5f*x;

	v.i=0x5f375a86-(v.i>>1);
	v.f=v.f*(1.5f-xhalf*v.f*v.f);
	return v.f*(1.5f-xhalf*v.f*v.f); // Newton step, repeating increases accuracy
}

THMatrix33 THMatrix33::RotateAxis(const THVector3& axis,const THRot2& rot)
{
	  const THMatrix33 ux
		  (
		  THVector3(0.0f , -axis.z , axis.y),
		  THVector3(axis.z , 0.0f , -axis.x),
		  THVector3(-axis.y , axis.x , 0.0f)
		  );
	  const THMatrix33 uxu
		  (
		  axis.x*axis,
		  axis.y*axis,
		  axis.z*axis
		  );
	  const THMatrix33 Ident;

	  return (rot.x*Ident) + (rot.y*ux) + ((1.0f-rot.x)*uxu);
}
THMatrix33 THMatrix33::RotatePoint(const THVector3& p1,const THVector3& p2)
{
	if(p1==p2)
	{
		return THMatrix33();
	}else if(p1==-p2)
	{
		THError("Cannot get axis from negative vectors");
		return THMatrix33();
	}

	const float leni=1.0f/(p1.Length()*p2.Length());
	THVector3 axis=THCross(p1,p2);
	const float c=THDot(p1,p2)*leni;
	const float s=axis.Normalize()*leni;

	return THMatrix33::RotateAxis(axis,THRot2(c,s));
}
// yrot : yaw   xrot : pitch
THMatrix33 THMatrix33::EyeTrnsformMatrix(const THRot2& yrot,const THRot2& xrot,THVector3* eyenorm)
{
	if(eyenorm)
	{
		eyenorm->x=xrot.x*yrot.y;
		eyenorm->y=-xrot.y;
		eyenorm->z=-xrot.x*yrot.x;
	}
	return THMatrix33
		(
		THVector3(yrot.x , 0.0f , yrot.y),
		THVector3(xrot.y*yrot.y , xrot.x , -xrot.y*yrot.x),
		THVector3(-xrot.x*yrot.y , xrot.y , xrot.x*yrot.x)
		);
}
void THOrthoMatrix33(float *mat,const THVector2& minp,const THVector2& maxp)
{
	const THVector2 sizeI=1.0f/(maxp-minp);
	const THVector2 midI=(minp+maxp)*sizeI;
	/*
	sizeI.x*2.0f , 0.0f , -mid.x*sizeI.x,
		0.0f , 2.0f*sizeI.y , -mid.y*sizeI.y
		*/
	mat[0]=sizeI.x*2.0f;	mat[1]=0.0f;			mat[2]=-midI.x;
	mat[3]=0.0f;			mat[4]=sizeI.y*2.0f;	mat[5]=-midI.y;
}
void THPerspectiveMatrix44(float* mat,const THVector3& min,const THVector3& max)
{
	const THVector3 sizeI=1.0f/(max-min);
	const THVector3 mmid=(max+min)*sizeI;

	mat[0]=2.0f*min.z*sizeI.x;	mat[1]=0.0f;				mat[2]=mmid.x;			mat[3]=0.0f;
	mat[4]=0.0f;				mat[5]=2.0f*min.z*sizeI.y;	mat[6]=mmid.y;			mat[7]=0.0f;
	mat[8]=0.0f;				mat[9]=0.0f;				mat[10]=mmid.z;			mat[11]=2.0f*sizeI.z;
	mat[12]=0.0f;				mat[13]=0.0f;				mat[14]=-1.0f;			mat[15]=0.0f;
}

float THMatrix33::Discriminant() const
{
	return row1.x*(row2.y*row3.z - row2.z*row3.y) - row1.y*(row2.x*row3.z - row2.z*row3.x) + row1.z*(row2.x*row3.y - row2.y*row3.x);
}
THMatrix33 THMatrix33::Inverse() const
{
	const THVector3 c12=THCross(row1,row2);
	const THVector3 c13=THCross(row1,row3);
	const THVector3 c23=THCross(row2,row3);
	const float det=1.0f/Discriminant();

	return THMatrix33(
		THVector3( c23.x*det , -c13.x*det , c12.x*det ),
		THVector3( c23.y*det , -c13.y*det , c12.y*det ),
		THVector3( c23.z*det , -c13.z*det , c12.z*det )
		);
}
THVector3 THMatrix33::Solve(const THVector3& v,float invD) const
{
	const THVector3 c12=THCross(row1,row2);
	const THVector3 c23=THCross(row2,row3);
	const THVector3 c31=THCross(row3,row1);

	return THVector3
		(
		invD*(c23.x*v.x + c31.x*v.y + c12.x*v.z),
		invD*(c23.y*v.x + c31.y*v.y + c12.y*v.z),
		invD*(c23.z*v.x + c31.z*v.y + c12.z*v.z)
		);
}
THVector3 THMatrix33::Solve(const THVector3& v) const
{
	return Solve(v,1.0f/Discriminant());
}
void GetCirclePolygonVertices(THVector2* arr,int count,float radius)
{
	THVector2 last(radius,0.0f);

	const float angle=TH_2PI/(float)count;
	const THRot2 rot(cosf(angle),sinf(angle));
	arr[0]=last;
	for(int i=1;i<count;++i)
	{
		last=rot*last;
		arr[i]=last;
	}
}
#include "THMath.h"
#include "THMath3D.h"
#include <THPrivate.h>

//For Random
extern double InvRandomMax=0.0;

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

	  return (rot.c*Ident) + (rot.s*ux) + ((1.0f-rot.c)*uxu);
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
THMatrix33 THMatrix33::EyeTrnsformMatrix(const THRot2& yrot,const THRot2& xrot)
{
	return THMatrix33
		(
		THVector3(yrot.c , 0.0f , yrot.s),
		THVector3(xrot.s*yrot.s , xrot.c , -xrot.s*yrot.c),
		THVector3(-xrot.c*yrot.s , xrot.s , xrot.c*yrot.c)
		);
}
void EyeTrnsformMatrix(float* mat,const THRot2& yrot,const THRot2& xrot)
{
	mat[0]=yrot.c;	mat[1]=0.0f;	mat[2]=yrot.s;
	mat[3]=xrot.s*yrot.s;	mat[4]=xrot.c;	mat[5]=-xrot.s*yrot.c;
	mat[6]=-xrot.c*yrot.s;	mat[7]=xrot.s;  mat[8]=xrot.c*yrot.c;
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


void GetNormals(const THVector2 *points,THVector2* normals,unsigned int count)
{
	for(unsigned int i=0;i<count-1;++i)
	{
		THVector2 vect=points[i+1]-points[i];
		vect.Normalize();
		vect.Skew();

		normals[i]=vect;
	}

	THVector2 vect=points[0]-points[count-1];
	vect.Normalize();
	vect.Skew();

	normals[count-1]=vect;
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
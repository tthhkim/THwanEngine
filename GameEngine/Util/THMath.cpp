#include "THMath.h"
#include "THMath3D.h"
#include <THPrivate.h>

//For Random
extern double InvRandomMax=0.0;

THMatrix33 THMatrix33::RotateAxis(const THVector3& axis,float c,float s)
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

	  return (c*Ident) + (s*ux) + ((1.0f-c)*uxu);
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

	return THMatrix33::RotateAxis(axis,c,s);
}
void EyeTrnsformMatrix(float* mat,const THVector2& yrot,const THVector2& xrot)
{
	mat[0]=yrot.x;	mat[1]=0.0f;	mat[2]=yrot.y;
	mat[3]=xrot.y*yrot.y;	mat[4]=xrot.x;	mat[5]=-xrot.y*yrot.x;
	mat[6]=-xrot.x*yrot.y;	mat[7]=xrot.y;  mat[8]=xrot.x*yrot.x;
}
void THOrthoMatrix44(float* mat,const THVector3& min,const THVector3& max)
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

	const THVector2 rotv=THAngleVector(TH_2PI/(float)count);
	const THRot rot(rotv.x,rotv.y);
	for(int i=0;i<count-1;++i)
	{
		arr[i]=last;
		last=rot*last;
	}
	arr[count-1]=last;
}
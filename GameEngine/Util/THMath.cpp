#include "THMath.h"
#include "THMath3D.h"

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
float THMatrix33::Discriminant() const
{
	return row1.x*(row2.y*row3.z - row2.z*row3.y) - row1.y*(row2.x*row3.z - row2.z*row3.x) + row1.z*(row2.x*row3.y - row2.y*row3.x);
}
THMatrix33 THMatrix33::Inverse() const
{
	const THVector3& c12=THCross(row1,row2);
	const THVector3& c13=THCross(row1,row3);
	const THVector3& c23=THCross(row2,row3);
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

void GetScaledVertices(const THVector2 *points,THVector2 *out,unsigned int count,const THVector2& min,const THVector2& max)
{
	const THVector2 sizeI=2.0f/(max-min);
	const THVector2 offs=(min+max)*sizeI*-0.5f;

	for(unsigned int i=0;i<count;++i)
	{
		out[i]=(points[i]*sizeI)+offs;
	}
}
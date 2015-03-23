#include "THChar.h"
#include <THPrivate.h>

void THString::Refresh()
{
	if(m_children.num>0)
	{
		for(unsigned int i=0;i<m_children.num;++i)
		{
			delete m_children.arr[i];
		}
		m_children.Clear();
	}
}
void THString::SetString(const THChar** charArr)
{
	Refresh();
	m_length=0.0f;
	unsigned int i=0;

	while(charArr[i]!=0)
	{
		THDisplayObject *obj=new THDisplayObject;
		obj->texture=charArr[i];
		obj->position.x=m_scale*m_length;
		AddChild(obj);


		m_length+=charArr[i]->Length();
		++i;
	}
	m_length*=m_scale;
}
void THString::SetScale(float s)
{
	const float scalefactor=s/m_scale;
	m_scale=s;
	for(unsigned int i=0;i<m_children.num;++i)
	{
		m_children.arr[i]->position.x*=scalefactor;
	}
	m_scale*=scalefactor;
}
#include "THShadowEffect.h"
#include <GameEngine/DisplayObject/THDisplayObject.h>

void THShadowEffect::SetOrtho(const THVector2& minp,const THVector2& maxp)
{
	const THVector2 sizeI=1.0f/(maxp-minp);
	const THVector2 mid=(minp+maxp);
	const GLfloat matx[]={
		sizeI.x*2.0f , 0.0f , -mid.x*sizeI.x,
		0.0f , 2.0f*sizeI.y , -mid.y*sizeI.y
	};

	glUniform3fv(program.GetUniformLocation("ndcMat"),2,matx);
}
void THShadowEffect::Load(const THVector2& minp,const THVector2& maxp)
{
	const GLchar* vs=
		"precision mediump float;"
		"uniform vec3 ndcMat[2];"
		"uniform vec3 lightPos;"
		"attribute vec2 vert;"
		"attribute vec3 pos;"
		"attribute vec2 rot;"
		"attribute vec2 scale;"
		"attribute vec2 aTex;"
		"varying vec2 vTex;"
		"void main(){"
		"vTex=aTex;"
		"vec2 svert=vert*scale;"
		"vec2 cp=vec2( dot(svert, vec2(rot.x,-rot.y) ) , dot(svert, vec2(rot.y,rot.x) ))+pos.xy;"
		"vec2 lxy=lightPos.xy;"
		"if(pos.z==lightPos.z){return;}"
		"vec2 lp=lxy + ((-lightPos.z/(pos.z-lightPos.z))*(cp-lxy));"
		"vec3 np=vec3(lp,1.0);"
		"gl_Position=vec4(  dot(ndcMat[0],np) , dot(ndcMat[1],np)  , 0.0 , 1.0);"
		"}"
		;
	const GLchar* fs=
		"precision mediump float;"
		"uniform vec4 shadowColor;"
		"uniform sampler2D sTexture;"
		"varying vec2 vTex;"
		"void main(){"
		"float talpha=texture2D(sTexture,vTex).a;"
		"gl_FragColor=vec4(shadowColor.rgb,shadowColor.a*talpha);"
		"}"
		;

	program.Load(vs,fs);

	lightPositionHandler=program.GetUniformLocation("lightPos");

	vertexHandler=program.GetAttribLocation("vert");
	textureHandler=program.GetAttribLocation("aTex");
	positionHandler=program.GetAttribLocation("pos");
	rotationHandler=program.GetAttribLocation("rot");
	scaleHandler=program.GetAttribLocation("scale");

	glEnableVertexAttribArray(textureHandler);

	SetOrtho(minp,maxp);
}
void THShadowEffect::Draw()
{
	program.Use();

	for(unsigned int i=0;i<objects.num;++i)
	{
		const THShadowObject& so=objects.arr[i];
		
		glBindTexture(GL_TEXTURE_2D,so.mc->texture->image->textureID);

		const THVector2& pos=so.mc->GetWorldPosition();
		glVertexAttrib3f(positionHandler,pos.x,pos.y,so.z);
		glVertexAttrib2fv(rotationHandler,(const GLfloat*)&so.mc->rotation);
		glVertexAttrib2fv(scaleHandler,(const GLfloat*)&so.mc->size);

		
		glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,so.mc->texture->textureBuffer);
		THHalfVertices.BeginDrawing(vertexHandler);
		glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,0);

		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		THHalfVertices.EndDrawing(vertexHandler);
	}
}

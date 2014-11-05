#include "THShadowEffect.h"
#include <GameEngine/DisplayObject/THDisplayObject.h>

void THShadowEffect::SyncProjection()
{
	glUniform3fv(program.GetUniformLocation("ndcMat"),2,THProjectMatrix);
}
void THShadowEffect::Load()
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

	glEnableVertexAttribArray(vertexHandler);
	glEnableVertexAttribArray(textureHandler);

	SyncProjection();
	//SetOrtho(minp,maxp);
}
void THShadowEffect::Draw() const
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

		if(so.mc->vertexBuffer==0){ THHalfVertices.BeginDrawing(); }

		glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,so.mc->vertexBuffer);

		glDrawArrays(GL_TRIANGLE_STRIP,0,4);

		if(so.mc->vertexBuffer==0){ THHalfVertices.EndDrawing(); }
		
	}
}

void THShadowBlurEffect::Load(THTexture *src)
{
	const GLchar* vs=
		"precision mediump float;"
		"attribute vec2 vert;"
		"attribute vec2 aTex;"
		"varying vec2 vTex;"
		"attribute vec2 aDir;"
		"varying vec2 vDir;"
		"void main(){"
		"vTex=aTex;"
		"vDir=aDir;"
		"gl_Position=vec4(vert,0.0,1.0);"
		"}"
		;
	const GLchar* fs=
		"precision mediump float;"
		"uniform vec2 textureInvert;"
		"uniform float radiusi;"
		"uniform lowp int stepCount;"
		"uniform sampler2D sTexture;"
		
		"varying vec2 vTex;"
		"varying vec2 vDir;"
		"void main(){"

		"vec4 cSum=texture2D(sTexture,vTex)*radiusi;"

		"lowp int i=1;"
		"while(true){"
		"if(i==stepCount){break;}"

		"float len=float(i);"
		"float kern=radiusi-len*radiusi*radiusi;"

		"vec2 ccoord=len*vDir*textureInvert;"
		"cSum+=kern*texture2D(sTexture,vTex+ccoord);"
		"cSum+=kern*texture2D(sTexture,vTex-ccoord);"

		"++i;"
		"}"
		

		"gl_FragColor=cSum;"
		"}"
		;

	program.Load(vs,fs);

	vertexHandler=program.GetAttribLocation("vert");
	textureHandler=program.GetAttribLocation("aTex");
	directionHandler=program.GetAttribLocation("aDir");

	radiusHandler=program.GetUniformLocation("radiusi");
	stepHandler=program.GetUniformLocation("stepCount");

	SetTexture(src);

	glEnableVertexAttribArray(vertexHandler);
	glEnableVertexAttribArray(textureHandler);
}
void THShadowBlurEffect::Draw() const
{
	program.Use();

	glBindTexture(GL_TEXTURE_2D,srcTexture->image->textureID);

	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,vertex);
	glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,srcTexture->textureBuffer);
	glVertexAttrib2fv(directionHandler,(const GLfloat*)&direction);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
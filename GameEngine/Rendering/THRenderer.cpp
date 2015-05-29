#include <THPrivate.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <GameEngine/DisplayObject/THDisplayObject.h>
#include <GameEngine/DisplayObject/THFrame.h>

#include <GameEngine/Util/THMath.h>
#include <GameEngine/Util/THArray.h>

#include <stdlib.h>



//extern THDefProgram THDefaultProgram=THDefProgram();

/*
void THDefProgram::Load()
{
	const GLchar* vs=
			"precision mediump float;"
			"uniform vec3 projMat[2];"
			"attribute vec2 aVert;"
			"attribute vec2 aRot;"
			"attribute vec2 aScale;"
			"attribute vec2 aPos;"
			"attribute vec2 aCenter;"
			"attribute vec4 aTex;"

			"varying vec2 vTex;"
			"void main(){"

			"vec2 svert=(aVert-aCenter)*aScale;"
			"vec2 rp=vec2( dot(vec2(aRot.x,-aRot.y),svert) , dot(vec2(aRot.y,aRot.x),svert) );"
			"vec3 rrp=vec3(rp + aPos , 1.0);"
			"gl_Position=vec4(dot(projMat[0],rrp),dot(projMat[1],rrp),0.0,1.0);"

			"vTex=aVert*aTex.zw + aTex.xy;"
			"}";

	const GLchar* fs=
			"precision mediump float;"
			"varying vec2 vTex;"
			"uniform sampler2D sTexture;"
			"uniform vec4 aColor;"
			"uniform vec4 mColor;"
			"void main(){"
			"gl_FragColor=texture2D(sTexture,vTex)*mColor + aColor;"
			"}";
	THProgram::Load(vs,fs);

	vertexHandler=GetAttribLocation("aVert");
	rotationHandler=GetAttribLocation("aRot");
	scaleHandler=GetAttribLocation("aScale");
	positionHandler=GetAttribLocation("aPos");
	textureHandler=GetAttribLocation("aTex");
	centerHandler=GetAttribLocation("aCenter");

	projectMatrixHandler=GetUniformLocation("projMat");
	colorAddHandler=GetUniformLocation("aColor");
	colorMultiplyHandler=GetUniformLocation("mColor");
}
*/
#include "THEffects.h"

void THLightEffect::Load(THTexture* dest,THTexture* src)
{
	destTexture=dest;
	srcTexture=src;

	const GLchar* vs=
		"precision mediump float;"
		"attribute vec2 vert;"
		"attribute vec2 aDestTex;"
		"attribute vec2 aSrcTex;"
		"varying vec2 vDestTex;"
		"varying vec2 vSrcTex;"
		"void main(){"
		"vDestTex=aDestTex;"
		"vSrcTex=aSrcTex;"
		"gl_Position=vec4(vert,0.0,1.0);"
		"}"
		;
	const GLchar* fs=
		"precision mediump float;"
		"uniform vec3 ambient;"
		"uniform float lightAmpli;"
		"uniform sampler2D sDestTex;"
		"uniform sampler2D sSrcTex;"
		"varying vec2 vDestTex;"
		"varying vec2 vSrcTex;"
		"void main(){"
		"vec3 lColor=texture2D(sSrcTex,vSrcTex).rgb;"
		"vec4 dColor=texture2D(sDestTex,vDestTex);"
		"gl_FragColor=vec4(dColor.rgb*(ambient +(lightAmpli*lColor)),dColor.a);"
		"}"
		;
	program.Load(vs,fs);

	vertexHandler=program.GetAttribLocation("vert");
	destHandler=program.GetAttribLocation("aDestTex");
	srcHandler=program.GetAttribLocation("aSrcTex");
	amplitudeHandler=program.GetUniformLocation("lightAmpli");

	glUniform1i(program.GetUniformLocation("sDestTex"),0);
	glUniform1i(program.GetUniformLocation("sSrcTex"),1);

	glEnableVertexAttribArray(vertexHandler);
	glEnableVertexAttribArray(destHandler);
	glEnableVertexAttribArray(srcHandler);
}

void THLightEffect::Draw() const
{
	program.Use();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,srcTexture->image->textureID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,destTexture->image->textureID);
	

	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,vertex);
	glVertexAttribPointer(destHandler,2,GL_FLOAT,GL_FALSE,0,destTexture->textureBuffer);
	glVertexAttribPointer(srcHandler,2,GL_FLOAT,GL_FALSE,0,srcTexture->textureBuffer);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void THBlurEffect::Load(THTexture* src)
{
	srcTexture=src;

	const GLchar* vs=
		"precision mediump float;"
		"attribute vec2 vert;"
		"attribute vec2 aTex;"
		"varying vec2 vTex;"
		"void main(){"
		"vTex=aTex;"
		"gl_Position=vec4(vert,0.0,1.0);"
		"}"
		;
	const GLchar* fs=
		"precision mediump float;"
		"uniform float weights[20];" //weights max 20steps
		"uniform lowp int stepCount;" //counts of steps max 20
		"uniform vec2 dir;"  //blur direction
		"uniform vec2 blur;" // radius / textureResolution
		"uniform lowp int blurOppos;" // wheather blur opposite side
		"uniform sampler2D sTexture;"
		
		"varying vec2 vTex;"
		"void main(){"

		"vec4 cSum=texture2D( sTexture , vTex)*weights[0];"

		"lowp int i;"
		
		"if(blurOppos==1){"

		"for(i=1;i<=12;++i){"
		"if(i>stepCount){break;}"
		"cSum += texture2D( sTexture , vTex - (float(i)*blur*dir) ) * (2.0*weights[i]);"
		"}"

		"}else{"

		"for(i=1;i<=12;++i){"
		"if(i>stepCount){break;}"
		"cSum += texture2D( sTexture , vTex - (float(i)*blur*dir) ) * weights[i];"
		"cSum += texture2D( sTexture , vTex + (float(i)*blur*dir) ) * weights[i];"
		"}"

		"}"

		"gl_FragColor=cSum;"
		"}"
		;
	program.Load(vs,fs);

	vertexHandler=program.GetAttribLocation("vert");
	textureHandler=program.GetAttribLocation("aTex");
	directionHandler=program.GetUniformLocation("dir");
	blurOppositeHandler=program.GetUniformLocation("blurOppos");
	radiusHandler=program.GetUniformLocation("blur");
	glUniform1i(program.GetUniformLocation("stepCount"),stepCount);


//-------Calculating weights
	float weights[20];

	const float ci=1.0f/GAUSSIAN_SIGMA_SQUARE;
	const float xstep=GAUSSIAN_ZERO_X_VALUE/(float)(stepCount);

	float sum=0.0f;
	weights[0]=1.0f;
	float x=0.0f;
	for(int i=1;i<stepCount;++i)
	{
		x+=xstep;
		weights[i]=expf(-x*x*ci);
		sum+=weights[i];
	}
	sum*=2.0f;
	sum+=1.0f;
	const float sumi=1.0f/sum;
	for(int i=0;i<stepCount;++i)
	{
		weights[i]*=sumi;
	}

	glUniform1fv(program.GetUniformLocation("weights"),stepCount,weights);

	glEnableVertexAttribArray(vertexHandler);
	glEnableVertexAttribArray(textureHandler);
}

void THBlurEffect::Draw() const
{
	program.Use();

	glBindTexture(GL_TEXTURE_2D,srcTexture->image->textureID);

	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,vertex);
	glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,srcTexture->textureBuffer);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void THLightBaseCircle::Load(const THVector2& resolution)
{
	const GLchar* vs=
		"precision mediump float;"
		"uniform vec2 cPosition;"
		"uniform float lRadius;"
		"uniform vec2 ndcMat;"
		"attribute vec2 vert;"
		"varying vec2 vVert;"
		"void main(){"
		"vec2 np=(vert*lRadius)+cPosition;"
		"vVert=np;"
		"gl_Position=vec4(ndcMat.x*np.x - 1.0, ndcMat.y*np.y - 1.0,0.0,1.0);"
		"}"
		;
	const GLchar* fs=
		"precision mediump float;"
		"uniform vec2 cPosition;"
		"uniform float cRadius;"
		"uniform float lRadius;"
		"uniform float lRadiusi;"
		"uniform float lcGapi;"
		"varying vec2 vVert;"
		"void main(){"
		"vec2 light2vert=(vVert-cPosition)*lRadiusi;"
		"float dpos=length(light2vert);"
		"if(dpos>1.0){return;}"
		"if(dpos<=cRadius*lRadiusi){gl_FragColor=vec4(1.0,1.0,1.0,1.0);return;}"
		"if(cRadius==lRadius){return;}"
		"float factor=1.0-(dpos*lRadius-cRadius)*lcGapi;"
		"gl_FragColor=vec4(factor,factor,factor,1.0);"
		"}"
		;
	program.Load(vs,fs);

	const THVector2 sci=2.0f/resolution;
	program.SetUniform("ndcMat",sci.x,sci.y);

	positionHandler=program.GetUniformLocation("cPosition");
	vertexHandler=program.GetAttribLocation("vert");

	glEnableVertexAttribArray(vertexHandler);
}
void THLightBaseCircle::Draw()
{
	program.Use();
	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,defaultFullVertices);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void THSwirlEffect::Load(THTexture* src)
{
	const GLchar* vs=
		"precision mediump float;"
		"attribute vec2 vert;"
		"attribute vec2 aTex;"
		"varying vec2 vTexCoord;"
		"void main(){"
		"vTexCoord=aTex;"
		"gl_Position=vec4(vert,0.0,1.0);"
		"}"
		;
	const GLchar* fs=
		"precision mediump float;"
		"uniform vec2 cPosition;"
		"uniform float cAngle;"
		"uniform vec2 cRadius;"//radius,radiusInvert
		"uniform float angleCoeff;"
		"uniform vec4 textureInfo;"//x,y,size
		"uniform vec2 textureInverted;"
		"uniform sampler2D sTexture;"
		"varying vec2 vTexCoord;"
		"void main(){"
		"vec2 cCoord=(vTexCoord*textureInfo.zw) - textureInfo.xy;"
		"vec2 c2v=(cCoord-cPosition)*cRadius[1];"
		"float dpos=dot(c2v,c2v);"
		"if(dpos>1.0){gl_FragColor=texture2D(sTexture,vTexCoord);return;}"
		"dpos=(1.0-sqrt(dpos));"
		"float cangle=cAngle*dpos*dpos*angleCoeff;"
		"float tcos=cos(cangle);float tsin=sin(cangle);"
		"vec2 np=vec2(dot(c2v,vec2(tcos,-tsin)),dot(c2v,vec2(tsin,tcos)))*cRadius[0];"
		"gl_FragColor=texture2D(sTexture,(np+cPosition)*textureInverted);"
		"}"
		;
	program.Load(vs,fs);

	SetTexture(src);

	rotationHandler=program.GetUniformLocation("cAngle");
	SetCoeff(8.0f);

	vertexHandler=program.GetAttribLocation("vert");
	textureHandler=program.GetAttribLocation("aTex");

	glEnableVertexAttribArray(vertexHandler);
	glEnableVertexAttribArray(textureHandler);
}

void THSwirlEffect::Draw()
{
	program.Use();

	glBindTexture(GL_TEXTURE_2D,srcTexture->image->textureID);
	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,vertex);
	glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,srcTexture->textureBuffer);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void THLinearWaveEffect::Load(THTexture* src)
{
	srcTexture=src;

	const GLchar* vs=
		"precision mediump float;"
		"attribute vec2 vert;"
		"attribute vec2 aTex;"
		"varying vec2 vTexCoord;"
		"void main(){"
		"vTexCoord=aTex;"
		"gl_Position=vec4(vert,0.0,1.0);"
		"}"
		;
	const GLchar* fs=
		"precision mediump float;"
		"uniform float time;" // currentTime(sec)
		"uniform vec2 dir;"
		"uniform vec2 dirskew"
		"uniform float waveAmpli;"
		"uniform float waveFreq;"
		"uniform float waveVeli;"
		"uniform vec2 textureInfo;"//size
		"uniform sampler2D sTexture;"
		"varying vec2 vTexCoord;"
		"void main(){"
		"vec2 cCoord=vTexCoord*textureInfo;"
		"float dp=dot(dir,cCoord);"
		"float factor=waveAmpli*sin(6.2831853*waveFreq*(time-dp*waveVeli));"
		"gl_FragColor=texture2D( sTexture , vTexCoord + (dirskew*factor));"
		"}"
		;
	program.Load(vs,fs);

	timeHandler=program.GetUniformLocation("time");
	directionHandler=program.GetUniformLocation("dir");
	directionSkew=program.GetUniformLocation("dirskew");

	const THVector2 ts=src->GetSize()*0.01f;
	program.SetUniform("textureInfo",ts.x,ts.y);

	vertexHandler=program.GetAttribLocation("vert");
	textureHandler=program.GetAttribLocation("aTex");

	glEnableVertexAttribArray(vertexHandler);
	glEnableVertexAttribArray(textureHandler);
}

void THLinearWaveEffect::Draw(float dt)
{
	time+=dt;
	if(time>=freqI){time-=freqI;}
	program.Use();

	glBindTexture(GL_TEXTURE_2D,srcTexture->image->textureID);
	glUniform1f(timeHandler,time);
	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,vertex);
	glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,srcTexture->textureBuffer);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void THAngularWaveEffect::Load(THTexture* src)
{
	srcTexture=src;

	const GLchar* vs=
		"precision mediump float;"
		"attribute vec2 vert;"
		"attribute vec2 aTex;"
		"varying vec2 vTexCoord;"
		"void main(){"
		"vTexCoord=aTex;"
		"gl_Position=vec4(vert,0.0,1.0);"
		"}"
		;
	//scaled : center,
	const GLchar* fs=
		"precision mediump float;"
		"uniform float time;"
		"uniform float amplitude;"
		"uniform float angularVel;"
		"uniform float frequency;"
		"uniform vec2 center;"
		"uniform vec4 textureInfo;"//x,y,size
		"uniform vec2 textureInverted;"
		"uniform sampler2D sTexture;"
		"varying vec2 vTexCoord;"
		"void main(){"
		"vec2 cCoord=(vTexCoord*textureInfo.zw)-textureInfo.xy;"
		"vec2 deltaP=(cCoord-center);"
		"float theta=atan(deltaP.y,deltaP.x);"
		"float factor=sin(theta*frequency+time*angularVel)*amplitude + 1.0;"
		"vec2 scaledP=deltaP*factor + center;"
		"gl_FragColor=texture2D(sTexture,scaledP*textureInverted);"
		"}"
		;
	program.Load(vs,fs);

	timeHandler=program.GetUniformLocation("time");

	const THVector2 ts=src->image->size*0.01f;
	const THVector2 tp=src->GetPosition()*0.01f;
	program.SetUniform("textureInfo",tp.x,tp.y,ts.x,ts.y);
	program.SetUniform("textureInverted",1.0f/ts.x,1.0f/ts.y);

	vertexHandler=program.GetAttribLocation("vert");
	textureHandler=program.GetAttribLocation("aTex");

	glEnableVertexAttribArray(vertexHandler);
	glEnableVertexAttribArray(textureHandler);
}

void THAngularWaveEffect::Draw(float dt)
{
	time+=dt;
	if(time>=timeLimit){time-=timeLimit;}
	program.Use();

	glBindTexture(GL_TEXTURE_2D,srcTexture->image->textureID);
	glUniform1f(timeHandler,time);
	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,vertex);
	glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,srcTexture->textureBuffer);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void THShockWaveEffect::Load(THTexture* src)
{
	srcTexture=src;

	const GLchar* vs=
		"precision mediump float;"
		"attribute vec2 vert;"
		"attribute vec2 aTex;"
		"varying vec2 vTexCoord;"
		"void main(){"
		"vTexCoord=aTex;"
		"gl_Position=vec4(vert,0.0,1.0);"
		"}"
		;
	const GLchar* fs=
		"precision mediump float;"
		"uniform vec2 sPosition;"
		"uniform float time;" // currentTime(sec) * pixel per sec
		"uniform vec2 waveWidth;" //width, 1.0/width
		"uniform float waveHeight;"
		"uniform float lightCoeff;"
		"uniform lowp int isOn;"
		"uniform vec4 textureInfo;"//x,y,size
		"uniform vec2 textureInverted;"
		"uniform sampler2D sTexture;"
		"varying vec2 vTexCoord;"
		"void main(){"
		"if(isOn!=1){gl_FragColor=texture2D(sTexture,vTexCoord);return;}"
		"vec2 cCoord=(vTexCoord*textureInfo.zw) - textureInfo.xy;"
		"vec2 s2v=(cCoord-sPosition)*waveWidth[1];"
		"float dpos=length(s2v);"
		"float ecart=dpos-time*waveWidth[1];" // -waveWidth ~ +waveWidth
		"if(ecart<-1.0 || ecart>1.0){gl_FragColor=texture2D(sTexture,vTexCoord);return;}"
		"float factor=ecart*(1.0-pow(abs(ecart),0.4));"
		"ecart=waveHeight*waveWidth[0]*factor;"
		"vec4 cCol=texture2D(sTexture,vTexCoord+(s2v/dpos*ecart*textureInverted));"
		"gl_FragColor=vec4(cCol.rgb*(1.0+lightCoeff*factor),cCol.a);"
		//"gl_FragColor=cCol;"
		"}"
		;
	program.Load(vs,fs);

	const THVector2 tp=src->GetPosition();
	glUniform4f(program.GetUniformLocation("textureInfo"),tp.x,tp.y,src->image->size.x,src->image->size.y);
	glUniform2f(program.GetUniformLocation("textureInverted"),1.0f/src->image->size.x,1.0f/src->image->size.y);

	positionHandler=program.GetUniformLocation("sPosition");
	timeHandler=program.GetUniformLocation("time");
	isOnHandler=program.GetUniformLocation("isOn");

	vertexHandler=program.GetAttribLocation("vert");
	textureHandler=program.GetAttribLocation("aTex");

	glEnableVertexAttribArray(vertexHandler);
	glEnableVertexAttribArray(textureHandler);
}
void THShockWaveEffect::Draw(float dt)
{
	program.Use();
	shockTime+=dt;
	SetTime(shockTime);
	
	glBindTexture(GL_TEXTURE_2D,srcTexture->image->textureID);
	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,defaultFullVertices);
	glVertexAttribPointer(textureHandler,2,GL_FLOAT,GL_FALSE,0,srcTexture->textureBuffer);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


#include "FluidShader.h"

void THFluidShader::Load()
{
	LoadFile("D:/Resources/Games/Fluid/fluidshade.vs","D:/Resources/Games/Fluid/fluidshade.fs");

	vertexHandler=GetAttribLocation("vert");
	velocityHandler=GetAttribLocation("aVel");
	positionHandler=GetAttribLocation("aPos");
	isVelHandler=GetAttribLocation("aIsVel");
	aSizeHandler=GetAttribLocation("a_sizefactor");

	sizeHandler=GetUniformLocation("u_size");
	tailSizeHandler=GetUniformLocation("u_tail");
}

void THFluidShader::PreDraw(const THImage& circleImage) const
{
	glBindTexture(GL_TEXTURE_2D,circleImage.textureID);

	glEnableVertexAttribArray(vertexHandler);

	glVertexAttrib1f(aSizeHandler,1.0f);

	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,0);
}
void THFluidShader::PostDraw() const
{
	glDisableVertexAttribArray(vertexHandler);
}



void THFluidRenderShader::Load()
{
	LoadFile("D:/Resources/Games/Fluid/fluidrender.vs","D:/Resources/Games/Fluid/fluidrender.fs");

	vertexHandler=GetAttribLocation("vert");

	thresholdHandler=GetUniformLocation("u_threshold");
	alphaHandler=GetUniformLocation("u_alpha");
	colorHandler=GetUniformLocation("u_color");
	outColorHandler=GetUniformLocation("u_outColor");

	
}
void THFluidRenderShader::Draw(const THImage& fbImage) const
{
	glBindTexture(GL_TEXTURE_2D,fbImage.textureID);

	glEnableVertexAttribArray(vertexHandler);

	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,0);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	glDisableVertexAttribArray(vertexHandler);
}
void THSteamRenderShader::Load()
{
	LoadFile("D:/Resources/Games/Fluid/fluidrender.vs","D:/Resources/Games/Fluid/steamrender.fs");

	vertexHandler=GetAttribLocation("vert");
	alphaHandler=GetUniformLocation("u_alpha");
}
void THSteamRenderShader::Draw(const THImage& fbImage) const
{
	glBindTexture(GL_TEXTURE_2D,fbImage.textureID);

	glEnableVertexAttribArray(vertexHandler);

	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,0);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	glDisableVertexAttribArray(vertexHandler);
}


void THFluidBaseShader::Load()
{
	LoadFile("D:/Resources/Games/Fluid/fluidbase.vs","D:/Resources/Games/Fluid/fluidbase.fs");

	vertexHandler=GetAttribLocation("vert");
}
void THFluidBaseShader::Draw()
{
	glEnableVertexAttribArray(vertexHandler);
	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,0);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	glDisableVertexAttribArray(vertexHandler);
}
void THBoundaryBaseShader::Load()
{
	LoadFile("D:/Resources/Games/Fluid/fluidbase.vs","D:/Resources/Games/Fluid/boundarybase.fs");

	vertexHandler=GetAttribLocation("vert");
}
void THBoundaryBaseShader::Draw()
{
	glEnableVertexAttribArray(vertexHandler);

	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,0);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	glDisableVertexAttribArray(vertexHandler);
}
void THBoundaryShader::Load()
{
	LoadFile("D:/Resources/Games/Fluid/boundaryshade.vs","D:/Resources/Games/Fluid/boundaryshade.fs");

	vertexHandler=GetAttribLocation("vert");
	positionHandler=GetAttribLocation("aPos");

	sizeHandler=GetUniformLocation("u_size");
}
void THBoundaryRenderShader::Load()
{
	LoadFile("D:/Resources/Games/Fluid/fluidrender.vs","D:/Resources/Games/Fluid/boundaryrender.fs");

	vertexHandler=GetAttribLocation("vert");

	glUniform1i(GetUniformLocation("u_texture"),0);
	glUniform1i(GetUniformLocation("u_gtexture"),1);
}
void THBoundaryRenderShader::Draw(const THImage& fbImage,const THImage& groundTexture) const
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,groundTexture.textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,fbImage.textureID);

	glEnableVertexAttribArray(vertexHandler);
	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,0);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	glDisableVertexAttribArray(vertexHandler);
}

void THLightRenderShader::Load()
{
	LoadFile("D:/Resources/Games/Fluid/fluidrender.vs","D:/Resources/Games/Fluid/lightrender.fs");

	vertexHandler=GetAttribLocation("vert");

	glUniform1i(GetUniformLocation("u_texture"),0);
	glUniform1i(GetUniformLocation("u_lightmap"),1);

	lightcoeffHandler=GetUniformLocation("u_lightcoeff");
}
void THLightRenderShader::Draw(const THImage& frame,const THImage& light)
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,light.textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,frame.textureID);

	glEnableVertexAttribArray(vertexHandler);
	glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,0);
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	glDisableVertexAttribArray(vertexHandler);
}
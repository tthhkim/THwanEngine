#include "THDrawing.h"
#include <THPrivate.h>
#include <assert.h>
#include <malloc.h>
#include <GameEngine/DisplayObject/THFrame.h>

GLuint GenerateTexture(void* data,GLsizei width,GLsizei height,GLenum format)//,const unsigned int isRepeat)
{
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*
	if(isRepeat)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
	*/
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//}
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,format, GL_UNSIGNED_BYTE, data);

	return tex;
}
void THTexture::Set(THImage* _image,const THVector2& pos,const THVector2& _size)
{
	image=_image;
	const THVector2& invSize=1.0f/_size;
	const THVector2& min=pos*invSize;
	const THVector2& max=(pos+_size)*invSize;

	textureBuffer[0].Set(min.x,min.y);
	textureBuffer[1].Set(max.x,min.y);
	textureBuffer[2].Set(max.x,max.y);
	textureBuffer[3].Set(min.x,max.x);
	
	position=pos;
	size=_size;
}
void THTexture::Set(THImage* _image)
{
	image=_image;

	textureBuffer[0].Set(0.0f,0.0f);
	textureBuffer[1].Set(1.0f,0.0f);
	textureBuffer[2].Set(1.0f,1.0f);
	textureBuffer[3].Set(0.0f,1.0f);

	position.Set(0.0f,0.0f);
	size=image->size;
}
void THTexture::Set(const THTexture& _texture,const THVector2& pos,const THVector2& size)
{
	Set(_texture.image,position+pos,size);
}
void THTexture::UpsideDown()
{
	const float sx=textureBuffer[0].x;
	const float sy=textureBuffer[0].y;
	const float sx2=textureBuffer[2].x;
	const float sy2=textureBuffer[2].y;

	textureBuffer[0].Set(sx,sy2);
	textureBuffer[1].Set(sx2,sy2);
	textureBuffer[2].Set(sx2,sy);
	textureBuffer[3].Set(sx,sy);
}


void THVertexBuffer::Load(void* data,GLuint bytes,GLenum usage)
{
	THLog("VertexBuffer Generation; %d Bytes",bytes);

	glGenBuffers(1,&vboHandler);

	glBindBuffer(GL_ARRAY_BUFFER,vboHandler);
	glBufferData(GL_ARRAY_BUFFER,bytes,data,usage);

	glBindBuffer(GL_ARRAY_BUFFER,0);
	//ToDo Returning to default vertexbuffer
}
void THVertexBuffer::Update(GLvoid* data,GLintptr offset,GLuint bytes)
{
	glBindBuffer(GL_ARRAY_BUFFER,vboHandler);
	glBufferSubData(GL_ARRAY_BUFFER,offset,bytes,data);

	glBindBuffer(GL_ARRAY_BUFFER,0);
	//ToDo Returning to default vertexbuffer
}

void THFrameBuffer::Load(GLenum format)
{
#ifndef NDEBUG
	const char* formatName="NAN";
	switch(format)
	{
	case GL_RGB:
		formatName="GL_RGB";
		break;
	case GL_RGBA:
		formatName="GL_RGBA";
		break;
	case GL_ALPHA:
		formatName="GL_ALPHA";
		break;
	case GL_DEPTH_COMPONENT:
		formatName="GL_DEPTH_COMPONENT";
		break;
	}

	THLog("FrameBuffer Generation; %.1f / %.1f / %s",fboImage.size.x,fboImage.size.y,formatName);
#endif

	GLuint fboTextureHandler;
	glGenTextures(1,&fboTextureHandler);
	glBindTexture(GL_TEXTURE_2D,fboTextureHandler);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,format,(GLsizei)fboImage.size.x,(GLsizei)fboImage.size.y,0,format,GL_UNSIGNED_BYTE,0);
	fboImage.textureID=fboTextureHandler;

	/*
	glGenRenderbuffers(1,&rbHandler);
	glBindRenderbuffer(GL_RENDERBUFFER,rbHandler);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA4,width,height);
	*/

	glGenFramebuffers(1,&fboHandler);
	glBindFramebuffer(GL_FRAMEBUFFER,fboHandler);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,rbHandler);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,fboTextureHandler,0);

	glBindFramebuffer(GL_FRAMEBUFFER,0);
	//ToDo Returning to default framebuffer
}
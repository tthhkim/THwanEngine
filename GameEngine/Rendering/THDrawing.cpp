#include "THDrawing.h"
#include <THPrivate.h>
#include <assert.h>
#include <malloc.h>
#include <GameEngine/DisplayObject/THFrame.h>

static GLuint InitShader(const GLchar* source,GLenum type)
{
	GLuint shader=glCreateShader(type);
	glShaderSource(shader,1,&source,NULL);
	glCompileShader(shader);

#ifndef NDEBUG
	//Shader Error Checking
	const char* shaderName=0;
	switch(type)
	{
	case GL_VERTEX_SHADER:
		shaderName="Vertex";
		break;
	case GL_FRAGMENT_SHADER:
		shaderName="Fragment";
		break;
	}

	GLint compileSt;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&compileSt);
	THLog("%sShader Compile : %s",shaderName,compileSt==GL_TRUE?"SUCCESS":"FAIL");

	GLint buflen;
	glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&buflen);

	if(buflen>12)
	{
		GLchar* log_string=new char[buflen+3];
		glGetShaderInfoLog(shader,buflen,0,log_string);

		THError("%sShader Log : \n%s",shaderName,log_string);

		delete[] log_string;
	}
#endif

	return shader;
}
void THProgram::LoadFile(const char *vs,const char *fs)
{
	THAsset vsf,fsf;
#if THPLATFORM==THPLATFORM_ANDROID
	vsf=THAsset_open(vs,AASSET_MODE_STREAMING);
	fsf=THAsset_open(fs,AASSET_MODE_STREAMING);
#elif THPLATFORM==THPLATFORM_WINDOWS
	vsf=THAsset_open(vs,"rb");
	fsf=THAsset_open(fs,"rb");
#endif

	size_t vl=THAsset_length(vsf);
	size_t fl=THAsset_length(fsf);

	GLchar *vsc=new GLchar[vl+1];
	GLchar *fsc=new GLchar[fl+1];

	THAsset_read(vsf,vsc,vl);
	THAsset_read(fsf,fsc,fl);
	THAsset_close(vsf);
	THAsset_close(fsf);

	vsc[vl]=0;
	fsc[fl]=0;

	Load(vsc,fsc);
	delete[] vsc;
	delete[] fsc;
}
void THProgram::Load(const GLchar* vs,const GLchar* fs)
{
	vertex=InitShader(vs,GL_VERTEX_SHADER);
	fragment=InitShader(fs,GL_FRAGMENT_SHADER);
	program=glCreateProgram();
	glAttachShader(program,vertex);
	glAttachShader(program,fragment);
	glLinkProgram(program);
	glUseProgram(program);
}
void THImage::Load(void* data,GLenum format,GLfloat filter,bool isRepeat)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	if(isRepeat)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,format, GL_UNSIGNED_BYTE, data);
}
void THImage::LoadFrameBuffer(GLenum format,GLenum type,GLfloat filter,bool isRepeat)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	if(isRepeat)
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,format, type,0);
}


void THTexture::Set(THImage* _image,const THVector2& pos,const THVector2& _size)
{
	image=_image;
	const THVector2 invSize=1.0f/_image->size;

	position=pos * invSize;
	size=_size * invSize;
}
void THTexture::Set(THImage* _image)
{
	image=_image;

	position.SetZero();
	size.Set(1.0f,1.0f);
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
void THVertexBuffer::Update(GLvoid* data,GLintptr offset,GLuint bytes) const
{
	glBindBuffer(GL_ARRAY_BUFFER,vboHandler);
	glBufferSubData(GL_ARRAY_BUFFER,offset,bytes,data);

	glBindBuffer(GL_ARRAY_BUFFER,0);
	//ToDo Returning to default vertexbuffer
}

void THFrameBuffer::Load(THImage* img)
{
	fboImage=img;

#ifndef NDEBUG
	THLog("FrameBuffer Generation;");
#endif

	/*
	GLuint fboTextureHandler;
	glGenTextures(1,&fboTextureHandler);
	glBindTexture(GL_TEXTURE_2D,fboTextureHandler);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,format,(GLsizei)fboImage.size.x,(GLsizei)fboImage.size.y,0,format,GL_UNSIGNED_BYTE,0);
	fboImage.textureID=fboTextureHandler;
	*/

	/*
	glGenRenderbuffers(1,&rbHandler);
	glBindRenderbuffer(GL_RENDERBUFFER,rbHandler);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_RGBA4,width,height);
	*/

	glGenFramebuffers(1,&fboHandler);
	glBindFramebuffer(GL_FRAMEBUFFER,fboHandler);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,rbHandler);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,fboImage->textureID,0);

	glBindFramebuffer(GL_FRAMEBUFFER,0);
	//ToDo Returning to default framebuffer
}
void THFrameBuffer::EndDrawing() const
{
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	//ToDo Returning to default framebuffer
	//glBindRenderbuffer(GL_RENDERBUFFER,0);
}
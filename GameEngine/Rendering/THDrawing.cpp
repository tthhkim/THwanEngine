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
unsigned char *ReadFile(const char *name,size_t *length)
{
	THAsset f=THAsset_open(name);
	size_t l=THAsset_length(f);
	unsigned char *buf=new unsigned char[l+1];
	THAsset_read(f,buf,l);
	THAsset_close(f);
	buf[l]=0;
	if(length){*length=l;}
	return buf;
}
void THProgram::LoadFile(const char *vs,const char *fs)
{
	GLchar *vsc=(GLchar*)ReadFile(vs,0);
	GLchar *fsc=(GLchar*)ReadFile(fs,0);

	Load(vsc,fsc);
	delete[] vsc;
	delete[] fsc;
}
void THProgram::Load(const GLchar* vs,const GLchar* fs)
{
	vertex=InitShader(vs,GL_VERTEX_SHADER);
	fragment=InitShader(fs,GL_FRAGMENT_SHADER);
	TH_GLERROR_CHECK("InitShader")
	program=glCreateProgram();
	TH_GLERROR_CHECK("CreateProgram")
	glAttachShader(program,vertex);
	glAttachShader(program,fragment);
	TH_GLERROR_CHECK("AttachShader")
	glLinkProgram(program);
	TH_GLERROR_CHECK("LinkProgram")
	glUseProgram(program);
	TH_GLERROR_CHECK("UseProgram")
}
void THProgram::LoadVertexHandler(const GLchar *name)
{
	vertexHandler=GetAttribLocation(name?name:"vert");
}
#include <lodepng.h>
unsigned char* LoadImageBuffer(const char *filename,GLenum format,size_t *width,size_t *height)
{
	size_t size;
	unsigned char *mem=ReadFile(filename,&size);

	unsigned char* colorBuf;
	LodePNGColorType ctype;
	size_t w,h;
	switch(format)
	{
	case GL_LUMINANCE:
		ctype=LCT_GREY;
		break;
	case GL_LUMINANCE_ALPHA:
		ctype=LCT_GREY_ALPHA;
		break;
	case GL_RGB:
		ctype=LCT_RGB;
		break;
	case GL_RGBA:
		ctype=LCT_RGBA;
		break;
	}
	LodePNGState state;
	lodepng_state_init(&state);
	state.info_raw.colortype=ctype;
	state.info_raw.bitdepth=8;
	state.decoder.read_text_chunks=0;
	lodepng_decode(&colorBuf,&w,&h,&state,mem,size);
	lodepng_state_cleanup(&state);

	delete[] mem;
	THLog("LoadPNG // Width : %d , Height : %d",width,height);

	if(width){*width=w;}
	if(height){*height=h;}

	return colorBuf;
}
void THImage::Load(void *data,GLenum internelformat,GLenum format,GLenum type,GLfloat filter,GLfloat edgeparam)
{
	glGenTextures(1, &textureID);
	TH_GLERROR_CHECK("GenTextures")
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edgeparam);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edgeparam);
	TH_GLERROR_CHECK("TexParameterf")
	glTexImage2D(GL_TEXTURE_2D, 0, internelformat, width, height, 0,format, type, data);
	TH_GLERROR_CHECK("TexImage2D")
}
void THImage::LoadFile(const char* name,GLenum format,GLenum type,GLfloat filter,GLfloat edgeparam)
{
	size_t widthi,heighti;
	void* colorBuf=LoadImageBuffer(name,format,&widthi,&heighti);

	SetSize(widthi,heighti);

	Load(colorBuf,format,format,type,filter,edgeparam);
	free(colorBuf);
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
	TH_GLERROR_CHECK("GenBuffers")

	glBindBuffer(GL_ARRAY_BUFFER,vboHandler);
	glBufferData(GL_ARRAY_BUFFER,bytes,data,usage);

	glBindBuffer(GL_ARRAY_BUFFER,0);
	TH_GLERROR_CHECK("BufferData")
	//ToDo Returning to default vertexbuffer
}
void THVertexBuffer::Update(GLvoid* data,GLintptr offset,GLuint bytes) const
{
	glBindBuffer(GL_ARRAY_BUFFER,vboHandler);
	TH_GLERROR_CHECK("BindBuffer")
	glBufferSubData(GL_ARRAY_BUFFER,offset,bytes,data);

	glBindBuffer(GL_ARRAY_BUFFER,0);
	TH_GLERROR_CHECK("BufferSubData")
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
	TH_GLERROR_CHECK("GenFrameBuffers")
	glBindFramebuffer(GL_FRAMEBUFFER,fboHandler);
	TH_GLERROR_CHECK("BindFrameBuffer")
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,rbHandler);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,fboImage->textureID,0);
	
#ifndef NDEBUG
	/*
	GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
Not all framebuffer attachment points are framebuffer attachment complete. This means that at least one attachment point with a renderbuffer or texture attached has its attached object no longer in existence or has an attached image with a width or height of zero, or the color attachment point has a non-color-renderable image attached, or the depth attachment point has a non-depth-renderable image attached, or the stencil attachment point has a non-stencil-renderable image attached.

Color-renderable formats include GL_RGBA4, GL_RGB5_A1, and GL_RGB565. GL_DEPTH_COMPONENT16 is the only depth-renderable format. GL_STENCIL_INDEX8 is the only stencil-renderable format.

GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
Not all attached images have the same width and height.

GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
No images are attached to the framebuffer.

GL_FRAMEBUFFER_UNSUPPORTED
The combination of internal formats of the attached images violates an implementation-dependent set of restrictions.
	*/
	GLenum st=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch(st)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		THError("FrameBuffer : INCOMPLETE_ATTACHMENT");
		break;
	//case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
	//	THError("FrameBuffer : INCOMPLETE_DIMENSIONS");
	//	break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		THError("FrameBuffer : MISSING_ATTACHMENT");
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		THError("FrameBuffer : UNSUPPORTED");
		break;
	}
#endif
	TH_GLERROR_CHECK("FrameBufferTexture2D")


	glBindFramebuffer(GL_FRAMEBUFFER,0);
	TH_GLERROR_CHECK("BindFrameBuffer")
	//ToDo Returning to default framebuffer
}
void THFrameBuffer::EndDrawing() const
{
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	TH_GLERROR_CHECK("BindFrameBuffer")
	//ToDo Returning to default framebuffer
	//glBindRenderbuffer(GL_RENDERBUFFER,0);
}

void THFrameBufferPingPong::SetSize(GLsizei w,GLsizei h)
{
	m_image1.SetSize(w,h);
	m_image2.SetSize(w,h);
}
void THFrameBufferPingPong::Load(void *data,GLenum internelformat,GLenum format,GLenum type,GLfloat filter,GLfloat edgeparam)
{
	m_image1.Load(data,internelformat,format,type,filter,edgeparam);
	m_image2.Load(data,internelformat,format,type,filter,edgeparam);
}
void THFrameBufferPingPong::SyncFrameBuffer()
{
	m_fb1.Load(&m_image1);
	m_fb2.Load(&m_image2);
}
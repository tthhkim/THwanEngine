#include "THBuffer.h"
#include <THPrivate.h>

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
void THVertexBuffer::Delete() const
{
	glDeleteBuffers(1,&vboHandler);
	assert(glGetError()==GL_NO_ERROR);
}
void THFrameBuffer::Load()
{
#ifdef TH_ISDEBUG
	THLog("FrameBuffer Generation;");
#endif
	glGenFramebuffers(1,&fboHandler);
	TH_GLERROR_CHECK("GenFrameBuffers")
	glBindFramebuffer(GL_FRAMEBUFFER,fboHandler);
	TH_GLERROR_CHECK("BindFrameBuffer")
}
void THFrameBuffer::Attach(THImage* img,GLenum attachment)
{
	//glBindFramebuffer(GL_FRAMEBUFFER,fboHandler);
	//TH_GLERROR_CHECK("BindFrameBuffer")
	glFramebufferTexture2D(GL_FRAMEBUFFER,attachment,GL_TEXTURE_2D,img->textureID,0);
	
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

	//glBindFramebuffer(GL_FRAMEBUFFER,0);
	//TH_GLERROR_CHECK("BindFrameBuffer")
}
void THFrameBuffer::Delete() const
{
	glDeleteFramebuffers(1,&fboHandler);
	assert(glGetError()==GL_NO_ERROR);
}

void THFrameBufferSet::SetSize(size_t w,size_t h){img.SetSize(w,h);}
void THFrameBufferSet::Load(void *data,GLenum internelformat,GLenum format,GLenum type,GLfloat filter,GLfloat edgeparam)
{
	img.Load(data,internelformat,format,type,filter,edgeparam);
	fbo.Load();
	fbo.Attach(&img);
	fbo.EndDrawing();
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
	m_fb1.Load();
	m_fb1.Attach(&m_image1);
	m_fb2.Load();
	m_fb2.Attach(&m_image2);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void THPixelBuffer::LoadReader(size_t size)
{
	m_isreadonly=true;

	glGenBuffers(1,&pboHandler);
	glBindBuffer(GL_PIXEL_PACK_BUFFER,pboHandler);
	glBufferData(GL_PIXEL_PACK_BUFFER,size,0,GL_STREAM_READ);
	glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
	TH_GLERROR_CHECK("BufferData")
}
void THPixelBuffer::LoadWriter(size_t size)
{
	m_isreadonly=false;

	glGenBuffers(1,&pboHandler);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER,pboHandler);
	glBufferData(GL_PIXEL_UNPACK_BUFFER,size,0,GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
	TH_GLERROR_CHECK("BufferData")
}
void THPixelBuffer::Begin() const
{
	glBindBuffer(GetTarget(),pboHandler);
}
void THPixelBuffer::End() const
{
	glBindBuffer(GetTarget(),0);
}
void* THPixelBuffer::MapBuffer() const
{
	return glMapBuffer(GetTarget(),m_isreadonly?GL_READ_ONLY:GL_WRITE_ONLY);
}
void THPixelBuffer::UnmapBuffer() const
{
	glUnmapBuffer(GetTarget());
}
void THPixelBuffer::Delete()
{
	glDeleteBuffers(1,&pboHandler);
}
#include "THBuffer.h"
#include <THPrivate.h>

void THGLBuffer::Load(GLenum target,size_t size,GLenum usage,void *data)
{
	m_target=target;
	glGenBuffers(1,&handler);
	TH_GLERROR_CHECK("GenBuffer");

	glBindBuffer(target,handler);
	glBufferData(target,size,data,usage);
	TH_GLERROR_CHECK("BufferData");
}
void THGLBuffer::Delete()
{
	glDeleteBuffers(1,&handler);
	TH_GLERROR_CHECK("DeleteBuffer");
}
void* THGLBuffer::MapBuffer(GLenum access)
{
	return glMapBuffer(m_target,access);
	TH_GLERROR_CHECK("MapBuffer");
}
void THGLBuffer::UnmapBuffer()
{
	glUnmapBuffer(m_target);
	TH_GLERROR_CHECK("UnmapBuffer");
}

void THGLBuffer::LoadVertexBuffer(size_t size,GLenum usage,void *data)
{
	Load(GL_ARRAY_BUFFER,size,usage,data);
}
void THGLBuffer::LoadPixelReader(size_t size,GLenum usage,void *data)
{
	Load(GL_PIXEL_PACK_BUFFER,size,usage,data);
}
void THGLBuffer::LoadPixelWriter(size_t size,GLenum usage,void *data)
{
	Load(GL_PIXEL_UNPACK_BUFFER,size,usage,data);
}


void THRenderBuffer::Load(GLsizei width,GLsizei height,GLenum internalformat)
{
	glGenRenderbuffers(1,&renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER,renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER,internalformat,width,height);
	glBindRenderbuffer(GL_RENDERBUFFER,0);
	TH_GLERROR_CHECK("RenderBuffer Create");
}
void THRenderBuffer::Delete() const
{
	glDeleteRenderbuffers(1,&renderBuffer);
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
void THFrameBuffer::CheckAttachment()
{
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
}
void THFrameBuffer::Attach(THImage* img,GLenum attachment)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER,attachment,GL_TEXTURE_2D,img->textureID,0);
	CheckAttachment();

	TH_GLERROR_CHECK("FrameBufferTexture2D")
}
void THFrameBuffer::Attach(THRenderBuffer *renderbuf,GLenum attachment)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,attachment,GL_RENDERBUFFER,renderbuf->renderBuffer);
	CheckAttachment();

	TH_GLERROR_CHECK("RenderBuffer Attachment");
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
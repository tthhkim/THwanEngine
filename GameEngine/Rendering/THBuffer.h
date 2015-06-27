#ifndef TH_RENDERING_BUFFER_H
#define TH_RENDERING_BUFFER_H

#include <THSettings.h>
#include <GameEngine/Rendering/THDrawing.h>
#include <assert.h>

class THGLBuffer
{
public:
	GLuint handler;

	inline GLenum GetTarget(){return m_target;}
/*
GL_STATIC_?? The buffer object data will be specified once by the
application and used many times to draw primitives.

GL_DYNAMIC_?? The buffer object data will be specified repeatedly by the
application and used many times to draw primitives.

GL_STREAM_?? The buffer object data will be specified once by the
application and used a few times to draw primitives.
*/
	void Load(GLenum target,size_t size,GLenum usage,void *data=0);
	void Delete();
	/*
	GL_READ_ONLY 0x88B8
	GL_WRITE_ONLY 0x88B9
	GL_READ_WRITE 0x88BA
	*/
	void *MapBuffer(GLenum access);
	void UnmapBuffer();
	inline void Begin(){glBindBuffer(m_target,handler);}
	inline void End(){glBindBuffer(m_target,0);}

	void LoadVertexBuffer(size_t size,GLenum usage,void *data=0);
	void LoadPixelReader(size_t size,GLenum usage,void *data=0);
	void LoadPixelWriter(size_t size,GLenum usage,void *data=0);
protected:
	GLenum m_target;
};
class THRenderBuffer
{
public:
	GLuint renderBuffer;

	/*
	where # is the bit size of stencil buffer
	GL_STENCIL_INDEX# 
	GL_DEPTH_COMPONENT#
	GL_DEPTH#_STENCIL#
	*/
	void Load(GLsizei width,GLsizei height,GLenum internalformat);
	void Delete() const;
};
class THFrameBuffer
{
public:
	GLuint fboHandler;

	void Load();
	void Attach(THImage* img,GLenum attachment=GL_COLOR_ATTACHMENT0);
	void Attach(THRenderBuffer *renderbuf,GLenum attachment);
	inline void BeginDrawing() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER,fboHandler);
		assert(glGetError()==GL_NO_ERROR);
	}
	inline void EndDrawing() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	void Delete() const;
protected:
	void CheckAttachment();
};
class THFrameBufferSet
{
public:
	THFrameBuffer fbo;
	THImage img;

	void SetSize(size_t w,size_t h);
	void Load(void *data,GLenum internelformat,GLenum format,GLenum type,GLfloat filter=GL_NEAREST,GLfloat edgeparam=GL_CLAMP_TO_EDGE);
	inline void Viewport(){img.Viewport();}
	inline void BeginDrawing(){fbo.BeginDrawing();}
	inline void EndDrawing(){fbo.EndDrawing();}
};
#endif
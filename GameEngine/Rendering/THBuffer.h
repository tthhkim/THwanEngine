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

/*
class THVertexBuffer
{
public:
	GLuint vboHandler;
	void Load(void* data,GLuint bytes,GLenum usage);
	void Update(GLvoid* data,GLintptr offset,GLuint bytes) const;

	inline void BeginDrawing() const
	{
		glBindBuffer(GL_ARRAY_BUFFER,vboHandler);
		assert(glGetError()==GL_NO_ERROR);
	}
	inline void EndDrawing() const
	{
		glBindBuffer(GL_ARRAY_BUFFER,0);
		assert(glGetError()==GL_NO_ERROR);
		//ToDo Returning to default vertexbuffer
	}
	void Delete() const;
};
*/
class THFrameBuffer
{
public:
	GLuint fboHandler;

	void Load();
	void Attach(THImage* img,GLenum attachment=GL_COLOR_ATTACHMENT0);
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
/*
class THPixelBuffer
{
public:
	GLuint pboHandler;

	void LoadReader(size_t size);
	void LoadWriter(size_t size);
	void Begin() const;
	void End() const;
	void* MapBuffer() const;
	void UnmapBuffer() const;
	void Delete();
	inline bool IsReadOnly() const{return m_isreadonly;}
	inline bool IsWriteOnly() const{return !m_isreadonly;}
	inline GLenum GetTarget() const{return m_isreadonly?GL_PIXEL_PACK_BUFFER:GL_PIXEL_UNPACK_BUFFER;}
protected:
	bool m_isreadonly;
};
*/
#endif
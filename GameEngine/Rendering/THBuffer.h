#ifndef TH_RENDERING_BUFFER_H
#define TH_RENDERING_BUFFER_H

#include <THSettings.h>
#include <GameEngine/Rendering/THDrawing.h>
#include <assert.h>


class THVertexBuffer
{
public:
	GLuint vboHandler;
	/*
GL_STATIC_DRAW The buffer object data will be specified once by the
application and used many times to draw primitives.

GL_DYNAMIC_DRAW The buffer object data will be specified repeatedly by the
application and used many times to draw primitives.

GL_STREAM_DRAW The buffer object data will be specified once by the
application and used a few times to draw primitives.
*/
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

class THFrameBuffer
{
public:
	GLuint fboHandler;
	THImage* fboImage;

	void Load(THImage* img);
	inline void BeginDrawing() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER,fboHandler);
		assert(glGetError()==GL_NO_ERROR);
	}
	void EndDrawing() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	void Delete() const;
	inline void Viewport() const
	{
		fboImage->Viewport();
	}
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
class THFrameBufferPingPong
{
public:
	inline THImage& GetImage1(){return m_image1;}
	inline THImage& GetImage2(){return m_image2;}
	void Viewport(){m_image1.Viewport();}
	void SetSize(GLsizei w,GLsizei h);
	void Load(void *data,GLenum internelformat,GLenum format,GLenum type,GLfloat filter=GL_NEAREST,GLfloat edgeparam=GL_CLAMP_TO_EDGE);
	void SyncFrameBuffer();
	void Change(){m_isone=!m_isone;}
	THFrameBuffer& GetDstFrameBuffer(){return m_isone?m_fb2:m_fb1;}
	void BeginDrawing(){GetDstFrameBuffer().BeginDrawing();}
	void EndDrawing(){GetDstFrameBuffer().EndDrawing();}
	THImage& GetSourceImage(){return m_isone?m_image1:m_image2;}
protected:
	THFrameBuffer m_fb1,m_fb2;
	THImage m_image1,m_image2;
	bool m_isone;
};
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

#endif
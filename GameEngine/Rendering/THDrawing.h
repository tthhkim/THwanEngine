#ifndef THKim_THDrawing
#define THKim_THDrawing

#include <THSettings.h>
#include <malloc.h>

#include <GameEngine/Util/THMath.h>
#include <assert.h>


class THColor
{
public:
	float red,green,blue,alpha;

	THColor(float r,float g,float b,float a=1.0f)
	{
		red=r;
		green=g;
		blue=b;
		alpha=a;
	}
	THColor(){}

	void Set(float r,float g,float b,float a=1.0f)
	{
		red=r;
		green=g;
		blue=b;
		alpha=a;
	}
};
class THProgram
{
protected:
	GLuint vertex;
	GLuint fragment;
public:
	GLuint program;

	THProgram(){}

	void Load(const GLchar* vs,const GLchar* fs);
	void LoadFile(const char *vs,const char *fs);

	inline void SetUniform(const char* name,float f1) const
	{
		glUniform1f(glGetUniformLocation(program,name),f1);
		assert(glGetError()==GL_NO_ERROR);
	}
	inline void SetUniform(const char* name,float f1,float f2) const
	{
		glUniform2f(glGetUniformLocation(program,name),f1,f2);
		assert(glGetError()==GL_NO_ERROR);
	}
	inline void SetUniform(const char* name,float f1,float f2,float f3) const
	{
		glUniform3f(glGetUniformLocation(program,name),f1,f2,f3);
		assert(glGetError()==GL_NO_ERROR);
	}
	inline void SetUniform(const char* name,float f1,float f2,float f3,float f4) const
	{
		glUniform4f(glGetUniformLocation(program,name),f1,f2,f3,f4);
		assert(glGetError()==GL_NO_ERROR);
	}
	inline GLint GetUniformLocation(const GLchar* name) const
	{
		return glGetUniformLocation(program,name);
		assert(glGetError()==GL_NO_ERROR);
	}
	inline GLint GetAttribLocation(const GLchar* name) const
	{
		return glGetAttribLocation(program,name);
		assert(glGetError()==GL_NO_ERROR);
	}
	inline void Use() const
	{
		glUseProgram(program);
		assert(glGetError()==GL_NO_ERROR);
	}

	void Delete() const
	{
		glDetachShader(program,vertex);
		glDetachShader(program,fragment);
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		glDeleteProgram(program);
	}
};
unsigned char* LoadImageBuffer(const char *filename,GLenum format,size_t *width,size_t *height);
class THImage
{
public:
	GLuint textureID;
	THVector2 size;
	GLsizei width,height;

	THImage(){}
	THImage(GLsizei w,GLsizei h):size((float)w,(float)h),width(w),height(h){}

	void Load(void *data,GLenum internelformat,GLenum format,GLenum type,GLfloat filter=GL_NEAREST,GLfloat edgeparam=GL_CLAMP_TO_EDGE);
	void LoadFile(const char* name,GLenum format,GLenum type,GLfloat filter=GL_NEAREST,GLfloat edgeparam=GL_CLAMP_TO_EDGE);
	void SetSize(GLsizei w,GLsizei h)
	{
		width=w;
		height=h;
		size.Set((float)w,(float)h);
	}

	inline void DeleteTexture() const
	{
		glDeleteTextures(1,&textureID);
	}
	inline void Viewport() const
	{
		glViewport(0,0,width,height);
	}
};

class THTexture
{
public:
	THImage* image;
	THVector2 position,size;

	void Set(THImage* _image);
	void Set(THImage* _image,const THVector2& pos,const THVector2& size);
	void UpsideDown();

	inline THVector2 GetSize() const
	{
		return size;
	}
	inline THVector2 GetPosition() const
	{
		return position;
	}

protected:
};


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
	inline void DeleteVBO() const
	{
		glDeleteBuffers(1,&vboHandler);
		assert(glGetError()==GL_NO_ERROR);
	}
};

class THFrameBuffer
{
public:
	GLuint fboHandler;//,rbHandler;
	THImage* fboImage;

	void Load(THImage* img);
	inline void BeginDrawing() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER,fboHandler);
		assert(glGetError()==GL_NO_ERROR);
		//glBindRenderbuffer(GL_RENDERBUFFER,rbHandler);
	}
	void EndDrawing() const;
	void DeleteFBO() const
	{
		glDeleteFramebuffers(1,&fboHandler);
		assert(glGetError()==GL_NO_ERROR);
		//glDeleteRenderbuffers(1,&rbHandler);
	}
	inline void Viewport() const
	{
		fboImage->Viewport();
	}
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

#endif

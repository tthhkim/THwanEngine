#ifndef THKim_THDrawing
#define THKim_THDrawing

#include <GLES2/gl2.h>
#include <malloc.h>

#include <GameEngine/Util/THMath.h>


class THProgram
{
protected:
	GLuint vertex;
	GLuint fragment;
public:
	GLuint program;

	void Load(const GLchar* vs,const GLchar* fs);

	inline void SetUniform(const char* name,float f1) const
	{
		glUniform1f(glGetUniformLocation(program,name),f1);
	}
	inline void SetUniform(const char* name,float f1,float f2) const
	{
		glUniform2f(glGetUniformLocation(program,name),f1,f2);
	}
	inline void SetUniform(const char* name,float f1,float f2,float f3) const
	{
		glUniform3f(glGetUniformLocation(program,name),f1,f2,f3);
	}
	inline void SetUniform(const char* name,float f1,float f2,float f3,float f4) const
	{
		glUniform4f(glGetUniformLocation(program,name),f1,f2,f3,f4);
	}
	inline GLuint GetUniformLocation(const GLchar* name) const
	{
		return glGetUniformLocation(program,name);
	}
	inline GLuint GetAttribLocation(const GLchar* name) const
	{
		return glGetAttribLocation(program,name);
	}
	inline void Use() const
	{
		glUseProgram(program);
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

class THImage
{
public:
	GLuint textureID;
	THVector2 size;
	GLsizei width,height;

	THImage(GLsizei w,GLsizei h):size((float)w,(float)h){width=w;height=h;}

	void Load(void* data,GLenum format,GLfloat filter=GL_NEAREST,bool isRepeat=false);

	inline void DeleteTexture() const
	{
		glDeleteTextures(1,&textureID);
	}
};

class THTexture
{
public:
	THImage* image;
	THVector2 textureBuffer[4];

	void Set(THImage* _image);
	void Set(THImage* _image,const THVector2& pos,const THVector2& size);
	void Set(const THTexture& _texture,const THVector2& pos,const THVector2& size);
	void UpsideDown();

	inline THVector2 GetSize() const
	{
		return size;
	}
	inline THVector2 GetPosition() const
	{
		return position;
	}
	inline THVector2 GetRelative(const THVector2& v) const
	{
		return GetPosition()+(GetSize()*v);
	}

protected:
	THVector2 position,size;

	void SetBuffer(const THVector2& minp,const THVector2& maxp);
};


class THVertexBuffer
{
public:
	GLuint vboHandler;
	GLuint attribHandler;

	THVertexBuffer(GLuint _attribHandler){
		attribHandler=_attribHandler;
	}
	/*
GL_STATIC_DRAW The buffer object data will be specified once by the
application and used many times to draw primitives.

GL_DYNAMIC_DRAW The buffer object data will be specified repeatedly by the
application and used many times to draw primitives.

GL_STREAM_DRAW The buffer object data will be specified once by the
application and used a few times to draw primitives.
*/
	void Load(void* data,GLuint bytes,GLenum usage);
	inline void Load(GLfloat* vertices,GLuint count)
	{
		Load(vertices,sizeof(GLfloat)*count,GL_STATIC_DRAW);
	}
	void Update(GLvoid* data,GLintptr offset,GLuint bytes) const;

	inline void BeginDrawing() const
	{
		glBindBuffer(GL_ARRAY_BUFFER,vboHandler);
		glEnableVertexAttribArray(attribHandler);
	}
	inline void EndDrawing() const
	{
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glDisableVertexAttribArray(attribHandler);
		//ToDo Returning to default vertexbuffer
	}
	inline void DeleteVBO() const
	{
		glDeleteBuffers(1,&vboHandler);
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
		glViewport(0,0,fboImage->width,fboImage->height);
		glBindFramebuffer(GL_FRAMEBUFFER,fboHandler);
		glBindTexture(GL_TEXTURE_2D,fboImage->textureID);
		//glBindRenderbuffer(GL_RENDERBUFFER,rbHandler);
	}
	void EndDrawing() const;
	void DeleteFBO() const
	{
		glDeleteFramebuffers(1,&fboHandler);
		//glDeleteRenderbuffers(1,&rbHandler);
	}
};

#endif

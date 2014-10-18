#ifndef THKim_THDrawing
#define THKim_THDrawing

#include <GLES2/gl2.h>
#include <malloc.h>

#include <GameEngine/Util/THMath.h>

GLuint GenerateTexture(void* data,GLsizei width,GLsizei height,GLenum format,GLfloat filter=GL_NEAREST,bool isRepeat=false);//,const unsigned int isRepeat);
//void DrawTexture();

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

	THImage(float w=0.0f,float h=0.0f):size(w,h){}
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
	THVector2 position,size;

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
	THImage fboImage;

	THFrameBuffer(unsigned int width,unsigned int height):fboImage((float)width,(float)height){}
	void Load(GLenum format);
	inline void BeginDrawing() const
	{
		glViewport(0,0,(GLsizei)fboImage.size.x,(GLsizei)fboImage.size.y);
		glBindFramebuffer(GL_FRAMEBUFFER,fboHandler);
		glBindTexture(GL_TEXTURE_2D,fboImage.textureID);
		//glBindRenderbuffer(GL_RENDERBUFFER,rbHandler);
	}
	inline void EndDrawing() const
	{
		extern THVector2 windowSize;
		glViewport(0,0,(GLsizei)windowSize.x,(GLsizei)windowSize.y);
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		//ToDo Returning to default framebuffer
		//glBindRenderbuffer(GL_RENDERBUFFER,0);
	}
	void DeleteFBO() const
	{
		glDeleteFramebuffers(1,&fboHandler);
		//glDeleteRenderbuffers(1,&rbHandler);
	}
};

#endif

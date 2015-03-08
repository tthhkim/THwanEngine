#ifndef THKim_THDrawing
#define THKim_THDrawing

#include <GLES2/gl2.h>
#include <malloc.h>

#include <GameEngine/Util/THMath.h>


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
	inline GLint GetUniformLocation(const GLchar* name) const
	{
		return glGetUniformLocation(program,name);
	}
	inline GLint GetAttribLocation(const GLchar* name) const
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

	THImage(){}
	THImage(GLsizei w,GLsizei h):size((float)w,(float)h),width(w),height(h){}

	void Load(void* data,GLenum format,GLfloat filter=GL_NEAREST,bool isRepeat=false);
	void LoadFrameBuffer(GLenum format=GL_RGB,GLenum type=GL_UNSIGNED_SHORT_5_6_5,GLfloat filter=GL_NEAREST,bool isRepeat=false);
	void LoadFile(const char* name,int colorType,GLfloat filter=GL_NEAREST,bool isRepeat=false);
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
	}
	inline void EndDrawing() const
	{
		glBindBuffer(GL_ARRAY_BUFFER,0);
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

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
	GLint vertexHandler;

	THProgram(){}

	void Load(const GLchar* vs,const GLchar* fs,bool link=true);
	void LoadFile(const char *vs,const char *fs,bool link=true);
	void Link();
	void LoadVertexHandler(const GLchar *name);
	inline void EnableVertexAttribArray(){glEnableVertexAttribArray(vertexHandler);}
	inline void DisableVertexAttribArray(){glDisableVertexAttribArray(vertexHandler);}
	inline void VertexAttribPointer(const void *p){glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,p);}
	

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
	void SetSize(GLsizei w,GLsizei h);
	void Delete() const;

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



#endif

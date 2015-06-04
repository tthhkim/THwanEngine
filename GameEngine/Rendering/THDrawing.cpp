#include "THDrawing.h"
#include <THPrivate.h>
#include <assert.h>
#include <malloc.h>
#include <GameEngine/DisplayObject/THFrame.h>

GLuint LoadShader(const GLchar* source,GLenum type)
{
	GLuint shader=glCreateShader(type);
	glShaderSource(shader,1,&source,NULL);
	glCompileShader(shader);

#ifdef TH_ISDEBUG
	//Shader Error Checking
	const char* shaderName=0;
	switch(type)
	{
	case GL_VERTEX_SHADER:
		shaderName="Vertex";
		break;
	case GL_FRAGMENT_SHADER:
		shaderName="Fragment";
		break;
	case GL_GEOMETRY_SHADER:
		shaderName="Geometry";
		break;
	}

	GLint compileSt;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&compileSt);
	THLog("%sShader Compile : %s",shaderName,compileSt==GL_TRUE?"SUCCESS":"FAIL");

	GLint buflen;
	glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&buflen);

	if(buflen>12)
	{
		GLchar* log_string=new char[buflen+3];
		glGetShaderInfoLog(shader,buflen,0,log_string);

		THError("%sShader Log : \n%s",shaderName,log_string);

		delete[] log_string;
	}
#endif

	TH_GLERROR_CHECK("CompileShader")
	return shader;
}
GLuint LoadShaderFile(const char *name,GLenum type)
{
	GLchar *code=(GLchar*)ReadFile(name,0);
	GLuint shader=LoadShader(code,type);
	delete[] code;
	return shader;
}
unsigned char *ReadFile(const char *name,size_t *length)
{
	THAsset f=THAsset_open(name);
	size_t l=THAsset_length(f);
	unsigned char *buf=new unsigned char[l+1];
	THAsset_read(f,buf,l);
	THAsset_close(f);
	buf[l]=0;
	if(length){*length=l;}
	return buf;
}
void THProgram::AttachShader(GLuint shader)
{
	glAttachShader(program,shader);
	TH_GLERROR_CHECK("AttachShader")
}
void THProgram::Load()
{
	program=glCreateProgram();
	TH_GLERROR_CHECK("CreateProgram")
}
void THProgram::Link()
{
	glLinkProgram(program);
	TH_GLERROR_CHECK("LinkProgram")
	glUseProgram(program);
	TH_GLERROR_CHECK("UseProgram")
}
void THProgram::LoadVertexHandler(const GLchar *name)
{
	vertexHandler=GetAttribLocation(name?name:"vert");
}
#include <lodepng.h>
unsigned char* LoadImageBuffer(const char *filename,GLenum format,size_t *width,size_t *height)
{
	size_t size;
	unsigned char *mem=ReadFile(filename,&size);

	unsigned char* colorBuf;
	LodePNGColorType ctype;
	size_t w,h;
	switch(format)
	{
	case GL_LUMINANCE:
		ctype=LCT_GREY;
		break;
	case GL_LUMINANCE_ALPHA:
		ctype=LCT_GREY_ALPHA;
		break;
	case GL_RGB:
		ctype=LCT_RGB;
		break;
	case GL_RGBA:
		ctype=LCT_RGBA;
		break;
	}
	LodePNGState state;
	lodepng_state_init(&state);
	state.info_raw.colortype=ctype;
	state.info_raw.bitdepth=8;
	state.decoder.read_text_chunks=0;
	lodepng_decode(&colorBuf,&w,&h,&state,mem,size);
	lodepng_state_cleanup(&state);

	delete[] mem;
	THLog("LoadPNG // Width : %d , Height : %d",width,height);

	if(width){*width=w;}
	if(height){*height=h;}

	return colorBuf;
}
void THImage::Load(void *data,GLenum internelformat,GLenum format,GLenum type,GLfloat filter,GLfloat edgeparam)
{
	glGenTextures(1, &textureID);
	TH_GLERROR_CHECK("GenTextures")
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edgeparam);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edgeparam);
	TH_GLERROR_CHECK("TexParameterf")
	glTexImage2D(GL_TEXTURE_2D, 0, internelformat, width, height, 0,format, type, data);
	TH_GLERROR_CHECK("TexImage2D")
}
void THImage::LoadFile(const char* name,GLenum format,GLenum type,GLfloat filter,GLfloat edgeparam)
{
	size_t widthi,heighti;
	void* colorBuf=LoadImageBuffer(name,format,&widthi,&heighti);

	SetSize(widthi,heighti);

	Load(colorBuf,format,format,type,filter,edgeparam);
	free(colorBuf);
}
void THImage::SetSize(GLsizei w,GLsizei h)
{
	width=w;
	height=h;
	size.Set((float)w,(float)h);
}

void THImage::Delete() const
{
	glDeleteTextures(1,&textureID);
}


void THTexture::Set(THImage* _image,const THVector2& pos,const THVector2& _size)
{
	image=_image;
	const THVector2 invSize=1.0f/_image->size;

	position=pos * invSize;
	size=_size * invSize;
}
void THTexture::Set(THImage* _image)
{
	image=_image;

	position.SetZero();
	size.Set(1.0f,1.0f);
}



#ifndef TH_FLUID_SHADER
#define TH_FLUID_SHADER

#include <THPrivate.h>
#include <GameEngine/Util/THMath3D.h>


class THFluidShader : public THProgram
{
public:
	void Load();
	inline void SyncProjection(const THVector2& minp,const THVector2& maxp) const
	{
		GLfloat proj[6];
		THOrthoMatrix33(proj,minp,maxp);
		glUniform3fv(GetUniformLocation("projMat"),2,proj);
	}
	inline void SetSize(float s) const
	{
		glUniform1f(sizeHandler,s);
	}
	inline void SetTailSize(float l) const
	{
		glUniform1f(tailSizeHandler,l);
	}
	void PreDraw(const THImage& circleImage) const;
	void PostDraw() const;
	inline void Draw(const THVector2& velocity,const THVector2& position)
	{
		glVertexAttrib2fv(positionHandler,(GLfloat*)&position);
		glVertexAttrib2fv(velocityHandler,(GLfloat*)&velocity);
		glVertexAttrib1f(isVelHandler,1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		glVertexAttrib1f(isVelHandler,0.0f);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	}
	inline void Draw(const THVector2& velocity,const THVector2& position,float size)
	{
		glVertexAttrib2fv(positionHandler,(GLfloat*)&position);
		glVertexAttrib2fv(velocityHandler,(GLfloat*)&velocity);
		glVertexAttrib1f(aSizeHandler,size);
		glVertexAttrib1f(isVelHandler,1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		glVertexAttrib1f(isVelHandler,0.0f);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	}
protected:
	GLint vertexHandler;
	GLint tailSizeHandler;
	GLint velocityHandler,positionHandler,isVelHandler,aSizeHandler;

	GLint sizeHandler;
};
class THFluidRenderShader : public THProgram
{
public:
	//THFluidRenderShader(){}
	void Load();
	void Draw(const THImage& fbImage) const;

	void SetColor(const THVector3& col,const THVector3& out)
	{
		glUniform3f(colorHandler,col.x,col.y,col.z);
		glUniform3f(outColorHandler,out.x,out.y,out.z);
	}
	void SetThreshold(float threshold)
	{
		glUniform1f(thresholdHandler,threshold);
	}
	void SetAlpha(float alpha)
	{
		glUniform1f(alphaHandler,alpha);
	}
protected:
	GLint vertexHandler;

	GLint colorHandler,outColorHandler;
	GLint thresholdHandler,alphaHandler;
};
class THSteamRenderShader : public THProgram
{
public:
	void Load();
	void SetAlpha(float a)
	{
		glUniform1f(alphaHandler,a);
	}
	void Draw(const THImage& fbImage) const;

protected:
	GLint vertexHandler;
	GLint alphaHandler;
};

class THFluidBaseShader : public THProgram
{
public:
	void Load();
	void Draw();
protected:
	GLint vertexHandler;
};
class THBoundaryBaseShader : public THProgram
{
public:
	void Load();
	void Draw();
protected:
	GLint vertexHandler;
};
class THBoundaryShader : public THProgram
{
public:
	void Load();
	
	inline void SetSize(float s)
	{
		glUniform1f(sizeHandler,s);
	}
	inline void PreDraw(const THImage& cImage) const
	{
		glBindTexture(GL_TEXTURE_2D,cImage.textureID);
		glEnableVertexAttribArray(vertexHandler);
		glVertexAttribPointer(vertexHandler,2,GL_FLOAT,GL_FALSE,0,0);
	}
	void Draw(const THVector2& pos)
	{
		glVertexAttrib2fv(positionHandler,(const GLfloat*)&pos);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
	}
	inline void PostDraw() const
	{
		glDisableVertexAttribArray(vertexHandler);
	}
	inline void SyncProjection(const THVector2& minp,const THVector2& maxp) const
	{
		GLfloat proj[6];
		THOrthoMatrix33(proj,minp,maxp);
		glUniform3fv(GetUniformLocation("projMat"),2,proj);
	}
protected:
	GLint vertexHandler;
	GLint positionHandler;
	GLint sizeHandler;
};
class THBoundaryRenderShader : public THProgram
{
public:
	void Load();
	void Draw(const THImage& fbImage,const THImage& groundTexture) const;
	void SetTile(const THVector2& textureSize)
	{
		SetUniform("u_groundtexscale",2048.0f/textureSize.x,2048.0f/textureSize.y);
		//SetUniform("u_size",textureSize.x,textureSize.y);
		//SetUniform("u_invSize",invSize.x,invSize.y);
	}
protected:
	GLint vertexHandler;
};

class THLightRenderShader : public THProgram
{
public:
	void Load();
	void Draw(const THImage& frame,const THImage& light);
	void SetAmbient(const THVector3& c)
	{
		SetUniform("u_ambient",c.x,c.y,c.z);
	}
	void SetLightCoeff(float c)
	{
		glUniform1f(lightcoeffHandler,c);
	}
protected:
	GLint vertexHandler;
	GLint lightcoeffHandler;
};
#endif
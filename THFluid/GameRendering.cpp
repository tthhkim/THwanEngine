#include "GameFrame.h"

void GameFrame::LoadTextures()
{
	notebgImage.LoadFile("notebg.png",TH_PNG_RGB);
	notebgTexture.Set(&notebgImage);
	notebgObject.texture=&notebgTexture;
	notebgObject.size.Set(20.0f,20.0f);
	AddChild(&notebgObject);

	groundTileImage.LoadFile("gtile.png",TH_PNG_RGB,GL_NEAREST,true);
	hangerTexture.Set(&groundTileImage);
}

#define CIRCLE_SIZE 64
#define CIRCLE_BIG_RAD (CIRCLE_SIZE*0.5f)
#define CIRCLE_SMALL_RAD 15.0f
#define BCIRCLE_SIZE 64
#define BCIRCLE_BIG_RAD (BCIRCLE_SIZE*0.5f)
void GameFrame::LoadCircleImage()
{
	circleImage.SetSize(CIRCLE_SIZE,CIRCLE_SIZE);
	circleImage.LoadFrameBuffer();

	THFrameBuffer circleFBO;
	circleFBO.Load(&circleImage);

	oneVBO.BeginDrawing();
	THFluidBaseShader fluidBaseShader;
	fluidBaseShader.Load();
	glViewport(0,0,CIRCLE_SIZE,CIRCLE_SIZE);
	circleFBO.BeginDrawing();
	glClear(GL_COLOR_BUFFER_BIT);
	fluidBaseShader.Draw();
	circleFBO.EndDrawing();
	fluidBaseShader.Delete();
	circleFBO.DeleteFBO();
	oneVBO.EndDrawing();
	

	boundaryCircleImage.SetSize(BCIRCLE_SIZE,BCIRCLE_SIZE);
	boundaryCircleImage.LoadFrameBuffer();
	
	THFrameBuffer bCircleFBO;
	bCircleFBO.Load(&boundaryCircleImage);

	oneVBO.BeginDrawing();
	THBoundaryBaseShader bBaseShader;
	bBaseShader.Load();
	glViewport(0,0,BCIRCLE_SIZE,BCIRCLE_SIZE);
	bCircleFBO.BeginDrawing();
	glClear(GL_COLOR_BUFFER_BIT);
	bBaseShader.Draw();
	bCircleFBO.EndDrawing();
	bBaseShader.Delete();
	bCircleFBO.DeleteFBO();
	oneVBO.EndDrawing();
	
	

	ViewportInit();
}
void GameFrame::LoadRender()
{
	fluidRenderImage.SetSize(1024,2048);
	fluidRenderImage.LoadFrameBuffer();
	fluidRenderFBO.Load(&fluidRenderImage);

	shader.Load();
	//shader.SyncProjection(engine.GetMinGrid(),engine.GetMaxGrid());
	shader.SetSize(0.4f);
	shader.SetTailSize(0.2f);

	rshader.Load();
	rshader.SetColor(THVector3(0.0f,0.0f,0.0f));
	rshader.SetThreshold(0.7f);
	rshader.SetAlpha(1.0f);

	bshader.Load();
	bshader.SetSize(0.1f);
	//bshader.SyncProjection(engine.GetMinGrid(),engine.GetMaxGrid());

	brshader.Load();
	brshader.SetTile(THVector2(256.0f,256.0f));

	
}
void GameFrame::LoadOneVBO()
{
	GLfloat vertices[]=
	{
		-1.0f,-1.0f,
		 1.0f,-1.0f,
		-1.0f, 1.0f,
		 1.0f, 1.0f
	};
	oneVBO.Load(vertices,sizeof(GLfloat)*8,GL_STATIC_DRAW);
}

void GameFrame::RenderBoundary()
{
	THParticle *particle;

	fluidRenderFBO.BeginDrawing();
	glViewport(0,0,1024,2048);
	glClear(GL_COLOR_BUFFER_BIT);
	bshader.Use();
	bshader.SetSize(0.2f);
	//bshader.SetAlphaFactor(factor);
	bshader.PreDraw(boundaryCircleImage);
	for(particle=boundaryGroup.list;particle;particle=particle->GetNext())
	{
		bshader.Draw(particle->position);
	}
	bshader.PostDraw();
	fluidRenderFBO.EndDrawing();

	ViewportInit();

	//framebaseFBO.BeginDrawing();
	brshader.Use();
	brshader.Draw(fluidRenderImage,groundTileImage);
	//framebaseFBO.EndDrawing();
}
void GameFrame::RenderBoundary(const THParticleGroup& group,const THVector3& color)
{
	THParticle *particle;

	fluidRenderFBO.BeginDrawing();
	glViewport(0,0,1024,2048);
	glClear(GL_COLOR_BUFFER_BIT);
	bshader.Use();
	
	bshader.PreDraw(boundaryCircleImage);
	for(particle=group.list;particle;particle=particle->GetNext())
	{
		bshader.Draw(particle->position);
	}
	bshader.PostDraw();
	fluidRenderFBO.EndDrawing();

	ViewportInit();

	//framebaseFBO.BeginDrawing();
	rshader.Use();
	rshader.SetColor(color);
	rshader.Draw(fluidRenderImage);
	//framebaseFBO.EndDrawing();
}
void GameFrame::RenderBoundary(const THVector2 *arr,unsigned int count,const THVector3& color)
{
	fluidRenderFBO.BeginDrawing();
	glViewport(0,0,1024,2048);
	glClear(GL_COLOR_BUFFER_BIT);
	bshader.Use();
	bshader.PreDraw(boundaryCircleImage);
	for(unsigned int i=0;i<count;++i)
	{
		bshader.Draw(arr[i]);
	}
	bshader.PostDraw();
	fluidRenderFBO.EndDrawing();

	ViewportInit();

	//framebaseFBO.BeginDrawing();
	rshader.Use();
	rshader.SetColor(color);
	rshader.Draw(fluidRenderImage);
	//framebaseFBO.EndDrawing();
}
void GameFrame::RenderFluid(const THParticleGroup& group,const THVector3& color)
{
	THParticle *particle;

	fluidRenderFBO.BeginDrawing();
	glViewport(0,0,1024,2048);
	glClear(GL_COLOR_BUFFER_BIT);
	shader.Use();
	shader.PreDraw(circleImage);
	for(particle=group.list;particle;particle=particle->GetNext())
	{
		shader.Draw(particle->velocity,particle->position);
	}
	shader.PostDraw();
	fluidRenderFBO.EndDrawing();

	ViewportInit();

	//framebaseFBO.BeginDrawing();
	rshader.Use();
	rshader.SetColor(color);
	rshader.Draw(fluidRenderImage);
	//framebaseFBO.EndDrawing();
}

/*
void GameFrame::RenderFlame(const THFlameGroup& group)
{
	THParticle *particle;

	fluidRenderFBO.BeginDrawing();
	glViewport(0,0,1024,2048);
	glClear(GL_COLOR_BUFFER_BIT);
	shader.Use();
	shader.PreDraw(circleImage);
	for(particle=group.list;particle;particle=particle->GetNext())
	{
		shader.Draw(particle->velocity,particle->position,group.GetSizeFactor(particle->data.fdata));
	}
	shader.PostDraw();
	fluidRenderFBO.EndDrawing();

	//ViewportInit();

	framebaseFBO.BeginDrawing();
	rshader.Use();
	rshader.Draw(fluidRenderImage);
	framebaseFBO.EndDrawing();
}
*/
/*
void GameFrame::RenderSteam()
{
	THParticle *particle;

	fluidRenderFBO.BeginDrawing();
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0,0,1024,2048);
	shader.Use();
	shader.PreDraw(circleImage);
	for(particle=steamGroup.list;particle;particle=particle->GetNext())
	{
		shader.Draw(particle->velocity,particle->position,particle->temperature);
	}
	shader.PostDraw();
	fluidRenderFBO.EndDrawing();

	ViewportInit();
	srshader.Use();
	srshader.Draw(fluidRenderImage);
}
*/
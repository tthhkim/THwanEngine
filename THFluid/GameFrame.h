#ifndef TH_FLUID_GAMEFRAME
#define TH_FLUID_GAMEFRAME

#include <THPrivate.h>
#include <GameEngine\DisplayObject\THFrame.h>
#include <GameEngine/Util/THMath3D.h>
#include <GameEngine/Util/THVectorArray.h>

#include <THFluid/THFluid.h>
#include <THFluid/FluidShader.h>
#include <THFluid/THFluidParser.h>
#include <THFluid/THGroups.h>
#include <THFluid/THRopeGroup.h>
#include <THFluid/THLightGroup.h>


class GameFrame : public THFrame
{
public:
	THImage notebgImage;
	THTexture notebgTexture;
	THDisplayObject notebgObject;

	THFluidEngine engine;

	//rendering
	THVertexBuffer oneVBO;
	THImage circleImage;
	THImage boundaryCircleImage;
	THImage fluidRenderImage;
	THFrameBuffer fluidRenderFBO; 

	THFluidShader shader;
	THFluidRenderShader rshader;
	THBoundaryShader bshader;
	THBoundaryRenderShader brshader;

	THBoundaryGroup boundaryGroup;
	THWaterGroup waterGroup;
	THRopeGroup ropeGroup;
	//THFlameGroup flameGroup;

	THImage groundTileImage;
	
	GameFrame():engine(THVector2(0.0f,-5.0f))
	{
	}
	void LoadTextures();
	void LoadGroups();
	void LoadCircleImage();
	void LoadRender();
	void LoadOneVBO();
	void Load()
	{
		LoadOneVBO();
		LoadCircleImage();
		LoadRender();
		SetViewport(THVector2(0.0f,0.0f),THVector2(7.2f,12.8f));

		LoadTextures();
	}

	void SetViewport(const THVector2& minp,const THVector2& maxp);
	void ParseFile();
	void Action(THFrame* callFrame,void* data)
	{
		engine.Load(THVector2(-0.0f,-0.0f),THVector2(7.2f,12.8f),0.80f);
		engine.LoadCellMap(54,96);
		LoadGroups();

		ParseFile();

		engine.TestStep(1.0f/100.0f,10);
	}

	void OnEnterFrame()
	{
		const THTimeStep step(THDeltaTime);
		ropeGroup.Step(step.dt_inv);
		engine.Step(step);

		//float theta=THRandf(0.0f,TH_2PI);
		//engine.AddParticle(&waterGroup,THVector2(1.0f,10.0f)+THRandf(0.0f,1.0f)*THVector2(cosf(theta),sinf(theta)));
		
		//lightmanager.ParticleTest(&waterGroup);
		//eball.Step();

		//flameGroup.Step(THDeltaTime);
		//flameGroup.CreateNew(THVector2(3.6f,4.0f));
		//flameGroup.CreateNew(THVector2(3.6f,4.0f));
	}

	void RenderBoundary();
	void RenderFluid(const THParticleGroup& group,const THVector3& color);
	//void RenderSteam();
	//void RenderFlame(const THFlameGroup& group);
	void Draw()
	{
		//DrawObjects(0,1);

		oneVBO.BeginDrawing();


		RenderFluid(waterGroup,THVector3(1.0f,1.0f,1.0f));
		//RenderFlame(flameGroup);
		//RenderBoundary();
		RenderFluid(boundaryGroup,THVector3(0.0f,0.0f,0.0f));


		oneVBO.EndDrawing();

		engine.DebugDraw();
	}

	void OnTouchDown(const THVector2& p)
	{
		ropeGroup.OnTouchDown(p);
	}
	void OnTouchMove(const THVector2& p,const THVector2& delta)
	{
		if(ropeGroup.OnTouchMove(p)==false)
		{
			//engine.AddParticle(&waterGroup,p+THVector2(THRandf(-0.3f,0.3f),THRandf(-0.3f,0.3f)));
			//engine.AddParticle(&waterGroup,p+THVector2(THRandf(-0.3f,0.3f),THRandf(-0.3f,0.3f)));
		}

		//boundaryGroup.Create(p,0.3f);
		//boundaryGroup.Delete(p,0.4f);
		//bridgeGroup.AddNew(THVector2(x,y));
	}
	void OnTouchUp(const THVector2& p)
	{
		ropeGroup.OnTouchUp(p);
	}
};

#endif
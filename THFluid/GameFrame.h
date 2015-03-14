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
#include <THFluid/THLightGroup.h>

class GameFrame : public THFrame
{
public:
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
	THParticleRope ropeGroup;
	//THFlameGroup flameGroup;

	THImage groundTileImage;

	THParticle *mparticle,*mparticle2;
	
	GameFrame():engine(THVector2(0.0f,-5.0f))
	{
	}
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
	}

	void SetViewport(const THVector2& minp,const THVector2& maxp);
	void ParseFile();
	void Action(THFrame* callFrame,void* data)
	{
		engine.Load(THVector2(-0.0f,-0.0f),THVector2(7.2f,12.8f),0.80f);
		LoadGroups();

		ParseFile();
		//ParseLevelFromFile("D:/lvd.dat");

		engine.TestStep(1.0f/100.0f,10);
	}

	THVector2 mp,mp2;
	void OnEnterFrame()
	{
		/*
		mparticle->force+=20000.0f*(mp-mparticle->position);
		mparticle2->force+=20000.0f*(mp2-mparticle2->position);
		ropeGroup.Step(200.0f);
		*/
		engine.Step(THDeltaTime);
		
		//lightmanager.ParticleTest(&waterGroup);
		//eball.Step();

		//flameGroup.Step(THDeltaTime);
		//flameGroup.CreateNew(THVector2(3.6f,4.0f));
		//flameGroup.CreateNew(THVector2(3.6f,4.0f));
	}

	void RenderBoundary();
	void RenderFluid(const THParticleGroup& group);
	//void RenderSteam();
	//void RenderFlame(const THFlameGroup& group);
	void Draw()
	{
		oneVBO.BeginDrawing();


		RenderFluid(waterGroup);
		//RenderFlame(flameGroup);
		RenderBoundary();


		oneVBO.EndDrawing();

		//engine.DebugDraw();
	}

	void OnTouchDown(const THVector2& p)
	{
		//mp=p;
	}
	void OnTouchMove(const THVector2& p,const THVector2& delta)
	{
		//mp=p;
		//boundaryGroup.Delete(p,0.4f);
		//bridgeGroup.AddNew(THVector2(x,y));
	}
	void OnTouchUp(const THVector2& p)
	{
	}
};

#endif
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

	THImage endpointImage;
	THTexture endpointTexture;

	THFluidEngine engine;
	THEndPoint endpoint;

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
	THEndPointShader eshader;

	THBoundaryGroup boundaryGroup;
	THWaterGroup waterGroup;
	THRopeGroup ropeGroup;
	THRopeDampGroup ropedampGroup;
	THVector2 emitPoint;

	//THFlameGroup flameGroup;

	
	
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
		//engine.LoadCellMap(54,96);
		LoadGroups();

		ParseFile();
		//engine.Step(0.1f);
		const THVector2 grav=engine.gravity*0.001f;
		for(unsigned int i=0;i<40;++i)
		{
			ropeGroup.TestStep(grav);
			ropeGroup.Step(100.0f);
		}

		//engine.TestStep(1.0f/100.0f,10);
	}

	void OnEnterFrame()
	{
		waterGroup.Create(emitPoint,0.5f);

		const THTimeStep step(THDeltaTime);
		ropeGroup.Step(step.dt_inv);
		endpoint.Step();
		engine.Step(step);
		//endpoint.Refresh();

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
	void RenderBoundary(const THParticleGroup& group,const THVector3& color);
	void RenderBoundary(const THVector2 *arr,unsigned int count,const THVector3& color);
	void RenderEndPoint();
	//void RenderSteam();
	//void RenderFlame(const THFlameGroup& group);
	void Draw()
	{
		PreDraw();
		DrawObjects(0,1);
		DrawTexture(endpoint.position,THVector2(0.5f,0.5f),THVector2(TH_ENDPOINT_RADIUS*0.1f,TH_ENDPOINT_RADIUS*0.10f),THRot2(),&endpointTexture);
		PostDraw();

		oneVBO.BeginDrawing();


		RenderFluid(waterGroup,THVector3(0.0f,0.0f,0.0f));
		//RenderFlame(flameGroup);
		//RenderBoundary();
		RenderFluid(boundaryGroup,THVector3(0.0f,0.0f,0.0f));
		RenderBoundary(ropedampGroup,THVector3(0.0f,0.0f,0.0f));
		RenderBoundary(ropeGroup,THVector3(0.0f,0.0f,0.0f));
		const THVector2Array& rtemparr=ropeGroup.GetTempArr();
		RenderBoundary(rtemparr.arr,rtemparr.num,THVector3(0.3f,0.3f,0.3f));

		RenderEndPoint();
		//RenderBoundary(endpoint,THVector3(0.4f,0.4f,0.4f));


		oneVBO.EndDrawing();

		//engine.DebugDraw();
	}

	void OnTouchDown(const THVector2& p)
	{
		ropeGroup.OnTouchDown(p);
	}
	void OnTouchMove(const THVector2& p,const THVector2& delta)
	{
		ropeGroup.OnTouchMove(p);

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
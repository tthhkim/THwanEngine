#include "GameFrame.h"

void GameFrame::LoadGroups()
{
	boundaryGroup.Load();
	waterGroup.Load();
	ropeGroup.Load(64);

	
	engine.AddParticleGroup(&boundaryGroup);
	engine.AddParticleGroup(&waterGroup);
	engine.AddParticleGroup(&ropeGroup);
	//engine.AddParticleGroup(&iceGroup);
	//engine.AddParticleGroup(&steamGroup);
	//engine.AddParticleGroup(&heatGroup);
	//engine.AddParticleGroup(&endGroup);
	//engine.AddParticleGroup(&wheelGroup);
	//engine.AddParticleGroup(&bridgeGroup);
	//engine.AddParticleGroup(&bridgelGroup);
	//engine.AddParticleGroup(&flameGroup);
	//engine.AddParticleGroup(&cshape);
	//engine.AddParticleGroup(&bshape);
	//engine.AddParticleGroup(&eball);

	//engine.LoadCellMap(154,154);
	
	//flameGroup.SetFlame(0.8f,1.3f);
}
void GameFrame::SetViewport(const THVector2& minp,const THVector2& maxp)
{
	shader.Use();
	shader.SyncProjection(minp,maxp);
	bshader.Use();
	bshader.SyncProjection(minp,maxp);
	//lightmanager.SyncProjection(minp,maxp);
}
void GameFrame::ParseFile()
{
	THFluidParser *parser=new THFluidParser;
	parser->AddSet(0,1,&boundaryGroup,1,true);
	parser->AddSet(10,11,&waterGroup,2);
	parser->AddSet(20,21,&waterGroup,2);

	size_t iw,ih;
	int colorType=TH_PNG_GREY;
	unsigned char *data=LoadImageBuffer("D:/map2.png",iw,ih,colorType);

	parser->Parse(&engine,data,iw,ih);

	free(data);
	delete parser;

	/*
	THVector2Array varr0(3);
	THVector2Array varr(64);

	varr0.Push(THVector2(3.6f,10.0f));
	varr0.Push(THVector2(3.6f,5.0f));
	varr0.Bridge(varr,0.2f);

	

	THParticlePair pair;
	ropeGroup.LoadParticles(varr.arr,varr.num,&pair);
	varr-=THVector2(0.2f,0.0f);
	ropeGroup.LoadParticles(varr.arr,varr.num,0);
	ropeGroup.LoadSprings(0.6f);
	mparticle=pair.p1;
	mparticle2=pair.p2;

	mp.Set(1.00f,3.5f);
	mp2.Set(6.2f,3.5f);
	*/
}
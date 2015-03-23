#include "GameFrame.h"



void GameFrame::LoadGroups()
{
	boundaryGroup.Load();
	waterGroup.Load();
	ropeGroup.Load(6);

	
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
	//parser->AddSet(0,1,&boundaryGroup,1,true);
	parser->AddSet(10,&waterGroup,2);
	parser->AddSet(20,&waterGroup,2);

	size_t iw,ih;
	int colorType=TH_PNG_GREY;
	unsigned char *data=LoadImageBuffer("map2.png",iw,ih,colorType);

	//parser->Parse(&engine,data,iw,ih);

	free(data);
	delete parser;

	THRopeHanger *h1=ropeGroup.NewHanger();
	h1->position.Set(0.2f,6.4f);

	THRopeHanger *h2=ropeGroup.NewHanger();
	h2->position.Set(7.0f,6.4f);

	ropeGroup.MakeRope(h1,h2,1.4f);
}
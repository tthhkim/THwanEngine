#include "GameFrame.h"



void GameFrame::LoadGroups()
{
	boundaryGroup.Load();
	waterGroup.Load();
	ropeGroup.Load(64);
	ropedampGroup.Load();
	//endpoint.Load();
	stone.Load();
	clickgroups.Load(3);
	clickgroup.Load();
	THClickGroup *g=new THClickGroup;
	g->Load();
	
	clickgroups.Push(g);

	
	engine.AddParticleGroup(&boundaryGroup);
	engine.AddParticleGroup(&waterGroup);
	engine.AddParticleGroup(&ropeGroup);
	engine.AddParticleGroup(&ropedampGroup);
	//engine.AddParticleGroup(&endpoint);
	engine.AddParticleGroup(&stone);
	engine.AddParticleGroup(&clickgroup);
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
	THFluidParser parser;
	parser.AddSet(0,&boundaryGroup,1,true);
	parser.AddSet(10,&stone);
	parser.AddSet(30,&clickgroup,1,true);
	//parser.AddSet(30,&waterGroup,2);
	//parser.AddSet(30,&waterGroup,2);
	//parser.AddSet(20,&varr);
	//parser.AddSet(10,&ropedampGroup,1,true);
	//parser.AddSet(40,&endpoint.position);
	//parser.AddSet(50,&emitPoint);

	size_t iw,ih;
	int colorType=TH_PNG_GREY;
	unsigned char *data=LoadImageBuffer("map5.png",iw,ih,colorType);

	parser.Parse(&engine,data,iw,ih);

	free(data);

	stone.CalculateMass();
	stone.CalculateInertia();

	//ropeGroup.LoadRope(varr.arr[0],varr.arr[1],1.4f);
	//delete parser;

	/*
	THRopeHanger *h1=ropeGroup.NewHanger();
	h1->position.Set(0.2f,6.4f);

	THRopeHanger *h2=ropeGroup.NewHanger();
	h2->position.Set(7.0f,6.4f);

	ropeGroup.MakeRope(h1,h2,1.4f);
	*/
}
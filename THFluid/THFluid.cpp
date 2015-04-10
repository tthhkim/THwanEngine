#include "THFluid.h"


void THFluidEngine::Load(const THVector2& ming,const THVector2& maxg,float smoothingLength)
{
	smoothLength=smoothingLength;
	smoothLengthSq=smoothingLength*smoothingLength;
	smoothLengthInv=1.0f/smoothingLength;

	m_mingrid=ming;
	m_maxgrid=maxg;
	m_outposition=2.0f*maxg-ming;
	//gridSize=size;

	const THVector2 gSize=(maxg-ming)*smoothLengthInv;
	gridWidth=(int)ceilf(gSize.x);
	gridHeight=(int)ceilf(gSize.y);
	gridCount=gridWidth*gridHeight;
	//gcmgh=gridCount-gridHeight;
	THLog("Fluid GridSet : %d , %d",gridWidth,gridHeight);

	gbuf=(THGrid*)malloc(sizeof(THGrid)*(gridCount+1));
	grids=(THGrid**)malloc(sizeof(THGrid*)*gridWidth);

	int i,j;
	for(i=0;i<gridWidth;++i)
	{
		grids[i]=gbuf+(i*gridHeight);
		for(j=0;j<gridHeight;++j)
		{
			grids[i][j].Init(i,j);
		}
	}
	outgrid=gbuf + gridCount;
	int cx,cy;
	GetGridPos(m_outposition,cx,cy);
	outgrid->Init(cx,cy);
}
/*
void THFluidEngine::LoadCellMap(int w,int h)
{
	m_cellWidth=w;
	m_cellHeight=h;

	m_cellMapScale=THVector2((float)w,(float)h)/(m_maxgrid-m_mingrid);

	const size_t size=m_cellWidth*m_cellHeight;
	m_cellCount=size;
	m_cellBuffer=(THCell*)malloc(sizeof(THCell)*size);
		
	const THVector2 invBoundaryMapScale=1.0f/m_cellMapScale;
	int j;
	for(int i=0;i<m_cellWidth;++i)
	{
		//boundaryMap[i]=boundaryBuffer+(boundaryHeight*i);

		for(j=0;j<m_cellHeight;++j)
		{
			THCell& bound=m_cellBuffer[GetCellIndex(i,j)];
			bound.particle=0;
			//bound.layer=0xffffffff;
			bound.position.Set
				(
				((float)i+0.5f)*invBoundaryMapScale.x + m_mingrid.x,
				((float)j+0.5f)*invBoundaryMapScale.y + m_mingrid.y
				);
		}
	}
	//memset(boundaryBuffer,0,sizeof(THBoundaryType)*size);
}
*/
void THFluidEngine::FreeAll()
{
	if(gbuf)
	{
		free(gbuf);
		free(grids);

		gbuf=0;
		grids=0;
	}
	/*
	if(m_cellBuffer)
	{
		free(m_cellBuffer);
		m_cellBuffer=0;
	}
	*/
	for(unsigned int i=0;i<groups.num;++i)
	{
		THParticleGroup *group=groups.arr[i];
		group->FreeAll();
		delete group;
	}
	groups.Free();
}
void THFluidEngine::AddParticleGroup(THParticleGroup *group)
{
	group->m_groupindex=groups.num;
	groups.Push(group);
	group->m_engine=this;
}
THParticle* THFluidEngine::NewParticle(THParticleGroup *group)
{
	if(group->m_autoremove)
	{
		THLog("New Particle : Position Test Failed");
		return 0;
	}else
	{
		THParticle *particle=new THParticle;
		assert(particle);
		outgrid->Add(particle);
		group->Add(particle);
		particle->position=m_outposition;
		return particle;
	}
}
void THFluidEngine::ParticleOut(THParticle *p)
{
	p->grid->Remove(p);
	outgrid->Add(p);
	p->position=m_outposition;
}
THParticle* THFluidEngine::AddParticle(THParticleGroup *group,const THVector2& p)
{
	if(OutOfBound(p))
	{
		THLog("New Particle : Position Test Failed");
		return 0;
	}
	int cx,cy;
	GetGridPos(p,cx,cy);
	THParticle *particle=new THParticle;
	assert(particle);
	grids[cx][cy].Add(particle);
	group->Add(particle);
	particle->position=p;

	return particle;
}

void THFluidEngine::QueryAABB(const THVector2& minBound,const THVector2& maxBound,THParticleFilter layer,THParticleQuery *callback,void *data)
{
	int minx,miny,maxx,maxy;
	GetGridPos(minBound,minx,miny);
	GetGridPos(maxBound,maxx,maxy);
	GetGridIndexClamped(minx,miny,maxx,maxy);

	THParticle *particle,*np;
	int tx,ty;
	for(tx=minx;tx<=maxx;++tx)
	{
		for(ty=miny;ty<=maxy;++ty)
		{
			particle=grids[tx][ty].list;
			while(particle)
			{
				np=particle->gnext;
				if(layer&particle->group->layer)
				{
					const THVector2& p=particle->position;
					if(p.x>minBound.x && p.y>minBound.y && p.x<maxBound.x && p.y<maxBound.y)
					{
						if(callback->QueryCallback(particle,data)==false){return;}
					}
				}
				
				particle=np;
			}
		}
	}
}
void THFluidEngine::QueryCircle(const THVector2& position,float radius,THParticleFilter layer,THParticleQuery *callback,void *data)
{
	int minx,miny,maxx,maxy;
	GetGridPos(THVector2(position.x-radius,position.y-radius),minx,miny);
	GetGridPos(THVector2(position.x+radius,position.y+radius),maxx,maxy);
	
	GetGridIndexClamped(minx,miny,maxx,maxy);

	const float rsq=radius*radius;
	THParticle *particle,*np;
	int tx,ty;
	for(tx=minx;tx<=maxx;++tx)
	{
		for(ty=miny;ty<=maxy;++ty)
		{
			particle=grids[tx][ty].list;
			while(particle)
			{
				np=particle->gnext;
				if(layer&particle->group->layer)
				{
					if( (particle->position - position).LengthSquared()<rsq )
					{
						if(callback->QueryCallback(particle,data)==false){return;}
					}
				}
				particle=np;
			}
		}
	}
}
/*
void THFluidEngine::QueryCellCircle(const THVector2& position,float radius,THCellQuery *callback,void *data)
{
	int minx,miny,maxx,maxy;
	GetCellPosition(THVector2(position.x-radius,position.y-radius),minx,miny);
	GetCellPosition(THVector2(position.x+radius,position.y+radius),maxx,maxy);
	
	minx=minx<0?0:minx; maxx=maxx<m_cellWidth-1?maxx:m_cellWidth-1;
	miny=miny<0?0:miny; maxy=maxy<m_cellHeight-1?maxy:m_cellHeight-1;

	const float rsq=radius*radius;
	int tx,ty;
	for(tx=minx;tx<=maxx;++tx)
	{
		for(ty=miny;ty<=maxy;++ty)
		{
			THCell& cell=m_cellBuffer[GetCellIndex(tx,ty)];
			if((cell.position-position).LengthSquared()<rsq)
			{
				if(callback->QueryCellCallback(&cell,data)==false){return;}
			}
		}
	}
}
*/
void THFluidEngine::FixGridAndInit()
{
	THParticle *particle,*nextp;
	THParticleGroup *group;
	float rk;
	for(unsigned int i=0;i<groups.num;++i)
	{
		group=groups.arr[i];

		particle=group->list;
		THVector2 gforce=gravity*(group->m_mass.m_mass*group->m_gravityscale);
		rk=group->m_resistance;
		while(particle)
		{
			//Init
			particle->p=0.0f;
			particle->pnear=0.0f;
			particle->force+=gforce-particle->velocity*rk;

			particle=particle->next;
		}
		//if(group->m_isStatic){continue;}

		particle=group->list;
		while(particle)
		{
			nextp=particle->next;

			//Fixing Grid
			int cx,cy;
			GetGridPos(particle->position,cx,cy);

			if(cx<0 || cy<0 || cx>=gridWidth || cy>=gridHeight)
			{
				if(particle->group->m_destructionListener)
				{
					particle->group->m_destructionListener->ParticleDestruct(particle);
				}
				particle->grid->Remove(particle);
				if(group->m_autoremove)
				{
					particle->group->Remove(particle);
					delete particle;
				}
				else{outgrid->Add(particle);}
				
				particle=nextp;
				continue;
				//Á¦°Å
			}
			if( cx!=particle->grid->x || cy!=particle->grid->y )
			{
				particle->grid->Remove(particle);
				grids[cx][cy].Add(particle);
			}
	

			particle=nextp;
		}


	}
}
void THFluidEngine::CheckNeighbor(THParticle *p1,THParticle *p2)
{
	float temp,q,q2;
	THVector2 rel;

	if((p1->group->layer&p2->group->layer)==0){return;}
	
	
	rel=p2->position-p1->position;
	q=rel.LengthSquared();
	if(q>smoothLengthSq || q==0.0f){return;}

	q=(rel.Normalize()*smoothLengthInv);
	if(p1->group->collisionListener && p1->group->collisionListener->ParticleCollide(p1,p2,q)){return;}
	if(p2->group->collisionListener && p2->group->collisionListener->ParticleCollide(p2,p1,q)){return;}
	q=1.0f-q;
	q2=q*q;

	temp=q2*q*p2->group->m_mass.m_mass;
	p1->p+=temp;
	p1->pnear+=temp*q;
	temp=q2*q*p1->group->m_mass.m_mass;
	p2->p+=temp;
	p2->pnear+=temp*q;

	if(p1->group==p2->group && p1->group->m_collideEach==false){return;}
	if(p1->group->m_mass.m_isstatic && p2->group->m_mass.m_isstatic){return;}

	const THNeighbor neighbor={p1,p2,rel,q,q2};
	neighbors.Push(neighbor);
}
void THFluidEngine::GetNeighborsGrid(const THGrid& g1,const THGrid& g2)
{
	if(g1.list==0){return;}
	THParticle *p1=g1.list,*p2,*p1n,*p2n;

	while(p1)
	{
		p1n=p1->gnext;

		p2=g2.list;
		while(p2)
		{
			p2n=p2->gnext;

			CheckNeighbor(p1,p2);

			p2=p2n;
		}


		p1=p1n;
	}
}
void THFluidEngine::GetNeighborsGrid2(const THGrid& g)
{
	THParticle *p1=g.list,*p2,*p1n,*p2n;

	while(p1)
	{
		p1n=p1->gnext;

		p2=p1n;
		while(p2)
		{
			p2n=p2->gnext;

			CheckNeighbor(p1,p2);

			p2=p2n;
		}

		p1=p1n;
	}
}
void THFluidEngine::GetNeighbors()
{
	neighbors.Clear();

	bool xbig,ybig;
	for(unsigned int i=0;i<gridCount;++i)
	{
		THGrid& grid=gbuf[i];
		if(grid.list==0){continue;}

		xbig=grid.x!=0;
		ybig=grid.y!=0;

		GetNeighborsGrid2(grid);
		if(xbig)
		{
			GetNeighborsGrid(grids[grid.x-1][grid.y],grid);
			if(ybig)
			{
				GetNeighborsGrid(grids[grid.x-1][grid.y-1],grid);
			}
		}
		if(ybig)
		{
			GetNeighborsGrid(grids[grid.x][grid.y-1],grid);
			if(grid.x<gridWidth-1)
			{
				GetNeighborsGrid(grids[grid.x+1][grid.y-1],grid);
			}
		}
	}

	for(unsigned int j=0;j<groups.num;++j)
	{
		groups.arr[j]->CalculatePressure();
	}

}

void THFluidEngine::DoubleDensityRelaxation(const THTimeStep& step)
{
	THVector2 d;
	for(unsigned int i=0;i<neighbors.num;++i)
	{
		THNeighbor& neighbor=neighbors.arr[i];

		d=(0.5f*((neighbor.p1->p+neighbor.p2->p)*neighbor.q + (neighbor.p1->pnear+neighbor.p2->pnear)*neighbor.q2)   )*neighbor.one2two;

		neighbor.p1->force-=d;
		neighbor.p2->force+=d;

		neighbor.one2two.Skew();
		d=neighbor.p2->velocity-neighbor.p1->velocity;
		d.FastNormalize();
		d=THDot(neighbor.one2two,d)*neighbor.one2two;
		d*=neighbor.q2*sqrtf(neighbor.p1->group->m_friction*neighbor.p2->group->m_friction);

		neighbor.p1->force+=d;
		neighbor.p2->force-=d;
		

		//d=neighbor.p1->velocity-neighbor.p2->velocity;
		//neighbor.p1->force-=d*neighbor.p1->group->viscosity*neighbor.q;
		//neighbor.p2->force+=d*neighbor.p2->group->viscosity*neighbor.q;
	}
}
void THFluidEngine::ApplyForceAndAdvect(const THTimeStep& step)
{
	for(unsigned int i=0;i<groups.num;++i)
	{
		groups.arr[i]->ApplyForceAndAdvect(step);
	}
}
void THFluidEngine::TestStep(float dt,unsigned int count)
{
	THVector2 tg=gravity;
	gravity.SetZero();

	for(unsigned int i=0;i<count;++i)
	{
		Step(dt);
	}
	gravity=tg;
}


void THFluidEngine::DebugDraw()
{
	THDefaultProgram.Use();
	
	THDefaultProgram.SetColorAdd(0.1f,0.4f,1.0f,1.0f);
	THDefaultProgram.SetColorMultiply(0.0f,0.0f,0.0f,0.0f);

	glVertexAttrib2f(THDefaultProgram.rotationHandler,1.0f,0.0f);
	glVertexAttrib2f(THDefaultProgram.centerHandler,0.0f,0.0f);
	glVertexAttrib2f(THDefaultProgram.scaleHandler,1.0f,1.0f);

	const GLfloat verts[]={0.0f,0.0f};
	glEnableVertexAttribArray(THDefaultProgram.vertexHandler);
	glVertexAttribPointer(THDefaultProgram.vertexHandler,2,GL_FLOAT,GL_FALSE,0,verts);
	

	THParticle *particle;

	for(unsigned int j=0;j<groups.num;++j)
	{
		particle=groups.arr[j]->list;

		while(particle)
		{
			glVertexAttrib2f(THDefaultProgram.positionHandler,particle->position.x,particle->position.y);
			glDrawArrays(GL_POINTS, 0, 1);
			particle=particle->next;
		}
	}

	glDisableVertexAttribArray(THDefaultProgram.vertexHandler);
	
}

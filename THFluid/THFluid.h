#ifndef THFLUID_H
#define THFLUID_H

#include <THFluid/THFluidObject.h>
#include <THFluid/THParticleCallbacks.h>

#include <THPrivate.h>
#include <GameEngine/Util/THMath3D.h>
#include <GameEngine/Util/THArray.h>

#include <string.h>

#define NEIGHBORS_INITIAL 500

class THFluidParser;

class THFluidEngine
{
	friend class THFluidParser;
public:
	THArray<THParticleGroup*> groups;
	THGrid **grids;
	int gridWidth,gridHeight;
	THVector2 gravity;
	THArray<THNeighbor> neighbors;

	THFluidEngine(const THVector2& _gravity):neighbors(NEIGHBORS_INITIAL),groups(10)
	{
		gravity=_gravity;

		gbuf=0;
		grids=0;


		//m_cellBuffer=0;
	}
	void AddParticleGroup(THParticleGroup *group);
	THParticle* NewParticle(THParticleGroup *group);
	void ParticleOut(THParticle *p);
	THParticle* AddParticle(THParticleGroup *group,const THVector2& p);
	void RemoveParticle(THParticle *p);
	inline void GetGridPos(const THVector2& position,int& cx,int& cy)
	{
		cx=(int)floorf((position.x-m_mingrid.x)*smoothLengthInv);
		cy=(int)floorf((position.y-m_mingrid.y)*smoothLengthInv);
	}
	inline bool OutOfBound(const THVector2& p)
	{
		return p.x<m_mingrid.x || p.x>m_maxgrid.x || p.y<m_mingrid.y || p.y>m_maxgrid.y;
	}
	inline void GetGridIndexClamped(int& minx,int& miny,int& maxx,int& maxy)
	{
		minx=minx<0?0:minx; maxx=maxx<gridWidth-1?maxx:gridWidth-1;
		miny=miny<0?0:miny; maxy=maxy<gridHeight-1?maxy:gridHeight-1;
	}
	inline const THVector2& GetMinGrid() const
	{
		return m_mingrid;
	}
	inline const THVector2& GetMaxGrid() const
	{
		return m_maxgrid;
	}
	
	/*
	inline void GetCellPosition(const THVector2& pos,int& x,int& y)
	{
		x=(int)((pos.x-minGrid.x)*m_cellMapScale.x);
		y=(int)((pos.y-minGrid.y)*m_cellMapScale.y);
	}
	inline int GetCellIndex(const THVector2& pos)
	{
		return (int)((pos.x-minGrid.x)*m_cellMapScale.x)*m_cellHeight + (int)((pos.y-minGrid.y)*m_cellMapScale.y);
	}
	inline int GetCellIndex(int x,int y)
	{
		return x*m_cellHeight + y;
	}
	
	inline unsigned int GetCellCount() const
	{
		return m_cellCount;
	}
	*/

	void QueryAABB(const THVector2& minBound,const THVector2& maxBound,THParticleFilter layer,THParticleQuery *callback,void *data);
	void QueryCircle(const THVector2& position,float radius,THParticleFilter layer,THParticleQuery *callback,void *data);
	//void QueryCellCircle(const THVector2& position,float radius,THCellQuery *callback,void *data);

	void Load(const THVector2& ming,const THVector2& maxg,float smoothingLength);
	//void LoadCellMap(int w,int h);
	void FreeAll();

	inline void Step(float dt)
	{
		const THTimeStep step(dt);
		FixGridAndInit();
		GetNeighbors();
		DoubleDensityRelaxation(step);
		ApplyForceAndAdvect(step);
		
	}
	void TestStep(float dt,unsigned int count);
	


	
	void DebugDraw();
protected:
	THGrid *gbuf,*outgrid;
	THVector2 m_mingrid,m_maxgrid,m_outposition;
	unsigned int gridCount;


	float smoothLength,smoothLengthSq,smoothLengthInv;

	/*
	int m_cellWidth,m_cellHeight;
	unsigned int m_cellCount;
	THVector2 m_cellMapScale;
	THCell *m_cellBuffer;
	*/
	
	void FixGridAndInit();
	void CheckNeighbor(THParticle *p1,THParticle *p2);
	void GetNeighborsGrid(const THGrid& g1,const THGrid& g2);
	void GetNeighborsGrid2(const THGrid& g);
	void GetNeighbors();
	void DoubleDensityRelaxation(const THTimeStep& step);
	void ApplyForceAndAdvect(const THTimeStep& step);
	

	
};

#endif
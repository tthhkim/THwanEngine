#ifndef TH_PARTICLE_CALLBACKS
#define TH_PARTICLE_CALLBACKS

class THParticle;
class THCell;
class THParticleQuery
{
public:
	virtual bool QueryCallback(THParticle *particle,void *data)=0;
};
class THCellQuery
{
public:
	virtual bool QueryCellCallback(THCell *cell,void *data)=0;
};
class THParticleCollisionListener
{
public:
	//return 0 normal (both temperature & force)
	//return 1 not add to neighbor list
	virtual int ParticleCollide(THParticle *p1,THParticle *p2,float fraction)=0;
};

#endif
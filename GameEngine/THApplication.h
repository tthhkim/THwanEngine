#ifndef TH_APPLICATION_H
#define TH_APPLICATION_H

#include <THSettings.h>
#include <assert.h>

#include <GameEngine/Util/THArray.h>
#include <GameEngine/Util/THMath.h>
#include <GameEngine/Util/THMath3D.h>

#include <GameEngine/Rendering/THDrawing.h>
#include <GameEngine/Rendering/THBuffer.h>

class THFrame;

//Timer Function Definition
class THTimerDef : public THLinkedNode
{
public:
	void (*action)(void* data);
	float timeSkip;
	void* data;
};
class THApplication
{
public:
	void *data;
	float m_dt;

	THApplication();
	void Start();

	inline void ViewportInit(){glViewport(0,0,m_windowWidthi,m_windowHeighti);}
	void SetOrtho(const THVector2& minp,const THVector2& maxp);

	void Loop();

	void SetWindowSize(int w,int h);
	void GLInit();
	void OnCreate();
	void OnSurfaceCreated();
	void OnResume();
	void OnPause();
	void OnDestroy();

	void SetFrameRate(float _frameRate);

	void GoFrame(THFrame* f,void* data=0);
	void AddTimer(float _timeSkip,void (*_action)(void*),void* _data);
	inline void SetRunning(bool r){m_isrunning=r;}

	void OnTouchDown(const THVector2& p);
	void OnTouchMove(const THVector2& p);
	void OnTouchUp(const THVector2& p);
	int OnBackReleased();
	void OnKeyUp(int k);
	void OnKeyDown(int k);

	inline THVector2& GetGameScale(){return m_gameScale;}
	inline THVector2& GetGameBoundMin(){return m_gameMinBound;}
	inline THVector2& GetGameBoundMax(){return m_gameMaxBound;}
	inline GLfloat *GetProjectionMatrix(){return THProjectMatrix;}
	inline THVector2& GetWindowSize(){return m_windowSize;}
	inline THVertexBuffer& GetOneVBO(){return m_oneVBO;}
protected:
	bool m_isrunning;
	long long m_lastMicroSec;
	long long m_minTimeGap,m_maxTimeGap;

	//GameSize
	THVector2 m_gameScale;
	THVector2 m_gameMinBound,m_gameMaxBound;

	//WindowSize
	THVector2 m_windowSize;
	GLsizei m_windowWidthi,m_windowHeighti;

	GLfloat THProjectMatrix[6];

	THVertexBuffer m_oneVBO;

	THFrame* m_currentFrame;

	THLinkedList m_timerlist;

	

	inline float getGameX(float _x){return _x*m_gameScale.x + m_gameMinBound.x;}
	inline float getGameY(float _y){return -_y*m_gameScale.y + m_gameMaxBound.y;}

	void OnDrawFrame();
	void OnEnterFrame();
	void SwapBuffer();
};

#endif
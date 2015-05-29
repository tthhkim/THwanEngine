#ifndef TH_APPLICATION_H
#define TH_APPLICATION_H

#include <THSettings.h>
#include <assert.h>

#include <GameEngine/Util/THArray.h>
#include <GameEngine/Util/THMath.h>
#include <GameEngine/Util/THMath3D.h>

#include <GameEngine/Rendering/THDrawing.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

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
	THApplication();
	void Start();

	inline void ViewportInit(){glViewport(0,0,m_windowWidthi,m_windowHeighti);}
	void SetOrtho(const THVector2& minp,const THVector2& maxp);
	void EGLInit(EGLNativeWindowType window);
	void TermDisplay();
	void SetEGL(int glesversion,int depthsize);
	void Loop();

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
	void OnKeyUp(unsigned int k);
	void OnKeyDown(unsigned int k);
protected:
	bool m_isrunning;
	long long m_lastMicroSec;
	long long m_minTimeGap,m_maxTimeGap;

	//EGL Variables
	EGLDisplay m_eglDisplay;
	EGLSurface m_eglSurface;
	EGLContext m_eglContext;
	EGLConfig m_eglConfig;
	int m_glesversion,m_egldepthsize;

	//GameSize
	THVector2 m_gameScale;
	THVector2 m_gameMinBound;
	THVector2 m_gameMaxBound;

	//WindowSize
	THVector2 m_windowSize;
	GLsizei m_windowWidthi;
	GLsizei m_windowHeighti;

	GLfloat THProjectMatrix[6];

	THVertexBuffer m_zerooneVBO;

	THFrame* m_currentFrame;

	THLinkedList m_timerlist;

	float m_dt;

	inline float getGameX(float _x){return _x*m_gameScale.x + m_gameMinBound.x;}
	inline float getGameY(float _y){return -_y*m_gameScale.y + m_gameMaxBound.y;}
	
	void InitZeroOneVBO();

	void EGLInitDisplay();
	void EGLChooseConfig();
	//platform independent
	void EGLInitSurface(EGLNativeWindowType window);
	void EGLMakeContext();

	void OnDrawFrame();
	void OnEnterFrame();
};

#endif
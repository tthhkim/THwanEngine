#ifndef TH_EGL_WRAPPER_H
#define TH_EGL_WRAPPER_H

#include <THSettings.h>

#if THPLATFORM==THPLATFORM_ANDROID

#define DEPTH_SIZE_BIT 0
#define GLES_VERSION 3

#include <EGL/egl.h>

class THEGLWrapper
{
public:
	THEGLWrapper();

	void EGLInit(EGLNativeWindowType window);
	void GetFrameBufferSize(int *w,int *h);
	void EGLInitDisplay();
	void EGLChooseConfig();
	//platform independent
	void EGLInitSurface(EGLNativeWindowType window);
	void EGLMakeContext();
	void TermDisplay();
protected:
	//EGL Variables
	EGLDisplay m_eglDisplay;
	EGLSurface m_eglSurface;
	EGLContext m_eglContext;
	EGLConfig m_eglConfig;
};

#endif

#endif
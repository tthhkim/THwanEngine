#include "THEGLWrapper.h"
#include <THPrivate.h>

#if THPLATFORM==THPLATFORM_ANDROID

THEGLWrapper::THEGLWrapper()
{
	//EGL Variables
	m_eglDisplay=0;
	m_eglSurface=0;
	m_eglContext=0;
	m_eglConfig=0;
}

void THEGLWrapper::EGLInit(EGLNativeWindowType window)
{
	EGLInitDisplay();
	EGLChooseConfig();
	EGLInitSurface(window);
	EGLMakeContext();
}
void THEGLWrapper::EGLInitDisplay()
{
	m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint eglMajor,eglMinor;
    eglInitialize(m_eglDisplay, &eglMajor,&eglMinor);
	THLog("EGL Initialization : %d.%d",eglMajor,eglMinor);
}
void THEGLWrapper::EGLChooseConfig()
{
	const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
			EGL_DEPTH_SIZE,DEPTH_SIZE_BIT,
            EGL_NONE
    };
	EGLint numConfigs;
    eglChooseConfig(m_eglDisplay, attribs, &m_eglConfig, 1, &numConfigs);
}
void THEGLWrapper::EGLMakeContext()
{
	const EGLint attrib_list[] = {
		EGL_CONTEXT_CLIENT_VERSION, GLES_VERSION,
        EGL_NONE
    };
    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, NULL, attrib_list);
	/*
#if THPLATFORM==THPLATFORM_WINDOWS
	eglBindAPI(EGL_OPENGL_ES_API);
#endif
	*/
	if (eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext) == EGL_FALSE) {
		THError("Unable to eglMakeCurrent");
		assert(0);
		return;
    }
	
}
void THEGLWrapper::GetFrameBufferSize(int *w,int *h)
{
    eglQuerySurface(m_eglDisplay, m_eglSurface, EGL_WIDTH, w);
    eglQuerySurface(m_eglDisplay, m_eglSurface, EGL_HEIGHT, h);
}
void THEGLWrapper::TermDisplay() {
    if (m_eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_eglContext != EGL_NO_CONTEXT) {
            eglDestroyContext(m_eglDisplay, m_eglContext);
        }
        if (m_eglSurface != EGL_NO_SURFACE) {
            eglDestroySurface(m_eglDisplay, m_eglSurface);
        }
        eglTerminate(m_eglDisplay);
    }
    m_eglDisplay = EGL_NO_DISPLAY;
    m_eglContext = EGL_NO_CONTEXT;
    m_eglSurface = EGL_NO_SURFACE;
}

#endif
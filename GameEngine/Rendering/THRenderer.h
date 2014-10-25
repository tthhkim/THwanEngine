#ifndef THKim_RENDERING
#define THKim_RENDERING

#include <THPrivate.h>

void OnDrawFrame();
void THGLInit();
void THEGLInit(THApplicaation* state);
void THTerm_Display();
void Touch_Point_Down(const float x,const float y);
void Touch_Point_Up(const float x,const float y);
void MainEnterFrame();

#endif
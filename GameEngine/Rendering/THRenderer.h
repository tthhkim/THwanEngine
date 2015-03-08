#ifndef THKim_RENDERING
#define THKim_RENDERING

#include <GameEngine/Util/THMath.h>
#include <THPrivate.h>

void OnDrawFrame();
void THGLInit();
void THEGLInit(THApplicaation* state);
void THTerm_Display();
void Touch_Point_Down(const THVector2& p);
void Touch_Point_Up(const THVector2& p);
void MainEnterFrame();

#endif
#ifndef THKim_RENDERING
#define THKim_RENDERING

void OnDrawFrame(float dt);
void THGLInit();
void THTerm_Display();
void Touch_Point_Down(const float x,const float y);
void Touch_Point_Up(const float x,const float y);
void MainEnterFrame(float dt);

#endif
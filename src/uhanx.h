#ifndef UHANX_H
#define UHANX_H

#include <string>
#include <SDL2/SDL.h>

extern int hanx, hany;

void InitHan(SDL_Renderer* renderer);
void SetFont(int num);
void HPrint(const char* str);
void HPrintXY(int x, int y, const char* str);
void HPrintXY4Select(int x, int y, const char* str);
void OutHPrintXY(int x, int y, const char* str, bool extension);
void bHPrint(int x, int y, const char* str);
void cHPrint(int x, int y, const char* str);
void eHPrint(int x, int y, const char* str);

void HWindow(int x1, int y1, int x2, int y2);
void HGotoXY(int x, int y);
void Pause(bool ok);

void SetColor(int colorIndex); 
void SetPalette(int index, uint8_t r, uint8_t g, uint8_t b);
int GetColor();
void PutPixel(int x, int y, int colorIndex);
void Bar(int x1, int y1, int x2, int y2);
void Line(int x1, int y1, int x2, int y2);

// Utility to handle screen updates if we emulate VGA memory
bool HasBatchim(const std::string& s);
void UpdateScreen();

void Delay(Uint32 delay_ms);

#endif

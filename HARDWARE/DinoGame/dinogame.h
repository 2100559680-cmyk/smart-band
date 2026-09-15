#ifndef __DINOGAME_H
#define __DINOGAME_H

void OLED_DrawBMPFast(const unsigned char BMP[]);
void oled_drawbmp_block_clear(int bx, int by, int clear_size);
void OLED_DrawGround(void);
void OLED_DrawCloud(void);
void OLED_DrawDino(void);
void OLED_DrawCactus(void);
int OLED_DrawCactusRandom(unsigned char ver, unsigned char reset);
int OLED_DrawDinoJump(char reset);
void OLED_DrawRestart(void);
void OLED_DrawCover(void);

#endif

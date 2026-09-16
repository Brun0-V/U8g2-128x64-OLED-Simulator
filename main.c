#include "u8g2.h"
#include <SDL2/SDL.h>

u8g2_t u8g2;

/*
 * Put your u8g2 drawing code here (same plain-C API you'd use in
 * ESP-IDF, e.g. u8g2_DrawBox(&u8g2, ...), u8g2_DrawStr(&u8g2, ...)).
 * This function is called once per frame.
 */
static void draw(u8g2_t *u8g2)
{
    u8g2_SetFont(u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(u8g2, 2, 12, "Hello, u8g2!");

    u8g2_DrawFrame(u8g2, 0, 0, 128, 64);
    u8g2_DrawCircle(u8g2, 64, 40, 15, U8G2_DRAW_ALL);
    u8g2_DrawBox(u8g2, 10, 30, 20, 20);
}

int main(void)
{
    u8g2_SetupBuffer_SDL_128x64(&u8g2, &u8g2_cb_r0);
    u8x8_InitDisplay(u8g2_GetU8x8(&u8g2));
    u8x8_SetPowerSave(u8g2_GetU8x8(&u8g2), 0);

    u8g2_ClearBuffer(&u8g2);
    draw(&u8g2);
    u8g2_SendBuffer(&u8g2);

    /* Keep the window open until it's closed (SDL_QUIT) */
    while (1) {
        u8g_sdl_get_key();
        SDL_Delay(20);
    }

    return 0;
}

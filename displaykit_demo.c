#include "u8g2.h"
#include <SDL2/SDL.h>
#include <stdio.h>

/*
 * Converted from the DisplayKit Arduino sketch you pasted:
 *   u8g2.method(args)  ->  u8g2_Method(&u8g2, args)
 *   Serial screen-switch -> Left/Right arrow keys (SDL has no Serial)
 *   millis()/delay()     -> SDL_GetTicks()/SDL_Delay()
 *   setup()/loop()        -> main()
 */

u8g2_t u8g2;

typedef enum { DK_SCREEN_0, DK_SCREEN_1, DK_SCREEN_2, DK_SCREEN_3, DK_SCREEN_4, DK_SCREEN_5, DK_SCREEN_6 } DkScreenId;
static DkScreenId g_screen = DK_SCREEN_6;
static int g_dirty = 1;
static Uint32 g_lastDrawMs = 0;

static const char *g_menuItems[3] = {"Start", "Settings", "About"};
static int g_menuIndex = 0;

static void drawHomeScreen(void) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFontPosTop(&u8g2);
    u8g2_DrawBox(&u8g2, 0, 0, 128, 10);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_DrawUTF8(&u8g2, 2, 0, "Home");
    u8g2_SetDrawColor(&u8g2, 1);

    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_DrawUTF8(&u8g2, 2, 16, "DisplayKit Demo");
    u8g2_DrawUTF8(&u8g2, 2, 28, "Left/Right: switch");
    u8g2_DrawUTF8(&u8g2, 2, 40, "screen");

    u8g2_SendBuffer(&u8g2);
}

static void drawSettingsScreen(void) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFontPosTop(&u8g2);
    u8g2_DrawBox(&u8g2, 0, 0, 128, 10);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_DrawUTF8(&u8g2, 2, 0, "Settings");
    u8g2_SetDrawColor(&u8g2, 1);

    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_DrawUTF8(&u8g2, 2, 16, "Brightness: 80%");
    u8g2_DrawUTF8(&u8g2, 2, 28, "Sound: On");
    u8g2_DrawUTF8(&u8g2, 2, 40, "Timezone: UTC");

    u8g2_SendBuffer(&u8g2);
}

static void drawSimpleMenu(void) {
    int i;
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFontPosTop(&u8g2);
    u8g2_DrawBox(&u8g2, 0, 0, 128, 12);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_DrawUTF8(&u8g2, 4, 2, "Menu");
    u8g2_SetDrawColor(&u8g2, 1);

    for (i = 0; i < 3; i++) {
        int y = 16 + i * 16;
        u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
        if (i == g_menuIndex) {
            u8g2_DrawRBox(&u8g2, 8, y, 112, 14, 3);
            u8g2_SetDrawColor(&u8g2, 0);
            u8g2_DrawUTF8(&u8g2, 14, y + 3, g_menuItems[i]);
            u8g2_SetDrawColor(&u8g2, 1);
        } else {
            u8g2_DrawRFrame(&u8g2, 8, y, 112, 14, 3);
            u8g2_DrawUTF8(&u8g2, 14, y + 3, g_menuItems[i]);
        }
    }

    u8g2_SendBuffer(&u8g2);
}

static void drawDashboard(void) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFontPosTop(&u8g2);
    u8g2_DrawBox(&u8g2, 0, 0, 128, 10);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_DrawUTF8(&u8g2, 2, 0, "Dashboard");
    u8g2_SetDrawColor(&u8g2, 1);

    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_DrawUTF8(&u8g2, 2, 11, "CPU 42%");
    u8g2_DrawFrame(&u8g2, 2, 22, 124, 8);
    u8g2_DrawBox(&u8g2, 4, 24, 52, 4); /* 42% of interior width */

    u8g2_DrawUTF8(&u8g2, 2, 33, "RAM 68%");
    u8g2_DrawFrame(&u8g2, 2, 44, 124, 8);
    u8g2_DrawBox(&u8g2, 4, 46, 82, 4); /* 68% of interior width */

    u8g2_SendBuffer(&u8g2);
}

static void drawMediaPlayer(void) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFontPosTop(&u8g2);
    u8g2_DrawBox(&u8g2, 0, 0, 128, 10);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_DrawUTF8(&u8g2, 2, 0, "Now Playing");
    u8g2_SetDrawColor(&u8g2, 1);

    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_DrawUTF8(&u8g2, 2, 11, "Track Name");

    u8g2_DrawFrame(&u8g2, 2, 23, 124, 6);
    u8g2_DrawDisc(&u8g2, 40, 26, 3, U8G2_DRAW_ALL);

    u8g2_DrawRBox(&u8g2, 2, 32, 38, 16, 3);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_DrawUTF8(&u8g2, 14, 38, "|<");
    u8g2_SetDrawColor(&u8g2, 1);

    u8g2_DrawRBox(&u8g2, 42, 32, 38, 16, 3);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_DrawUTF8(&u8g2, 50, 38, "||>");
    u8g2_SetDrawColor(&u8g2, 1);

    u8g2_DrawRBox(&u8g2, 82, 32, 38, 16, 3);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_DrawUTF8(&u8g2, 94, 38, ">|");
    u8g2_SetDrawColor(&u8g2, 1);

    u8g2_SendBuffer(&u8g2);
}

static void drawOLEDStatus(void) {
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFontPosTop(&u8g2);
    u8g2_DrawBox(&u8g2, 0, 0, 128, 10);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_DrawUTF8(&u8g2, 2, 0, "STATUS");
    u8g2_SetDrawColor(&u8g2, 1);

    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_DrawUTF8(&u8g2, 2, 11, "WiFi: OK");
    u8g2_DrawUTF8(&u8g2, 2, 22, "BAT: 82%");

    u8g2_DrawFrame(&u8g2, 66, 22, 58, 9);
    u8g2_DrawBox(&u8g2, 68, 24, 44, 5); /* 82% of interior width */

    u8g2_SendBuffer(&u8g2);
}

static void drawOLEDMenu(void) {
    int i;
    char line[16];
    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFontPosTop(&u8g2);
    u8g2_DrawBox(&u8g2, 0, 0, 128, 12);
    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_DrawUTF8(&u8g2, 4, 8, "MENU");
    u8g2_SetDrawColor(&u8g2, 1);

    u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
    for (i = 0; i < 3; i++) {
        snprintf(line, sizeof(line), "%s %s", i == g_menuIndex ? ">" : " ", g_menuItems[i]);
        u8g2_DrawUTF8(&u8g2, 4, 16 + i * 12, line);
    }

    u8g2_SendBuffer(&u8g2);
}

static void drawCurrentScreen(void) {
    switch (g_screen) {
        case DK_SCREEN_0: drawHomeScreen(); break;
        case DK_SCREEN_1: drawSettingsScreen(); break;
        case DK_SCREEN_2: drawSimpleMenu(); break;
        case DK_SCREEN_3: drawDashboard(); break;
        case DK_SCREEN_4: drawMediaPlayer(); break;
        case DK_SCREEN_5: drawOLEDStatus(); break;
        case DK_SCREEN_6: drawOLEDMenu(); break;
    }
}

/* Original code drove this from Serial input; SDL has no serial port,
 * so Left/Right arrow keys cycle screens, and Up/Down move the
 * highlighted item on the menu screens. */
static void handleUiActions(void) {
    int key = u8g_sdl_get_key();
    if (key == 275 /* right */) {
        g_screen = (DkScreenId)((g_screen + 1) % 7);
        g_dirty = 1;
    } else if (key == 276 /* left */) {
        g_screen = (DkScreenId)((g_screen + 6) % 7);
        g_dirty = 1;
    } else if (key == 273 /* up */) {
        g_menuIndex = (g_menuIndex + 2) % 3;
        g_dirty = 1;
    } else if (key == 274 /* down */) {
        g_menuIndex = (g_menuIndex + 1) % 3;
        g_dirty = 1;
    }
}

int main(void) {
    u8g2_SetupBuffer_SDL_128x64(&u8g2, &u8g2_cb_r0);
    u8x8_InitDisplay(u8g2_GetU8x8(&u8g2));
    u8g2_SetFontMode(&u8g2, 0);
    u8g2_SetPowerSave(&u8g2, 0);

    drawCurrentScreen();
    g_dirty = 0;
    g_lastDrawMs = SDL_GetTicks();

    while (1) {
        handleUiActions();
        if (g_dirty || (SDL_GetTicks() - g_lastDrawMs) > 1000) {
            drawCurrentScreen();
            g_dirty = 0;
            g_lastDrawMs = SDL_GetTicks();
        }
        SDL_Delay(5);
    }

    return 0;
}

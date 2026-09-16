# u8g2-sim

Run [u8g2](https://github.com/olikraus/u8g2) drawing code on your desktop instead of real hardware, using SDL2 as a simulated 128x64 display. Useful for writing/testing display UI without an ESP32 or Arduino plugged in.

## Requirements

- A C compiler (gcc/clang)
- SDL2 (`sdl2-config` needs to be on your PATH)

On Fedora: `sudo dnf install SDL2-devel`
On Debian/Ubuntu: `sudo apt install libsdl2-dev`

## Build & run

```
make run
```

This builds `sim` (from `main.c`) and launches it. A window pops up showing whatever `main.c` draws.

There's also a second demo target built from `displaykit_demo.c`, which shows a simple menu screen with keyboard navigation:

```
make run-demo
```

Other Makefile targets:

- `make sim` / `make demo` — build without running
- `make clean` — remove build artifacts

## Writing your own screen

Open `main.c`. There's a `draw()` function that gets called once per frame — put your normal u8g2 calls in there, exactly like you would on real hardware:

```c
static void draw(u8g2_t *u8g2)
{
    u8g2_SetFont(u8g2, u8g2_font_ncenB08_tr);
    u8g2_DrawStr(u8g2, 2, 12, "Hello, u8g2!");
    u8g2_DrawFrame(u8g2, 0, 0, 128, 64);
}
```

Any u8g2 function works (`u8g2_DrawBox`, `u8g2_DrawCircle`, fonts, etc.) since this is the same plain C API you'd use in ESP-IDF or Arduino — nothing simulator-specific to learn. Rebuild with `make run` and your changes show up in the window.

Close the window (or hit the window's close button) to quit.

## Keyboard input

The simulator maps keyboard keys to u8g2's key codes so you can test menu/navigation logic:

- Arrow keys → up/down/left/right
- `a`-`z` and space → passed through as ASCII
- Poll with `u8g_sdl_get_key()`, same as you'd read buttons on real hardware

Check `displaykit_demo.c` for an example that uses arrow keys to move through a menu.

## Project layout

- `main.c` — your playground, edit this
- `displaykit_demo.c` — a more complete example (menu screen)
- `csrc/` — the actual u8g2/u8x8 library (unmodified upstream source)
- `sdl/` — the SDL glue: the fake "display driver" (`u8x8_d_sdl_128x64.c`) and keyboard handling (`u8x8_sdl_key.c`)

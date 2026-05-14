/* PrintBoy - entry point.
 *
 * Boucle :
 *   - poll periodique de /api/v1/status
 *   - render 30 FPS
 *   - eventloop SDL2
 */
#include "ui.h"
#include "config.h"
#include "http.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_PATH "/mnt/SDCARD/App/PrintBoy/printer.cfg"

int main(int argc, char **argv)
{
    (void)argc; (void)argv;

    pb_config_t cfg;
    pb_config_load(CONFIG_PATH, &cfg);

    http_init();
    if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
        fprintf(stderr, "IMG_Init JPG failed: %s\n", IMG_GetError());
        /* on continue : la cam ne marchera pas, mais le reste oui */
    }

    ui_t ui;
    if (!ui_init(&ui, &cfg)) {
        fprintf(stderr, "ui_init failed: %s\n", SDL_GetError());
        http_cleanup();
        return 1;
    }

    bool running = true;
    Uint32 last_frame = 0;
    const Uint32 frame_ms = 33;  /* ~30 FPS */

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (!ui_handle_event(&ui, &ev)) {
                running = false;
                break;
            }
        }
        ui_tick(&ui);
        ui_render(&ui);

        Uint32 now = SDL_GetTicks();
        Uint32 delta = now - last_frame;
        if (delta < frame_ms) SDL_Delay(frame_ms - delta);
        last_frame = SDL_GetTicks();
    }

    pb_config_save(CONFIG_PATH, &cfg);
    ui_quit(&ui);
    IMG_Quit();
    http_cleanup();
    return 0;
}

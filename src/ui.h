/*
 * PrintBoy - couche UI minimale au-dessus de SDL2.
 *
 * 5 ecrans accessibles via L1/R1 :
 *   0. Dashboard  (temps, etat, progress)
 *   1. Webcam     (snapshot JPEG poll lent)
 *   2. Controls   (pause/resume/cancel + home + set temps)
 *   3. Jog        (XYZ +/- step)
 *   4. Settings   (URL + API key + intervals)
 */
#ifndef PRINTBOY_UI_H
#define PRINTBOY_UI_H

#include "prusa_api.h"
#include "config.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef enum {
    SCREEN_DASHBOARD = 0,
    SCREEN_WEBCAM,
    SCREEN_CONTROLS,
    SCREEN_JOG,
    SCREEN_SETTINGS,
    SCREEN__COUNT,
} screen_id_t;

typedef struct {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    TTF_Font     *font_small;
    TTF_Font     *font_medium;
    TTF_Font     *font_large;

    screen_id_t current;
    int  cursor;   /* generique : index focus dans l'ecran courant */

    prusa_status_t status;
    Uint32 last_poll_ms;

    pb_config_t *cfg;
    char status_message[128];  /* affiche dans le footer pendant 3s */
    Uint32 status_message_until;
} ui_t;

bool ui_init(ui_t *ui, pb_config_t *cfg);
void ui_quit(ui_t *ui);

void ui_show_message(ui_t *ui, const char *fmt, ...);

void ui_render(ui_t *ui);

/* Renvoie false si on doit quitter l'application. */
bool ui_handle_event(ui_t *ui, const SDL_Event *e);

/* Polling de l'API selon cfg->poll_interval_s. */
void ui_tick(ui_t *ui);

/* Helpers exposes aux ecrans : */
void ui_text(ui_t *ui, TTF_Font *f, const char *s, int x, int y,
             SDL_Color color);
void ui_text_center(ui_t *ui, TTF_Font *f, const char *s, int y, int w,
                    SDL_Color color);
void ui_box(ui_t *ui, int x, int y, int w, int h, SDL_Color color);
void ui_box_outline(ui_t *ui, int x, int y, int w, int h, SDL_Color color);

extern const SDL_Color C_BG;
extern const SDL_Color C_FG;
extern const SDL_Color C_DIM;
extern const SDL_Color C_ACCENT;   /* Prusa orange */
extern const SDL_Color C_OK;
extern const SDL_Color C_WARN;
extern const SDL_Color C_ERR;
extern const SDL_Color C_PANEL;

#endif /* PRINTBOY_UI_H */

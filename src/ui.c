/*
 * PrintBoy - implementation UI generique.
 */
#include "ui.h"
#include "screens/screens.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

const SDL_Color C_BG     = {  18,  20,  28, 255};
const SDL_Color C_FG     = { 240, 240, 245, 255};
const SDL_Color C_DIM    = { 140, 140, 160, 255};
const SDL_Color C_ACCENT = { 255, 132,  20, 255};  /* Prusa orange */
const SDL_Color C_OK     = {  60, 200, 100, 255};
const SDL_Color C_WARN   = { 245, 200,  60, 255};
const SDL_Color C_ERR    = { 230,  80,  80, 255};
const SDL_Color C_PANEL  = {  32,  36,  48, 255};

static TTF_Font *load_font(int size)
{
    /* OnionOS embarque DejaVuSans dans /mnt/SDCARD/.tmp_update/res/fonts/.
     * On essaie d'abord ce chemin, sinon une copie locale dans res/. */
    TTF_Font *f = TTF_OpenFont(
        "/mnt/SDCARD/.tmp_update/res/fonts/DejaVuSans-Regular.ttf", size);
    if (f) return f;
    return TTF_OpenFont("res/DejaVuSans-Regular.ttf", size);
}

bool ui_init(ui_t *ui, pb_config_t *cfg)
{
    memset(ui, 0, sizeof(*ui));
    ui->cfg = cfg;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
    if (TTF_Init() != 0) return false;

    ui->window = SDL_CreateWindow("PrintBoy",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480, SDL_WINDOW_SHOWN);
    if (!ui->window) return false;

    ui->renderer = SDL_CreateRenderer(ui->window, -1, SDL_RENDERER_ACCELERATED);
    if (!ui->renderer) return false;

    ui->font_small  = load_font(14);
    ui->font_medium = load_font(20);
    ui->font_large  = load_font(36);
    if (!ui->font_small || !ui->font_medium || !ui->font_large) return false;

    /* Joystick / pad : Miyoo mappe les boutons sur SDL_GameController.
     * Mais Onion expose aussi /dev/input/event0 mappe en touches clavier.
     * SDL2 capture les deux. Pour simplicite on s'appuie sur les keysym. */
    SDL_GameControllerEventState(SDL_ENABLE);

    return true;
}

void ui_quit(ui_t *ui)
{
    if (ui->font_small)  TTF_CloseFont(ui->font_small);
    if (ui->font_medium) TTF_CloseFont(ui->font_medium);
    if (ui->font_large)  TTF_CloseFont(ui->font_large);
    if (ui->renderer)    SDL_DestroyRenderer(ui->renderer);
    if (ui->window)      SDL_DestroyWindow(ui->window);
    TTF_Quit();
    SDL_Quit();
}

void ui_show_message(ui_t *ui, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(ui->status_message, sizeof(ui->status_message), fmt, ap);
    va_end(ap);
    ui->status_message_until = SDL_GetTicks() + 3000;
}

void ui_text(ui_t *ui, TTF_Font *f, const char *s, int x, int y, SDL_Color color)
{
    if (!s || !*s) return;
    SDL_Surface *surf = TTF_RenderUTF8_Blended(f, s, color);
    if (!surf) return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ui->renderer, surf);
    if (tex) {
        SDL_Rect dst = {x, y, surf->w, surf->h};
        SDL_RenderCopy(ui->renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
}

void ui_text_center(ui_t *ui, TTF_Font *f, const char *s, int y, int w,
                    SDL_Color color)
{
    if (!s || !*s) return;
    int tw = 0, th = 0;
    TTF_SizeUTF8(f, s, &tw, &th);
    ui_text(ui, f, s, (w - tw) / 2, y, color);
    (void)th;
}

void ui_box(ui_t *ui, int x, int y, int w, int h, SDL_Color c)
{
    SDL_SetRenderDrawColor(ui->renderer, c.r, c.g, c.b, c.a);
    SDL_Rect r = {x, y, w, h};
    SDL_RenderFillRect(ui->renderer, &r);
}

void ui_box_outline(ui_t *ui, int x, int y, int w, int h, SDL_Color c)
{
    SDL_SetRenderDrawColor(ui->renderer, c.r, c.g, c.b, c.a);
    SDL_Rect r = {x, y, w, h};
    SDL_RenderDrawRect(ui->renderer, &r);
}

static void render_tabs(ui_t *ui)
{
    const char *labels[SCREEN__COUNT] = {
        "DASH", "CAM", "CTRL", "JOG", "SET",
    };
    ui_box(ui, 0, 0, 640, 30, C_PANEL);
    int x = 10;
    for (int i = 0; i < SCREEN__COUNT; i++) {
        SDL_Color c = (i == ui->current) ? C_ACCENT : C_DIM;
        ui_text(ui, ui->font_medium, labels[i], x, 4, c);
        x += 130;
    }
    /* Indicateur connexion */
    SDL_Color dot = ui->status.valid ? C_OK : C_ERR;
    ui_box(ui, 620, 10, 10, 10, dot);
}

static void render_footer(ui_t *ui)
{
    ui_box(ui, 0, 450, 640, 30, C_PANEL);
    if (ui->status_message[0] &&
        SDL_GetTicks() < ui->status_message_until) {
        ui_text(ui, ui->font_small, ui->status_message, 10, 458, C_WARN);
    } else {
        const char *hint = "L/R: change tab     B: back     A: action    SELECT: quit";
        ui_text(ui, ui->font_small, hint, 10, 458, C_DIM);
    }
}

void ui_render(ui_t *ui)
{
    SDL_SetRenderDrawColor(ui->renderer, C_BG.r, C_BG.g, C_BG.b, 255);
    SDL_RenderClear(ui->renderer);

    render_tabs(ui);

    switch (ui->current) {
        case SCREEN_DASHBOARD: screen_dashboard_render(ui); break;
        case SCREEN_WEBCAM:    screen_webcam_render(ui);    break;
        case SCREEN_CONTROLS:  screen_controls_render(ui);  break;
        case SCREEN_JOG:       screen_jog_render(ui);       break;
        case SCREEN_SETTINGS:  screen_settings_render(ui);  break;
        default: break;
    }

    render_footer(ui);
    SDL_RenderPresent(ui->renderer);
}

bool ui_handle_event(ui_t *ui, const SDL_Event *e)
{
    if (e->type == SDL_QUIT) return false;

    /* Mapping clavier Miyoo + claviers PC pour dev :
     *   - L1=q, R1=p, A=Space, B=LCtrl, X=LShift, Y=LAlt
     *   - D-pad = fleches
     *   - SELECT = RCtrl, START = Return
     * Sur Miyoo, Onion route les events hardware en evdev. SDL2 les voit
     * comme des touches clavier (sauf si configure en game-controller).
     */
    if (e->type == SDL_KEYDOWN) {
        SDL_Keycode k = e->key.keysym.sym;
        if (k == SDLK_RCTRL || k == SDLK_ESCAPE) return false;
        if (k == SDLK_q) {  /* L1 = previous tab */
            ui->current = (ui->current + SCREEN__COUNT - 1) % SCREEN__COUNT;
            ui->cursor = 0;
            return true;
        }
        if (k == SDLK_p || k == SDLK_TAB) {  /* R1 = next tab */
            ui->current = (ui->current + 1) % SCREEN__COUNT;
            ui->cursor = 0;
            return true;
        }
    }

    /* Deleguer le reste a l'ecran courant */
    switch (ui->current) {
        case SCREEN_DASHBOARD: return screen_dashboard_event(ui, e);
        case SCREEN_WEBCAM:    return screen_webcam_event(ui, e);
        case SCREEN_CONTROLS:  return screen_controls_event(ui, e);
        case SCREEN_JOG:       return screen_jog_event(ui, e);
        case SCREEN_SETTINGS:  return screen_settings_event(ui, e);
        default: return true;
    }
}

void ui_tick(ui_t *ui)
{
    Uint32 now = SDL_GetTicks();
    int interval_ms = ui->cfg->poll_interval_s * 1000;
    if (interval_ms < 500) interval_ms = 500;
    if (now - ui->last_poll_ms >= (Uint32)interval_ms) {
        prusa_get_status(ui->cfg->url, ui->cfg->api_key, &ui->status);
        ui->last_poll_ms = now;
    }
}

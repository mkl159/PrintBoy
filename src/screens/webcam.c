/* Ecran 1 : webcam - snapshot JPEG via Prusa-Link. */
#include "screens.h"
#include "../http.h"
#include "../prusa_api.h"

#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>

/* Pour eviter de spammer l'imprimante (et limiter la conso WiFi/CPU), on
 * rafraichit la cam toutes les 5 secondes seulement quand on est sur cet
 * ecran. Bouton A pour rafraichir manuellement. */

static SDL_Texture *cached_tex = NULL;
static Uint32 last_fetch_ms = 0;
static char last_error[128] = "";
static char cam_id[64] = "";

static void refresh(ui_t *ui)
{
    /* IMPORTANT : on horodate l'essai DES LE DEBUT, y compris en cas
     * d'echec. Sinon les "return" prematures ci-dessous laissent
     * last_fetch_ms inchange et screen_webcam_render() rappellerait
     * refresh() a chaque frame (30 FPS) -> requetes HTTP bloquantes en
     * rafale qui gelent l'UI et matraquent l'imprimante. */
    last_fetch_ms = SDL_GetTicks();

    /* L'id camera vient du bloc "camera" de /api/v1/status si present,
     * sinon on interroge /api/v1/cameras (fallback documente). */
    if (ui->status.has_camera && ui->status.camera_id[0]) {
        snprintf(cam_id, sizeof(cam_id), "%s", ui->status.camera_id);
    } else if (cam_id[0] == '\0') {
        prusa_get_cameras(ui->cfg->url, ui->cfg->api_key,
                          cam_id, sizeof(cam_id));
    }
    if (cam_id[0] == '\0') {
        snprintf(last_error, sizeof(last_error),
                 "Aucune camera detectee sur l'imprimante.");
        return;
    }
    unsigned char *jpeg = NULL;
    size_t jpeg_size = 0;
    if (!prusa_get_snapshot(ui->cfg->url, ui->cfg->api_key,
                            cam_id, &jpeg, &jpeg_size)) {
        snprintf(last_error, sizeof(last_error),
                 "Echec snap (cam=%s)", cam_id);
        return;
    }
    SDL_RWops *rw = SDL_RWFromMem(jpeg, (int)jpeg_size);
    SDL_Surface *surf = IMG_LoadJPG_RW(rw);
    SDL_RWclose(rw);
    free(jpeg);
    if (!surf) {
        snprintf(last_error, sizeof(last_error),
                 "Decode JPEG failed: %s", IMG_GetError());
        return;
    }
    if (cached_tex) SDL_DestroyTexture(cached_tex);
    cached_tex = SDL_CreateTextureFromSurface(ui->renderer, surf);
    SDL_FreeSurface(surf);
    last_error[0] = '\0';
    last_fetch_ms = SDL_GetTicks();
}

void screen_webcam_render(ui_t *ui)
{
    /* Refresh auto toutes les 5s */
    if (SDL_GetTicks() - last_fetch_ms > 5000) {
        refresh(ui);
    }

    if (cached_tex) {
        /* Fit dans 560x360 en conservant le ratio (letterbox centre). */
        SDL_Rect dst = {40, 50, 560, 360};
        int tw = 0, th = 0;
        SDL_QueryTexture(cached_tex, NULL, NULL, &tw, &th);
        if (tw > 0 && th > 0) {
            float sx = 560.f / (float)tw;
            float sy = 360.f / (float)th;
            float sc = (sx < sy) ? sx : sy;
            dst.w = (int)((float)tw * sc);
            dst.h = (int)((float)th * sc);
            dst.x = 40 + (560 - dst.w) / 2;
            dst.y = 50 + (360 - dst.h) / 2;
        }
        ui_box(ui, 40, 50, 560, 360, C_PANEL);
        SDL_RenderCopy(ui->renderer, cached_tex, NULL, &dst);
        ui_box_outline(ui, 40, 50, 560, 360, C_PANEL);
    } else {
        ui_box(ui, 40, 50, 560, 360, C_PANEL);
        ui_text_center(ui, ui->font_medium,
                       last_error[0] ? last_error : "Chargement webcam...",
                       220, 640, C_DIM);
    }

    /* Overlay temps */
    if (ui->status.valid) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Buse %.0f C   Plateau %.0f C",
                 ui->status.temp_nozzle, ui->status.temp_bed);
        ui_text(ui, ui->font_small, buf, 50, 60, C_FG);
    }

    ui_text(ui, ui->font_small, "A: rafraichir maintenant", 50, 420, C_DIM);
}

bool screen_webcam_event(ui_t *ui, const SDL_Event *e)
{
    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_SPACE) {
        refresh(ui);
    }
    return true;
}

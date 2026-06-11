/* Ecran 0 : dashboard - vue d'ensemble. */
#include "screens.h"

#include <stdio.h>
#include <math.h>

static void fmt_time(long s, char *out, size_t sz)
{
    if (s <= 0) {
        snprintf(out, sz, "--:--");
        return;
    }
    long h = s / 3600;
    long m = (s % 3600) / 60;
    if (h > 0) snprintf(out, sz, "%ldh%02ld", h, m);
    else       snprintf(out, sz, "%ldmin", m);
}

void screen_dashboard_render(ui_t *ui)
{
    const prusa_status_t *s = &ui->status;
    SDL_Color stateColor = C_DIM;
    switch (s->state) {
        case PRUSA_STATE_PRINTING: stateColor = C_OK;     break;
        case PRUSA_STATE_PAUSED:   stateColor = C_WARN;   break;
        case PRUSA_STATE_ERROR:
        case PRUSA_STATE_ATTENTION:stateColor = C_ERR;    break;
        case PRUSA_STATE_FINISHED: stateColor = C_ACCENT; break;
        default: break;
    }

    if (!s->valid) {
        ui_text_center(ui, ui->font_large, "PAS DE CONNEXION", 180, 640, C_ERR);
        ui_text_center(ui, ui->font_small, s->last_error, 240, 640, C_DIM);
        ui_text_center(ui, ui->font_small,
            "Verifie le WiFi (Onion > Network) et la config (SET).",
            260, 640, C_DIM);
        return;
    }

    /* Etat en grand */
    ui_text_center(ui, ui->font_large, prusa_state_label(s->state), 40, 640,
                   stateColor);

    /* Fraicheur des donnees (le poll est asynchrone) */
    if (ui->last_status_ms) {
        char age[32];
        Uint32 age_s = (SDL_GetTicks() - ui->last_status_ms) / 1000;
        snprintf(age, sizeof(age), "maj %lus", (unsigned long)age_s);
        ui_text(ui, ui->font_small, age, 560, 40, C_DIM);
    }

    /* Panneau temperatures */
    ui_box(ui, 20, 100, 280, 140, C_PANEL);
    ui_text(ui, ui->font_small, "BUSE", 30, 108, C_DIM);
    char buf[64];
    snprintf(buf, sizeof(buf), "%.0f / %.0f C",
             s->temp_nozzle, s->target_nozzle);
    ui_text(ui, ui->font_large, buf, 30, 120, C_ACCENT);

    ui_text(ui, ui->font_small, "PLATEAU", 30, 175, C_DIM);
    snprintf(buf, sizeof(buf), "%.0f / %.0f C",
             s->temp_bed, s->target_bed);
    ui_text(ui, ui->font_large, buf, 30, 187, C_ACCENT);

    /* Panneau positions / fan */
    ui_box(ui, 320, 100, 300, 140, C_PANEL);
    ui_text(ui, ui->font_small, "POSITION", 330, 108, C_DIM);
    if (isnan(s->axis_x) || isnan(s->axis_y)) {
        snprintf(buf, sizeof(buf), "Z %.2f mm", s->axis_z);
    } else {
        snprintf(buf, sizeof(buf), "X %.0f  Y %.0f  Z %.2f",
                 s->axis_x, s->axis_y, s->axis_z);
    }
    ui_text(ui, ui->font_medium, buf, 330, 125, C_FG);

    ui_text(ui, ui->font_small, "VENTILOS", 330, 165, C_DIM);
    snprintf(buf, sizeof(buf), "Hotend %d  Print %d",
             s->fan_hotend, s->fan_print);
    ui_text(ui, ui->font_medium, buf, 330, 180, C_FG);

    ui_text(ui, ui->font_small, "FLOW / SPEED", 330, 210, C_DIM);
    snprintf(buf, sizeof(buf), "%d%% / %d%%", s->flow, s->speed);
    ui_text(ui, ui->font_medium, buf, 330, 220, C_FG);

    /* Job + progress */
    if (s->state == PRUSA_STATE_PRINTING || s->state == PRUSA_STATE_PAUSED) {
        ui_box(ui, 20, 260, 600, 150, C_PANEL);
        ui_text(ui, ui->font_small, "IMPRESSION", 30, 268, C_DIM);
        ui_text(ui, ui->font_medium, s->job_filename, 30, 285, C_FG);

        /* Barre de progression */
        ui_box(ui, 30, 340, 580, 24, C_BG);
        int w = (int)(580 * (s->progress / 100.f));
        if (w > 580) w = 580;
        if (w < 0) w = 0;
        ui_box(ui, 30, 340, w, 24, C_ACCENT);
        ui_box_outline(ui, 30, 340, 580, 24, C_FG);
        snprintf(buf, sizeof(buf), "%.1f%%", s->progress);
        ui_text(ui, ui->font_medium, buf, 30, 372, C_FG);

        char trem[32], tprt[32];
        fmt_time(s->time_remaining_s, trem, sizeof(trem));
        fmt_time(s->time_printing_s, tprt, sizeof(tprt));
        snprintf(buf, sizeof(buf), "Reste %s   /   ecoule %s", trem, tprt);
        ui_text(ui, ui->font_small, buf, 280, 380, C_DIM);
    } else {
        ui_text_center(ui, ui->font_medium,
            "Aucune impression en cours", 320, 640, C_DIM);
    }
}

bool screen_dashboard_event(ui_t *ui, const SDL_Event *e)
{
    (void)ui; (void)e;
    return true;
}

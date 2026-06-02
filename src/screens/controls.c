/* Ecran 2 : controles job (pause/resume/cancel + home + targets temps). */
#include "screens.h"

#include <stdio.h>

#define NUM_ACTIONS 9

/* Presets de prechauffage (buse, plateau) en degres C. */
#define PREHEAT_PLA_NOZZLE  215
#define PREHEAT_PLA_BED      60
#define PREHEAT_PETG_NOZZLE 230
#define PREHEAT_PETG_BED     85

static const char *labels[NUM_ACTIONS] = {
    "Pause / Reprendre",
    "Annuler l'impression",
    "Home XYZ",
    "Buse +5 C",
    "Buse -5 C",
    "Plateau +5 C",
    "Plateau -5 C",
    "Prechauffer PLA (215/60)",
    "Refroidir tout (0/0)",
};

void screen_controls_render(ui_t *ui)
{
    ui_text_center(ui, ui->font_medium, "CONTROLE DU JOB", 36, 640, C_ACCENT);

    int y = 70;
    for (int i = 0; i < NUM_ACTIONS; i++) {
        SDL_Color bg = (i == ui->cursor) ? C_ACCENT : C_PANEL;
        SDL_Color fg = (i == ui->cursor) ? C_BG : C_FG;
        ui_box(ui, 60, y, 520, 36, bg);
        ui_text(ui, ui->font_medium, labels[i], 80, y + 5, fg);
        y += 42;
    }

    ui_text_center(ui, ui->font_small,
        "Haut/Bas : choisir   A : executer", 444, 640, C_DIM);
}

static void do_action(ui_t *ui, int idx)
{
    const pb_config_t *c = ui->cfg;
    prusa_status_t *st = &ui->status;
    if (!st->valid) {
        ui_show_message(ui, "Pas de connexion : action ignoree.");
        return;
    }
    switch (idx) {
        case 0:  /* Pause / Resume toggle */
            if (st->state == PRUSA_STATE_PRINTING) {
                if (prusa_job_pause(c->url, c->api_key, st->job_id))
                    ui_show_message(ui, "Pause demandee.");
                else
                    ui_show_message(ui, "Erreur de pause.");
            } else if (st->state == PRUSA_STATE_PAUSED) {
                if (prusa_job_resume(c->url, c->api_key, st->job_id))
                    ui_show_message(ui, "Reprise demandee.");
                else
                    ui_show_message(ui, "Erreur reprise.");
            } else {
                ui_show_message(ui, "Aucun job a pauser.");
            }
            break;
        case 1:
            if (st->job_id > 0 &&
                (st->state == PRUSA_STATE_PRINTING ||
                 st->state == PRUSA_STATE_PAUSED)) {
                if (prusa_job_cancel(c->url, c->api_key, st->job_id))
                    ui_show_message(ui, "Annulation demandee.");
                else
                    ui_show_message(ui, "Erreur annulation.");
            } else {
                ui_show_message(ui, "Aucun job a annuler.");
            }
            break;
        case 2:
            if (prusa_home_all(c->url, c->api_key))
                ui_show_message(ui, "Home XYZ envoye.");
            else
                ui_show_message(ui, "Erreur Home.");
            break;
        case 3:
            if (prusa_set_nozzle(c->url, c->api_key,
                                 (int)st->target_nozzle + 5))
                ui_show_message(ui, "Buse +5 C.");
            else
                ui_show_message(ui, "Erreur set buse.");
            break;
        case 4: {
            int t = (int)st->target_nozzle - 5;
            if (t < 0) t = 0;
            if (prusa_set_nozzle(c->url, c->api_key, t))
                ui_show_message(ui, "Buse -5 C.");
            else
                ui_show_message(ui, "Erreur set buse.");
            break;
        }
        case 5:
            if (prusa_set_bed(c->url, c->api_key,
                              (int)st->target_bed + 5))
                ui_show_message(ui, "Plateau +5 C.");
            else
                ui_show_message(ui, "Erreur set plateau.");
            break;
        case 6: {
            int t = (int)st->target_bed - 5;
            if (t < 0) t = 0;
            if (prusa_set_bed(c->url, c->api_key, t))
                ui_show_message(ui, "Plateau -5 C.");
            else
                ui_show_message(ui, "Erreur set plateau.");
            break;
        }
        case 7: {
            bool a = prusa_set_nozzle(c->url, c->api_key, PREHEAT_PLA_NOZZLE);
            bool b = prusa_set_bed(c->url, c->api_key, PREHEAT_PLA_BED);
            if (a && b)
                ui_show_message(ui, "Prechauffage PLA (215/60).");
            else
                ui_show_message(ui, "Erreur prechauffage.");
            break;
        }
        case 8: {
            bool a = prusa_set_nozzle(c->url, c->api_key, 0);
            bool b = prusa_set_bed(c->url, c->api_key, 0);
            if (a && b)
                ui_show_message(ui, "Refroidissement (0/0).");
            else
                ui_show_message(ui, "Erreur refroidissement.");
            break;
        }
    }
}

bool screen_controls_event(ui_t *ui, const SDL_Event *e)
{
    if (e->type != SDL_KEYDOWN) return true;
    switch (e->key.keysym.sym) {
        case SDLK_UP:
            ui->cursor = (ui->cursor + NUM_ACTIONS - 1) % NUM_ACTIONS;
            break;
        case SDLK_DOWN:
            ui->cursor = (ui->cursor + 1) % NUM_ACTIONS;
            break;
        case SDLK_SPACE:  /* A */
        case SDLK_RETURN:
            do_action(ui, ui->cursor);
            break;
    }
    return true;
}

/* Ecran 3 : Jog XYZ. */
#include "screens.h"

#include <stdio.h>

static int jog_step_idx = 1;  /* 0:1mm, 1:10mm, 2:50mm */
static const int steps[3] = {1, 10, 50};

void screen_jog_render(ui_t *ui)
{
    ui_text_center(ui, ui->font_medium, "JOG (manuel)", 40, 640, C_ACCENT);

    /* Position courante */
    const prusa_status_t *s = &ui->status;
    char buf[96];
    snprintf(buf, sizeof(buf), "X %.1f   Y %.1f   Z %.2f",
             s->axis_x, s->axis_y, s->axis_z);
    ui_text_center(ui, ui->font_medium, buf, 80, 640, C_FG);

    /* Pas */
    snprintf(buf, sizeof(buf), "Pas: %d mm  (Y pour cycler)", steps[jog_step_idx]);
    ui_text_center(ui, ui->font_small, buf, 115, 640, C_DIM);

    /* Diagramme : croix XY au centre, Z separe a droite */
    int cx = 240, cy = 280, r = 70;
    ui_box(ui, cx - r - 25, cy - r - 25, 2 * r + 50, 2 * r + 50, C_PANEL);
    /* fleches */
    ui_text(ui, ui->font_large, "X-", cx - r - 18, cy - 18, C_FG);
    ui_text(ui, ui->font_large, "X+", cx + r - 18, cy - 18, C_FG);
    ui_text(ui, ui->font_large, "Y-", cx - 18, cy + r - 18, C_FG);
    ui_text(ui, ui->font_large, "Y+", cx - 18, cy - r - 18, C_FG);

    /* Z separe */
    ui_box(ui, 450, 200, 130, 180, C_PANEL);
    ui_text_center(ui, ui->font_medium, "Z", 215, 640, C_ACCENT);
    ui_text(ui, ui->font_large, "Z+", 480, 220, C_FG);
    ui_text(ui, ui->font_large, "Z-", 480, 320, C_FG);
    ui_text(ui, ui->font_small, "L1 / R1", 477, 360, C_DIM);

    /* Hint */
    ui_text(ui, ui->font_small,
            "D-pad : XY    L1/R1 : Z    A : Home    B : retour    Y : pas",
            20, 420, C_DIM);
}

static void do_jog(ui_t *ui, char axis, int sign)
{
    int step = steps[jog_step_idx] * sign;
    if (!prusa_jog(ui->cfg->url, ui->cfg->api_key, axis, (float)step)) {
        ui_show_message(ui, "Jog %c %+d mm : erreur.", axis, step);
    } else {
        ui_show_message(ui, "Jog %c %+d mm.", axis, step);
    }
}

bool screen_jog_event(ui_t *ui, const SDL_Event *e)
{
    if (e->type != SDL_KEYDOWN) return true;
    switch (e->key.keysym.sym) {
        case SDLK_LEFT:  do_jog(ui, 'X', -1); break;
        case SDLK_RIGHT: do_jog(ui, 'X', +1); break;
        case SDLK_UP:    do_jog(ui, 'Y', +1); break;
        case SDLK_DOWN:  do_jog(ui, 'Y', -1); break;
        case SDLK_q:     do_jog(ui, 'Z', -1); break;  /* L1 */
        case SDLK_p:     do_jog(ui, 'Z', +1); break;  /* R1 */
        case SDLK_LALT:  /* Y : cycle step */
            jog_step_idx = (jog_step_idx + 1) % 3;
            ui_show_message(ui, "Pas = %d mm", steps[jog_step_idx]);
            break;
        case SDLK_SPACE: /* A : home */
            if (prusa_home_all(ui->cfg->url, ui->cfg->api_key))
                ui_show_message(ui, "Home XYZ envoye.");
            else
                ui_show_message(ui, "Erreur home.");
            break;
    }
    return true;
}

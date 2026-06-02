/* Ecran 4 : settings (URL + api_key + intervals).
 *
 * Pour eviter d'embarquer un IME complet, on edite ici uniquement :
 *  - poll_interval_s : +/- avec gauche/droite
 *  - jog_step_mm     : +/- avec gauche/droite
 *  - URL et api_key : affiches en lecture seule, on indique a l'utilisateur
 *    d'editer App/PrintBoy/printer.cfg depuis le PC (SD branchee).
 *  - Bouton A sur "Tester la connexion" : declenche un GET /status immediat.
 */
#include "screens.h"
#include <stdio.h>
#include <string.h>

#define NUM_ITEMS 4

void screen_settings_render(ui_t *ui)
{
    ui_text_center(ui, ui->font_medium, "PARAMETRES", 40, 640, C_ACCENT);

    char buf[256];

    /* URL */
    ui_box(ui, 20, 80, 600, 50, C_PANEL);
    ui_text(ui, ui->font_small, "URL Prusa-Link", 30, 86, C_DIM);
    snprintf(buf, sizeof(buf), "%s", ui->cfg->url[0] ? ui->cfg->url
                                                     : "(vide - editer printer.cfg)");
    ui_text(ui, ui->font_medium, buf, 30, 100, C_FG);

    /* API key (masquee) */
    ui_box(ui, 20, 140, 600, 50, C_PANEL);
    ui_text(ui, ui->font_small, "X-API-Key", 30, 146, C_DIM);
    char masked[64];
    int kl = (int)strlen(ui->cfg->api_key);
    if (kl == 0) snprintf(masked, sizeof(masked), "(vide)");
    else if (kl <= 4) snprintf(masked, sizeof(masked), "****");
    else snprintf(masked, sizeof(masked), "%c%c%c***%c%c",
                  ui->cfg->api_key[0], ui->cfg->api_key[1],
                  ui->cfg->api_key[2],
                  ui->cfg->api_key[kl - 2], ui->cfg->api_key[kl - 1]);
    ui_text(ui, ui->font_medium, masked, 30, 160, C_FG);

    /* Poll interval */
    SDL_Color c1 = (ui->cursor == 0) ? C_ACCENT : C_PANEL;
    SDL_Color t1 = (ui->cursor == 0) ? C_BG : C_FG;
    ui_box(ui, 20, 210, 290, 50, c1);
    ui_text(ui, ui->font_small, "Poll (s)", 30, 216, t1);
    snprintf(buf, sizeof(buf), "< %d s >", ui->cfg->poll_interval_s);
    ui_text(ui, ui->font_medium, buf, 30, 230, t1);

    /* Jog step */
    SDL_Color c2 = (ui->cursor == 1) ? C_ACCENT : C_PANEL;
    SDL_Color t2 = (ui->cursor == 1) ? C_BG : C_FG;
    ui_box(ui, 330, 210, 290, 50, c2);
    ui_text(ui, ui->font_small, "Pas jog par defaut (mm)", 340, 216, t2);
    snprintf(buf, sizeof(buf), "< %d mm >", ui->cfg->jog_step_mm);
    ui_text(ui, ui->font_medium, buf, 340, 230, t2);

    /* Tester la connexion */
    SDL_Color c3 = (ui->cursor == 2) ? C_ACCENT : C_PANEL;
    SDL_Color t3 = (ui->cursor == 2) ? C_BG : C_FG;
    ui_box(ui, 20, 280, 290, 44, c3);
    ui_text(ui, ui->font_medium, "Tester (A)", 30, 288, t3);

    /* Deconnexion (efface url + cle) */
    SDL_Color c4 = (ui->cursor == 3) ? C_ERR : C_PANEL;
    SDL_Color t4 = C_FG;
    ui_box(ui, 330, 280, 290, 44, c4);
    ui_text(ui, ui->font_medium, "Deconnexion (A)", 340, 288, t4);

    /* Aide */
    ui_text(ui, ui->font_small,
        "Pour modifier l'URL ou la cle : edite App/PrintBoy/printer.cfg depuis le PC.",
        20, 350, C_DIM);
    ui_text(ui, ui->font_small,
        "La cle est visible sur l'imprimante : Settings > Network > Prusa Link > API key.",
        20, 370, C_DIM);
    ui_text(ui, ui->font_small,
        "Haut/Bas : choisir    Gauche/Droite : modifier    A : action",
        20, 410, C_DIM);
}

static void save(const ui_t *ui)
{
    pb_config_save("/mnt/SDCARD/App/PrintBoy/printer.cfg", ui->cfg);
}

bool screen_settings_event(ui_t *ui, const SDL_Event *e)
{
    if (e->type != SDL_KEYDOWN) return true;
    switch (e->key.keysym.sym) {
        case SDLK_UP:
            ui->cursor = (ui->cursor + NUM_ITEMS - 1) % NUM_ITEMS;
            break;
        case SDLK_DOWN:
            ui->cursor = (ui->cursor + 1) % NUM_ITEMS;
            break;
        case SDLK_LEFT:
            if (ui->cursor == 0 && ui->cfg->poll_interval_s > 1)
                ui->cfg->poll_interval_s--;
            if (ui->cursor == 1 && ui->cfg->jog_step_mm > 1) {
                if (ui->cfg->jog_step_mm > 10) ui->cfg->jog_step_mm -= 10;
                else ui->cfg->jog_step_mm--;
            }
            save(ui);
            break;
        case SDLK_RIGHT:
            if (ui->cursor == 0 && ui->cfg->poll_interval_s < 60)
                ui->cfg->poll_interval_s++;
            if (ui->cursor == 1 && ui->cfg->jog_step_mm < 100) {
                if (ui->cfg->jog_step_mm >= 10) ui->cfg->jog_step_mm += 10;
                else ui->cfg->jog_step_mm++;
            }
            save(ui);
            break;
        case SDLK_SPACE:
            if (ui->cursor == 2) {
                if (prusa_get_status(ui->cfg->url, ui->cfg->api_key,
                                     &ui->status)) {
                    ui_show_message(ui, "Connexion OK : etat = %s",
                                    prusa_state_label(ui->status.state));
                } else {
                    ui_show_message(ui, "Echec : %s",
                                    ui->status.last_error);
                }
            } else if (ui->cursor == 3) {
                /* Deconnexion : on efface l'URL et la cle du fichier */
                ui->cfg->url[0] = '\0';
                ui->cfg->api_key[0] = '\0';
                save(ui);
                memset(&ui->status, 0, sizeof(ui->status));
                ui_show_message(ui, "Deconnecte. printer.cfg vide.");
            } else {
                save(ui);
                ui_show_message(ui, "Sauvegarde.");
            }
            break;
    }
    return true;
}

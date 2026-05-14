/* Header commun pour les 5 ecrans. */
#ifndef PRINTBOY_SCREENS_H
#define PRINTBOY_SCREENS_H

#include "../ui.h"

void screen_dashboard_render(ui_t *ui);
bool screen_dashboard_event (ui_t *ui, const SDL_Event *e);

void screen_webcam_render(ui_t *ui);
bool screen_webcam_event (ui_t *ui, const SDL_Event *e);

void screen_controls_render(ui_t *ui);
bool screen_controls_event (ui_t *ui, const SDL_Event *e);

void screen_jog_render(ui_t *ui);
bool screen_jog_event (ui_t *ui, const SDL_Event *e);

void screen_settings_render(ui_t *ui);
bool screen_settings_event (ui_t *ui, const SDL_Event *e);

#endif

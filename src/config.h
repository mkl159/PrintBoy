#ifndef PRINTBOY_CONFIG_H
#define PRINTBOY_CONFIG_H

#include <stdbool.h>

typedef struct {
    char url[256];      /* http://192.168.1.42 ou https://prusa.local */
    char api_key[128];
    int  poll_interval_s;
    int  jog_step_mm;   /* pas par defaut dans l'ecran Jog */
} pb_config_t;

/* Charge depuis /mnt/SDCARD/App/PrintBoy/printer.cfg.
 * Format simple "key = value", commentaires '#'.
 * En cas d'absence du fichier, remplit avec des defauts (url vide).
 * Retourne true si lu sans erreur. */
bool pb_config_load(const char *path, pb_config_t *out);

bool pb_config_save(const char *path, const pb_config_t *cfg);

#endif

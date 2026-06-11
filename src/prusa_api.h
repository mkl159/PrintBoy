/*
 * PrintBoy - client Prusa-Link v1.
 * Endpoints couverts :
 *   GET    /api/v1/status
 *   GET    /api/v1/job
 *   PUT    /api/v1/job/{id}/pause | /resume
 *   DELETE /api/v1/job/{id}
 *   POST   /api/printer/printhead {command:"jog",x|y|z:n}
 *   POST   /api/printer/printhead {command:"home"}
 *   POST   /api/printer/tool {command:"target",targets:{tool0:t}}
 *   POST   /api/printer/bed   {command:"target",target:t}
 *   GET    /api/v1/cameras
 *   GET    /api/v1/cameras/{id}/snap
 */
#ifndef PRINTBOY_PRUSA_API_H
#define PRINTBOY_PRUSA_API_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    PRUSA_STATE_UNKNOWN = 0,
    PRUSA_STATE_IDLE,
    PRUSA_STATE_BUSY,
    PRUSA_STATE_PRINTING,
    PRUSA_STATE_PAUSED,
    PRUSA_STATE_FINISHED,
    PRUSA_STATE_STOPPED,
    PRUSA_STATE_ERROR,
    PRUSA_STATE_ATTENTION,
    PRUSA_STATE_READY,
} prusa_state_t;

typedef struct {
    /* Donnees parsees du GET /api/v1/status. Toutes les valeurs sont des
     * "snapshots" : 0/NAN si non dispo. */
    prusa_state_t state;
    float temp_nozzle;
    float target_nozzle;
    float temp_bed;
    float target_bed;
    float axis_x;
    float axis_y;
    float axis_z;
    int   fan_hotend;     /* RPM ou % selon firmware */
    int   fan_print;
    int   flow;           /* en % */
    int   speed;          /* en % */
    /* Job (si state == PRINTING ou PAUSED) */
    int     job_id;
    float   progress;     /* 0-100 */
    long    time_remaining_s;
    long    time_printing_s;
    char    job_filename[128];
    /* Camera */
    bool    has_camera;
    char    camera_id[64];
    /* Meta */
    bool    valid;        /* false si on n'a pas pu joindre l'imprimante */
    char    last_error[128];
} prusa_status_t;

const char *prusa_state_label(prusa_state_t s);

/* Telemetrie principale. */
bool prusa_get_status(const char *url, const char *key, prusa_status_t *out);

/* Controles job. id = celui retourne dans prusa_status_t.job_id. */
bool prusa_job_pause (const char *url, const char *key, int id);
bool prusa_job_resume(const char *url, const char *key, int id);
bool prusa_job_cancel(const char *url, const char *key, int id);

/* Mouvement. delta_mm peut etre negatif pour le sens inverse. */
bool prusa_jog       (const char *url, const char *key, char axis, float delta_mm);
bool prusa_home_all  (const char *url, const char *key);

/* Temperatures. 0 pour eteindre. */
bool prusa_set_nozzle(const char *url, const char *key, int target_c);
bool prusa_set_bed   (const char *url, const char *key, int target_c);

/* Decouverte camera : GET /api/v1/cameras. Ecrit l'id de la premiere
 * camera connectee dans out_id. Renvoie false si aucune. Sert de fallback
 * quand /api/v1/status n'expose pas de bloc "camera". */
bool prusa_get_cameras(const char *url, const char *key,
                       char *out_id, size_t out_id_sz);

/* Webcam : telecharge un JPEG dans *out_jpeg / *out_size (malloc'd).
 * Renvoie false si la camera n'est pas dispo. */
bool prusa_get_snapshot(const char *url, const char *key, const char *cam_id,
                        unsigned char **out_jpeg, size_t *out_size);

#endif /* PRINTBOY_PRUSA_API_H */

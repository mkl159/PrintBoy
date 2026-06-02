/*
 * PrintBoy - client Prusa-Link v1.
 *
 * Hypotheses :
 *   - L'imprimante expose Prusa-Link sur http(s)://<ip> ou un nom DNS local.
 *   - Auth : header X-Api-Key avec la cle visible dans LCD > Settings > API key.
 *   - Le serveur Prusa peut ne pas avoir https : on accepte http en clair.
 */
#include "prusa_api.h"
#include "http.h"

#include <cjson/cJSON.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static prusa_state_t parse_state(const char *s)
{
    if (!s) return PRUSA_STATE_UNKNOWN;
    /* Les valeurs viennent du firmware Prusa : IDLE, READY, BUSY, PRINTING,
     * PAUSED, FINISHED, STOPPED, ERROR, ATTENTION. */
    if (!strcasecmp(s, "IDLE"))      return PRUSA_STATE_IDLE;
    if (!strcasecmp(s, "READY"))     return PRUSA_STATE_READY;
    if (!strcasecmp(s, "BUSY"))      return PRUSA_STATE_BUSY;
    if (!strcasecmp(s, "PRINTING"))  return PRUSA_STATE_PRINTING;
    if (!strcasecmp(s, "PAUSED"))    return PRUSA_STATE_PAUSED;
    if (!strcasecmp(s, "FINISHED")) return PRUSA_STATE_FINISHED;
    if (!strcasecmp(s, "STOPPED"))   return PRUSA_STATE_STOPPED;
    if (!strcasecmp(s, "ERROR"))     return PRUSA_STATE_ERROR;
    if (!strcasecmp(s, "ATTENTION")) return PRUSA_STATE_ATTENTION;
    return PRUSA_STATE_UNKNOWN;
}

const char *prusa_state_label(prusa_state_t s)
{
    switch (s) {
        case PRUSA_STATE_IDLE:      return "IDLE";
        case PRUSA_STATE_READY:     return "READY";
        case PRUSA_STATE_BUSY:      return "BUSY";
        case PRUSA_STATE_PRINTING:  return "PRINTING";
        case PRUSA_STATE_PAUSED:    return "PAUSED";
        case PRUSA_STATE_FINISHED:  return "FINISHED";
        case PRUSA_STATE_STOPPED:   return "STOPPED";
        case PRUSA_STATE_ERROR:     return "ERROR";
        case PRUSA_STATE_ATTENTION: return "ATTENTION";
        default:                    return "?";
    }
}

static float json_float(const cJSON *obj, const char *key, float defv)
{
    cJSON *it = cJSON_GetObjectItem(obj, key);
    if (!it || !cJSON_IsNumber(it)) return defv;
    return (float)it->valuedouble;
}

static int json_int(const cJSON *obj, const char *key, int defv)
{
    cJSON *it = cJSON_GetObjectItem(obj, key);
    if (!it || !cJSON_IsNumber(it)) return defv;
    return (int)it->valuedouble;
}

static long json_long(const cJSON *obj, const char *key, long defv)
{
    cJSON *it = cJSON_GetObjectItem(obj, key);
    if (!it || !cJSON_IsNumber(it)) return defv;
    return (long)it->valuedouble;
}

static void json_string(const cJSON *obj, const char *key,
                        char *out, size_t out_sz, const char *defv)
{
    cJSON *it = cJSON_GetObjectItem(obj, key);
    const char *v = (it && cJSON_IsString(it)) ? it->valuestring : defv;
    if (v == NULL) v = "";
    snprintf(out, out_sz, "%s", v);
}

bool prusa_get_status(const char *url, const char *key, prusa_status_t *out)
{
    memset(out, 0, sizeof(*out));
    out->valid = false;

    http_response_t r = http_get(url, "/api/v1/status", key);
    if (r.http_code != 200) {
        snprintf(out->last_error, sizeof(out->last_error),
                 "HTTP %ld - %s", r.http_code,
                 r.error[0] ? r.error : "no body");
        http_response_free(&r);
        return false;
    }

    cJSON *root = cJSON_Parse(r.data ? r.data : "");
    http_response_free(&r);
    if (!root) {
        snprintf(out->last_error, sizeof(out->last_error), "JSON parse error");
        return false;
    }

    cJSON *p = cJSON_GetObjectItem(root, "printer");
    if (p) {
        cJSON *st = cJSON_GetObjectItem(p, "state");
        out->state = parse_state(cJSON_IsString(st) ? st->valuestring : NULL);
        out->temp_nozzle    = json_float(p, "temp_nozzle", 0);
        out->target_nozzle  = json_float(p, "target_nozzle", 0);
        out->temp_bed       = json_float(p, "temp_bed", 0);
        out->target_bed     = json_float(p, "target_bed", 0);
        out->axis_x         = json_float(p, "axis_x", NAN);
        out->axis_y         = json_float(p, "axis_y", NAN);
        out->axis_z         = json_float(p, "axis_z", 0);
        out->fan_hotend     = json_int(p, "fan_hotend", 0);
        out->fan_print      = json_int(p, "fan_print", 0);
        out->flow           = json_int(p, "flow", 100);
        out->speed          = json_int(p, "speed", 100);
    }

    cJSON *job = cJSON_GetObjectItem(root, "job");
    if (job) {
        out->job_id          = json_int(job, "id", 0);
        out->progress        = json_float(job, "progress", 0);
        out->time_remaining_s = json_long(job, "time_remaining", 0);
        out->time_printing_s  = json_long(job, "time_printing", 0);
        const cJSON *file = cJSON_GetObjectItem(job, "file");
        if (file) {
            json_string(file, "display_name", out->job_filename,
                        sizeof(out->job_filename), "");
            if (out->job_filename[0] == '\0') {
                json_string(file, "name", out->job_filename,
                            sizeof(out->job_filename), "");
            }
        }
    }

    cJSON *cam = cJSON_GetObjectItem(root, "camera");
    if (cam && !cJSON_IsNull(cam)) {
        json_string(cam, "id", out->camera_id, sizeof(out->camera_id), "");
        out->has_camera = (out->camera_id[0] != '\0');
    }

    cJSON_Delete(root);
    out->valid = true;
    return true;
}

static bool simple_put(const char *url, const char *key, const char *path)
{
    http_response_t r = http_put_json(url, path, key, "");
    bool ok = (r.http_code >= 200 && r.http_code < 300);
    http_response_free(&r);
    return ok;
}

bool prusa_job_pause(const char *url, const char *key, int id)
{
    char path[64];
    snprintf(path, sizeof(path), "/api/v1/job/%d/pause", id);
    return simple_put(url, key, path);
}

bool prusa_job_resume(const char *url, const char *key, int id)
{
    char path[64];
    snprintf(path, sizeof(path), "/api/v1/job/%d/resume", id);
    return simple_put(url, key, path);
}

bool prusa_job_cancel(const char *url, const char *key, int id)
{
    char path[64];
    snprintf(path, sizeof(path), "/api/v1/job/%d", id);
    http_response_t r = http_delete(url, path, key);
    bool ok = (r.http_code >= 200 && r.http_code < 300);
    http_response_free(&r);
    return ok;
}

bool prusa_jog(const char *url, const char *key, char axis, float delta_mm)
{
    char body[128];
    char ax = (axis >= 'a' && axis <= 'z') ? (char)(axis - 32) : axis;
    if (ax != 'X' && ax != 'Y' && ax != 'Z') return false;
    /* feedrate par defaut : 1500 mm/min (raisonnable) */
    snprintf(body, sizeof(body),
             "{\"command\":\"jog\",\"%c\":%.2f,\"feedrate\":1500,\"absolute\":false}",
             ax, delta_mm);
    http_response_t r = http_post_json(url, "/api/printer/printhead", key, body);
    bool ok = (r.http_code >= 200 && r.http_code < 300);
    http_response_free(&r);
    return ok;
}

bool prusa_home_all(const char *url, const char *key)
{
    const char *body = "{\"command\":\"home\",\"axes\":[\"x\",\"y\",\"z\"]}";
    http_response_t r = http_post_json(url, "/api/printer/printhead", key, body);
    bool ok = (r.http_code >= 200 && r.http_code < 300);
    http_response_free(&r);
    return ok;
}

bool prusa_set_nozzle(const char *url, const char *key, int target_c)
{
    char body[128];
    snprintf(body, sizeof(body),
             "{\"command\":\"target\",\"targets\":{\"tool0\":%d}}", target_c);
    http_response_t r = http_post_json(url, "/api/printer/tool", key, body);
    bool ok = (r.http_code >= 200 && r.http_code < 300);
    http_response_free(&r);
    return ok;
}

bool prusa_set_bed(const char *url, const char *key, int target_c)
{
    char body[64];
    snprintf(body, sizeof(body),
             "{\"command\":\"target\",\"target\":%d}", target_c);
    http_response_t r = http_post_json(url, "/api/printer/bed", key, body);
    bool ok = (r.http_code >= 200 && r.http_code < 300);
    http_response_free(&r);
    return ok;
}

bool prusa_get_cameras(const char *url, const char *key,
                       char *out_id, size_t out_id_sz)
{
    if (out_id && out_id_sz) out_id[0] = '\0';

    http_response_t r = http_get(url, "/api/v1/cameras", key);
    if (r.http_code != 200) {
        http_response_free(&r);
        return false;
    }
    cJSON *root = cJSON_Parse(r.data ? r.data : "");
    http_response_free(&r);
    if (!root) return false;

    /* Prusa-Link a expose plusieurs formes selon les versions : un tableau
     * a la racine, ou {"camera_list":[...]}, ou {"cameras":[...]}. On gere
     * les trois. Chaque entree a "camera_id" (ou "id") et parfois "connected"
     * / "registered". On prend la premiere camera connectee, sinon la
     * premiere tout court. */
    cJSON *list = root;
    if (!cJSON_IsArray(list)) {
        list = cJSON_GetObjectItem(root, "camera_list");
        if (!cJSON_IsArray(list)) list = cJSON_GetObjectItem(root, "cameras");
    }

    bool found = false;
    char first_id[64] = "";
    if (cJSON_IsArray(list)) {
        cJSON *cam = NULL;
        cJSON_ArrayForEach(cam, list) {
            char id[64] = "";
            json_string(cam, "camera_id", id, sizeof(id), "");
            if (id[0] == '\0') json_string(cam, "id", id, sizeof(id), "");
            if (id[0] == '\0') continue;
            if (first_id[0] == '\0')
                snprintf(first_id, sizeof(first_id), "%s", id);
            cJSON *conn = cJSON_GetObjectItem(cam, "connected");
            if (conn && cJSON_IsBool(conn) && !cJSON_IsTrue(conn)) continue;
            snprintf(out_id, out_id_sz, "%s", id);
            found = true;
            break;
        }
    }
    if (!found && first_id[0] != '\0') {
        snprintf(out_id, out_id_sz, "%s", first_id);
        found = true;
    }

    cJSON_Delete(root);
    return found;
}

bool prusa_get_snapshot(const char *url, const char *key, const char *cam_id,
                        unsigned char **out_jpeg, size_t *out_size)
{
    *out_jpeg = NULL;
    *out_size = 0;
    if (!cam_id || !*cam_id) return false;
    char path[160];
    snprintf(path, sizeof(path), "/api/v1/cameras/%s/snap", cam_id);
    http_response_t r = http_get_binary(url, path, key);
    if (r.http_code != 200) {
        http_response_free(&r);
        return false;
    }
    *out_jpeg = (unsigned char *)r.data;
    *out_size = r.size;
    /* On vole r.data, on libere pas. Le caller fera free(*out_jpeg). */
    return true;
}

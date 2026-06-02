#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char *trim(char *s)
{
    while (*s && isspace((unsigned char)*s)) s++;
    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1])) end--;
    *end = '\0';
    return s;
}

bool pb_config_load(const char *path, pb_config_t *out)
{
    memset(out, 0, sizeof(*out));
    out->poll_interval_s = 2;
    out->jog_step_mm = 10;

    FILE *fp = fopen(path, "r");
    if (!fp) return false;

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        char *p = trim(line);
        if (*p == '#' || *p == '\0') continue;
        char *eq = strchr(p, '=');
        if (!eq) continue;
        *eq = '\0';
        const char *key = trim(p);
        const char *val = trim(eq + 1);
        if (!strcmp(key, "url")) {
            snprintf(out->url, sizeof(out->url), "%s", val);
        } else if (!strcmp(key, "api_key")) {
            snprintf(out->api_key, sizeof(out->api_key), "%s", val);
        } else if (!strcmp(key, "poll_interval_s")) {
            out->poll_interval_s = atoi(val);
            if (out->poll_interval_s < 1) out->poll_interval_s = 1;
            if (out->poll_interval_s > 60) out->poll_interval_s = 60;
        } else if (!strcmp(key, "jog_step_mm")) {
            out->jog_step_mm = atoi(val);
            if (out->jog_step_mm < 1) out->jog_step_mm = 1;
            if (out->jog_step_mm > 100) out->jog_step_mm = 100;
        }
    }
    fclose(fp);
    return true;
}

bool pb_config_save(const char *path, const pb_config_t *cfg)
{
    /* Atomic write via temp + rename */
    char tmp[300];
    snprintf(tmp, sizeof(tmp), "%s.tmp", path);
    FILE *fp = fopen(tmp, "w");
    if (!fp) return false;
    fprintf(fp,
            "# PrintBoy - config Prusa-Link\n"
            "url = %s\n"
            "api_key = %s\n"
            "poll_interval_s = %d\n"
            "jog_step_mm = %d\n",
            cfg->url, cfg->api_key,
            cfg->poll_interval_s, cfg->jog_step_mm);
    fflush(fp);
    fclose(fp);
    return rename(tmp, path) == 0;
}

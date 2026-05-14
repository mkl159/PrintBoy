/*
 * PrintBoy - HTTP via libcurl.
 *
 * Timeouts agressifs (5s connect, 10s total) : on est sur un Miyoo Mini Plus
 * en WiFi 2.4GHz, la latence peut etre molle. Mais on ne veut pas geler l'UI
 * pendant 30s si l'imprimante a perdu le reseau.
 */
#include "http.h"

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BODY_BYTES (4 * 1024 * 1024)  /* 4 MiB : assez pour JPEG webcam */

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userdata)
{
    http_response_t *r = (http_response_t *)userdata;
    size_t add = size * nmemb;
    if (r->size + add > MAX_BODY_BYTES) {
        snprintf(r->error, sizeof(r->error), "response too large");
        return 0; /* abort */
    }
    char *new_buf = realloc(r->data, r->size + add + 1);
    if (!new_buf) {
        snprintf(r->error, sizeof(r->error), "out of memory");
        return 0;
    }
    r->data = new_buf;
    memcpy(r->data + r->size, ptr, add);
    r->size += add;
    r->data[r->size] = '\0';
    return add;
}

void http_init(void)
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void http_cleanup(void)
{
    curl_global_cleanup();
}

static void make_full_url(char *out, size_t out_sz,
                          const char *base, const char *path)
{
    size_t bl = strlen(base);
    int trailing = (bl > 0 && base[bl - 1] == '/') ? 1 : 0;
    int leading  = (path[0] == '/') ? 1 : 0;
    const char *sep = (trailing || leading) ? "" : "/";
    snprintf(out, out_sz, "%s%s%s", base, sep,
             (trailing && leading) ? path + 1 : path);
}

static http_response_t do_request(const char *base_url, const char *path,
                                  const char *api_key,
                                  const char *method,
                                  const char *json_body)
{
    http_response_t r = {0};
    CURL *c = curl_easy_init();
    if (!c) {
        snprintf(r.error, sizeof(r.error), "curl_easy_init failed");
        return r;
    }

    char url[1024];
    make_full_url(url, sizeof(url), base_url, path);

    char apikey_header[512];
    snprintf(apikey_header, sizeof(apikey_header), "X-Api-Key: %s",
             api_key ? api_key : "");
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, apikey_header);
    headers = curl_slist_append(headers, "Accept: application/json");
    if (json_body) {
        headers = curl_slist_append(headers, "Content-Type: application/json");
    }

    curl_easy_setopt(c, CURLOPT_URL, url);
    curl_easy_setopt(c, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(c, CURLOPT_WRITEDATA, &r);
    curl_easy_setopt(c, CURLOPT_CONNECTTIMEOUT, 5L);
    curl_easy_setopt(c, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(c, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(c, CURLOPT_USERAGENT, "PrintBoy/0.1");

    if (strcmp(method, "POST") == 0) {
        curl_easy_setopt(c, CURLOPT_POSTFIELDS, json_body ? json_body : "");
    } else if (strcmp(method, "PUT") == 0) {
        curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(c, CURLOPT_POSTFIELDS, json_body ? json_body : "");
    } else if (strcmp(method, "DELETE") == 0) {
        curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, "DELETE");
    }

    CURLcode rc = curl_easy_perform(c);
    if (rc != CURLE_OK) {
        snprintf(r.error, sizeof(r.error), "%s", curl_easy_strerror(rc));
    } else {
        curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &r.http_code);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(c);
    return r;
}

http_response_t http_get(const char *base_url, const char *path,
                         const char *api_key)
{
    return do_request(base_url, path, api_key, "GET", NULL);
}

http_response_t http_post_json(const char *base_url, const char *path,
                               const char *api_key, const char *json_body)
{
    return do_request(base_url, path, api_key, "POST", json_body);
}

http_response_t http_put_json(const char *base_url, const char *path,
                              const char *api_key, const char *json_body)
{
    return do_request(base_url, path, api_key, "PUT", json_body);
}

http_response_t http_delete(const char *base_url, const char *path,
                            const char *api_key)
{
    return do_request(base_url, path, api_key, "DELETE", NULL);
}

http_response_t http_get_binary(const char *base_url, const char *path,
                                const char *api_key)
{
    /* identique a GET, mais on ne fait pas semblant que c'est JSON */
    return do_request(base_url, path, api_key, "GET", NULL);
}

void http_response_free(http_response_t *r)
{
    if (!r) return;
    free(r->data);
    r->data = NULL;
    r->size = 0;
}

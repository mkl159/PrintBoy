/*
 * PrintBoy - mini HTTP client over libcurl.
 * Toutes les requetes ajoutent automatiquement le header X-Api-Key.
 */
#ifndef PRINTBOY_HTTP_H
#define PRINTBOY_HTTP_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char  *data;       /* malloc'd, owned by caller, NUL-terminated */
    size_t size;       /* size in bytes (excluding the NUL) */
    long   http_code;  /* HTTP status code, 0 if transport failed */
    char   error[256]; /* '\0' if OK */
} http_response_t;

/* Initialise libcurl globalement. Appeler une fois au demarrage. */
void http_init(void);
void http_cleanup(void);

/* GET sur base_url + path. Renvoie un http_response_t. Le caller doit
 * appeler http_response_free(). */
http_response_t http_get(const char *base_url, const char *path,
                         const char *api_key);

/* POST avec body JSON (peut etre NULL pour body vide). */
http_response_t http_post_json(const char *base_url, const char *path,
                               const char *api_key, const char *json_body);

/* PUT avec body JSON. */
http_response_t http_put_json(const char *base_url, const char *path,
                              const char *api_key, const char *json_body);

/* DELETE. */
http_response_t http_delete(const char *base_url, const char *path,
                            const char *api_key);

/* Telecharge une JPEG dans un buffer. data sera malloc'd. */
http_response_t http_get_binary(const char *base_url, const char *path,
                                const char *api_key);

void http_response_free(http_response_t *r);

#endif /* PRINTBOY_HTTP_H */

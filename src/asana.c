#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>
#include "asana.h"

typedef struct {
    char *data;
    size_t size;
} Memory;

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    Memory *mem = (Memory *)userp;

    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (!ptr) return 0;

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = 0;

    return realsize;
}

char *asana_list_tasks(char *asanaTok)
{
    CURL *hnd = curl_easy_init();

    Memory chunk;
    chunk.data = malloc(1);
    chunk.size = 0;
    chunk.data[0] = '\0';

    curl_easy_setopt(hnd, CURLOPT_URL, "https://app.asana.com/api/1.0/tasks");
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&chunk);

    struct curl_slist *headers = NULL;

    char auth[512];
    snprintf(auth, sizeof(auth), "Authorization: Bearer %s", asanaTok);

    headers = curl_slist_append(headers, "accept: application/json");
    headers = curl_slist_append(headers, auth);

    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    CURLcode ret = curl_easy_perform(hnd);

    curl_slist_free_all(headers);
    curl_easy_cleanup(hnd);

    if (ret != CURLE_OK) {
        free(chunk.data);
        return NULL;
    }

    return chunk.data;
}

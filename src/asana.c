#include <stdlib.h>
#include <string.h>
 
#include <curl/curl.h>

int asana_list_tasks() {
	CURL *hnd = curl_easy_init();

	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, stdout);
	curl_easy_setopt(hnd, CURLOPT_URL, "https://app.asana.com/api/1.0/tasks");

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "accept: application/json");
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

	CURLcode ret = curl_easy_perform(hnd);

	return 0;
}

#include "StdInc.h"

#define CURL_STATICLIB
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

size_t UDataReceived(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t rsize = (size * nmemb);
	char* status = (char*)ptr;

	bool getAuthServer(const char* status);

	return getAuthServer(status);

	return rsize;
}

bool getAuthServer(const char* status)
{

	char authurl[255];

	if (status == "repziw4ok")
	{
		_snprintf(authurl, sizeof(authurl), "http://%s/remauth.php", "repziw4.de");
		return authurl;
	}
	else
	{
		_snprintf(authurl, sizeof(authurl), "http://%s/remauth.php", "178.77.68.77");
		return authurl;
	}

}

const char* checkAuthStatus()
{
	curl_global_init(CURL_GLOBAL_ALL);

	CURL* curl = curl_easy_init();

	if (curl)
	{
		char url[255];
		_snprintf(url, sizeof(url), "http://repz.eu/status/status.txt");

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, UDataReceived);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "RepZIW4");
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, true);

		CURLcode code = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		curl_global_cleanup();


		if (code == CURLE_OK)
		{
		}
		else
		{
			Com_Error(1, "Could not reach repz server.");
		}
	}

	curl_global_cleanup();

}
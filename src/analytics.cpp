#include "analytics.h"

static void send_analytics_event(WorkQueue *queue, AnalyticsEventData data))
{
    printf("SEND!\n");
    CURL *curl_handle = curl_easy_init();

    if(curl_handle)
    {
	printf("DAMN!\n");
	curl_easy_setopt(curl_handle, CURLOPT_URL, "http://www.google-analytics.com/collect");
	char *user_agent = "Superverse/0.3 (Windows NT 6.2)";
	char *post_data = "v=1&tid=UA-128027751-1&cid=UUID&sc=start";
    
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, user_agent);
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_data);

	curl_easy_perform(curl_handle);
	curl_easy_cleanup(curl_handle);
    }
}

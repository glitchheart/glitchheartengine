#include "analytics.h"

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	return size * nmemb;
}

void process_analytics_event(WorkQueue *queue, void *data_ptr)
{
	AnalyticsEventData *data = (AnalyticsEventData*)data_ptr;
	
	CURL *curl_handle = curl_easy_init();

    if(curl_handle)
    {
		curl_easy_setopt(curl_handle, CURLOPT_URL, "http://www.google-analytics.com/collect");

		char *tracking_id = "UA-128027751-1";
		char event_string[256];
		char *type = "";
		
		switch(data->type)
		{
		case AnalyticsEventType::STARTED_LEVEL:
			type = "event";
			strcpy(event_string, "ec=level&ea=started");
			break;
		case AnalyticsEventType::FINISHED_LEVEL:
			type = "event";
			strcpy(event_string, "ec=level&ea=ended");
			break;
		}

		char post_data_buffer[256];
		sprintf(post_data_buffer, "v=1&tid=%s&cid=%s&t=%s&%s", tracking_id, data->user_id, type, event_string);
	
		char *user_agent = "Superverse/0.3 (Windows NT 6.2)";
    
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, user_agent);
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_data_buffer);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
		
		CURLcode res = curl_easy_perform(curl_handle);
		curl_easy_cleanup(curl_handle);

		data->state->not_completed--;
    }
}

extern PlatformApi platform;

static void send_analytics_event(WorkQueue *queue, AnalyticsEventData *data)
{
	platform.add_entry(queue, process_analytics_event, data);
}

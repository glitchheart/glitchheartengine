#include "analytics.h"

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	return size * nmemb;
}

// void process_analytics_event(WorkQueue *queue, void *data_ptr)
// {
//     AnalyticsEventData *data = (AnalyticsEventData*)data_ptr;
    
	
//     if(curl_handle)
//     {
// 	curl_easy_setopt(curl_handle, CURLOPT_URL, "http://www.google-analytics.com/collect");

// 	char *tracking_id = "UA-128027751-1";
// 	char event_string[256];
// 	char *type = "";
		
// 	switch(data->type)
// 	{
// 	case AnalyticsEventType::SKIPPED_LEVEL:
// 	    type = "event";
// 	    sprintf(event_string, "ec=level&ea=skipped&el=%d&ev=%d&=cm1=%d", data->world_id, data->level_id, (i32)data->play_time);
// 	    break;
// 	case AnalyticsEventType::STARTED_LEVEL:
// 	    type = "event";
// 	    sprintf(event_string, "ec=level&ea=started&el=%d", data->level_id);
// 	    break;
// 	case AnalyticsEventType::FINISHED_LEVEL:
// 	    type = "event";
// 	    sprintf(event_string, "ec=level&ea=finished&el=%d&ev=%d&cm1=%d", data->world_id, data->level_id, (i32)data->play_time);
// 	    break;
// 	case AnalyticsEventType::SESSION:
// 	    type = "event";ana
// 	    sprintf(event_string, "ec=session&ea=closed&cm1=%d", (i32)data->play_time);
// 	    break;
// 	}

// 	char post_data_buffer[256];
// 	sprintf(post_data_buffer, "v=1&tid=%s&cid=%s&t=%s&%s", tracking_id, data->user_id, type, event_string);
	
// 	char *user_agent = "Superverse/0.3 (Windows NT 6.2)";
    
// 	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, user_agent);
// 	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_data_buffer);
// 	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
		
// 	CURLcode res = curl_easy_perform(curl_handle);
// 	if (res != CURLE_OK)
// 	{
// 	    printf("asdasasd");
// 	}

// 	data->state->not_completed--;
//     }
// }

extern PlatformApi platform;

static void send_analytics_event(WorkQueue *queue, AnalyticsEventData *data)
{
    char world[32];
    char level[32];
    sprintf(world, "world%d", data->world_id);
    sprintf(level, "level%d", data->level_id);
    
    switch(data->type)
    {
    case AnalyticsEventType::SKIPPED_LEVEL:
	//sprintf(event_string, "d", data->world_id, data->level_id, (i32)data->play_time);
	break;
    case AnalyticsEventType::STARTED_LEVEL:
	
	gameanalytics::GameAnalytics::addProgressionEvent(gameanalytics::EGAProgressionStatus::Start, world, level, "");
	break;
    case AnalyticsEventType::FINISHED_LEVEL:
	gameanalytics::GameAnalytics::addProgressionEvent(gameanalytics::EGAProgressionStatus::Complete, world, level, "");
	break;
    case AnalyticsEventType::SESSION:
	//sprintf(event_string, "ec=session&ea=closed&cm1=%d", (i32)data->play_time);
	break;
    }
}

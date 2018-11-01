#ifndef ANALYTICS_H
#define ANALYTICS_H

#define USER_ID_SIZE 32
#define MAX_EVENTS 16

enum class AnalyticsEventType
{
    STARTED_LEVEL,
    FINISHED_LEVEL
};

enum class AnalyticsSessionInfo
{
	STARTED,
	ENDED,
   	RUNNING
};

struct AnalyticsEventData
{
    AnalyticsEventType type;
	AnalyticsSessionInfo session_info;
	char user_id[USER_ID_SIZE];
    i32 level_id;

	AnalyticsEventState *state;
};

struct AnalyticsEventState
{
	AnalyticsEventData events[MAX_EVENTS];
	u32 event_count;

	AnalyticsEventData persistent_events[MAX_EVENTS];
	u32 current_index;
	u32 not_completed;
};

#endif

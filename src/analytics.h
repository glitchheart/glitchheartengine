#ifndef ANALYTICS_H
#define ANALYTICS_H

enum class AnalyticsEventType
{
    STARTED_SESSION,
    ENDED_SESSION,
    STARTED_LEVEL,
    FINISHED_LEVEL
};

struct AnalyticsEventData
{
    AnalyticsEventType type;
    i32 level_id;
};

#endif

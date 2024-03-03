#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include "event.h"

extern int64_t current_time_ms();

class BlockingQueue
{
public:
    BlockingQueue(int capacity);
    void Push(const Event &event);
    const Event& Peak();
    void Pop();
    size_t Size();

private:
    int capacity_;
    std::queue<Event> queue_;
    std::mutex mtx_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
};

struct TimeoutEvent
{
    long timeout_ms;
    Event event;
};

struct TimeOutEventGreater
{
    bool operator()(const TimeoutEvent l, const TimeoutEvent r) const { return l.timeout_ms > r.timeout_ms; }
};

class TimeoutQueue
{
public:
    TimeoutQueue(int capacity): capacity_(capacity) {}
    void Push(const TimeoutEvent &event);
    const TimeoutEvent& Peak();
    void Pop();
    bool HasTimeoutEvents();
    size_t Size(){ return queue_.size(); }

private:
    int capacity_;
    std::priority_queue<TimeoutEvent, std::vector<TimeoutEvent>, TimeOutEventGreater> queue_;
};
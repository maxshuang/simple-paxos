#include <iostream>
#include "queue.h"
#include <chrono>

BlockingQueue::BlockingQueue(int capacity)
    : capacity_(capacity)
{
}

void BlockingQueue::Push(const Event &event)
{
    std::unique_lock<std::mutex> lock(mtx_);
    not_full_.wait(lock, [this]
                   { return queue_.size() < (size_t)capacity_; });
    queue_.push(event);
    not_empty_.notify_one();
}

const Event& BlockingQueue::Peak()
{
    std::unique_lock<std::mutex> lock(mtx_);
    not_empty_.wait(lock, [this]
                    { return !queue_.empty(); });
    return queue_.front();
}

void BlockingQueue::Pop()
{
    std::unique_lock<std::mutex> lock(mtx_);
    not_empty_.wait(lock, [this]
                    { return !queue_.empty(); });
    queue_.pop();
    not_full_.notify_one();
}

size_t BlockingQueue::Size()
{
    std::unique_lock<std::mutex> lock(mtx_);
    return queue_.size();
}

///////////////////////////////////////////////////////////////
int64_t current_time_ms()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void TimeoutQueue::Push(const TimeoutEvent &event)
{
    if (queue_.size() < (size_t)capacity_)
    {
        queue_.push(event);
        return;
    }

    std::cout << "TimeoutQueue is full, cannot push event" << std::endl;
}

const TimeoutEvent& TimeoutQueue::Peak()
{
    return queue_.top();
}

void TimeoutQueue::Pop()
{
    queue_.pop();
}

bool TimeoutQueue::HasTimeoutEvents()
{
    if (!queue_.empty())
    {
        const TimeoutEvent &event = queue_.top();
        if (event.timeout_ms <= current_time_ms())
        {
            return true;
        }
    }

    return false;
}